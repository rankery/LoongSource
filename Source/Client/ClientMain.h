//-----------------------------------------------------------------------------
//!\file ClientMain.h
//!\author Lyp
//!
//!\date 2008-03-27
//! last 2008-03-27
//!
//!\brief 龙客户端
//!
//!	Copyright (c) 2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once


//-----------------------------------------------------------------------------
// Client
//-----------------------------------------------------------------------------
class Client
{
public:
	BOOL Init(HINSTANCE hInst);
	VOID Mainloop();
	VOID Destroy();

	VOID WaitThreadLoad();
	VOID ExitThreadLoad();

	Client():m_Trunk(this){}

private:
	TSFPTrunk<Client>			m_Trunk;
	TObjRef<InputDX8>			m_pInput;
	TObjRef<GameInputMap>		m_pInputMap;
	TObjRef<Util>				m_pUtil;
	TObjRef<WindowWrap>			m_pWindow;
	TObjRef<LuaManager>			m_pLuaMgr;
	TObjRef<GUISystem>			m_pGUI;
	TObjRef<GUIEditor>			m_pGUIEditor;
	TObjRef<Console>			m_pConsole;
	TObjRef<GUIRender>			m_pGUIRender;
	TObjRef<GUIEditor>			m_pEditor;
	TObjRef<GameFrameMgr>		m_pFrameMgr;
	TObjRef<GameScriptMgr>		m_pGameScriptMgr;
	TObjRef<NetSession>			m_pSession;
	TObjRef<AudioSys>			m_pAudioSys;


	BOOL InitRenderSystem(HINSTANCE hInst);

	VOID RegisterControl();
	VOID RegisterFrame();

	// GUIWnd显示开关
	DWORD SwitchGUIWnd(DWORD dwID, BOOL bDown);
	// 截图
	DWORD ScreenShot(DWORD dwID, BOOL bDown);

	// GM命令处理
	DWORD GMCommand(LPCTSTR szCommand);
	
	// 将3D引擎资源池状态写入日志文件
	DWORD LogResPool(LPCTSTR szCommand);

	// 显示本地角色所有属性
	DWORD DisplayLocalRoleAtt(LPCTSTR szCommand);

	// 关闭/开起头顶tips
	DWORD HeadTipsCommand(LPCTSTR szCommand);

    // 删除一个文件夹下所有文件
    BOOL DeleteDirectory(LPCTSTR szDirName);
	
	HANDLE	m_hThreadLoad;	// 读取线程的HANDLE
	VOLATILE BOOL m_bTerminateThreadLoad; // 是否结束读取线程
	// 读取资源文件的线程
	UINT ThreadLoad(LPVOID);

};

