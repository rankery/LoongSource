//-----------------------------------------------------------------------------
// File: globe_define
// Desc: base define
// Auth: Lyp
// Date: 2003/12/11
// Last: 2007/12/14
//-----------------------------------------------------------------------------
#pragma once
//-----------------------------------------------------------------------------
// 基本环境定义
//-----------------------------------------------------------------------------
#ifndef UNICODE
	#define _UNICODE
	#define  UNICODE
#endif

#define X_HUGE_STRING		512
#define X_LONG_STRING		256
#define X_SHORT_STRING		32

// 注意此行必须在include <dinput.h>前
#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
// The following ifdef block is the standard way of creating macros which make  
// exporting from a DLL simpler. All files within this DLL are compiled with 
// the VENGINE_EXPORTS symbol defined on the command line. this symbol should 
// not be defined on any project that uses this DLL. This way any other project 
// whose source files include this file see VENGINE_API functions as being 
// imported from a DLL, wheras this DLL sees symbols defined with this macro 
// as being exported.
//-----------------------------------------------------------------------------
#ifdef VENGINE_EXPORTS
	#define VENGINE_API __declspec(dllexport)
#else
	#define VENGINE_API __declspec(dllimport)
#endif
