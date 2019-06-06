//-----------------------------------------------------------------------------
//!\file stdafx.cpp
//!\author Lyp
//!
//!\date 2008-03-27
//! last 2008-03-27
//!
//!\brief 标准系统包含文件的包含文件
//!
//!	Copyright (c) 2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"

//-----------------------------------------------------------------------------
// 全局变量
//-----------------------------------------------------------------------------
LPCTSTR g_pAppName = _T("LoongClient");
LPCTSTR g_pVersion = _T("1.0.0");
LPCTSTR g_pBuildTime = _T(__DATE__)_T(" ")_T(__TIME__);
#ifdef _DEBUG
	LPCTSTR g_pBuildType = _T("Debug");
#else
	LPCTSTR g_pBuildType = _T("Release");
#endif


StringTable g_StrTable;



