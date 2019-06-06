//-----------------------------------------------------------------------------
//!\file vChatServer.h
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


struct tagClient
{
	DWORD	dwIP;
	DWORD	dwHandle;
	tstring strName;
};

//-----------------------------------------------------------------------------
// test
//-----------------------------------------------------------------------------
class ChatServer
{
public:
	BOOL Init(HINSTANCE hInst);
	VOID Mainloop();
	VOID Destroy();

	ChatServer():m_Trunk(this){}

private:
	TSFPTrunk<ChatServer>		m_Trunk;
	TObjRef<GDIRender>			m_pRender;
	TObjRef<Util>				m_pUtil;
	TObjRef<WindowWrap>			m_pWindow;
	TObjRef<GUISystem>			m_pGUI;
	TObjRef<Console>			m_pConsole;
	TObjRef<SystemTray>			m_pTray;
	TObjRef<XServer>			m_pServer;
	TObjRef<XClient>			m_pClient;
	IGUIRenderNative			m_render;
	
	HICON						m_hIconInfo;
	CRITICAL_SECTION			m_LockPlayerData;
	DWORD						m_dwIDHolder;
	INT							m_nPort;
	TMap<DWORD, tagClient*>		m_mapClient;
	std::map<DWORD, tstring>	m_mapIP;

	VOID Maximise();
	VOID Minimise();
	VOID InitSet();

	// 客户端登陆本服务器回调函数
	DWORD LoginCallBack(LPVOID, LPVOID);
	DWORD LogoutCallBack(LPVOID);

	// 接受所有客户端发来的信息
	VOID RecvAll(BOOL bHeartBeat);
	// 向所有客户端发送消息
	VOID SendToAll(tagMsg* pMsg, DWORD dwSize);
};

