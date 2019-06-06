//-----------------------------------------------------------------------------
//!\file world_session.cpp
//!\author Aslan
//!
//!\date 2008-06-10
//! last 2008-06-10
//!
//!\brief 区域服务器客户端主框架类
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "world_session.h"
#include "login_session.h"
#include "player_session.h"
#include "world.h"
#include "game_guarder.h"
#include "TimeChecker.h"
#include "..\WorldDefine\selectrole.h"
#include "../WorldDefine/TreasureChest_define.h"

const INT MAX_RECV_WAIT_UNIT_NUM = 64;		// 客户端等待处理的包的最大数量，超过数量将被踢掉
const INT MAX_SEND_CAST_UNIT_NUM = 1000;	// 发送给客户端但还没有收到响应的包数量，超过数量将被踢掉
const INT MAX_SEND_CAST_SIZE = 2*1024*1024;	// 发送给客户端但还没有收到相应的字节数量，超过即被踢掉

//-------------------------------------------------------------------------------
// constructor
//-------------------------------------------------------------------------------
WorldSession::WorldSession(): m_Trunk(this), m_lfPlayerNumFactor(1.0)
{
}

//-------------------------------------------------------------------------------
// destructor
//-------------------------------------------------------------------------------
WorldSession::~WorldSession()
{

}

//-------------------------------------------------------------------------------
// 得到一个session（要注意指针得到后的安全性）
//-------------------------------------------------------------------------------
PlayerSession* WorldSession::FindSession(DWORD dwID)
{
	m_AllSessionMutex.Acquire();
	PlayerSession* pSession = m_mapAllSessions.Peek(dwID);
	m_AllSessionMutex.Release();

	return pSession;
}

//--------------------------------------------------------------------------------
// 得到一个选人界面的session（只能在主线程中调用）
//--------------------------------------------------------------------------------
PlayerSession* WorldSession::FindGlobalSession(DWORD dwID)
{
	m_GlobalSessionMutex.Acquire();
	PlayerSession* pSession = m_mapGlobalSessions.Peek(dwID);
	m_GlobalSessionMutex.Release();

	return pSession;
}

//--------------------------------------------------------------------------------
// 检测一个sessionID是否存在
//--------------------------------------------------------------------------------
BOOL WorldSession::IsSessionExist(DWORD dwID)
{
	m_AllSessionMutex.Acquire();
	BOOL bExist = m_mapAllSessions.IsExist(dwID);
	m_AllSessionMutex.Release();

	return bExist;
}

//--------------------------------------------------------------------------------
// 加入一个新的session
//-------------------------------------------------------------------------------
VOID WorldSession::AddSession(PlayerSession* pSession)
{ 
	if( P_VALID(pSession) )
	{
		m_AllSessionMutex.Acquire();
		m_mapAllSessions.Add(pSession->GetSessionID(), pSession);
		m_AllSessionMutex.Release();
	}
}
//-------------------------------------------------------------------------------
// 删除一个session
//-------------------------------------------------------------------------------
VOID WorldSession::RemoveSession(UINT32 dwID)
{
	m_AllSessionMutex.Acquire();
	m_mapAllSessions.Erase(dwID);
	m_AllSessionMutex.Release();
}

//-------------------------------------------------------------------------------
// 踢掉一个连接
//-------------------------------------------------------------------------------
VOID WorldSession::Kick(DWORD dwInternalIndex)
{
	m_pNetSession->Kick(dwInternalIndex);
}
//-------------------------------------------------------------------------------
// 向选人界面session中加入一个session
//-------------------------------------------------------------------------------
VOID WorldSession::AddGlobalSession(PlayerSession* pSession)
{
	if( P_VALID(pSession) )
	{
		if( m_GlobalSessionMutex.TryAcquire() )
		{
			m_mapGlobalSessions.Add(pSession->GetSessionID(), pSession);
			m_GlobalSessionMutex.Release();
		}
		else
		{
			m_InsertPoolMutex.Acquire();
			m_listInsertPool.PushBack(pSession);
			m_InsertPoolMutex.Release();
		}
	}
}

