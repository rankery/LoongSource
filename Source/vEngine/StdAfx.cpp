//-----------------------------------------------------------------------------
// File: stdafx
// Desc: 生成预编译头文件
// Auth: Lyp
// Date: 2003/11/30
//
//
// stdafx.obj will contain the pre-compiled type information
// reference any additional headers you need in stdafx.h
// and not in this file
//-----------------------------------------------------------------------------
#include "stdafx.h"

HINSTANCE g_hInstance; 
//-----------------------------------------------------------------------------
// dll 入口
//-----------------------------------------------------------------------------
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID )
{
	g_hInstance = (HINSTANCE)hModule;
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}