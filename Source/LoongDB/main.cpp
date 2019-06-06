//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: main.cpp
// author: Sxg
// actor:
// data: 2008-05-12
// last:
// brief: 控制台程序入口点
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "LoongDB_Def.h"
#include "LoongDB.h"
#include "Loong.h"


//-----------------------------------------------------------------------------
// 控制台程序入口点
//-----------------------------------------------------------------------------
INT APIENTRY _tWinMain(HINSTANCE hInst, HINSTANCE, LPTSTR, INT)
{
	// 禁止屏幕保护及电源管理
	::SystemParametersInfo(SPI_SETLOWPOWERTIMEOUT, 0, NULL, 0);
	::SystemParametersInfo(SPI_SETPOWEROFFTIMEOUT, 0, NULL, 0);
	::SystemParametersInfo(SPI_SETSCREENSAVETIMEOUT, 0, NULL, 0);

	vEngine::InitNetwork();
	vEngine::InitEngine();

#ifdef _DEBUG
	EXCEPTION_PROTECT;
#endif

	CLoong *pLoong = new CLoong;
	if(!pLoong->Init(hInst))
	{
		//初始化失败
		SAFE_DEL(pLoong);
		ERR(_T("Loong init failed!"));
		return FALSE;
	}

	pLoong->MainLoop();

	SAFE_DEL(pLoong);

	vEngine::DestroyEngine();
	vEngine::DestroyNetwork();
	return 0;
}