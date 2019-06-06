// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

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

// 如果您必须使用下列所指定的平台之前的平台，则修改下面的定义。
// 有关不同平台的相应值的最新信息，请参考 MSDN。
#ifndef WINVER				// 允许使用特定于 Windows XP 或更高版本的功能。
#define WINVER 0x0501		// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif

#ifndef _WIN32_WINNT		// 允许使用特定于 Windows XP 或更高版本的功能。
#define _WIN32_WINNT 0x0501	// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif						

/*
#ifndef _WIN32_WINDOWS		// 允许使用特定于 Windows 98 或更高版本的功能。
#define _WIN32_WINDOWS 0x0410 // 将它更改为适合 Windows Me 或更高版本的相应值。
#endif
*/
#ifndef _WIN32_IE			// 允许使用特定于 IE 6.0 或更高版本的功能。
#define _WIN32_IE 0x0600	// 将此值更改为相应的值，以适用于 IE 的其他版本。值。
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件


#pragma warning(disable:4355)// Level 3: "this": 用于基成员初始值设定项列表
// 定义线程函数类型
typedef UINT (WINAPI* THREADPROC)(LPVOID);


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

CONST INT GT_NET_BLOCK_TIME = 1000*150;	// 150 ms

#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>	// for _beginthreadex
#include <mmsystem.h>
#include <tchar.h>
#include <assert.h>
#include <afxinet.h>


#include <sstream>
#include <vector>
#include <list>
#include <map>

#include "sfp_trunk.h"
#include "msg_queue_ts.h"


#ifndef tstring
#ifdef _UNICODE
#define tstring std::wstring
#define tstringstream std::wstringstream
#define tstreambuf std::wstreambuf
#else
#define tstring std::string
#define tstringstream std::stringstream
#define tstreambuf std::streambuf
#endif
#endif
typedef unsigned long int		UINT4;


#include "util.h"
#include "var_container.h"
#include "StringTable.h"


extern const unsigned long	globe_Crc32Table[256];

extern StringTable g_StrTable;

class HttpSocket;
class CPKVfs;
class CTSMemMgr;


