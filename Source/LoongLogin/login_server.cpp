//-----------------------------------------------------------------------------
//!\file login_server.cpp
//!\author Aslan
//!
//!\date 2008-11-30
//! last 2008-11-30
//!
//!\brief LoginServer程序入口
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"

#include "../WorldDefine/svn_revision.h"	//这个文件是自动生成,如果没有请重新编译WorldDefine

#include "login_server.h"
#include "loongworld_mgr.h"
#include "player_mgr.h"
#include "db.h"
#include "proof_policy_mgr.h"
#include "rt_session.h"
#include "fatigue_mgr.h"
#include "fatigue_session.h"
#include "fatigue_server.h"

//------------------------------------------------------------------------------
// CONSTANT
//------------------------------------------------------------------------------
#define CONFIG_INI  "server_config\\login\\login"

//------------------------------------------------------------------------------
// CONSTRUCT
//------------------------------------------------------------------------------
LoginServer::LoginServer() : m_Trunk(this), m_bTerminateUpdate(FALSE), m_nCpuNum(0), m_dwTotalPhys(0), m_dwAvailPhys(0), m_dwTotalRunMinute(0),
							m_dwIdleTime(0), m_dwTick(0)
{
}

LoginServer::~LoginServer()
{
}

//-------------------------------------------------------------------------------
// 初始化
//-------------------------------------------------------------------------------
BOOL LoginServer::Init(HINSTANCE hInst)
{
	m_pLog		=	"Log";
	m_pThread	=	"Thread";
	m_pRender	=	"GDIRender";
	m_pWindow	=	"WindowWrap";
	m_pGUI		=	"GUISystem";
	m_pConsole	=	"Console";
	m_pUtil		=	"Util";

	// 生成Log
	m_pLog->Create();
	
	// 读入配置文件
	CreateObj("LoginServerConfig", "VarContainer");
	TObjRef<VarContainer> pVar = "LoginServerConfig";
	if( !P_VALID(pVar) )
	{
		return FALSE;
	}


	//	初始化随机数种子
	m_pUtil->Randomize(timeGetTime() * 54321);


	TCHAR tszPath[MAX_PATH];
	ZeroMemory(tszPath, sizeof(tszPath));
	if (!m_pUtil->GetIniPath(tszPath, _T(CONFIG_INI))||
		!pVar->Load("VirtualFileSys", tszPath))
	{
		ERR(_T("配置文件未找到"));
		return FALSE;
	}
	// 加载配置文件
	CreateObj("GMAccess", "VarContainer");
	TObjRef<VarContainer> pGmVar = "GMAccess";
	if( !P_VALID(pGmVar) ) return FALSE;
	if(!pGmVar->Load("VirtualFileSys", _T("server_config/login/gm_access.ini")))
	{
		ERR(_T("配置文件未找到"));
		return FALSE;
	}

	// 读取ID
	m_dwID			=	pVar->GetDword(_T("id section"));

	// 读取窗口大小
	LONG lWidth		=	pVar->GetDword(_T("width display"));
	LONG lHeight	=	pVar->GetDword(_T("height display"));
	
	// 创建GUI渲染
	m_pGUIRender = new IGUIRenderNative;
	if( !P_VALID(m_pGUIRender))	return FALSE;
	m_pGUIRender->Init(m_pRender);

	// 初始化窗口系统
	m_pWindow->Init(m_pGUIRender, lWidth, lHeight, TRUE);
	m_pWindow->CreateWnd(_T("LoginServer"), hInst);

	// 初始化GUIRender
	m_pRender->Init(m_pWindow->GetWidth(), m_pWindow->GetHeight());
	
	// 初始化GUISystem
	m_pGUI->Init(m_pWindow->GetWidth(), m_pWindow->GetHeight(), m_pGUIRender);
	m_pRender->SetShowFPS(TRUE);
	
	// 创建显示其他信息的控制台
	m_pConsole->Init(NULL, 1000, TRUE);

	// 系统参数
	m_nCpuNum = TObjRef<SysInfo>()->GetProcessorNum();
	m_dwTotalRunMinute = 0;

	// 防沉迷配置
	m_bSimFatigueServer = (BOOL)pVar->GetDword(_T("sim_server fatigue_guard"));
	m_bEnableFatigueGuard = (BOOL)pVar->GetDword(_T("guard_switch fatigue_guard"));

	// 生成各个单件
	new ProofPolicyMgr;
	new PlayerMgr;
	new LoongWorldMgr;
	new DB;
	new RTSession;

	// 初始化数据库
	if( FALSE == sBeton.Init() )
	{
		ILOG->Write(_T("DataBase  Init Failure\r\n"));
		return FALSE;
	}

	if(IsSimFatigueServer())
	{
		new FatigueServer;
		if (!sFatigueServer.Init())
		{
			MessageBox(NULL, _T("FagitueServer初始化失败"), _T("LoginServer"), MB_OK);
			ILOG->Write(_T("FagitueServer Init Failure\r\n"));
			return FALSE;
		}
	}

	if (IsEnableFatigueGuard())
	{
		new FatigueSession;
		if (!sFatigueSession.Init())
		{
			MessageBox(NULL, _T("FagitueSession初始化失败"), _T("LoginServer"), MB_OK);
			ILOG->Write(_T("Fatigue_Session Init Failure\r\n"));
			return FALSE;
		}

		new FatigueMgr;
		if (!sFatigueMgr.Init())
		{
			MessageBox(NULL, _T("FagitueMgr初始化失败"), _T("LoginServer"), MB_OK);
			ILOG->Write(_T("FagitueMgr Init Failure\r\n"));
			return FALSE;
		}
	}

	// 初始化ProofPolicyMgr
	if( FALSE == sProofPolicyMgr.Init() )
	{
		ILOG->Write(_T("ProofPolicyMgr Init Failure\r\n"));
		return FALSE;
	}

	// 初始化LoongWorldMgr
	if( FALSE == sLoongWorldMgr.Init() )
	{
		ILOG->Write(_T("LoongWorldMgr Init Failure\r\n"));
		return FALSE;
	}

	// 初始化PlayerMgr
	if( FALSE == sPlayerMgr.Init() )
	{
		ILOG->Write(_T("PlayerMgr Init Failure !\r\n"));
		return FALSE;
	}

	if (sRTSession.Init() == FALSE)
	{
		MessageBox(NULL, _T("监控Session初始化失败"), _T("LoginServer"), MB_OK);
		ILOG->Write(_T("RT_Session Init Failure\r\n"));
		return FALSE;
	}

	// 重置DB
	sBeton.ResetAllPlayerLoginStatus(EPLS_Loging, EPLS_Unknown);
	sBeton.ResetAllPlayerLoginStatus(EPLS_Online, EPLS_Unknown);


	// 初创建初始化线程
	if( FALSE == m_pThread->CreateThread( _T("thread_update"), 
				(THREADPROC)m_Trunk.sfp1(&LoginServer::ThreadUpdate), NULL) )
		return FALSE;
	
	return TRUE;
}

