#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NON_CONFORMING_SWPRINTFS

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// 从 Windows 头中排除极少使用的资料
#endif

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

#pragma warning(disable:4311)
#pragma warning(disable:4996)
#pragma warning(disable:4273)
#pragma warning(disable:4267)
#pragma warning(disable:4312)
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

INT ErrMsg(LPCTSTR lpFormat, ...);
#define IMSG ErrMsg

#define X_HUGE_STRING		512
#define X_LONG_STRING		256
#define X_SHORT_STRING		32
#define GT_INVALID				(-1)
#define GT_VALID(n)				(((INT)(n)) != GT_INVALID)
#define P_VALID(n)				( (((INT)(n)) != GT_INVALID) && ((n) != NULL) )

#define GT_SEEK_SET				0
#define GT_SEEK_CUR				1
#define GT_SEEK_END				2

#ifndef SAFE_DEL
#define SAFE_DEL(p)				{ if(p) { delete(p);		(p) = NULL; } }
#endif
#ifndef SAFE_DEL_ARRAY
#define SAFE_DEL_ARRAY(p)		{ if(p) { delete[](p);		(p) = NULL; } }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)			{ if(p) { (p)->Release();	(p) = NULL; } }
#endif
#ifndef SAFE_CLOSE_HANDLE
#define SAFE_CLOSE_HANDLE(h)	{ if(h) { CloseHandle(h);	(h) = NULL; } }
#endif
#ifndef ASSERT
#define ASSERT(f)				assert((f))
#endif

// 定义线程函数类型
typedef UINT (WINAPI* THREADPROC)(LPVOID);


#include "util.h"
#include "var_container.h"
#include "StringTable.h"
#include "net_command_manager.h"
#include "stream_transport.h"

typedef unsigned long int		UINT4;
extern const unsigned long	globe_Crc32Table[256];
extern StringTable g_StrTable;
extern Util	g_Util;

#include "net_define.h"
//#include "NetSessionMgr.h"
#include "../WorldDefine/time.h"
