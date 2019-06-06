//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: role_mgr.cpp
// author: Aslan
// actor:
// data: 2008-7-11
// last:
// brief: 人物数据管理器，用于负责所有人物的生成及删除，并存储服务器内的所有人物
//------------------------------------------------------------------------------
#include "StdAfx.h"
#include "role_mgr.h"
#include "role.h"
#include "player_session.h"
#include "group_mgr.h"
#include "social_mgr.h"
#include "famehall.h"
#include "TradeYuanBao.h"
#include "vip_stall.h"
#include "..\\ServerDefine\\role_data_define.h"
#include "..\\ServerDefine\\role_data.h"

#include "../WorldDefine/msg_script.h"



//------------------------------------------------------------------------------
// 重置
//------------------------------------------------------------------------------
VOID tagRoleMgr::Reset()
{
	nMaxUseIndex	= -1;
	nLastSaveIndex	= 0;
	nFreeHeadIndex	= 0;
	nFreeNodeNum	= MAX_PLAYER_NUM;
	
	for(INT i=0; i<MAX_PLAYER_NUM - 1; ++i)
	{
		sAllRole[i].nNext = i + 1;
		sAllRole[i].pRole = NULL;
	}

	// 最后一个
	sAllRole[MAX_PLAYER_NUM - 1].nNext = GT_INVALID;
	sAllRole[MAX_PLAYER_NUM - 1].pRole = NULL;
}

//------------------------------------------------------------------------------
// 获取需保存的角色
//------------------------------------------------------------------------------
Role* tagRoleMgr::GetRoleNeedSave()
{
	if(nMaxUseIndex < 0)
	{
		return NULL;
	}
	
	INT nIndexSave = nLastSaveIndex;
	while(true)
	{
		if(nIndexSave >= nMaxUseIndex)
		{
			nIndexSave = 0;
		}
		else
		{
			++nIndexSave;
		}

		if(sAllRole[nIndexSave].pRole != NULL)
		{
			nLastSaveIndex = nIndexSave;
			break;
		}

		if(nIndexSave == nLastSaveIndex)
		{
			// 没有找到需要存储的角色
			nLastSaveIndex	= 0;
			nMaxUseIndex	= -1;
			
			// 检查代码是否有遗漏
			if(nFreeNodeNum != MAX_PLAYER_NUM)
			{
				IMSG(_T("\n\n\n\n\nCritical Error in tagRoleMgr!!!!!\n\n"));
			}

			return NULL;
		}
	}

	if(!sAllRole[nLastSaveIndex].pRole->IsNeedSave2DB())
	{
		return NULL;
	}

	return sAllRole[nLastSaveIndex].pRole;
}

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
RoleMgr::RoleMgr()
{
}

//-------------------------------------------------------------------------------
// destructor
//-------------------------------------------------------------------------------
RoleMgr::~RoleMgr()
{
	Destroy();
}

//--------------------------------------------------------------------------------
// 初始化
//--------------------------------------------------------------------------------
BOOL RoleMgr::Init()
{
	m_pFastCode = "FastCode";
	m_vctRole.Reset();
	return TRUE;
}

//---------------------------------------------------------------------------------
// 销毁
//---------------------------------------------------------------------------------
VOID RoleMgr::Destroy()
{
	// 删除所有的在线角色
	m_RoleMapMutex.Acquire();

	INT nIndex;
	Role* pRole = NULL;

	RoleMap::TMapIterator it = m_mapRole.Begin();
	while( m_mapRole.PeekNext(it, nIndex) )
	{
		pRole = m_vctRole.Remove(nIndex);
		Role::Delete(pRole);
	}
	m_mapRole.Clear();
	m_vctRole.Reset();

	m_RoleMapMutex.Release();

	// 删除所有的离线角色
	EraseAllRoleInfo();
}

//-----------------------------------------------------------------------------------
// 创建一个新的离线角色
//-----------------------------------------------------------------------------------
BOOL RoleMgr::CreateRoleInfo(const tagRoleInfo* pInfo)
{
	m_RoleInfoMapMutex.Acquire();
	if (!m_mapRoleInfo.IsExist(pInfo->dwRoleID))
	{
		tagRoleInfo* pNew = new tagRoleInfo;
		if( !P_VALID(pNew) ) return FALSE;

		m_pFastCode->MemCpy(pNew, pInfo, sizeof(tagRoleInfo));
		pNew->bOnline = false;
		m_mapRoleInfo.Add(pNew->dwRoleID, pNew);
		m_mapRoleNameCrcID.Add(pNew->dwRoleNameCrc, pNew->dwRoleID);
	}
	m_RoleInfoMapMutex.Release();

	return TRUE;
}