//---------------------------------------------------------------------------------
// 销毁
//---------------------------------------------------------------------------------
VOID LoginServer::Destroy()
{
	// 关闭Update线程
	InterlockedExchange((LPLONG)&m_bTerminateUpdate, TRUE );
	m_pThread->WaitForThreadDestroy(_T("thread_update"), INFINITE);
	
	// 删除单件指针
	delete sPlayerMgr.getSingletonPtr();
	delete sLoongWorldMgr.getSingletonPtr();
	delete sBeton.getSingletonPtr();
	delete sProofPolicyMgr.getSingletonPtr();
	delete sRTSession.getSingletonPtr();

	// 删除配置文件
	KillObj("LoginServerConfig");
	// 删除配置文件对象
	KillObj("GMAccess");

	// 关闭窗口
	m_pWindow->Destroy();
	m_pGUI->Destroy();
	m_pLog->Close();

	SAFE_DEL(m_pGUIRender);
}

//-----------------------------------------------------------------------------------
// 主循环
//-----------------------------------------------------------------------------------
VOID LoginServer::Mainloop()
{
	DWORD dwMsg, dwParam1, dwParam2;
	static DWORD dwTimeKeepr = timeGetTime();

	while ( FALSE == m_pWindow->MsgLoop() && FALSE == m_bTerminateUpdate )
	{
		if( FALSE == m_pWindow->IsWindowActive() )
		{
			Sleep(30);
			continue;
		}

		while( m_pWindow->PeekWindowsMsg( dwMsg, dwParam1, dwParam2 ) )
		{
			m_pGUI->OnWindowsMsg( dwMsg, dwParam1, dwParam2 );
			if( dwMsg == WM_QUIT )
			{
				return;
			}
		}
		
		m_pGUI->Update();

		// render
		m_pRender->Clear( 0, 0, 0 );
		m_pGUI->Render();
		m_pRender->Render();

		// 更新内存使用
		UpdateMemoryUsage();

		m_dwTotalRunMinute = timeGetTime() - dwTimeKeepr;
		INT nHour = m_dwTotalRunMinute / 3600000;
		INT nMin = (m_dwTotalRunMinute % 3600000) / 60000;
		INT nSec = ((m_dwTotalRunMinute % 3600000) % 60000) / 1000;

		// 加入要观察的实时变量
		m_pConsole->Watch(_T("Beton"),					BETON_BUILD_REVISION);
		m_pConsole->Watch(_T("Login"),					LOONGLOGIN_BUILD_REVISION);
		m_pConsole->Watch(_T("Lua"),					LUA_BUILD_REVISION);
		m_pConsole->Watch(_T("ServerDefine"),			SERVERDEFINE_BUILD_REVISION);
		m_pConsole->Watch(_T("vEngine"),				VENGINE_BUILD_REVISION);
		m_pConsole->Watch(_T("WorldDefine"),			WORLDDEFINE_BUILD_REVISION);

		// 系统信息
		m_pConsole->Watch(_T("cpu_num:"),				m_nCpuNum);
		m_pConsole->Watch(_T("mem_total:"),				m_dwTotalPhys);
		m_pConsole->Watch(_T("mem_avail:"),				m_dwAvailPhys);

		// 加入要观察的实时变量
		m_pConsole->Watch(_T("sec: "),					nSec);
		m_pConsole->Watch(_T("min: "),					nMin);
		m_pConsole->Watch(_T("hour: "),					nHour);

		m_pConsole->Watch(_T("tick: "),					m_dwTick);
		m_pConsole->Watch(_T("idle: "),					m_dwIdleTime);
		
		m_pConsole->Watch(_T("all:"),					sPlayerMgr.GetPlayerNum());
		m_pConsole->Watch(_T("loging:"),				sPlayerMgr.GetPlayerLoging());
		m_pConsole->Watch(_T("proof_wait:"),			sPlayerMgr.GetProofResultNum());

		// 华丽的分割线
		m_pConsole->Watch(_T("---------------------"), 0);

		// DB的一些相关信息
		m_pConsole->Watch(_T("available_stream_num:"),	sBeton.GetAvailableStreamNum());
		m_pConsole->Watch(_T("all_stream_num:"),		sBeton.GetAllStreamNum());
		m_pConsole->Watch(_T("asy_stream_num:"),		sBeton.GetUnhandledAsynStreamNum());

		Sleep(50);
	}
}