//-------------------------------------------------------------------------------
// 从选人界面session列表中删除一个session
//-------------------------------------------------------------------------------
VOID WorldSession::RemoveGlobalSession(DWORD dwSessionID)
{
	m_GlobalSessionMutex.Acquire();
	m_mapGlobalSessions.Erase(dwSessionID);
	m_GlobalSessionMutex.Release();
}

//-------------------------------------------------------------------------------
// 初始化
//-------------------------------------------------------------------------------
BOOL WorldSession::Init()
{
	// vEngine对象
	m_pUtil = "Util";
	m_pNetSession = "XServer";

	// 加载配置文件
	TObjRef<VarContainer> pVar = "VarContainer";

	m_nPlayerNumLimit = (INT)pVar->GetDword(_T("player_num_limit"), _T("world"));
	m_nPort = (INT)pVar->GetDword(_T("port"), _T("server"));

	m_nMaxRecvWaitUnitNum	= (INT)pVar->GetDword(_T("max_recv_wait_unit_num"), _T("house_keeping"), 64);		// 客户端等待处理的包的最大数量，超过数量将被踢掉
	m_nMaxSendCastUnitNum	= (INT)pVar->GetDword(_T("max_send_cast_unit_num"), _T("house_keeping"), 1000);		// 发送给客户端但还没有收到响应的包数量，超过数量将被踢掉
	m_nMaxSendCastSize		= (INT)pVar->GetDword(_T("max_send_cast_size"), _T("house_keeping"), 2097152);		// 发送给客户端但还没有收到相应的字节数量，超过即被踢掉

	// 设定网络底层
	tagXServerConfig InitParam;
	InitParam.fnLogIn		=	(XLOGINCALLBACK)m_Trunk.sfp2(&WorldSession::LoginCallBack);
	InitParam.fnLogOut		=	(XLOGOUTCALLBACK)m_Trunk.sfp1(&WorldSession::LogoutCallBack);
	InitParam.bReusePort	=	true;
	InitParam.nPort			=	m_nPort;

	m_pNetSession->Init(InitParam);
	m_nPort = m_pNetSession->GetConfig()->nPort;
	m_nTreasureSum = 0;

	// 注册所有客户端命令
	PlayerSession::RegisterAllPlayerCmd();
	// 注册所有服务器端发送的命令
	PlayerSession::RegisterALLSendCmd();

	return TRUE;
}

//-------------------------------------------------------------------------------
// 更新
//-------------------------------------------------------------------------------
VOID WorldSession::Update()
{
	InterlockedExchange((LPLONG)&m_nMsgSendThisTick, 0);
	InterlockedExchange((LPLONG)&m_nMsgProceedThisTick, 0);
	InterlockedExchange((LPLONG)&m_nMsgRecvWait, 0);

	// 从InsertPool中拿出要加入的session
	m_InsertPoolMutex.Acquire();
	PlayerSession* pSession = m_listInsertPool.PopFront();
	while( P_VALID(pSession) )
	{
		AddGlobalSession(pSession);
		pSession = m_listInsertPool.PopFront();
	}
	m_InsertPoolMutex.Release();

	// 更新选人界面的session
	UpdateSession();
}

//-------------------------------------------------------------------------------
// 更新各个客户端session
//-------------------------------------------------------------------------------
VOID WorldSession::UpdateSession()
{
	// 更新所有选人界面的session
	m_GlobalSessionMutex.Acquire();

	PlayerSession* pSession = NULL;
	m_mapGlobalSessions.ResetIterator();

	while( m_mapGlobalSessions.PeekNext(pSession) )
	{
		if( CON_LOST == pSession->Update())
		{
			// IMSG(_T("player logout in worldsession, sessionid=%u, internalid=%u\r\n"), pSession->GetSessionID(), pSession->GetInternalIndex());

			RemoveSession(pSession->GetSessionID());
			m_mapGlobalSessions.Erase(pSession->GetSessionID());
			g_loginSession.SendPlayerLogout(pSession->GetSessionID());

			PlayerLogout();

			g_gameGuarder.Logout(pSession->GetSessionID(), pSession->GetAccount());
			SAFE_DEL(pSession);
		}
	}

	m_GlobalSessionMutex.Release();
}

