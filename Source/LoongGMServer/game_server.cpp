//-----------------------------------------------------------------------------
//!\file game_server.h
//!\brief 服务器基类，LoginServer，LoongWorld和LoongDB均从该基类继承
//!
//!\date 2009-04-20
//! last 2009-04-20
//!\author zhangrong
//!
//! Copyright (c) 1985-2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"

#include "../ServerDefine/msg_rt_s.h"

#include "game_server.h"
#include "section_mgr.h"


//-----------------------------------------------------------------------------
// GameServer基类——CONSTRUCT
//-----------------------------------------------------------------------------
GameServer::GameServer(DWORD dwID, LPCSTR szIP) : m_dwID(dwID), m_bValid(FALSE), m_pTran(NULL),
												  m_eStatus(EWS_InitNotDone), 
												  m_nPlayerNum(0),
												  m_nPlayerNumDisplay(0),
												  m_nMaxPlayerNum(0),
												  m_bTryingConnect(FALSE), m_szIP(szIP)
{
	//m_dwLastestTick = GetCurrentDWORDTime();

	// 生成连接远端进程的连接
	m_pTran = new StreamTransport;
	m_pTran->Init();

	TObjRef<VarContainer> pVar = "VarContainer";
	m_nPort = pVar->GetInt(_T("port rt_service"));

	m_pTran->TryToConnect(m_szIP.c_str(), m_nPort);
	m_bTryingConnect = TRUE;
}

GameServer::~GameServer()
{
	if( P_VALID(m_pTran) )
	{
		m_pTran->Destroy();
		SAFE_DEL(m_pTran);
	}
}

//-----------------------------------------------------------------------------
// GameServer基类——更新
//-----------------------------------------------------------------------------
VOID GameServer::Update()
{
	UpdateConnection();
	UpdateSession();
}
//-----------------------------------------------------------------------------
// GameServer基类——更新连接
//-----------------------------------------------------------------------------
VOID GameServer::UpdateConnection()
{
	// 当还是连接的状态下
	if( m_pTran->IsConnected() )
	{
		// 如果是刚连接上，则发送第一条消息
		if( m_bTryingConnect )
		{
			m_bTryingConnect = FALSE;

			// 发送第一条消息
			tagNLSS_LoginService send;
			send.eType = m_eType;
			m_pTran->Send(&send, sizeof(tagNLSS_LoginService));
		}
		
		//if(m_eStatus == EWS_InitNotDone)
		//{	
		//	tagDWORDTime dwNow;
		//	dwNow = GetCurrentDWORDTime();
		//	if (IncreaseTime( m_dwLastestTick, 60 * MAX_WAIT_FOR_TICK_M ) <= dwNow)
		//		// 如果已有1分钟没有收到该server的心跳，但连接还在，那么给认为该server故障
		//		ChangeStatus(EWS_SystemError);
		//	else
		//		ChangeStatus(EWS_Well);
		//}

		if (m_eStatus == EWS_Well)
		{
			tagDWORDTime dwNow;
			dwNow = GetCurrentDWORDTime();
			
			if (IncreaseTime( m_dwLastestTick, 10 * MAX_WAIT_FOR_TICK_M ) <= dwNow)
				// 如果已有10秒钟没有收到该server的心跳，但连接还在，那么给认为该server故障
				ChangeStatus(EWS_SystemError);
		}

		// todo接收发送来的消息
	}
	// 当前是没有连接的状态
	else
	{
		tagDWORDTime dwNow;
		dwNow = GetCurrentDWORDTime();
		
		// 如果已有10秒钟没有收到该server的心跳，并且连接已经断了，那么给认为该server处于未初始化状态
		if (IncreaseTime( m_dwLastestTick, 10 * MAX_WAIT_FOR_TICK_M ) <= dwNow)
			ChangeStatus(EWS_InitNotDone);

		// 如果当前正在连接，则一直尝试连接
		if( m_bTryingConnect )
		{
			if( !m_pTran->IsTryingToConnect() )
			{
				m_pTran->TryToConnect(m_szIP.c_str(), m_nPort);
			}
		}
		// 可能是中途断掉了，则开启重连
		else
		{
			m_pTran->TryToConnect(m_szIP.c_str(), m_nPort);
			m_bTryingConnect = TRUE;
		}
	}
}
//-----------------------------------------------------------------------------
// GameServer基类——更新网络消息
//-----------------------------------------------------------------------------
VOID GameServer::UpdateSession()
{
	LPBYTE pMsg = NULL;
	DWORD dwSize = 0;
	INT nUnRecved = 0;

	pMsg = g_SectionMgr.RecvMsg(m_dwID, dwSize, nUnRecved);

	while( P_VALID(pMsg) )
	{
		if ( m_eStatus != EWS_Well)
			m_eStatus = EWS_Well;

		g_SectionMgr.HandleCmd(pMsg, dwSize, m_dwID);
		g_SectionMgr.ReturnMsg(m_dwID, pMsg);

		pMsg = g_SectionMgr.RecvMsg(m_dwID, dwSize, nUnRecved);
		//return;
	}
}
//-----------------------------------------------------------------------------
// GameServer基类——服务器开启回调
//-----------------------------------------------------------------------------
BOOL GameServer::ServerStart()
{
	if( IsValid() ) return FALSE;

	m_bValid = TRUE;
	ChangeStatus(EWS_InitNotDone);
	m_nPlayerNum	= 0;
	m_nMaxPlayerNum	= 0;
	m_nPlayerNumDisplay = 0;

	return TRUE;
}
//-----------------------------------------------------------------------------
// GameServer基类——服务器关闭回调
//-----------------------------------------------------------------------------
BOOL GameServer::ServerShutDown()
{
	m_bValid		= FALSE;
	ChangeStatus(EWS_InitNotDone);
	m_nPlayerNum	= 0;
	m_nMaxPlayerNum	= 0;
	m_nPlayerNumDisplay = 0;
	return TRUE;
}

