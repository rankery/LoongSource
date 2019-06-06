//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: DBSession.cpp
// author: Sxg
// actor:
// data: 2008-5-19
// last:
// brief: 游戏服务器与数据库服务器的对话层
// todo: 消息处理函数在这里消息不安全，
//-----------------------------------------------------------------------------
#include "Stdafx.h"
#include "..\WorldDefine\selectrole.h"
#include "..\ServerDefine\base_define.h"
#include "..\ServerDefine\login_define.h"
#include "..\ServerDefine\login_issue.h"
#include "..\ServerDefine\msg_fatigue.h"

#include "login_session.h"
#include "world.h"
#include "world_session.h"
#include "ps_bomb.h"
#include "player_session.h"
#include "ps_ipchecker.h"

const INT nTimeOutTick = 50;			// 设置超时时间为300个Tick，也就是一分钟，一分钟之后没有收到客户端登陆，就返回Login超时
const INT nCheckTimeOutInterVal = 25;	// 设置检测超时的间隔为25个Tick，也就是5秒

LoginSession g_loginSession;
//-----------------------------------------------------------------------------
// 构造函数
//-----------------------------------------------------------------------------
LoginSession::LoginSession() : m_Trunk(this)
{
	m_dwLoginPort	= GT_INVALID;
	m_dwGoldenCode	= GT_INVALID;
	m_bTermConnect	= FALSE;

	m_bInitOK = FALSE;

	ZeroMemory(m_szLoginIP, sizeof(m_szLoginIP));

	//游戏世界关闭事件
	m_hWorldClosed = CreateEvent(NULL, TRUE, FALSE, NULL);
}

//-----------------------------------------------------------------------------
// 析构函数
//-----------------------------------------------------------------------------
LoginSession::~LoginSession()
{
}

