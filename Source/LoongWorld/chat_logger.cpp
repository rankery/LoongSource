#include "StdAfx.h"
#include "chat_mgr.h"
#include "chat_logger.h"

ChatLogger::ChatLogger()
: m_Trunk(this)
{
	m_bOn = FALSE;
	m_pMsgQueue = NULL;
	m_bTerminated = TRUE;
	m_nIndex = 0;
	memset(m_pBuf, 0, sizeof(m_pBuf));
}

ChatLogger& ChatLogger::getInstance()
{
	static ChatLogger _instance;
	return _instance;
}

BOOL ChatLogger::Init()
{
	m_pThread		=	"Thread";

	// 初始化成员属性
	TObjRef<VarContainer> pVar = "VarContainer";
	m_bOn		=	(BOOL)pVar->GetDword(_T("on chat_log"));

	m_bOn = 1;

	if(m_bOn)
	{
		m_pMsgQueue = new MsgQueueTS(TRUE, FALSE);
		m_bTerminated = FALSE;
		vEngine::g_pInterfaceMgr->Create("ChatLogger", "Log");
		m_Logger = "ChatLogger";
		m_Logger->Create();

		m_dwChatCmdID = TObjRef<Util>()->Crc32("NS_RoleChat");
		m_dwEquipCmdID = TObjRef<Util>()->Crc32("NS_RoleShowEquip");
		m_dwItemCmdID = TObjRef<Util>()->Crc32("NS_RoleShowItem");

		// 启动线程
		if(!m_pThread->CreateThread(_T("ChatLogger"), 
			(vEngine::THREADPROC)m_Trunk.sfp0(&ChatLogger::ThreadWriteLog), NULL))
		{
			return FALSE;
		}

		while( !m_pThread->IsThreadActive(_T("ChatLogger")) )
		{
			continue;
		}
	}
	return TRUE;
}

void ChatLogger::Log(const MsgCmd& msgCmd)
{
	if(m_bOn)
	{
		LPCTSTR pStr = Format(msgCmd);
		if(P_VALID(pStr))
		{
			m_pMsgQueue->AddMsg((LPCVOID)pStr, (_tcslen(pStr)+1)* sizeof(TCHAR));
			delete pStr;
		}
	}
}


UINT ChatLogger::ThreadWriteLog()
{
	while(!m_bTerminated)
	{
		DWORD msgNum;
		LPBYTE lpByte = m_pMsgQueue->GetMsg(msgNum);
		while(lpByte)
		{
			LPCTSTR pMsg = (LPCTSTR)lpByte;
			size_t len = _tcslen(pMsg);
			if(len < sizeof(m_pBuf))
			{
				if(len + m_nIndex > sizeof(m_pBuf) - 1)
					FlushToDisk();
				memcpy(m_pBuf + m_nIndex, pMsg, len*sizeof(TCHAR));
				m_nIndex += len;
			}	
			m_pMsgQueue->FreeMsg(lpByte);
			lpByte = m_pMsgQueue->GetMsg(msgNum);
		}
		::WaitForSingleObject(m_pMsgQueue->GetEvent(), 200);
		FlushToDisk();
	}
	FlushToDisk();
	return 0;
}

void ChatLogger::Destroy()
{
	InterlockedExchange((LPLONG)&m_bTerminated, TRUE);
	m_pThread->WaitForThreadDestroy(_T("ChatLogger"), INFINITE);
	SAFE_DEL(m_pMsgQueue);
}

void ChatLogger::FlushToDisk()
{
	if(m_nIndex > 0)
	{
		m_pBuf[m_nIndex] = 0;
		m_Logger->Write(m_pBuf);
	}
	m_nIndex = 0;
}


LPTSTR ChatLogger::Format(const MsgCmd& msgCmd)
{
	MTRANS_ELSE_RET(pSender, msgCmd.GetSender(), Role, NULL);
	MTRANS_ELSE_RET(pSend, msgCmd.GetSendMsg(), tagNetCmd, NULL);
	MTRANS_ELSE_RET(pRecv, msgCmd.GetRecvMsg(), tagNetCmd, NULL);
	ESendChatChannel eChannel = msgCmd.GetChannelType();
	DWORD roleID = pSender->GetID();
	DWORD receiverID = msgCmd.GetReceiverID();
	TCHAR pPre[256];
	tagDWORDTime now = g_world.GetWorldTime();
	_stprintf_s(pPre, 256, _T("%02d-%02d-%02d %02d:%02d:%02d, %d %d %d, "), now.year, now.month, now.day, now.hour, now.min, now.sec, roleID, eChannel, receiverID);
	TCHAR pBuf[256];
	DWORD dSize = 0;
	if( pSend->dwID == m_dwEquipCmdID)
	{
		dSize += sizeof(_T("show equipment "));
		memcpy(pBuf, _T("show equipment "), sizeof(_T("show equipment ")));
	}
	else if (pSend->dwID == m_dwItemCmdID)
	{
		tagNS_RoleShowItem* pShowItem = static_cast<tagNS_RoleShowItem*>(pSend);
		_stprintf_s(pBuf, 256, _T("show item %d"), pShowItem->dwTypeID);
		dSize += _tcslen(pBuf)* sizeof(TCHAR); 
	}
	DWORD preSize = _tcslen(pPre) * sizeof(TCHAR);
	DWORD size = dSize + msgCmd.GetContentSize() + preSize + 6;
	LPBYTE pOut = new BYTE[size];
	memset(pOut, 0, size);
	memcpy(pOut, pPre, preSize);
	if(dSize > 0) memcpy(pOut+preSize, pBuf, dSize);
	if(msgCmd.GetContent() && msgCmd.GetContentSize() >0)
	{
		memcpy(pOut+preSize+dSize, msgCmd.GetContent(), msgCmd.GetContentSize());
	}
	_tcscat((wchar_t*)pOut, _T("\r\n"));

	return (LPTSTR)pOut;
}