//------------------------------------------------------------------------------------
// 删除一个离线角色，这一般在删除玩家时调用
//------------------------------------------------------------------------------------
BOOL RoleMgr::DeleteRoleInfo(const DWORD dwRoleID)
{
	m_RoleInfoMapMutex.Acquire();

	tagRoleInfo* pInfo = m_mapRoleInfo.Peek(dwRoleID);
	if( P_VALID(pInfo) )
	{
		DeletAllFriend(dwRoleID);
		g_VIPStall.RemoveRoleVIPStall(dwRoleID);	// VIP摊位处理
		DeleteYBOrder(dwRoleID);
		m_mapRoleInfo.Erase(dwRoleID);
		m_mapRoleNameCrcID.Erase(pInfo->dwRoleNameCrc);
		SAFE_DEL(pInfo);
	}

	m_RoleInfoMapMutex.Release();

	return TRUE;
}

//-------------------------------------------------------------------------------------
// 创建一个在线玩家，这在LoongDB载入角色返回时调用
//-------------------------------------------------------------------------------------
Role* RoleMgr::CreateRole(DWORD dwRoleID, const tagRoleDataLoad* pData, PlayerSession* pSession, BOOL& bFirst)
{
	Role* pOld = GetRolePtrByID(dwRoleID);
	if (P_VALID(pOld))
	{
		IMSG(_T("Role already loaded! roleid:%u, accountid:%u!\n"), dwRoleID, pSession->GetSessionID());
		ILOG->Write(_T("Role already loaded! roleid:%u, accountid:%u!\n"), dwRoleID, pSession->GetSessionID());
		return NULL;
	}

	Role* pNew = Role::Create(dwRoleID, pData, pSession);
	if( !P_VALID(pNew) ) return NULL;

	m_RoleMapMutex.Acquire();

	INT nIndex = m_vctRole.Add(pNew);
	if(GT_INVALID == nIndex)
	{
		IMSG(_T("\n\n\n\n\nCritical Error in tagRoleMgr!!!!!\n\n"));
		ASSERT(0);
		Role::Delete(pNew);
		return NULL;
	}

	m_mapRole.Add(pNew->GetID(), nIndex);

	m_RoleMapMutex.Release();

	// 查看是不是第一次上线
	if( GT_INVALID == pNew->GetOnlineTime() )
	{
		bFirst = TRUE;
	}

	PlayerOnline(pNew, bFirst);

	return pNew;
}

//--------------------------------------------------------------------------------------
// 删除一个在线玩家，这一般在人物离线时删除（是一离线就删除，还是等数据库返回了才删除）
//--------------------------------------------------------------------------------------
BOOL RoleMgr::DeleteRole(const DWORD dwRoleID)
{
	m_RoleMapMutex.Acquire();

	INT nIndex = m_mapRole.Peek(dwRoleID);
	Role* pRole = m_vctRole.GetRole(nIndex);
	if( P_VALID(pRole) )
	{
		PlayerOutLine(pRole);
		m_vctRole.Remove(nIndex);
		m_mapRole.Erase(dwRoleID);
		Role::Delete(pRole);
	}
	m_RoleMapMutex.Release();

	return TRUE;
}

//--------------------------------------------------------------------------------------
// 保存一个玩家到DB
//--------------------------------------------------------------------------------------
VOID RoleMgr::SaveOneRoleToDB()
{
	if(m_mapRole.Empty())
	{
		return;
	}
	
	Role *pRole = m_vctRole.GetRoleNeedSave();
	if(pRole != NULL)
	{
		pRole->SaveToDB();
	}
}

//--------------------------------------------------------------------------------------
// 保存所有玩家到DB（一般在系统停机时）
//--------------------------------------------------------------------------------------
VOID RoleMgr::SaveAllRolesToDB()
{
	INT nIndex = GT_INVALID;
	Role* pRole = NULL;
	RoleMap::TMapIterator it = m_mapRole.Begin();

	while( m_mapRole.PeekNext(it, nIndex) )
	{
		pRole = m_vctRole.GetRole(nIndex);
		if( P_VALID(pRole) )
		{
			pRole->SaveToDB();
		}
	}
}

