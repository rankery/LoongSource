// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

#pragma warning(disable:4800)
//----------------------------------------------------------------------
// Beton
//----------------------------------------------------------------------
//包含头文件
#include "..\Beton\Beton.h"
using namespace Beton;
#ifdef _DEBUG
#pragma comment(lib,"..\\vsout\\Beton\\debug\\Beton.lib")
#else
#pragma comment(lib,"..\\vsout\\Beton\\release\\Beton.lib")
#endif

//----------------------------------------------------------------------
// vEngine
//----------------------------------------------------------------------
//包含头文件
#include "..\vEngine\vEngine.h"
using namespace Beton;
#ifdef _DEBUG
#pragma comment(lib,"..\\vsout\\vEngine\\debug\\vEngine.lib")
#else
#pragma comment(lib,"..\\vsout\\vEngine\\release\\vEngine.lib")
#endif

//----------------------------------------------------------------------
// WorldDefine
//----------------------------------------------------------------------
#include "../WorldDefine/time.h"
#ifdef _DEBUG
#pragma comment(lib,"..\\vsout\\WorldDefine\\debug\\WorldDefine.lib")
#else
#pragma comment(lib,"..\\vsout\\WorldDefine\\release\\WorldDefine.lib")
#endif
//----------------------------------------------------------------------
// wxWidget
//----------------------------------------------------------------------
#ifdef _DEBUG
#pragma comment(lib,"wxbase29ud.lib")
#pragma comment(lib,"wxmsw29ud_core.lib")
#pragma comment(lib,"wxmsw29ud_adv.lib")
#else
#pragma comment(lib,"wxbase29u.lib")
#pragma comment(lib,"wxmsw29u_core.lib")
#pragma comment(lib,"wxmsw29u_adv.lib")
#endif

#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"rpcrt4.lib")
#pragma comment(lib,"wsock32.lib")
#pragma comment(lib,"oleacc.lib")
#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"shlwapi.lib")

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>	// for _beginthreadex
#include <mmsystem.h>
#include <tchar.h>
#include <assert.h>
#include <tchar.h>
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <ctype.h>
using namespace std;

#ifndef UNICODE
#define _UNICODE
#define  UNICODE
#endif

#ifdef _UNICODE
typedef std::wstring		tstring;
typedef std::wstringstream	tstringstream;
typedef std::wstreambuf		tstreambuf;
#else
typedef std::string			tstring;
typedef std::stringstream	tstringstream;
typedef std::streambuf		tstreambuf;
#endif


