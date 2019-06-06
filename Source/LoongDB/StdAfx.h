// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once

//-----------------------------------------------------------------------------
// 引用数据库引擎(该引擎的头文件中已包含vEngine)
//-----------------------------------------------------------------------------
#include "../Beton/Beton.h"
using namespace vEngine;

#ifdef _DEBUG
#define X_STRING_RUN_TIME "Debug"
#pragma comment(lib, "../vsout/Beton/debug/Beton.lib")
#pragma comment(lib, "../vsout/ServerDefine/debug/ServerDefine.lib")
#else
#define X_STRING_RUN_TIME "Release"
#pragma comment(lib, "../vsout/Beton/release/Beton.lib")
#pragma comment(lib, "../vsout/ServerDefine/release/ServerDefine.lib")
#endif

//------------------------------------------------------------------------------

#pragma comment(lib,"libmysql.lib")
#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"winmm.lib")


#include "ResGuard.h"
#include "../WorldDefine/base_define.h"
#include "../WorldDefine/msg_common_errorcode.h"
#include "../ServerDefine/base_define.h"
#include "../WorldDefine/time.h"
//-----------------------------------------------------------------