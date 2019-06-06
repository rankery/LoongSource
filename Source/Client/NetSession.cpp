//-----------------------------------------------------------------------------
//!\file NetSession.cpp
//!\author Lyp
//!
//!\date 2008-06-10
//! last 2008-06-12
//!
//!\brief net session
//!
//!	Copyright (c) 1985-2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "NetSession.h"

//-----------------------------------------------------------------------------
//! destruction
//-----------------------------------------------------------------------------
NetSession::~NetSession()
{
	m_pCmdMgr->Destroy();

	stringstream stream;
	stream << "XClient_" << this;

	KillObj(stream.str().c_str());
}


//-----------------------------------------------------------------------------
//! construction
//-----------------------------------------------------------------------------
VOID NetSession::Init()
{
	stringstream stream;
	stream << "XClient_" << this;
	m_pStream = CreateObj(stream.str().c_str(), "XClient");

	m_pStream->Init(FALSE);	// 关闭Nagle算法
	m_dwLastSendTime = timeGetTime();

	m_pCmdMgr->Init();
	m_pConsole->Register(_T("sendqueue"), m_Trunk.sfp1(&NetSession::SendQueue), _T("send queue list"), 1);
}



//-----------------------------------------------------------------------------
// Connect
//-----------------------------------------------------------------------------
VOID NetSession::Connect(LPCTSTR szIP, INT nPort)
{
	m_strRemoteIP = szIP;
	m_pStream->TryToConnect(m_pUtil->UnicodeToUnicode8(szIP), nPort);
}


//-----------------------------------------------------------------------------
// Disconnect
//-----------------------------------------------------------------------------
VOID NetSession::Disconnect()
{
	m_pStream->Disconnect();
}


//-----------------------------------------------------------------------------
// IsConnect
//-----------------------------------------------------------------------------
BOOL NetSession::IsConnect()
{
	return m_pStream->IsConnected();
}



//-----------------------------------------------------------------------------
// Update
//-----------------------------------------------------------------------------
VOID NetSession::Update()
{
	static DWORD dwID_Skill = m_pUtil->Crc32("NC_Skill");	// 使用技能

	// 不停收网络消息
	for(;;)
	{
		DWORD dwRecvSize = 0;
		LPBYTE pRecv = m_pStream->Recv(dwRecvSize);
		if( !P_VALID(pRecv) )
			break;

		// 从大包中解出小包依次处理
		LPBYTE pRecvPointer = pRecv;
		DWORD dwMsgSize = 0;
		while( pRecvPointer < pRecv + dwRecvSize )
		{
			tagNetCmd* pRecvCmd = (tagNetCmd*)pRecvPointer;
			dwMsgSize = pRecvCmd->dwSize;
			m_pCmdMgr->HandleCmd(pRecvCmd, dwMsgSize, 0);
			pRecvPointer += dwMsgSize;
		}

		m_pStream->FreeRecved(pRecv);
	}

	
	if( m_SendQueueAction.GetMsgNum() > 0 )	
	{
		// 如果是使用技能消息，瞬间发
		m_SendQueueAction.ResetIterator();

		DWORD dwMsgSize = 0;
		LPBYTE pMsg = m_SendQueueAction.PeekMsg(dwMsgSize);
		if( ((tagNetCmd*)pMsg)->dwID == dwID_Skill )
		{
			m_pStream->Send(pMsg, dwMsgSize);
			m_SendQueueAction.RemoveMsg();
		}
	}


	// 每次心跳发送一条
	if( timeGetTime() - m_dwLastSendTime > TICK_TIME + 20 )
	{
		// 优先发送行为队列中的消息
		DWORD dwMsgSize = 0;
		LPBYTE pMsg = m_SendQueueAction.GetMsg(dwMsgSize);
		if( pMsg )
		{
			m_pStream->Send(pMsg, dwMsgSize);
			m_SendQueueAction.FreeMsg(pMsg);
		}
		else
		{
			pMsg = m_SendQueue.GetMsg(dwMsgSize);
			if( pMsg )
			{
				m_pStream->Send(pMsg, dwMsgSize);
				m_SendQueue.FreeMsg(pMsg);
			}
			else
			{
				pMsg = m_SendQueueIdle.GetMsg(dwMsgSize);
				if( pMsg )
				{
					m_pStream->Send(pMsg, dwMsgSize);
					m_SendQueueIdle.FreeMsg(pMsg);
				}
			}
		}
		
		m_dwLastSendTime = timeGetTime();
	}

	// 监视网络状态
	m_pConsole->Watch(_T("SendQueueAction"), m_SendQueueAction.GetMsgNum());
	m_pConsole->Watch(_T("SendQueue"), m_SendQueue.GetMsgNum());
	m_pConsole->Watch(_T("SendQueueIdle"), m_SendQueueIdle.GetMsgNum());
	m_pConsole->Watch(_T("NetSendSpeed"), m_pStream->GetBytesFlowOut());
	m_pConsole->Watch(_T("NetRecvSpeed"), m_pStream->GetBytesFlowIn());
	m_pConsole->Watch(_T("NetSendPack"), m_pStream->GetTotalSendPackNum());
	m_pConsole->Watch(_T("NetRecvPack"), m_pStream->GetTotalRecvPackNum());

}


