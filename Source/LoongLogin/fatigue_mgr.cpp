#include "StdAfx.h"
#include "fatigue_mgr.h"
#include "../ServerDefine/msg_fatigue.h"
#include "login_server.h"
#include "loongworld_mgr.h"
#include "db.h"
#include "fatigue_session.h"
#include "fatigue_server.h"

VOID	FatigueMgr::Update()
{
	if (!g_login.IsEnableFatigueGuard())	return;

	// 每分钟发给FatigueSession所有在线防沉迷玩家
	if (--m_nMinCounter <= 0)
	{
		m_nMinCounter = m_nCircleUnit;
		NotifyFagitueSvr();
	}

	// 通知所有防沉迷玩家
	SendWorldNotifys();
}

BOOL FatigueMgr::Init()
{
	CreateObj("FatigueMgr", "VarContainer");
	TObjRef<VarContainer> pVar = "FatigueMgr";
	if( !P_VALID(pVar) ) return FALSE;
	
	TObjRef<Util> pUtil = "Util";
	TCHAR tszPath[MAX_PATH];
	ZeroMemory(tszPath, sizeof(tszPath));
	if (!pUtil->GetIniPath(tszPath, _T("server_config\\login\\login"))||
		!pVar->Load("VirtualFileSys", tszPath))
	{
		ERR(_T("配置文件未找到"));
		return FALSE;
	}

	m_nCircleUnit = TICK_PER_SECOND * pVar->GetInt(_T("circle fatigue_guard"));
	// 删除配置文件对象
	KillObj("FatigueMgr");

	m_nMinCounter = m_nCircleUnit;

	// 读取所有在线玩家
	return LoadAllOLAccounts();
}

BOOL FatigueMgr::LoadAllOLAccounts()
{
	if (!g_login.IsEnableFatigueGuard())	return TRUE;

	m_setLock.Acquire();
	BOOL bRtv = sBeton.LoadOnLineGuardAccountIDs(m_mapAccountID2WorldNameCrc);
	m_setLock.Release();

	return TRUE;
}

VOID FatigueMgr::NotifyFagitueSvr()
{
	if (!g_login.IsEnableFatigueGuard())	return;

	if (!sFatigueSession.IsConnected())
		return;

	std::list<DWORD> listOLAccounts;
	m_setLock.Acquire();
	m_mapAccountID2WorldNameCrc.ExportAllKey(listOLAccounts);
	m_setLock.Release();

	DWORD dwNum = listOLAccounts.size();

	if (dwNum <= 0)
		return;

	if (g_login.IsSimFatigueServer())
	{
		DWORD *dwAccounts = new DWORD[dwNum];
		std::copy(listOLAccounts.begin(), listOLAccounts.end(), dwAccounts);
		sFatigueSession.SendOLGuardAccounts(dwAccounts, dwNum);
		SAFE_DEL_ARRAY(dwAccounts);
	}
	else
	{
#define MAX_SEND_SIZE 500
		DWORD dwAccounts[MAX_SEND_SIZE];
		INT nNum=0;
		BOOL bNotify = FALSE;
		std::list<DWORD>::iterator itr = listOLAccounts.begin();
		while (itr != listOLAccounts.end())
		{
			dwAccounts[nNum++] = *itr;
			++itr;
			if (nNum == MAX_SEND_SIZE)
			{
				bNotify = TRUE;
				sFatigueSession.SendOLGuardAccounts(dwAccounts, nNum);
				nNum = 0;
			}
		}
		if (!bNotify || bNotify && nNum != 0)
		{
			sFatigueSession.SendOLGuardAccounts(dwAccounts, nNum);
		}

#undef MAX_SEND_SIZE
	}
}

VOID FatigueMgr::LoginNotify( DWORD dwAccount, DWORD dwWorldNameCrc, BOOL bGuard )
{
	if (!g_login.IsEnableFatigueGuard())	return;

	if (!bGuard)	return;

	m_setLock.Acquire();
	m_mapAccountID2WorldNameCrc.ModifyValue(dwAccount, dwWorldNameCrc);
	m_setLock.Release();

	sFatigueSession.SendQueryStatus(dwAccount);

}

