#include "..\StdAfx.h"
#include ".\gwin32app.h"
#include "GInput.h"
#include "..\..\Cool3D\Util\DebugMisc.h"
#include "..\..\Cool3D\Util\Exception.h"
using namespace Cool3D;

GWin32App*	g_win32AppInst=NULL;

LRESULT CALLBACK GWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(g_win32AppInst==NULL)
		return DefWindowProc(hWnd, message, wParam, lParam);
	else
		return g_win32AppInst->OnWin32Msg(hWnd,message,wParam,lParam);
}

GWin32App::GWin32App(void)
{
	m_bRun=true;
	m_hWnd=NULL;

	ASSERT(g_win32AppInst==NULL);//只能有一个实例
	g_win32AppInst=this;
	m_bFullScreen=false;
}

GWin32App::~GWin32App(void)
{
}

GWin32App* GWin32App::Inst()
{
	return g_win32AppInst;
}

void GWin32App::Create(UINT windowW,UINT windowH,const TCHAR* szTitle,bool bFullScreenWndStyle,HICON hIcon)
{
	ASSERT(m_hWnd==NULL);//不可重复创建

	m_bFullScreen=bFullScreenWndStyle;
	HINSTANCE hInstance=GetModuleHandle(NULL);
	
	//--注册窗口class 
	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(wc));

	wc.style			= CS_HREDRAW | CS_VREDRAW |CS_DBLCLKS;//支持双击
	wc.lpfnWndProc		= (WNDPROC)GWndProc;
	wc.hInstance		= hInstance;
	wc.hIcon			= hIcon;
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszClassName	= g_szWindowClass;

	if( 0 == RegisterClass(&wc) )
		THROW_NULL(0, _T("error register window class"));

	//--创建一个在屏幕上居中的窗口
	DWORD style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	DWORD styleEx = 0;
	if( bFullScreenWndStyle )
	{
		styleEx = WS_EX_TOPMOST;
	}
	else
	{
		UINT scrW = ::GetSystemMetrics(SM_CXSCREEN);
		UINT scrH = ::GetSystemMetrics(SM_CYSCREEN);

		if(scrW > windowW && scrH > windowH)
			style = WS_OVERLAPPEDWINDOW;	// 窗口小于屏幕大小，窗口才带边框
	}

	m_hWnd=CreateWindowEx(styleEx,g_szWindowClass,szTitle,style, 0,0,0,0,
		NULL,NULL,hInstance, NULL);
	THROW_NULL(m_hWnd, _T("error creating window"));

	//------------------------------------------------------
	// 调节窗口客户区为要求的大小
	RECT rcInt;   // rcInt相当于rcClient
	RECT rcExt;   // rcExt相当于rcWindow

	// 计算边框的大小
	::GetWindowRect(m_hWnd, &rcExt);
	::GetClientRect(m_hWnd, &rcInt);

	INT x = (rcExt.right-rcExt.left) - (rcInt.right-rcInt.left);
	INT y = (rcExt.bottom-rcExt.top) - (rcInt.bottom-rcInt.top);

	RECT rc;
	rc.left = 0; rc.top = 0; rc.bottom = windowH; rc.right = windowW;

	if( !bFullScreenWndStyle )
	{
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);	// 得到工作区的大小(不含任务栏的区域)

		if( (LONG)windowW > rc.right - rc.left )
			windowW = rc.right - rc.left;
		if( (LONG)windowH > rc.bottom - rc.top )
			windowH = rc.bottom - rc.top;
	}

	// 新的窗口大小
	SetWindowPos(m_hWnd, HWND_TOP, rc.left, rc.top, windowW+x, windowH+y, SWP_SHOWWINDOW);


	//-----------------------------------------------------------------------

	//--初始化附属对象
	GInput::Inst()->CreateInput();

	//--给派生类响应Create事件的机会
	OnCreate();

	ShowWindow(m_hWnd, SW_HIDE);
	UpdateWindow(m_hWnd);

	m_bRun=true;
}

void GWin32App::PostMessage(DWORD msg,DWORD wParam,DWORD lParam)
{
	tagMsg tmp={msg,wParam,lParam};
	m_msgList.push_back(tmp);
}

void GWin32App::MainLoop()
{
	MSG msg;
	while(m_bRun)
	{
		//--消息处理
		if (::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
		{
			if( !::GetMessage( &msg, NULL, 0, 0 ) )
				break;

			::TranslateMessage(&msg); 
			::DispatchMessage(&msg);
		}
		else//只有把所有message取完，才进行游戏的下一帧，这样优先处理消息会使得输入更流畅
		{
			//--处理应用程序消息
			DoMessages();
			//--游戏处理机会
			OnIdle();
		}
	}

	DoMessages();
	Destroy();
}

void GWin32App::Destroy()
{
	//--先给派生类Destory的机会
	OnDestroy();
	//--再Destroy内部数据
	if(m_hWnd)
	{
		DestroyWindow(m_hWnd);
		m_hWnd=NULL;
	}
}

void GWin32App::ScreenToClient(POINT *pPt)
{
	::ScreenToClient(m_hWnd,pPt);
}

void GWin32App::DoMessages()
{
	
	for(list<tagMsg>::iterator iter=m_msgList.begin();
			iter!=m_msgList.end();iter++)
	{
		switch(iter->msg)
		{
		case GAPP_QUIT:
			m_bRun=false;
			ShowWindow(m_hWnd,SW_HIDE);
			break;
		}
		DoMessage(iter->msg,iter->wParam,iter->lParam);
	}
	m_msgList.clear();
}

DWORD GWin32App::OnWin32Msg(HWND hWnd,UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_ACTIVATEAPP:
		if(m_bFullScreen)
		{
			if(wParam==WA_INACTIVE)
				ShowWindow(m_hWnd,SW_MINIMIZE);
			else
				ShowWindow(m_hWnd,SW_SHOWNORMAL);
		}
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

bool GWin32App::IfPointInClient(const POINT& pt)
{
	RECT rc;
	GetWindowRect(m_hWnd,&rc);
	return PtInRect(&rc,pt)!=0;//窗口模式的标题栏怎么办??
}
