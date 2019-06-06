// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//
#pragma once

#define _WIN32_WINNT 0x0403

//---------------------------------------------------------------------
// vEngine
//---------------------------------------------------------------------
#include "..\vEngine\vEngine.h"
using namespace vEngine;

//---------------------------------------------------------------------
// 相关库
//---------------------------------------------------------------------

#ifdef _DEBUG
#pragma comment(lib,"..\\vsout\\vEngine\\debug\\vEngine.lib")
#else
#define X_STRING_RUN_TIME "Release"
#pragma comment(lib,"..\\vsout\\vEngine\\release\\vEngine.lib")
#endif



#include "../ServerDefine/msg_rt_s.h"

//配置文件路径
#define RTSERVICE_CONFIG_INI		"\\RTService.ini"

// name of the executable
#define SZAPPNAME				"RtService.exe"
// internal name of the service
#define SZSERVICENAME			"Local_RtService"
// displayed name of the service
#define SZSERVICEDISPLAYNAME	"Loong Server Local Tool Service"
// service description
#define SZSERVICEDESCRIPTION	"Loong Server Local Tool Service"
// list of service dependencies - "dep1\0dep2\0\0"
#define SZDEPENDENCIES			""
