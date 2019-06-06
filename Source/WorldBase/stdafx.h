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


//--STL
#include <string>
#include <vector>
#include <memory>
#include <list>
#include <algorithm>
#include <map>
#include <queue>
#include <tchar.h>

using namespace std;

//--
#include <time.h>
#include "..\WorldDefine\time.h"


#include "WorldBase.h"

#include "..\Cool3D\Cool3DEngine.h"
using namespace Cool3D;

#include "..\XMLReader\XMLReader.h"
