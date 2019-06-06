//-----------------------------------------------------------------------------
//!\file vServerTest.h
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
	DWORD dwTimeClientSend;
	DWORD dwTimeClientRecv;
	CHAR  szMess[1];
};

struct tagClient
{
	DWORD dwID;
	DWORD dwHandle;
};

//-----------------------------------------------------------------------------
// test
//-----------------------------------------------------------------------------
class CServerTest
{
public:
	BOOL Init(HINSTANCE hInst);
	VOID Mainloop();
	VOID Destroy();

	CServerTest():m_Trunk(this){}

private:
	TSFPTrunk<CServerTest>		m_Trunk;
	TObjRef<GDIRender>			m_pRender;
	TObjRef<Util>				m_pUtil;
	TObjRef<WindowWrap>			m_pWindow;
	TObjRef<GUISystem>			m_pGUI;
	IGUIRenderNative			m_render;

	TObjRef<Console>			m_pConsole;
	CRITICAL_SECTION			m_LockPlayerData;
	//TObjRef<CompletionSession>	m_pServer;
	TObjRef<XServer>			m_pServer;

	DWORD						m_dwIDHolder;
	TMap<DWORD, tagClient*>		m_mapClient;

	VOLATILE DWORD				m_dwMinDelay;	// 本次最小值
	VOLATILE DWORD				m_dwMaxDelay;	// 本次最大值
	VOLATILE DWORD				m_dwAveDelay;	// 本次平均值
	VOLATILE DWORD				m_dwPackageNum;	// 本次收包数
	VOLATILE DWORD				m_dwBigDelayNum;// 大于平均值乘2的数量



	// 客户端登陆本服务器回调函数
	DWORD LoginCallBack(LPVOID, LPVOID);
	DWORD LogoutCallBack(LPVOID);

	// 向所有客户端发送信息
	VOID SendToAll();
	// 接受所有客户端发来的信息
	VOID RecvAll();
};

