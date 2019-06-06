#pragma once

//-----------------------------------------------------------------------------
//!\file vChatClient.h
//!\author Lyp
//!
//!\date 2004-08-11
//! last 2007-10-11
//!
//!\brief vengine测试程序
//-----------------------------------------------------------------------------
#pragma once
struct tagMsg
{
	DWORD dwSize;
	DWORD dwState;	// 0:对话 1:上线 2:下线 3:心跳
	DWORD dwCode;
	TCHAR szName[32];
	TCHAR szText[1];

	tagMsg() { ZeroMemory(this, sizeof(*this)); dwSize = sizeof(tagMsg); }
};

//-----------------------------------------------------------------------------
// test
//-----------------------------------------------------------------------------
class ChatClient
{
public:
	BOOL Init(HINSTANCE hInst);
	VOID Mainloop();
	VOID Destroy();

	ChatClient():m_Trunk(this){}

private:
	TSFPTrunk<ChatClient>			m_Trunk;
	TObjRef<GDIRender>				m_pRender;
	TObjRef<Util>					m_pUtil;
	TObjRef<WindowWrap>				m_pWindow;
	TObjRef<GUISystem>				m_pGUI;
	TObjRef<Thread>					m_pThread;
	TObjRef<Console>				m_pConsole;
	TObjRef<SystemTray>				m_pTray;
	TObjRef<XClient>				m_pClient;
	IGUIRenderNative				m_render;

	BOOL VOLATILE 					m_bTerminateThread;
	CHAR							m_szDestIP[256];
	INT								m_nPort;
	DWORD							m_dwSelfIP;
	HICON							m_hIconQuest;
	HICON							m_hIconInfo;

	VOID Maximise();
	VOID Minimise();
	VOID InitClient();

	// 未知控制台命令处理函数
	DWORD Sender(LPCTSTR szText);
	// 接收线程
	DWORD ThreadXClient(INT);
	// 数据处理函数
	VOID Fileter(tagMsg* pMsg, DWORD dwSize);
	
};

