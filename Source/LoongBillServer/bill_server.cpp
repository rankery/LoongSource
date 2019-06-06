//-----------------------------------------------------------------------------
//!\file bill_server.cpp
//!\author Aslan
//!
//!\date 2009-06-09
//! last 2009-06-09
//!
//!\brief 直充划拨服务器
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"

#include "world.h"
#include "world_mgr.h"
#include "bill_server.h"
#include "login_db.h"
BillServer g_Server;

//-----------------------------------------------------------------------------
// 初始化
//-----------------------------------------------------------------------------
BOOL BillServer::Init(HINSTANCE hInst)
{
	// vEngine相关对象
	m_pLog			=	"Log";
	m_pRender		=	"GDIRender";
	m_pWindowWrap	=	"WindowWrap";
	m_pUtil			=	"Util";
	m_pGUI			=	"GUISystem";
	m_pConsole		=	"Console";

	// 创建VFS文件系统,读取XML需要用到
	CreateObj("VFS_System", "VirtualFileSys");

	// 生成log
	m_pLog->Create();

	// 加载配置文件
	CreateObj("BillConfig", "VarContainer");
	TObjRef<VarContainer> pVar = "BillConfig";
	TCHAR tszFilePath[MAX_PATH] = {0};
	if (!m_pUtil->GetIniPath(tszFilePath, _T("server_bill_config/config")))
		return FALSE;

	pVar->Load("VirtualFileSys", tszFilePath);

	// 大区ID
	m_dwSectionID	=	pVar->GetDword(_T("id"), _T("section"));

	// 窗口大小
	m_dwViewWidth	=	pVar->GetDword(_T("width"), _T("window"));
	m_dwViewHeight	=	pVar->GetDword(_T("height"), _T("window"));

	// 创建GUI渲染
	m_pGUIRender = new IGUIRenderNative;
	if( !P_VALID(m_pGUIRender) ) return FALSE;
	m_pGUIRender->Init(m_pRender);

	// 创建窗口
	m_pWindowWrap->Init(m_pGUIRender, m_dwViewWidth, m_dwViewHeight, TRUE);
	m_pWindowWrap->CreateWnd(_T("LoongBillServer"), hInst);

	// 创建GUI系统
	if( !m_pRender->Init(m_pWindowWrap->GetWidth(), m_pWindowWrap->GetHeight()) )
		return FALSE;

	m_pGUI->Init(m_pWindowWrap->GetWidth(), m_pWindowWrap->GetHeight(), m_pGUIRender);

	// 创建Console
	m_pConsole->Init(NULL, 1000, TRUE);

	// 系统参数
	m_nCPUNum = TObjRef<SysInfo>()->GetProcessorNum();

	//初始化logindb
	if( !g_LoginDB.Init() )
	{
		ILOG->Write(_T("login db init failed!\r\n"));
		return FALSE;
	}

	// 初始化游戏世界管理器
	if( !g_WorldMgr.Init() )
	{
		ILOG->Write(_T("world mgr init failed!\r\n"));
		return FALSE;
	}

	// 初始化成功
	ILOG->Write(_T("bill server init ok!\r\n"));
	IMSG(_T("bill server init ok!\r\n"));
	InitOK();

	return TRUE;
}

//-----------------------------------------------------------------------------------
// 销毁
//-----------------------------------------------------------------------------------
VOID BillServer::Destroy()
{
	// 删除worldmgr
	g_WorldMgr.Destroy();

	// 销毁窗口及GUI等系统
	m_pWindowWrap->Destroy();
	m_pGUI->Destroy();
	m_pRender->Destroy();
	m_pConsole->Destroy();
	m_pLog->Close();

	SAFE_DEL(m_pGUIRender);
}

//-----------------------------------------------------------------------------------
// 主循环
//-----------------------------------------------------------------------------------
VOID BillServer::MainLoop()
{
	DWORD dwMsg=0, dwParam1=0, dwParam2=0;

	while( FALSE == m_pWindowWrap->MsgLoop() && !IsShutingDown() )
	{
		if( FALSE == m_pWindowWrap->IsWindowActive() )
		{
			Sleep(30);
			continue;
		}

		while( m_pWindowWrap->PeekWindowsMsg(dwMsg, dwParam1, dwParam2) )
		{
			m_pGUI->OnWindowsMsg(dwMsg, dwParam1, dwParam2);
			if( dwMsg == WM_QUIT )
				return;
		}

		m_pGUI->Update();

		// render
		m_pRender->Clear(0, 0, 0);
		m_pGUI->Render();
		m_pRender->Render();

		// 得到内存占用
		UpdateMemoryUsage();

		// 显示运行时信息
		DisplayRunTimeInfo();

		Sleep(50);
	}
}

//-----------------------------------------------------------------------------
// 更新内存的占用情况
//-----------------------------------------------------------------------------
VOID BillServer::UpdateMemoryUsage()
{
	MEMORYSTATUS memStatus;
	memStatus.dwLength = sizeof(memStatus);
	GlobalMemoryStatus(&memStatus);

	m_dwTotalPhys = memStatus.dwTotalPhys;
	m_dwAvailPhys = memStatus.dwAvailPhys;

	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));

	m_nQuotaNonPagedPoolUsage = pmc.QuotaNonPagedPoolUsage;
}

//-----------------------------------------------------------------------------
// 显示运行时信息
//-----------------------------------------------------------------------------
VOID BillServer::DisplayRunTimeInfo()
{
	// CPU内存
	m_pConsole->Watch(_T("cpu_num:"),				GetCPUNum());
	m_pConsole->Watch(_T("mem_total:"),				m_dwTotalPhys/1024/1024);
	m_pConsole->Watch(_T("mem_avail:"),				m_dwAvailPhys/1024/1024);
	m_pConsole->Watch(_T("non_paged_pool:"),		m_nQuotaNonPagedPoolUsage);

	// TICK
	m_pConsole->Watch(_T("tick:"),					GetTick());

	// 大区ID
	m_pConsole->Watch(_T("section"),				GetSectionID());

	// world信息
	g_WorldMgr.DisPlayerRunTimeInfo(m_pConsole);

}

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------
INT APIENTRY _tWinMain(HINSTANCE hInst, HINSTANCE, LPTSTR, INT)
{
	// 设置进程优先级
	SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);

	// 禁止屏幕保护及电源管理
	::SystemParametersInfo(SPI_SETLOWPOWERTIMEOUT, 0, NULL, 0);
	::SystemParametersInfo(SPI_SETPOWEROFFTIMEOUT, 0, NULL, 0);
	::SystemParametersInfo(SPI_SETSCREENSAVETIMEOUT, 0, NULL, 0);

	// 设置保措时不弹窗口
	//Exception::m_bSendLogWindow = FALSE;

	vEngine::InitNetwork();
	vEngine::InitEngine();

#ifdef DEBUG
	EXCEPTION_PROTECT;
#endif

	if( g_Server.Init(hInst) )
	{
		g_Server.MainLoop();
	}
	g_Server.Destroy();

	vEngine::DestroyEngine();
	vEngine::DestroyNetwork();
	return 0;
}