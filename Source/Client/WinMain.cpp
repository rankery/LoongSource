//-----------------------------------------------------------------------------
//!\file winmain.cpp
//!\author Lyp
//!
//!\date 2008-04-20
//! last 2008-04-22
//!
//!\brief 客户端入口函数
//!
//!	Copyright (c) 2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "ClientMain.h"
#include "Render2D.h"

#include "AudioSys.h"
VOID InitDump();
//-----------------------------------------------------------------------------
// 入口
//-----------------------------------------------------------------------------
INT APIENTRY _tWinMain(HINSTANCE hInst, HINSTANCE, LPTSTR, INT)
{
	// 为了让Launcher程序识别客户端是否在运行，创建一个互斥量
	HANDLE hMutex = ::CreateMutex(NULL, FALSE, _T("Loong_client"));

	// 禁止屏幕保护及电源管理
	::SystemParametersInfo(SPI_SETLOWPOWERTIMEOUT, 0, NULL, 0);
	::SystemParametersInfo(SPI_SETPOWEROFFTIMEOUT, 0, NULL, 0);
	::SystemParametersInfo(SPI_SETSCREENSAVETIMEOUT, 0, NULL, 0);

	vEngine::InitNetwork();
	vEngine::InitEngine();
	_set_se_translator( vEngine::SEH_Translation );
	InitDump();

	// 向vEngine注册客户端自己实现的模块
	vEngine::Register(TWrap<Client>(),			"Client");
	vEngine::Register(TWrap<GUIRender>(),		"GUIRender");
	vEngine::Register(TWrap<GameFrameMgr>(),	"GameFrameMgr");
	vEngine::Register(TWrap<GameScriptMgr>(),	"GameScriptMgr");
	vEngine::Register(TWrap<NetSession>(),		"NetSession");

	vEngine::Register(TWrap<GameInputMap>(),	"GameInputMap");
	vEngine::Register(TWrap<MsgBox>(),			"MsgBox");
	vEngine::Register(TWrap<MsgBoxEx>(),		"MsgBoxEx");
	vEngine::Register(TWrap<MsgInputBox>(),		"MsgInputBox");
	vEngine::Register(TWrap<MsgMoneyInputBox>(),"MsgMoneyInputBox");
	vEngine::Register(TWrap<MsgOptionBox>(),    "MsgOptionBox");

	// 音乐音效系统
	vEngine::Register(TWrap<AudioSys>(),		"AudioSys");

	try
	{
		TObjRef<Client> pClient;
		if( pClient->Init(hInst) )
		{
			pClient->Mainloop();
			pClient->Destroy();
		}
	} 
	catch(vEngine::Exception)
	{
		throw;	// 方便调试器调试
	}

	vEngine::DestroyEngine();
	vEngine::DestroyNetwork();

	::CloseHandle(hMutex);
	return 0;
}


//-----------------------------------------------------------------------------
// 得到Dump FTP配置
//-----------------------------------------------------------------------------
VOID InitDump()
{
	TObjRef<VarContainer> pCfg = CreateObj("DumpConfig", "VarContainer");

	if( pCfg->Load(NULL, _T("config\\DumpCfg.xml")) )
	{
		const int MAX_STRING_SIZE = 512;
		TCHAR szVer[MAX_STRING_SIZE] = {0};
		::GetPrivateProfileString(_T("version"), _T("latest_game"), NULL,  szVer, MAX_STRING_SIZE, _T("launcher\\patchinfo.txt"));

		vEngine::InitDump(pCfg->GetString(_T("ftp")), 
			pCfg->GetString(_T("user")), pCfg->GetString(_T("password")), szVer);
	}

	KillObj("DumpConfig");
}