//--------------------------------------------------------------------------------
// 线程函数
//--------------------------------------------------------------------------------
VOID LoginServer::ThreadUpdate()
{
	DWORD dwTime = 0;

#ifdef DEBUG
	_set_se_translator(vEngine::SEH_Translation); 

	try
	{
#endif
		while( !m_bTerminateUpdate )
		{
			// 得到当前时间
			dwTime = timeGetTime();

			// 锁住更新
			LockUpdate();

			// 更新LoongWorldMgr
			sLoongWorldMgr.Update();

			// 更新 PlayerMgr
			sPlayerMgr.Update();

			//更新 RTSession
			sRTSession.Update();

			// 发送消息
			sPlayerMgr.StartSendAllMsg();

			// 防沉迷session
			sFatigueSession.Update();

			// 每分钟发给中心激活玩家在线情况
			if (IsEnableFatigueGuard())
				sFatigueMgr.Update();

			// 释放锁更新
			UnlockUpdate();

			// 计算Sleep时间,每个Tick更新一次
			dwTime = timeGetTime() - dwTime;
			if( dwTime < TICK_TIME )
			{
				InterlockedExchange((LPLONG)&m_dwIdleTime, TICK_TIME - dwTime);
			}
			else
			{
				InterlockedExchange((LPLONG)&m_dwIdleTime, 0);
			}

			// 设置心跳
			InterlockedExchangeAdd((LPLONG)&m_dwTick, 1);

			// 休息
			if( dwTime < TICK_TIME )
			{
				Sleep(TICK_TIME - dwTime);
			}
		}
#ifdef DEBUG
	}
	catch(vEngine::Exception)
	{
		//如果是调试，则抛出异常
		if( TObjRef<Util>()->IsDebuggerPresent() )
		{
			throw;
		}
		else
		{
			exit(1);
		}
	}
#endif
	_endthreadex(0);
}

//--------------------------------------------------------------------------------
// 更新内存信息
//--------------------------------------------------------------------------------
VOID LoginServer::UpdateMemoryUsage()
{
	MEMORYSTATUS memStatus;
	memStatus.dwLength = sizeof(memStatus);
	GlobalMemoryStatus(&memStatus);

	m_dwTotalPhys = memStatus.dwTotalPhys;
	m_dwAvailPhys = memStatus.dwAvailPhys;

	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
}

LoginServer g_login;