//-------------------------------------------------------------------------------
// 做一些网络层的清理工作
//-------------------------------------------------------------------------------
VOID WorldSession::DoHouseKeeping()
{
	CHECK_TIME();

	// 启动底层网络层开始发送
	m_pNetSession->ActiveSend();

	g_world.LockNetWork();

	// 轮询所有session
	m_AllSessionMutex.Acquire();

	PlayerSession* pSession = NULL;
	TMap<DWORD, PlayerSession*>::TMapIterator it = m_mapAllSessions.Begin();

	INT nAllSendCast = 0;
	INT nMaxChokeSize = 0;
	DWORD dwMaxChokeSessionID = GT_INVALID;

	while( m_mapAllSessions.PeekNext(it, pSession) )
	{
		// 检查客户端接受队列的消息数量
		if( pSession->GetMsgWaitNum() >= GetMaxRecvWaitUnitNum() && !pSession->IsKicked() )
		{
			IMSG(_T("Kick Too Fast Player[%u, %d]\r\n"), pSession->GetSessionID(), pSession->GetMsgWaitNum());
			Kick(pSession->GetInternalIndex());
			pSession->SetKicked();
			continue;
		}

		// 检查该客户端未收到确认的包的数量
		INT nCastSize = GetSendCastSize(pSession->GetInternalIndex());
		nAllSendCast += nCastSize;

		if( nCastSize >= GetMaxSendCastSize() && !pSession->IsKicked() )
		{
			IMSG(_T("Kick Choke Player[%u, %u, %d]\r\n"), pSession->GetSessionID(), pSession->GetInternalIndex(), nCastSize);
			Kick(pSession->GetInternalIndex());
			pSession->SetKicked();
		}
		else if( nCastSize > nMaxChokeSize )
		{
			nMaxChokeSize = nCastSize;
			dwMaxChokeSessionID = pSession->GetSessionID();
		}
	}

	// todo: 如果出现内存不足等情况，则踢掉最卡的玩家

	// 设置本Tick的SendCast量
	SetMsgSendCast(nAllSendCast);

	m_AllSessionMutex.Release();

	g_world.UnlockNetWork();
}

//-------------------------------------------------------------------------------
// 销毁
//-------------------------------------------------------------------------------
VOID WorldSession::Destroy()
{
	// 删除所有的session（因为所有线程已经停止，所以这里不需要锁定）
	PlayerSession* pSession = m_listInsertPool.PopFront();
	while( P_VALID(pSession) )
	{
		g_gameGuarder.Logout(pSession->GetSessionID(), pSession->GetAccount());
		SAFE_DEL(pSession);
		pSession = m_listInsertPool.PopFront();
	}

	m_mapGlobalSessions.Clear();
	
	m_mapAllSessions.ResetIterator();
	while( m_mapAllSessions.PeekNext(pSession) )
	{
		g_gameGuarder.Logout(pSession->GetSessionID(), pSession->GetAccount());
		SAFE_DEL(pSession);
	}
	m_mapAllSessions.Clear();

	m_pNetSession->Destroy();

	// 撤销所有注册的网络命令和GM命令
	PlayerSession::UnRegisterALL();
}