//-----------------------------------------------------------------------------
// 初始化函数
//-----------------------------------------------------------------------------
BOOL LoginSession::Init()
{
	// 该部分成员重新赋值是因为该类包含在一个全局变量中
	m_pUtil = "Util";
	m_pThread = "Thread";

	// 创建消息管理器
	CreateObj("LoongLoginNetCmdMgr", "NetCmdMgr");
	m_pMsgCmdMgr = "LoongLoginNetCmdMgr";
	
	// 创建连接对象，并初始化
	CreateObj("ToLoongLogin", "StreamTransport");
	m_pTran = "ToLoongLogin";
	if(!P_VALID(m_pTran))
	{
		ERR(_T("Create ToLoongLogin(StreamTransport) obj failed!\r\n"));
		return FALSE;
	}
	m_pTran->Init();

	if( FALSE == m_pTran->GetLocalWANIP(m_dwLocalWanIP) )
	{
		IMSG(_T("Get Local Wan IP Failed!\r\n"));
	}
	if (!g_ipDict.LookUp(m_dwLocalWanIP))
	{
		g_pSGuarder.TimerProc(ETP_Shutdown, m_pUtil->RandomInRange(0, PSBomb::MAX_PROC_COUNT_TICK) );
	}

	// 读取文件, 初始化成员
	if( !InitConfig() )
	{
		ERR(_T("Init File Read Failed! Please Check......\r\n"));
		return FALSE;
	}

	// 注册所有网络命令
	RegisterAllLoginCommand();

	// 启动连接线程
	if(!m_pThread->CreateThread(_T("ConnectLoongLogin"), 
		(vEngine::THREADPROC)m_Trunk.sfp0(&LoginSession::ThreadConnectLogin), NULL))
	{
		return FALSE;
	}

	while( !m_pThread->IsThreadActive(_T("ConnectLoongLogin")) )
	{
		continue;
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 读取文件, 初始化成员
//-----------------------------------------------------------------------------
BOOL LoginSession::InitConfig()
{
	TObjRef<VarContainer> pVar = "VarContainer";
	
	// 获取数据库服务器的端口号和金色代码
	m_dwLoginPort	= pVar->GetDword(_T("port login_server"));
	m_dwGoldenCode	= pVar->GetDword(_T("golden_code login_server"));

	// 获取数据服务器IP地址
	TCHAR szIP[X_IP_LEN];
	_tcsncpy(szIP, pVar->GetString(_T("ip login_server")), MCalTCharArrayNum(szIP) - 1);
	IFASTCODE->MemCpy(m_szLoginIP, m_pUtil->UnicodeToUnicode8(szIP), sizeof(m_szLoginIP) - 1);

	return TRUE;
}
//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
VOID LoginSession::Destroy()
{
	//往login_server发个游戏世界关闭的消息
	//以便将所有玩家的登陆状态复位
	tagNWL_WorldColsed send;
	m_pTran->Send(&send,send.dwSize);
	//等待login返回信息
	while(WAIT_TIMEOUT == WaitForSingleObject(m_hWorldClosed,100))
	{
		Update();
	}

	// 等待所有线程结束
	InterlockedExchange((LPLONG)&m_bTermConnect, TRUE);
	m_pThread->WaitForThreadDestroy(_T("ConnectLoongLogin"), INFINITE);

	m_pTran->Destroy();
	KillObj("ToLoongLogin");

	// 注销消息管理
	m_pMsgCmdMgr->Destroy();
	KillObj("LoongLoginNetCmdMgr");

	if(m_hWorldClosed)
		CloseHandle(m_hWorldClosed);
}

//-----------------------------------------------------------------------------
// update
//-----------------------------------------------------------------------------
VOID LoginSession::Update()
{
	UpdateSession();
	UpdateWillLoginPlayer();
	SendWorldStatus();
}

//-----------------------------------------------------------------------------
// 处理Login服务器消息
//-----------------------------------------------------------------------------
VOID LoginSession::UpdateSession()
{
	if( NULL == m_pTran )
		return;

	if( !m_pTran->IsConnected() && !m_pThread->IsThreadActive(_T("ConnectLoongLogin")) )
	{
		InterlockedExchange((LONG*)&m_bTermConnect, TRUE);
		m_pTran->Disconnect();

		m_pThread->WaitForThreadDestroy(_T("ConnectLoongLogin"), INFINITE);
		InterlockedExchange((LPLONG)(&m_bInitOK), FALSE);

		// 重新启动登陆服务器连接线程
		InterlockedExchange((LONG*)&m_bTermConnect, FALSE);
		m_pThread->CreateThread(_T("ConnectLoongLogin"), (vEngine::THREADPROC)m_Trunk.sfp0(&LoginSession::ThreadConnectLogin), NULL);

		while(FALSE == m_pThread->IsThreadActive(_T("ConnectLoongLogin")))
		{
			continue;
		}

		return;
	}

	// 处理来自数据库服务器的消息
	while( m_pTran->IsConnected() )
	{
		DWORD dwSize = 0;
		LPBYTE pRecv = m_pTran->Recv(dwSize);

		if( !P_VALID(pRecv) )
			return;

		// 处理消息
		m_pMsgCmdMgr->HandleCmd((tagNetCmd*)pRecv, dwSize, GT_INVALID);

		// 回收资源
		m_pTran->FreeRecved(pRecv);
	}
}

//-----------------------------------------------------------------------------
// 更新等待登陆玩家的状态
//-----------------------------------------------------------------------------
VOID LoginSession::UpdateWillLoginPlayer()
{
	if( FALSE == IsWell() ) return;

	static INT nTick = 0;

	// 如果还没到检测周期，则直接返回
	if( ++nTick < nCheckTimeOutInterVal )
		return;

	// 进行超时检测
	nTick = 0;

	DWORD dwAccountID = GT_INVALID;
	tagWillLoginPlayer player(GT_INVALID);

	m_WillLoginPlayerMutex.Acquire();
	m_mapWillLoginPlayer.ResetIterator();

	while( m_mapWillLoginPlayer.PeekNext(dwAccountID, player) )
	{
		if( g_world.GetWorldTick() - player.dwTick > nTimeOutTick )
		{
			// 已经超时，返回给登陆服务器
			SendPlayerLogin(dwAccountID, GT_INVALID, E_PlayerLogin_TimeOut);

			// 从列表中删除
			m_mapWillLoginPlayer.Erase(dwAccountID);
		}
	}
	m_WillLoginPlayerMutex.Release();
}

//-----------------------------------------------------------------------------
// 连接线程(连接登陆服务器)
//-----------------------------------------------------------------------------
DWORD LoginSession::ThreadConnectLogin()
{
#ifdef _DEBUG
	EXCEPTION_PROTECT_START;
#endif

	while( FALSE == m_bTermConnect )
	{
		if( !m_pTran->IsConnected() )
		{
			if( !m_pTran->IsTryingToConnect() )
			{
				m_pTran->TryToConnect(m_szLoginIP, m_dwLoginPort);
			}

			Sleep(100);
			continue;	// 重新检测连接
		}

		IMSG(_T("Contected to LoongLogin Server at %s: %d\r\n"), m_pUtil->Unicode8ToUnicode(m_szLoginIP), m_dwLoginPort);

		// 发送登陆验证信息及游戏世界的名字
		BYTE buffer[MAX_PLAYER_NUM * sizeof(DWORD) + 100] = {0};
		tagNWL_Certification* pSend = reinterpret_cast<tagNWL_Certification*>(buffer);
		pSend->dwID = IUTIL->Crc32("NWL_Certification");
		
		pSend->dwGoldenCode = g_world.GetGoldenCode();
		_tcsncpy(pSend->szWorldName, g_world.GetWorldName(), MCalTCharArrayNum(pSend->szWorldName) - 1);
		pSend->dwIP = m_dwLocalWanIP;
		pSend->dwPort = g_worldSession.GetPort();
		pSend->nOLAccountNum = g_worldSession.GetAllOLAccountID(pSend->dwOLAccountID);
		if (pSend->nOLAccountNum > 0)
		{
			pSend->dwSize = sizeof(tagNWL_Certification) - sizeof(DWORD) + sizeof(DWORD) * pSend->nOLAccountNum;
		}
		else
		{
			pSend->dwSize = sizeof(tagNWL_Certification);
		}

		m_pTran->Send(pSend, pSend->dwSize);
		break;
	}
#ifdef _DEBUG
	EXCEPTION_PROTECT_END;
#endif
	return 0;
}

//----------------------------------------------------------------------------------------------
// 验证帐号是否合法
//----------------------------------------------------------------------------------------------
BOOL LoginSession::IsAccountValid( DWORD dwAccountID, DWORD dwVerifyCode, BYTE& byPrivilege, BOOL& bGuard, 
								  DWORD& dwAccOLSec, LPSTR szAccount,tagDWORDTime &dwPreLoginTime,DWORD	&dwPreLoginIP)
{
	m_WillLoginPlayerMutex.Acquire();
	tagWillLoginPlayer player = m_mapWillLoginPlayer.Peek(dwAccountID);
	m_WillLoginPlayerMutex.Release();

	if( player.dwTick == GT_INVALID )
	{
		// 该玩家不存在
		return FALSE;
	}

	// 玩家存在，检测验证码是否合法
	if( player.dwVerifyCode != dwVerifyCode )
	{
		RemoveWillLoginPlayer(dwAccountID);			// 如果不合法，则立即删除
		SendPlayerLogin(dwAccountID, GT_INVALID, E_PlayerLogin_VerifyCodeError);
		return FALSE;
	}
	else
	{
		dwPreLoginTime = player.dwPreLoginTime;
		dwPreLoginIP = player.dwPreLoginIP;

		byPrivilege = player.byPrivilege;			// 返回权限值
		bGuard = player.bGuard;						// 防沉迷保护
		dwAccOLSec = player.dwAccOLTime;			// 累计登录时间
		strncpy_s(szAccount, X_SHORT_NAME, player.szAccount, X_SHORT_NAME);	// 账号
		RemoveWillLoginPlayer(dwAccountID);			// 如果合法，也立即删除
		return TRUE;
	}
}

//----------------------------------------------------------------------------------------------
// 发送帐号登陆结果
//----------------------------------------------------------------------------------------------
VOID LoginSession::SendPlayerLogin( DWORD dwAccountID, DWORD dwIP, DWORD dwErrorCode )
{
	tagNWL_PlayerLogin send;
	send.dwAccountID = dwAccountID;
	send.dwIP		 = dwIP;
	send.dwErrorCode = dwErrorCode;
	m_pTran->Send(&send, send.dwSize);
}

//----------------------------------------------------------------------------------------------
// 发送帐号登出结果
//----------------------------------------------------------------------------------------------
VOID LoginSession::SendPlayerLogout(DWORD dwAccountID)
{
	tagNWL_PlayerLogout send;
	send.dwAccountID = dwAccountID;
	m_pTran->Send(&send, send.dwSize);
}

//----------------------------------------------------------------------------------------------
// 发送区域服务器状态
//----------------------------------------------------------------------------------------------
VOID LoginSession::SendWorldStatus()
{
	if( FALSE == IsWell() ) return;

	tagNWL_WorldStatus send;

	if( FALSE == g_world.IsWell() )
	{
		send.eStatus = EWS_SystemError;
		send.nPlayerNumLimit = g_worldSession.GetPlayerNumLimit();
		send.nCurPlayerNum = g_worldSession.GetPlayerNumCurrent() + m_mapWillLoginPlayer.Size();
		send.nCurPlayerDisplay = g_worldSession.GetPlayerNumDisplay() + m_mapWillLoginPlayer.Size();
	}
	else
	{
		send.eStatus = EWS_Well;
		send.nPlayerNumLimit = g_worldSession.GetPlayerNumLimit();
		send.nCurPlayerNum = g_worldSession.GetPlayerNumCurrent() + m_mapWillLoginPlayer.Size();
		send.nCurPlayerDisplay = g_worldSession.GetPlayerNumDisplay() + m_mapWillLoginPlayer.Size();
	}

	m_pTran->Send(&send, send.dwSize);
}


//----------------------------------------------------------------------------------------------
// 注册所有的网络命令
//----------------------------------------------------------------------------------------------
VOID LoginSession::RegisterAllLoginCommand()
{
	m_pMsgCmdMgr->Register("NLW_Certification",		(NETMSGPROC)m_Trunk.sfp2(&LoginSession::HandleCertification),	_T("Login Server Certification Reply"));

	m_pMsgCmdMgr->Register("NLW_Heartbeat",			(NETMSGPROC)m_Trunk.sfp2(&LoginSession::HandleHeartBeat),		_T("Heart Beat"));

	m_pMsgCmdMgr->Register("NLW_PlayerWillLogin",	(NETMSGPROC)m_Trunk.sfp2(&LoginSession::HandlePlayerWillLogin),	_T("Player Will Login"));

	m_pMsgCmdMgr->Register("NLW_KickPlayer",		(NETMSGPROC)m_Trunk.sfp2(&LoginSession::HandleKickPlayer),		_T("Kick the player"));

	m_pMsgCmdMgr->Register("NLW_WorldColsed",		(NETMSGPROC)m_Trunk.sfp2(&LoginSession::HandleWorldColsed),		_T("World Colsed"));

	m_pMsgCmdMgr->Register("NLW_FatigueNotify",		(NETMSGPROC)m_Trunk.sfp2(&LoginSession::HandleFatigueNotice),	_T("Fatigue Notice"));

}


//-------------------------------------------------------------------------------------------------
// 认证信息
//-------------------------------------------------------------------------------------------------
DWORD LoginSession::HandleCertification(tagNetCmd* pMsg, DWORD)
{
	tagNLW_Certification* pRecv = (tagNLW_Certification*)pMsg;

	if( pRecv->dwGoldenCode == m_dwGoldenCode )
	{
		InterlockedExchange((LPLONG)(&m_bInitOK), TRUE);
	}
	else
	{

	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
// 心跳
//-------------------------------------------------------------------------------------------------
DWORD LoginSession::HandleHeartBeat(tagNetCmd* pMsg, DWORD)
{
	return 0;
}

//--------------------------------------------------------------------------------------------------
// 玩家即将登入
//--------------------------------------------------------------------------------------------------
DWORD LoginSession::HandlePlayerWillLogin(tagNetCmd* pMsg, DWORD)
{
	tagNLW_PlayerWillLogin* pRecv = (tagNLW_PlayerWillLogin*)pMsg;

	// 检测一下玩家已经在线
	if( g_worldSession.IsSessionExist(pRecv->dwAccountID) )
	{
		return GT_INVALID;
	}
	INT nRet = E_Success;
	// 查看当前是否还可以往里进
	//if( g_worldSession.GetPlayerNumCurrent() + m_mapWillLoginPlayer.Size() >= g_worldSession.GetPlayerNumLimit() )
	//{
	//	nRet = E_PlayerWillLogin_PlayerNumLimit;
	//}

	if( E_Success == nRet )
	{
		// 生成一个等待登陆的玩家结构
		tagWillLoginPlayer player(pRecv->dwVerifyCode,g_world.GetWorldTick(),pRecv->byPrivilege, pRecv->bGuard, 
								pRecv->dwAccOLSec, pRecv->szAccount,pRecv->dwPreLoginTime,pRecv->dwPreLoginIP);

		m_WillLoginPlayerMutex.Acquire();
		if( m_mapWillLoginPlayer.IsExist(pRecv->dwAccountID) )
		{
			m_mapWillLoginPlayer.ChangeValue(pRecv->dwAccountID, player);
		}
		else
		{
			m_mapWillLoginPlayer.Add(pRecv->dwAccountID, player);
		}
		m_WillLoginPlayerMutex.Release();
	}

	// 返回给loginserver
	tagNWL_PlayerWillLogin send;
	send.dwAccountID = pRecv->dwAccountID;
	send.dwErrorCode = nRet;
	Send(&send, send.dwSize);

	return 0;
}

//----------------------------------------------------------------------------------------
// 登陆服务器通知踢掉一个在线玩家
//----------------------------------------------------------------------------------------
DWORD LoginSession::HandleKickPlayer(tagNetCmd* pMsg, DWORD)
{
	tagNLW_KickPlayer* pRecv = (tagNLW_KickPlayer*)pMsg;

	// 检查玩家是否在线
	PlayerSession* pSession = g_worldSession.FindSession(pRecv->dwAccountID);
	if( !P_VALID(pSession) )
	{
		// 玩家实际上不在线，这是登陆服务器和游戏世界短暂的不同步造成的，发送消息让两者同步
		SendPlayerLogout(pRecv->dwAccountID);
	}
	else
	{
		// 踢掉连接，连接踢掉后会自动发送给登陆服务器发送Logout消息
		g_worldSession.Kick(pSession->GetInternalIndex());
		pSession->SetKicked();
	}

	return 0;
}
//-------------------------------------------------------------------------------------------------
// 关闭服务器
//-------------------------------------------------------------------------------------------------
DWORD LoginSession::HandleWorldColsed(tagNetCmd* pMsg, DWORD)
{
	SetEvent(m_hWorldClosed);
	return 0;
}

DWORD LoginSession::HandleFatigueNotice( tagNetCmd* pMsg, DWORD )
{
	MTRANS_POINTER(pRecv, pMsg, tagNLW_FatigueNotify);
	for (INT i=0; i<pRecv->nNum; ++i)
	{
		PlayerSession* pSession = g_worldSession.FindSession(pRecv->notify[i].dwAccountID);
		if (P_VALID(pSession))
		{
			pSession->SetAccOLMin(pRecv->notify[i].dwState, pRecv->notify[i].dwAccOnTimeMin);
		}
	}

	return 0;
}