//---------------------------------------------------------------------------------------
// 根据玩家名称CRC值得到玩家ID
//---------------------------------------------------------------------------------------
DWORD RoleMgr::GetRoleIDByNameCrc(const DWORD dwNameCrc)
{
	return m_mapRoleNameCrcID.Peek(dwNameCrc);
}

//---------------------------------------------------------------------------------------
// 根据玩家ID得到玩家名称
//---------------------------------------------------------------------------------------
VOID RoleMgr::GetRoleNameByID(const DWORD dwRoleID, LPTSTR szName)
{
	tagRoleInfo* pRoleInfo = m_mapRoleInfo.Peek(dwRoleID);
	if( P_VALID(pRoleInfo) )
	{
		_tcsncpy(szName, pRoleInfo->szRoleName, X_SHORT_NAME);
	}
	else
	{
		ZeroMemory(szName, X_SHORT_NAME * sizeof(TCHAR));
	}
}

//---------------------------------------------------------------------------------------
// 根据玩家ID得到玩家指针
//---------------------------------------------------------------------------------------
Role* RoleMgr::GetRolePtrByID(const DWORD dwRoleID)
{
	return m_vctRole.GetRole(m_mapRole.Peek(dwRoleID));
}

//---------------------------------------------------------------------------------------
// 随机抽取在线玩家bMustGet为真则必须取到一个
//---------------------------------------------------------------------------------------
Role* RoleMgr::GetRandRolePtr( BOOL bGetFirst /*= FALSE*/ )
{
	if (bGetFirst)
	{
		if (m_mapRole.Size() <= 0)
		{
			return NULL;
		}

		return m_vctRole.GetRole(m_mapRole.Begin()->second);
	}
	INT	nMaxUsedIndex = m_vctRole.GetMaxUsedIndex();

	if(nMaxUsedIndex == 0)
		return (Role*)GT_INVALID;

	INT nRandIndex = rand() % nMaxUsedIndex;

	return m_vctRole.GetRole(nRandIndex);
}

struct SendMsg
{
	SendMsg(PVOID pMsg, DWORD dwSize)
		:m_pMsg(pMsg), m_dwSize(dwSize){}
	VOID operator()(Unit* pUnit)
	{
		if (P_VALID(pUnit) && pUnit->IsRole())
		{
			Role* pRole = dynamic_cast<Role*>(pUnit);
			if (P_VALID(pRole))
			{
				pRole->SendMessage(m_pMsg, m_dwSize);
			}			
		}
	}
	PVOID m_pMsg;
	DWORD m_dwSize;
};

//--------------------------------------------------------------------------------------------
// 给服务器所有地图内的玩家发消息
//--------------------------------------------------------------------------------------------
VOID RoleMgr::SendWorldMsg(LPVOID pMsg, DWORD dwSize)
{
	ForEachRoleInWorld(SendMsg(pMsg, dwSize));
// 	RoleMap::TMapIterator it = m_mapRole.Begin();
// 	INT nIndex = -1;
// 	Role* pRole = NULL;
// 	while( m_mapRole.PeekNext(it, nIndex) )
// 	{
// 		pRole = m_vctRole.GetRole(nIndex);
// 		if( P_VALID(pRole) )
// 		{
// 			pRole->SendMessage(pMsg, dwSize);
// 		}
// 	}
}

//--------------------------------------------------------------------------------------------
// 得到玩家的简易信息（所有玩家包括不在线）
//--------------------------------------------------------------------------------------------
tagRoleInfo* RoleMgr::GetRoleInfo(const DWORD dwRoleID)
{
	tagRoleInfo* pRoleInfo = m_mapRoleInfo.Peek(dwRoleID);
	return pRoleInfo;	
}

//--------------------------------------------------------------------------------------------
// 玩家上线是相关处理
//--------------------------------------------------------------------------------------------
VOID RoleMgr::PlayerOnline(Role *pRole, BOOL bFirst)
{
	// 如果是第一次上线，则调用初次上线函数
	pRole->Online(bFirst);

	// 发送玩家上线的事件
	g_socialMgr.SendLoginToFriend(pRole);

	tagRoleInfo* pRoleInfo = g_roleMgr.GetRoleInfo(pRole->GetID());

	if(P_VALID(pRoleInfo))
		pRoleInfo->bOnline = true;
}

