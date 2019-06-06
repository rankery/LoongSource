//-----------------------------------------------------------------------------
//!\file	server.cpp
//!\brief	运营服务器
//!
//!\date	2009-04-20
//! last	2009-04-20
//!\author	zhangrong
//!
//! Copyright (c) 1985-2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"

#include "../WorldDefine/base_define.h"

#include "server.h"
#include "section_mgr.h"
#include "client_mgr.h"

//-----------------------------------------------------------------------------
// 全局变量
//-----------------------------------------------------------------------------
Server g_Server;

//-----------------------------------------------------------------------------
// 构造函数   
//-----------------------------------------------------------------------------
Server::Server() : m_Trunk(this), m_bTerminateUpdate(FALSE)
{

}

//------------------------------------------------------------------------------
// 初始化
//------------------------------------------------------------------------------
BOOL Server::Init()
{
	m_pLog			=	"Log";
	m_pUtil			=	"Util";
	m_pVar			=	"VarContainer";

	// 生成log
	m_pLog->Create();

	// 载入初始变量
	TCHAR tszFilePath[X_LONG_STRING] = {0};
	GetModuleFileName(NULL, tszFilePath, X_LONG_STRING);
	int nCount = _tcslen(tszFilePath);
	while( tszFilePath[nCount-1] != '\\' )
	{
		nCount--;
	}
	tszFilePath[nCount] = '\0';

	_tcscat(tszFilePath, _T(SERVICE_CONFIG_INI));
	if (!m_pUtil->GetIniPath(tszFilePath, tszFilePath))
		return FALSE;
	m_pVar->Load("VirtualFileSys", tszFilePath);

	// 初始化两个管理器
	if( !g_ClientMgr.Init() )
		return FALSE;
	if( !g_SectionMgr.Init() )
		return FALSE;

	return TRUE;
}

//-----------------------------------------------------------------
//    
//-----------------------------------------------------------------
void Server::MainLoop()
{
	_tprintf(_T("enter MainLoop........\n"));

	DWORD dwTime = 0;

	_set_se_translator(vEngine::SEH_Translation);

	try
	{
		while ( m_bTerminateUpdate == FALSE )
		{
			dwTime = timeGetTime();

			Lock();

			// 更新SectionMgr
			g_SectionMgr.Update();

			// 更新ClientMgr
			g_ClientMgr.Update();
			
			// 更新时限消息管理器
			g_TimeLimitMsgMgr.Update();

			UnLock();

			// 计算Sleep时间,每个Tick更新一次
			dwTime = timeGetTime() - dwTime;
			if( dwTime < TICK_TIME )
				Sleep(TICK_TIME - dwTime);
		}
	}
	catch( vEngine::Exception )
	{
		//如果是调试，则抛出异常
		if ( true == m_pUtil->IsDebuggerPresent() )
			throw;		
		else
			exit(1);
	}
}

//-----------------------------------------------------------------
//    
//-----------------------------------------------------------------
void Server::Destroy()
{
	g_SectionMgr.Destroy();
	g_ClientMgr.Destroy();
	m_pLog->Close();
}
