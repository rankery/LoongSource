// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//
#pragma once

//#define _Cool3D_LEAK_DECTOR_
#ifdef _Cool3D_LEAK_DECTOR_
#include "vld.h"
#endif
#pragma warning(disable:4996)

#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>

// TODO: 在此处引用程序要求的附加头文件
#include <stdio.h>
#include <stdlib.h>


#include <D3D9.h>
#include <d3dx9math.h>
#pragma comment(lib,"d3dx9.lib")

//--STL
#include <string>
#include <vector>
#include <memory>
#include <list>
#include <algorithm>
#include <map>
#include <set>
#include <queue>
#include <stack>
#include <locale>
using namespace std;

#include ".\Util\UnicodeUtil.h"

//#include "MemDebug/MemoryManager.h"