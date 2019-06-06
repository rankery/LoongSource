// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//
#pragma once

#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
#define _CRT_SECURE_NO_DEPRECATE
// Windows 头文件:
#include <windows.h>

// TODO: 在此处引用程序要求的附加头文件
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

//--STL
#include <string>
#include <sstream>
#include <vector>
#include <memory>
#include <list>
#include <algorithm>
#include <map>
#include <queue>
#include <tchar.h>

#ifdef _UNICODE
	typedef std::wstring		tstring;
	typedef std::wstringstream	tstringstream;
	typedef std::wstreambuf		tstreambuf;
#else
	typedef std::string			tstring;
	typedef std::stringstream	tstringstream;
	typedef std::streambuf		tstreambuf;
#endif

#include "..\Cool3D\Cool3DEngine.h"

using namespace std;