//-----------------------------------------------------------------------------
// Send
//-----------------------------------------------------------------------------
VOID NetSession::Send(tagNetCmd* pNetCmd)
{
	static DWORD dwID_MouseMove	= m_pUtil->Crc32("NC_MouseWalk");		// 鼠标行走
	static DWORD dwID_KeyboardMove = m_pUtil->Crc32("NC_KeyboardWalk");	// 键盘行走
	static DWORD dwID_StopKeyboardWalk = m_pUtil->Crc32("NC_StopWalk");	// 停止键盘行走
	static DWORD dwID_Jump = m_pUtil->Crc32("NC_Jump");					// 跳跃
	static DWORD dwID_Drop = m_pUtil->Crc32("NC_Drop");					// 掉落
	static DWORD dwID_VDrop = m_pUtil->Crc32("NC_VDrop");				// 垂直掉落
	static DWORD dwID_Slide = m_pUtil->Crc32("NC_Slide");				// 滑落
	static DWORD dwID_Skill = m_pUtil->Crc32("NC_Skill");				// 使用技能
	static DWORD dwID_UseItem = m_pUtil->Crc32("NC_UseItem");			// 使用物品
	static DWORD dwID_SwapWeapon = m_pUtil->Crc32("NC_SwapWeapon");		// 切换武器

	static DWORD dwID_GetRemoteRoleState = m_pUtil->Crc32("NC_GetRemoteRoleState");	// 得到远程玩家信息
	static DWORD dwID_RoleGetSomeName = m_pUtil->Crc32("NC_RoleGetSomeName");	// 得到远程玩家名字
	

	// 将指令分类
	if( pNetCmd->dwID == dwID_MouseMove
		|| pNetCmd->dwID == dwID_KeyboardMove
		|| pNetCmd->dwID == dwID_StopKeyboardWalk
		|| pNetCmd->dwID == dwID_Jump
		|| pNetCmd->dwID == dwID_Drop
		|| pNetCmd->dwID == dwID_VDrop
		|| pNetCmd->dwID == dwID_Slide
		|| pNetCmd->dwID == dwID_Skill
		|| pNetCmd->dwID == dwID_UseItem
		|| pNetCmd->dwID == dwID_SwapWeapon)
	{
		// 有些新指令可以取代队列中的老指令
		if( pNetCmd->dwID == dwID_MouseMove
			|| pNetCmd->dwID == dwID_KeyboardMove
			|| pNetCmd->dwID == dwID_StopKeyboardWalk
			|| pNetCmd->dwID == dwID_Slide
			|| pNetCmd->dwID == dwID_SwapWeapon)
		{
			DWORD dwMsgSize=0;
			tagNetCmd* pSendCmd=NULL;
			m_SendQueueAction.ResetIterator();
			while(pSendCmd=(tagNetCmd*)m_SendQueueAction.PeekMsg(dwMsgSize))
			{
				if( pSendCmd->dwID==pNetCmd->dwID )
					m_SendQueueAction.RemoveMsg();
				else
					m_SendQueueAction.IncIterator();
			}
		}

		// 发送
		m_SendQueueAction.AddMsg(pNetCmd, pNetCmd->dwSize);
	}
	else
	{
		if( pNetCmd->dwID == dwID_GetRemoteRoleState 
			|| pNetCmd->dwID == dwID_RoleGetSomeName )
			m_SendQueueIdle.AddMsg(pNetCmd, pNetCmd->dwSize);
		else
			m_SendQueue.AddMsg(pNetCmd, pNetCmd->dwSize);
	}
}


INT NetSession::GetSendQueueSize()
{
	return m_SendQueue.GetMsgNum();
}

INT NetSession::GetSendQueueActionSize()
{
	return m_SendQueueAction.GetMsgNum();
}

INT NetSession::GetSendQueueIdleSize()
{
	return m_SendQueueIdle.GetMsgNum();
}

VOID NetSession::ClearMsgQueue()
{
	static DWORD dwID = m_pUtil->Crc32("NC_GameGuarder");	// 反外挂消息

	m_SendQueueAction.Clear();
	m_SendQueueIdle.Clear();
	
	tagNetCmd* pSendCmd = NULL;
	DWORD dwMsgSize = 0;
	m_SendQueue.ResetIterator();
	while( pSendCmd = (tagNetCmd*)m_SendQueue.PeekMsg(dwMsgSize) )
	{
		if( pSendCmd->dwID != dwID )	// 清除掉反外挂以外所有的消息
			m_SendQueue.RemoveMsg();
		else
			m_SendQueue.IncIterator();
	}
}


// 显示sendQueue的信息
DWORD NetSession::SendQueue(LPCTSTR szCommand)
{
	DWORD dwID = _tstoi(szCommand);
	tagNetCmd* pSendCmd = NULL;
	DWORD dwMsgSize = 0;

	switch(dwID)
	{
	case 1:
		m_SendQueue.ResetIterator();
		while( pSendCmd = (tagNetCmd*)m_SendQueue.PeekMsg(dwMsgSize) )
		{
			IMSG(_T("SendQueue: 0x%X \r\n"), pSendCmd->dwID);
			m_SendQueue.IncIterator();
		}
		break;
	case 2:
		m_SendQueueIdle.ResetIterator();
		while( pSendCmd = (tagNetCmd*)m_SendQueueIdle.PeekMsg(dwMsgSize) )
		{
			IMSG(_T("SendQueueIdle: 0x%X \r\n"), pSendCmd->dwID);
			m_SendQueueIdle.IncIterator();
		}
		break;
	default:
		m_SendQueueAction.ResetIterator();
		while( pSendCmd = (tagNetCmd*)m_SendQueueAction.PeekMsg(dwMsgSize) )
		{
			IMSG(_T("SendQueueAction: 0x%X \r\n"), pSendCmd->dwID);
			m_SendQueueAction.IncIterator();
		}
		break;
	}
	return 0;
}