//-----------------------------------------------------------------------------
// GameServer基类——刷新状态
//-----------------------------------------------------------------------------
VOID GameServer::RefreshStatus(EWorldStatus eStatus, INT nOnlineNum, INT nMaxOnlineNum, INT nOnlineNumDisplay)
{
	m_nPlayerNum = nOnlineNum;
	m_nMaxPlayerNum = nMaxOnlineNum;
	m_nPlayerNumDisplay = nOnlineNumDisplay;
	ChangeStatus(eStatus);
}

VOID GameServer::ChangeStatus(EWorldStatus eStatus)
{
	if (m_eStatus != eStatus)
	{
		g_SectionMgr.SendDynamicInfo();
	}
	m_eStatus = eStatus;
}

//-----------------------------------------------------------------------------
// GameServer基类——开启服务器
//-----------------------------------------------------------------------------
BOOL GameServer::StartServer()
{
	if( !m_pTran->IsConnected() ) return FALSE;

	ChangeStatus(EWS_InitNotDone);
	m_nPlayerNum	= 0;
	m_nMaxPlayerNum	= 0;
	m_nPlayerNumDisplay = 0;

	// 发送开服的消息
	tagNLSS_Start send;
	m_pTran->Send(&send, sizeof(tagNLSS_Start));

	return TRUE;
}
//-----------------------------------------------------------------------------
// GameServer基类——关闭服务器
//-----------------------------------------------------------------------------
BOOL GameServer::ShutdownServer()
{
	if( !m_pTran->IsConnected() ) return FALSE;

	ChangeStatus(EWS_InitNotDone);
	m_nPlayerNum	= 0;
	m_nMaxPlayerNum	= 0;
	m_nPlayerNumDisplay = 0;

	// 发送强制关服务器的消息
	tagNLSS_Close send;
	m_pTran->Send(&send, sizeof(tagNLSS_Close));

	return TRUE;
}

//-----------------------------------------------------------------------------
// GameServer基类——执行系统命令
//-----------------------------------------------------------------------------
BOOL GameServer::ExecuteCmd(tagNetCmd* pMsg)
{
	// 发送记录有系统命令的消息
	m_pTran->Send((tagNLSS_SystemCmd*)pMsg, sizeof(tagNLSS_SystemCmd));

	return TRUE;
}

//--------------------------------------------------------------------------------
// GameServerLogin——CONSTRUCT
//--------------------------------------------------------------------------------
GameServerLogin::GameServerLogin(DWORD dwID, LPCSTR szIP)
: GameServer(dwID, szIP)
{
	m_eType = EST_Login;
}

//--------------------------------------------------------------------------------
// GameServerWorld——CONSTRUCT
//--------------------------------------------------------------------------------
GameServerWorld::GameServerWorld(DWORD dwID, LPCSTR szIP)
: GameServer(dwID, szIP)
{
	m_eType = EST_World;
}

DWORD GameServerWorld::SetDoubleRate( DWORD dwLastTime, DWORD dwOpenTime, DWORD dwRatio, DWORD dwDType )
{
	if(!P_VALID(m_pTran))		return E_GMTool_Err;

	tagNSS_Double send;	
	send.dwLastTime	= dwLastTime;
	send.dwOpenTime	= dwOpenTime;
	send.dwRatio	= dwRatio;
	send.eDType		= (EDoubleType)dwDType;

	g_SectionMgr.SendMsg(GetID(), &send, send.dwSize);
	
	return E_GMTool_Success;
}
//--------------------------------------------------------------------------------
// GameServerDB——CONSTRUCT
//--------------------------------------------------------------------------------
GameServerDB::GameServerDB(DWORD dwID, LPCSTR szIP)
: GameServer(dwID, szIP)
{
	m_eType = EST_DB;
}