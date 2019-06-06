#pragma once

//-----------------------------------------------------------------------------
//!\file vClientTest.h
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
	DWORD dwIndex;
	DWORD dwTimeServerSend;
	DWORD dwTimeServerRecv;
	DWORD dwCRC;
	CHAR  szMess[1];
};

//-----------------------------------------------------------------------------
// test
//-----------------------------------------------------------------------------
class CClientTest
{
public:
	BOOL Init(HINSTANCE hInst);
	VOID Mainloop();
	VOID Destroy();

	CClientTest():m_Trunk(this){}

private:
	TSFPTrunk<CClientTest>		m_Trunk;
	TObjRef<GDIRender>			m_pRender;
	TObjRef<Util>				m_pUtil;
	TObjRef<WindowWrap>			m_pWindow;
	TObjRef<GUISystem>			m_pGUI;
	TObjRef<Thread>				m_pThread;
	TObjRef<Console>			m_pConsole;
	IGUIRenderNative			m_render;

	BOOL VOLATILE 				m_bTerminateThread;
	LONG VOLATILE 				m_lClient;
	CHAR						m_szDestIP[256];

	// 创建指定数目的客户端
	DWORD CreateClient(LPCTSTR szDestIP, LPCTSTR szNum);
	// Client线程
	DWORD ThreadClient(INT);
	DWORD ThreadXClient(INT);
	DWORD ThreadXClientBurstLogin(INT);
};

