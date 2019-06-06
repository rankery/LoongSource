#pragma once

const TCHAR *const g_szWindowClass=_T("GWin32Wnd");
const DWORD GAPP_QUIT=1000;//1000以上的消息由用户定义
/**	\class GWin32App
	\brief Win32应用程序对象
	\remarks
	\par 创建一个窗口,并执行消息循环
	\par 一个游戏应用程序必须有一个,而且只能有一个GWin32App派生类对象
*/
class GWin32App
{
	struct tagMsg
	{
		DWORD	msg,
				wParam,
				lParam;
	};
public:
	GWin32App(void);
	virtual ~GWin32App(void);
	static GWin32App* Inst();

	void Create(UINT windowW,UINT windowH,const TCHAR* szTitle,bool bFullScreenWndStyle,HICON hIcon=NULL);
	/**	向应用程序加入一个消息,这些消息没帧开始执行OnIdle()之前才处理
	*/
	void PostMessage(DWORD msg,DWORD wParam,DWORD lParam);
	void MainLoop();

	HWND GetHWnd() { return m_hWnd; }
	
	virtual void OnCreate() =0;
	virtual void OnDestroy() =0;
	/**	当应用程序不处理Win32 Message时调用
	*/
	virtual void OnIdle() =0;
	/**	Windows Message通知函数
	*/
	virtual DWORD OnWin32Msg(HWND hWnd,UINT message, WPARAM wParam, LPARAM lParam);

	void ScreenToClient(POINT *pPt);
	bool IfPointInClient(const POINT& pt);

private:
	void DoMessages();
	virtual bool DoMessage(DWORD msg,DWORD wParam,DWORD lParam)=0;
	list<tagMsg>	m_msgList;

protected:
	void Destroy();

protected:
	bool			m_bFullScreen;
	bool			m_bRun;
	HWND			m_hWnd;
};
