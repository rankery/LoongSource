//-----------------------------------------------------------------------------
//!\file	main.cpp
//!\brief	游戏主循环
//!
//!\date	2009-04-20
//! last	2009-04-20
//!\author	zhangrong
//!
//! Copyright (c) 1985-2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"

#include "server.h"

//----------------------------------------------------------------------------------
// 主循环
//----------------------------------------------------------------------------------
INT main(VOID)
{	
	vEngine::InitNetwork();
	vEngine::InitEngine();

	if( g_Server.Init() )
	{
		g_Server.MainLoop();
	}
	g_Server.Destroy();

	vEngine::DestroyEngine();
	vEngine::DestroyNetwork();

	return 0;
}