VOID FatigueMgr::LogoutNotify( DWORD dwAccount )
{
	if (!g_login.IsEnableFatigueGuard())	return;

	m_setLock.Acquire();
	m_mapAccountID2WorldNameCrc.Erase(dwAccount);
	m_setLock.Release();
}

VOID FatigueMgr::NotifyPlayer( const tagFatigueNotify* pNotify )
{
	if (!g_login.IsEnableFatigueGuard())	return;

	DWORD dwWorldNameCrc = m_mapAccountID2WorldNameCrc.Peek(pNotify->dwAccountID);
	if (!GT_VALID(dwWorldNameCrc)) 
		return;

	tagFatigueNotify* pNewNotify = new tagFatigueNotify;
	*pNewNotify = *pNotify;

	TList<tagFatigueNotify*>* pListNotifys = m_mapNotifyLists.Peek(dwWorldNameCrc);
	if (!P_VALID(pListNotifys))
	{
		pListNotifys = new TList<tagFatigueNotify*>;
		m_mapNotifyLists.Add(dwWorldNameCrc, pListNotifys);
	}

	pListNotifys->PushBack(pNewNotify);
}

VOID FatigueMgr::SendWorldNotifys()
{
	if (!g_login.IsEnableFatigueGuard())	return;

	static BYTE buffer[5000 * 8] = {0};
	tagNLW_FatigueNotify* pSend = reinterpret_cast<tagNLW_FatigueNotify*>(buffer);
	pSend->dwID = IUTIL->Crc32("NLW_FatigueNotify");
	pSend->nNum = 0;


	TMap<DWORD, TList<tagFatigueNotify*>* >::TMapIterator itr = m_mapNotifyLists.Begin();
	DWORD dwWorldNameCrc = GT_INVALID;
	TList<tagFatigueNotify*>* pListNotifies = NULL;
	while(m_mapNotifyLists.PeekNext(itr, dwWorldNameCrc, pListNotifies))
	{
		if (!P_VALID(pListNotifies)) continue;

		pSend->nNum = 0;
		tagFatigueNotify* pNotify = NULL;
		while (!pListNotifies->Empty())
		{
			pNotify = pListNotifies->PopFront();
			pSend->notify[pSend->nNum++] = *pNotify;
			SAFE_DEL(pNotify);
		}

		SAFE_DEL(pListNotifies);

		if (pSend->nNum > 0)
		{
			pSend->dwSize = sizeof(tagNLW_FatigueNotify) - sizeof(tagFatigueNotify) + sizeof(tagFatigueNotify) * pSend->nNum;
		}
		else
		{
			pSend->dwSize = sizeof(tagNLW_FatigueNotify);
		}

		sLoongWorldMgr.SendMsg(dwWorldNameCrc, pSend, pSend->dwSize);
	}

	m_mapNotifyLists.Clear();
}

VOID FatigueMgr::ResetWorldAccounts( DWORD dwWorldNameCrcReset, DWORD *pOLAccountIDs, INT nOLAccountIDNum )
{
	if (!g_login.IsEnableFatigueGuard())	return;

	m_setLock.Acquire();
	
	TMap<DWORD, DWORD>::TMapIterator itr = m_mapAccountID2WorldNameCrc.Begin();
	DWORD dwAccountID = GT_INVALID;
	DWORD dwWorldNameCrc = GT_INVALID;
	while (m_mapAccountID2WorldNameCrc.PeekNext(itr, dwAccountID, dwWorldNameCrc))
	{
		if (GT_VALID(dwAccountID) && GT_VALID(dwWorldNameCrc) && dwWorldNameCrc == dwWorldNameCrcReset )
		{
			m_mapAccountID2WorldNameCrc.Erase(dwAccountID);
		}
	}

	if (!P_VALID(pOLAccountIDs) || 0 == nOLAccountIDNum)
	{
		m_setLock.Release();
		return;
	}

	for (INT i=0; i<nOLAccountIDNum; ++i)
	{
		if (!GT_VALID(m_mapAccountID2WorldNameCrc.Peek(pOLAccountIDs[i])))
		{
			m_mapAccountID2WorldNameCrc.Add(pOLAccountIDs[i], dwWorldNameCrcReset);
		}
	}

	m_setLock.Release();
}
