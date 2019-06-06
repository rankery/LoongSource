#pragma once

#pragma warning(disable:4355)	// Level 3: "this": 用于基成员初始值设定项列表
#pragma warning(disable:4251)	// Level 3: need to have dll-interface
#pragma warning(disable:4244)
#pragma warning(disable:4996)
#pragma warning(disable:4311)
#pragma warning(disable:4312)
#pragma warning(disable:4267)
#pragma warning(disable:4245)
#pragma warning(disable:4100)
#pragma warning(disable:4201)
#pragma warning(disable:4127)

#define WIN32_LEAN_AND_MEAN

#define _WIN32_WINNT 0x0403 

#include "..\vEngine\vEngine.h"
using namespace vEngine;

#ifdef _DEBUG
#define X_STRING_RUN_TIME "Debug"
#pragma comment(lib,"..\\vsout\\vEngine\\debug\\vEngine.lib")
#else
#define X_STRING_RUN_TIME "Release"
#pragma comment(lib,"..\\vsout\\vEngine\\release\\vEngine.lib")
#endif

#include <windows.h>
#include <tchar.h>
#include <ctype.h>
#include <assert.h>

#include <string>
#include <list>

#include <mysql.h>
#pragma comment(lib,"libmysql.lib")

#ifdef BETON_EXPORTS
#define BETON_API __declspec(dllexport)
#else
#define BETON_API __declspec(dllimport)
#endif