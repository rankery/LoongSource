// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#define  _WIN32_WINNT 0x500


#include <stdio.h>
#include <Windows.h>
#include <tchar.h>
#include <locale.h>
#include <winbase.h>

#pragma warning(disable:4311)
#pragma warning(disable:4312)
#pragma warning(disable:4267)

//---------------------------------------------------------------------
// vEngine
//---------------------------------------------------------------------
#include "..\..\vEngine\vEngine.h"
using namespace vEngine;

#ifdef _DEBUG
#pragma comment(lib,"..\\..\\vsout\\vEngine\\debug\\vEngine.lib")
#else
#define X_STRING_RUN_TIME "Release"
#pragma comment(lib,"..\\..\\vsout\\vEngine\\release\\vEngine.lib")
#endif

//-----------------------------------------------------------------------
// ServerDefine
//-----------------------------------------------------------------------
#ifdef _DEBUG
#pragma comment(lib,"..\\..\\vsout\\ServerDefine\\debug\\ServerDefine.lib")
#else
#pragma comment(lib,"..\\..\\vsout\\ServerDefine\\release\\ServerDefine.lib")
#endif

////----------------------------------------------------------------------
//// Beton
////----------------------------------------------------------------------
//#include "../../Beton/Beton.h"
//using namespace Beton
//
//#ifdef _DEBUG
//#pragma comment(lib, "../../vsout/Beton/Debug/Beton.lib")
//#else
//#pragma comment(lib, "../../vsout/Beton/release/Beton.lib")
//#endif

#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"winmm.lib")

//#define SHORT_STRING		32

// TODO: reference additional headers your program requires here