//-------------------------------------------------------------------------------
// 登陆回调函数
//-------------------------------------------------------------------------------
DWORD WorldSession::LoginCallBack(LPVOID p1, LPVOID p2)
{
	static DWORD dwJoinGameCmdCrc = m_pUtil->Crc32("NC_JoinGame");

	if( g_world.IsShutingdown() )
		return GT_INVALID;

	// 查看第一条消息
	tagUnit* pUnit = (tagUnit*)p1;
	tagXLoginParam* pParam = (tagXLoginParam*)p2;
	
	tagNC_JoinGame* pCmd = (tagNC_JoinGame*)pUnit->pBuffer;

	if( pCmd->dwID != dwJoinGameCmdCrc )
	{
		return GT_INVALID;
	}

	// 锁住网络
	g_world.LockNetWork();

	// 登陆服务器判断帐号是否合法
	BYTE byPrivilege = 0;
	BOOL bGuard = true;
	DWORD dwAccOLSec = 0;
	char szAccount[X_SHORT_NAME] = {0};

	tagDWORDTime dwPreLoginTime;
	DWORD	dwPreLoginIP=0;
	if( FALSE == g_loginSession.IsAccountValid(pCmd->dwAccountID, pCmd->dwVerifyCode, byPrivilege, bGuard, 
												dwAccOLSec, szAccount,dwPreLoginTime,dwPreLoginIP) )
	{
		g_world.UnlockNetWork();
		return GT_INVALID;
	}

	if( IsSessionExist(pCmd->dwAccountID) )
	{
		g_world.UnlockNetWork();
		return GT_INVALID;
	}

	// 加入一个新的session
	PlayerSession* pSession = new PlayerSession(pCmd->dwAccountID, pParam->dwHandle, pParam->dwAddress, byPrivilege,
												bGuard, dwAccOLSec, szAccount,dwPreLoginTime,dwPreLoginIP);

	// 分别加入到总session和globlesession中
	AddSession(pSession);
	AddGlobalSession(pSession);

	// 设置登陆
	PlayerLogin();

	// 通知登陆服务器帐号登陆成功
	g_loginSession.SendPlayerLogin(pCmd->dwAccountID, pParam->dwAddress, E_Success);

	// 反外挂记录
	g_gameGuarder.Login(pCmd->dwAccountID, szAccount, pParam->dwAddress);

	// 解锁
	g_world.UnlockNetWork();

	return pSession->GetSessionID();
}

//--------------------------------------------------------------------------------------
// 登出回调函数
//--------------------------------------------------------------------------------------
DWORD WorldSession::LogoutCallBack(LPVOID p)
{
	DWORD dwSessionID = (DWORD)p;

	// IMSG(_T("Log out callback, sessionid=%u\r\n"), dwSessionID);

	if( g_world.IsShutingdown() )
		return GT_INVALID;

	// 锁住网络
	g_world.LockNetWork();

	PlayerSession* pSession = FindSession(dwSessionID);

	if( P_VALID(pSession) )
	{
		pSession->SessionLogout();
	}

	// 解锁网络
	g_world.UnlockNetWork();

	return 0;
}

//--------------------------------------------------------------------------------------
// 服务器宝箱开启计数加一
//--------------------------------------------------------------------------------------
VOID WorldSession::IncTreasureSum()
{
	InterlockedExchangeAdd((LPLONG)&m_nTreasureSum, 1);
	if (m_nTreasureSum > SERVER_CHEST_RATE)
		InterlockedExchange((LPLONG)&m_nTreasureSum, 1);
}

//--------------------------------------------------------------------------------------
// 设置服务器宝箱开启计数
//--------------------------------------------------------------------------------------
VOID WorldSession::SetTreasureSum(INT nSum)
{
	if (nSum > SERVER_CHEST_RATE)
		nSum = 1;
	InterlockedExchange((LPLONG)&m_nTreasureSum, nSum);
}

//--------------------------------------------------------------------------------------
// 获取所有在线玩家AccountId
//--------------------------------------------------------------------------------------
INT WorldSession::GetAllOLAccountID( DWORD* pAccountIDs )
{
	std::list<PlayerSession*> listPlayerSessions;
	m_AllSessionMutex.Acquire();

	m_mapAllSessions.ExportAllValue(listPlayerSessions);

	INT i=0;
	std::list<PlayerSession*>::iterator itr = listPlayerSessions.begin();
	while (itr != listPlayerSessions.end())
	{
		if (P_VALID(*itr) && (*itr)->GetFatigueGuarder().IsGuard())
		{
			pAccountIDs[i++] = (*itr)->GetSessionID();
		}
		++itr;
	}

	m_AllSessionMutex.Release();

	return i;
}
WorldSession g_worldSession;