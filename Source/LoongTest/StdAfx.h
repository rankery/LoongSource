// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__32BCB17B_AC25_4212_BB60_3AC9C6F0708A__INCLUDED_)
#define AFX_STDAFX_H__32BCB17B_AC25_4212_BB60_3AC9C6F0708A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#pragma once
#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
#define _WIN32_WINNT 0x0400
//-----------------------------------------------------------------------------
// 引用底层引擎
//-----------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_DEPRECATE

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

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__32BCB17B_AC25_4212_BB60_3AC9C6F0708A__INCLUDED_)
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <list>
using namespace std;


#include <afxtempl.h> 

//#include "action.h"

//#include "LoginFrame.h"

//引用STL
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <list>
using namespace std;
#include <stdlib.h>

#include <afxtempl.h> 

#include "..\Cool3D\Cool3D.h"
#include "..\Cool3D\Cool3DEngine.h"
using namespace Cool3D;

#include "..\vEngine\vEngine.h"
using namespace vEngine;

#include "..\WorldBase\WorldBaseDefine.h"
using namespace WorldBase;

#ifdef _DEBUG
#pragma comment(lib,"..\\vsout\\vengine\\debug\\vengine.lib")
#pragma comment(lib,"..\\vsout\\cool3d\\debug\\cool3d.lib")
#pragma comment(lib,"..\\vsout\\worldbase\\debug\\worldbase.lib")
#pragma comment(lib,"..\\vsout\\WorldDefine\\debug\\WorldDefine.lib")
#else
#pragma comment(lib,"..\\vsout\\vengine\\release\\vengine.lib")
#pragma comment(lib,"..\\vsout\\cool3d\\release\\cool3d.lib")
#pragma comment(lib,"..\\vsout\\worldbase\\release\\worldbase.lib")
#endif

#pragma comment(lib, "Imm32.lib")
#pragma comment(lib, "WinMM.lib")