//--------------------------------------------------------------------------------------------
// 玩家下线是相关处理
//--------------------------------------------------------------------------------------------
VOID RoleMgr::PlayerOutLine(Role* pRole)
{
	// 玩家下时发送消息给好友
	g_socialMgr.SendLogoutToFriend(pRole);

	// 玩家退出小队
	DWORD dwTeamID = pRole->GetTeamID();
	if( GT_INVALID != dwTeamID )
	{
		g_groupMgr.RoleOutline(pRole);
	}

	tagRoleInfo* pRoleInfo = g_roleMgr.GetRoleInfo(pRole->GetID());

	if(P_VALID(pRoleInfo))
	{
		pRoleInfo->bOnline = false;
		pRoleInfo->dwTimeLastLogout = pRole->GetLogoutTime();
	}
}

//--------------------------------------------------------------------------------------------
// 玩家删除角色时，删除数据库中相关的好友信息
//--------------------------------------------------------------------------------------------
VOID RoleMgr::DeletAllFriend(DWORD dwRoleID)
{
	tagNDBC_DelAllFriRel	sendFri;
	sendFri.dwRoleID = dwRoleID;
	g_dbSession.Send(&sendFri, sendFri.dwSize);

	tagNDBC_DelAllFriValRel	sendVal;
	sendVal.dwRoleID = dwRoleID;
	g_dbSession.Send(&sendVal, sendVal.dwSize);
}

//--------------------------------------------------------------------------------------------
// 玩家删除角色时，删除玩家提交过的元宝交易订单
//--------------------------------------------------------------------------------------------
VOID RoleMgr::DeleteYBOrder(DWORD dwRoleID)
{
	tagYuanBaoOrder* pYBSellOrder= g_tradeYB.GetYBSellOrder(dwRoleID);
	if(P_VALID(pYBSellOrder))
		g_tradeYB.DeleteYBOrder(pYBSellOrder, EYBOM_Close);

	tagYuanBaoOrder* pYBBuyOrder= g_tradeYB.GetYBBuyOrder(dwRoleID);
	if(P_VALID(pYBBuyOrder))
		g_tradeYB.DeleteYBOrder(pYBBuyOrder, EYBOM_Close);
}

//--------------------------------------------------------------------------------------------
// 删除所有的离线角色
//--------------------------------------------------------------------------------------------
VOID RoleMgr::EraseAllRoleInfo()
{
	m_RoleInfoMapMutex.Acquire();

	tagRoleInfo* pInfo = NULL;
	RoleInfoMap::TMapIterator it2 = m_mapRoleInfo.Begin();
	while( m_mapRoleInfo.PeekNext(it2, pInfo) )
	{
		SAFE_DEL(pInfo);
	}
	m_mapRoleInfo.Clear();

	m_RoleInfoMapMutex.Release();
}

//--------------------------------------------------------------------------------------------
// 重置玩家声望值
//--------------------------------------------------------------------------------------------
VOID RoleMgr::ResetRoleRep( ECLanType eClanType, EReputationLevel eRepLvl, tagDWORDTime dwtResetTime )
{
	RoleMap::TMapIterator it = m_mapRole.Begin();
	INT nIndex = GT_INVALID;
	Role* pRole = NULL;
	while( m_mapRole.PeekNext(it, nIndex) )
	{
		pRole = m_vctRole.GetRole(nIndex);
		if( P_VALID(pRole) )
		{
			g_fameHall.RoleRepUpdate(pRole, eClanType);
		}
	}
}

//--------------------------------------------------------------------------------------------
// 给所有在线玩家加一个buff（只能由主线程调用）
//--------------------------------------------------------------------------------------------
VOID RoleMgr::AddAllRoleBuff(DWORD dwBuffTypeID)
{
	const tagBuffProto* pProto = g_attRes.GetBuffProto(dwBuffTypeID);
	if( !P_VALID(pProto) ) return;

	RoleMap::TMapIterator it = m_mapRole.Begin();
	INT nIndex = -1;
	Role* pRole = NULL;
	while( m_mapRole.PeekNext(it, nIndex) )
	{
		pRole = m_vctRole.GetRole(nIndex);
		if( P_VALID(pRole) )
		{
			pRole->TryAddBuff(pRole, pProto, NULL, NULL, NULL);
		}
	}
}
//tbd:通过名字id获取名字
VOID RoleMgr::GetRoleNameByNameID( const DWORD dwNameID, LPTSTR szName )
{
	// 暂时用RoleID做NameID
	GetRoleNameByID(dwNameID, szName);
}


RoleMgr g_roleMgr;