#pragma once

//-----------------------------------------------------------------------------
// 引用底层引擎
//-----------------------------------------------------------------------------


#pragma warning(disable:4311)
#pragma warning(disable:4312)
#pragma warning(disable:4267)

#define _WIN32_WINNT 0x0403
#define _USE_D3D7_
//包含vEngine头文件
#include "..\vEngine\vEngine.h"
using namespace vEngine;

#ifdef _DEBUG
#define X_STRING_RUN_TIME "Debug"
#pragma comment(lib,"..\\vsout\\vEngine\\debug\\vEngine.lib")
#else
#define X_STRING_RUN_TIME "Release"
#pragma comment(lib,"..\\vsout\\vEngine\\release\\vEngine.lib")
#endif

////----------------------------------------------------------------------
//// Cool3D
////----------------------------------------------------------------------
//#include "..\Cool3D\Cool3D.h"
//#include "..\Cool3D\Cool3DEngine.h"
//using namespace Cool3D;
//
//#ifdef _DEBUG
//#pragma comment(lib,"..\\vsout\\Cool3D\\debug\\Cool3D.lib")
//#else
//#pragma comment(lib,"..\\vsout\\Cool3D\\release\\Cool3D.lib")
//#endif

#include "..\WorldDefine\base_define.h"

#include "..\WorldDefine\time.h"

struct Vector3 
{
	float x;
	float y;
	float z;
};
struct tagWorldInfo
{
	
};
struct tagLoginInfo 
{
	INT			nPort;				// LoginServer IP
	INT			nInLogin;			// 正在登陆的玩家人数
	INT			nInSelect;			// 在选人的人数
	INT			nInWorld;			// 游戏中的人数
	TList<tstring>* listWorldName;	// LoongWorld名称列表
	tstring		strLoginIP;			// LoginServer IP

	//tstring		strWorldName;		// LoongWorld名称

	tagLoginInfo()
	{
		nPort = 0;
		nInLogin = nInSelect = nInWorld = 0;
		strLoginIP.clear();
		//strWorldName.clear();
		listWorldName = NULL;
	}

	~tagLoginInfo()
	{
		SAFE_DEL(listWorldName);
	}
};

//引用STL
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <list>
using namespace std;
