//-----------------------------------------------------------------------------
//!\file window_wrap.h
//!\author Lyp
//!
//!\date 2004-08-03
//! last 2004-08-03
//!
//!\brief window系统包装
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine {
#define	WM_VENGINE_USER		(WM_APP + 10)
//-----------------------------------------------------------------------------
//!\brief window系统包装
//!
//!
//-----------------------------------------------------------------------------
class VENGINE_API WindowWrap
{
public:
	BOOL Init(IGUIRender* pRender, INT nWidth=0, INT nHeight=0, BOOL bBoard=FALSE, BOOL bShowTaskBar=TRUE);
	VOID Destroy();
	BOOL CreateWnd(LPCTSTR szCaption, HINSTANCE hInst, HICON hIcon=0, HMENU hMenu=0, BOOL bMinBox=TRUE);
	__forceinline BOOL MsgLoop();
	__forceinline BOOL PeekWindowsMsg(DWORD& dwMsg, DWORD& dwParam1, DWORD& dwParam2);
	
	VOID SetCaption(LPCTSTR szCaption);
	VOID SetCursor(LPCTSTR szCursorName);
	VOID PushCursor(LPCTSTR szCursorName);
	VOID PopCursor();
	VOID SetAccelerators(HACCEL hAcc) { m_hAcc = hAcc; }

	DWORD GetMainThreadID() { return m_dwMainThreadID; }
	BOOL IsWindowActive() { return m_bWindowActive; }

	void SetInvisibleIme(BOOL bIme) { m_bInvisibleIme = bIme; }

	HWND GetHWnd() { return m_hWnd; }
	INT	GetWidth() { return m_nWidth; } // 得到窗口客户区宽度
	INT	GetHeight() { return m_nHeight; } // 得到窗口客户区高度

	WindowWrap();
	~WindowWrap();

private:
	TSFPTrunk<WindowWrap>			m_Trunk;
	IGUIRender*						m_pRender;

	HWND							m_hWnd;
	HCURSOR							m_hCursorCurrent;
	HCURSOR							m_hCursorArrow;
	HCURSOR							m_hCursorWait;
	HCURSOR							m_hCursorIbeam;
	HCURSOR							m_hCursorSize;
	HCURSOR							m_hCursorHand;

	HACCEL							m_hAcc;

	INT								m_nWidth;
	INT								m_nHeight;
	BOOL							m_bBoard;
	BOOL							m_bShowTaskBar;

	volatile DWORD					m_dwMainThreadID;
	volatile BOOL					m_bWindowActive;
	volatile BOOL					m_bInvisibleIme;
	std::list<tstring>				m_listCursor;


	struct tagvEngineWindowsMsg
	{
		DWORD dwMsg;
		DWORD dwParam1;
		DWORD dwParam2;
	};

	MsgQueue	m_listWindowsMsg;

	LRESULT	WndProc(HWND, UINT, WPARAM, LPARAM);
	__forceinline VOID OnWindowsMsg(DWORD dwMsg, DWORD dwParam1, DWORD dwParam2);
	

};


//-----------------------------------------------------------------------------
//! 消息循环处理，返回TRUE代表程序需要结束
//-----------------------------------------------------------------------------
BOOL WindowWrap::MsgLoop()
{
	MSG msg;
	while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE) )
	{
		if( WM_QUIT == msg.message )
			return TRUE;

		if( !TranslateAccelerator(msg.hwnd, m_hAcc, &msg) )
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return FALSE;
}



//-----------------------------------------------------------------------------
// window proc
//-----------------------------------------------------------------------------
VOID WindowWrap::OnWindowsMsg(DWORD dwMsg, DWORD dwParam1, DWORD dwParam2)
{
	tagvEngineWindowsMsg msg;
	msg.dwMsg = dwMsg;
	msg.dwParam1 = dwParam1;
	msg.dwParam2 = dwParam2;

	m_listWindowsMsg.AddMsg(&msg, sizeof(msg));
}


//-----------------------------------------------------------------------------
// window proc
//-----------------------------------------------------------------------------
BOOL WindowWrap::PeekWindowsMsg(DWORD& dwMsg, DWORD& dwParam1, DWORD& dwParam2)
{
	DWORD dwSize = 0;
	tagvEngineWindowsMsg* pMsg = (tagvEngineWindowsMsg*)m_listWindowsMsg.GetMsg(dwSize);
	if( !pMsg )
		return FALSE;

	dwMsg = pMsg->dwMsg;
	dwParam1 = pMsg->dwParam1;
	dwParam2 = pMsg->dwParam2;

	m_listWindowsMsg.FreeMsg(pMsg);
	return TRUE;
}


} // namespace vEngine {
