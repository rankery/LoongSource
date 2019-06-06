//-----------------------------------------------------------------------------
//!\file window_wrap.cpp
//!\author Lyp
//!
//!\date 2004-08-03
//! last 2004-08-03
//!
//!\brief window系统包装
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "window_wrap.h"

// 使用其它部件
#include "..\variable\var_container.h"
#include "..\gui\gui_render.h"
#include "..\resource\resource.h"

namespace vEngine {
//-----------------------------------------------------------------------------
//! construction
//-----------------------------------------------------------------------------
WindowWrap::WindowWrap():m_Trunk(this)
{
	m_hWnd = 0;
	m_hAcc = 0;
	m_pRender = 0;
	m_nWidth = 0;
	m_nHeight = 0;
	m_bBoard = FALSE;
	m_bShowTaskBar = FALSE;
	m_bInvisibleIme	= FALSE;
	m_listCursor.clear();
}

//-----------------------------------------------------------------------------
//! destruction
//-----------------------------------------------------------------------------
WindowWrap::~WindowWrap()
{
	m_hWnd = 0;
	this->Destroy();
}


//-----------------------------------------------------------------------------
//! init
//-----------------------------------------------------------------------------
BOOL WindowWrap::Init(IGUIRender* pRender, INT nWidth, INT nHeight, BOOL bBoard, BOOL bShowTaskBar)
{
	m_pRender = pRender;
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_bBoard = bBoard;
	m_bShowTaskBar = bShowTaskBar;

	if( m_nWidth == 0 )
		m_nWidth = GetSystemMetrics(SM_CXSCREEN);

	if( m_nHeight == 0 )
		m_nHeight = GetSystemMetrics(SM_CYSCREEN);

	return TRUE;
}

//-----------------------------------------------------------------------------
//! destroy
//-----------------------------------------------------------------------------
VOID WindowWrap::Destroy()
{
	m_pRender = 0;
	if( m_hWnd )
	{
		DestroyWindow(m_hWnd); 
		m_hWnd = 0;
	}
}



//-----------------------------------------------------------------------------
// create window
//-----------------------------------------------------------------------------
BOOL WindowWrap::CreateWnd(LPCTSTR szCaption, HINSTANCE hInst, HICON hIcon, HMENU hMenu, BOOL bMinBox)
{
	m_dwMainThreadID = GetCurrentThreadId();
	m_hCursorArrow = LoadCursor(NULL, IDC_ARROW);
	m_hCursorWait = LoadCursor(NULL, IDC_WAIT);
	m_hCursorIbeam = LoadCursor(NULL, IDC_IBEAM);
	m_hCursorSize = LoadCursor(NULL, IDC_SIZEALL);
	m_hCursorCurrent = m_hCursorArrow;
	m_hCursorHand = LoadCursor(NULL, IDC_HAND);

	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)(m_Trunk.sfp4(&vEngine::WindowWrap::WndProc));
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInst;
	wcex.hIcon			= hIcon;
	wcex.hCursor		= m_hCursorCurrent;
	wcex.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= szCaption;
	wcex.hIconSm		= 0;// LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	
	if( 0 == RegisterClassEx(&wcex) ) 
	{
		OutputDebugString(_T("error register window class"));
		return FALSE;
	}

	if( bMinBox )
		m_hWnd = ::CreateWindow(szCaption, szCaption, WS_OVERLAPPED|WS_CAPTION|WS_MINIMIZEBOX|WS_SYSMENU,
			CW_USEDEFAULT, CW_USEDEFAULT, m_nWidth, m_nHeight, NULL, NULL, hInst, NULL);
	else
		m_hWnd = ::CreateWindow(szCaption, szCaption, WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU,
		 	CW_USEDEFAULT, CW_USEDEFAULT, m_nWidth, m_nHeight, NULL, NULL, hInst, NULL);

	if( !m_hWnd ) 
	{
		OutputDebugString(_T("error creating window"));
		return 1;
	}

	// 将hwnd记录下来
	TObjRef<VarContainer>()->Add((DWORD)m_hWnd, _T("HWND"));

	if( hMenu )
		::SetMenu(m_hWnd, hMenu);

	if( !m_bBoard )
	{
		// 去掉窗口标题和边框
		SetWindowLong(m_hWnd, GWL_STYLE, WS_POPUP);
		SetWindowPos(m_hWnd,HWND_TOP,0,0,0,0,SWP_FRAMECHANGED|SWP_NOMOVE|SWP_NOSIZE);
	}

	// 计算窗口客户区大小以及边框大小
	RECT rcInt;   // rcInt相当于rcClient
	RECT rcExt;   // rcExt相当于rcWindow
	::GetWindowRect(m_hWnd, &rcExt);	// 计算边框的大小
	::GetClientRect(m_hWnd, &rcInt);
	INT x = (rcExt.right-rcExt.left) - (rcInt.right-rcInt.left);
	INT y = (rcExt.bottom-rcExt.top) - (rcInt.bottom-rcInt.top);

	if( m_bShowTaskBar )
	{
		// 是否工作区塞不下这么大的窗口
		RECT rc = {0, m_nWidth, 0, m_nHeight};
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);	// 得到工作区的大小(不含任务栏的区域)
		if( m_nWidth + x > rc.right - rc.left || m_nHeight + y > rc.bottom - rc.top )
		{
			::GetWindowRect(m_hWnd, &rcExt);	// 计算边框的大小
			::GetClientRect(m_hWnd, &rcInt);
			x = (rcExt.right-rcExt.left) - (rcInt.right-rcInt.left);
			y = (rcExt.bottom-rcExt.top) - (rcInt.bottom-rcInt.top);
			m_nWidth = rc.right - rc.left - x;
			m_nHeight = rc.bottom - rc.top - y;
		}
	}

	// 最终窗口大小
	SetWindowPos(m_hWnd, HWND_TOP, 0, 0, m_nWidth + x, m_nHeight + y, SWP_SHOWWINDOW|SWP_NOMOVE);

	ShowWindow(m_hWnd, SW_SHOWNORMAL);
	UpdateWindow(m_hWnd);

	::SetCursor(m_hCursorCurrent);
	return TRUE;
}


//-----------------------------------------------------------------------------
// set window caption
//-----------------------------------------------------------------------------
VOID WindowWrap::SetCaption(LPCTSTR szCaption)
{
	ASSERT(szCaption);

	HWND hWnd = (HWND)TObjRef<VarContainer>()->GetDword(_T("HWND"));
	::SetWindowText(hWnd, szCaption);
}



//-----------------------------------------------------------------------------
// set cursor
//-----------------------------------------------------------------------------
VOID WindowWrap::SetCursor(LPCTSTR szCursorName)
{
	if( 0 == _tcscmp(szCursorName, _T("ibeam")) )
	{
		m_hCursorCurrent = m_hCursorIbeam;
	}
	else if( 0 == _tcscmp(szCursorName, _T("size")) )
	{
		m_hCursorCurrent = m_hCursorSize;
	}
	else if( 0 == _tcscmp(szCursorName, _T("hand")) )
	{
		m_hCursorCurrent = m_hCursorHand;
	}
	else
	{
		m_hCursorCurrent = m_hCursorArrow;
	}

	// 使其他线程也能调用
	DWORD dwMainThreadID = TObjRef<WindowWrap>()->GetMainThreadID();
	DWORD dwCurrentThreadID = GetCurrentThreadId();
	if( dwMainThreadID != dwCurrentThreadID )
		AttachThreadInput(dwCurrentThreadID, dwMainThreadID, TRUE);
	
	::SetCursor(m_hCursorCurrent);

	if( dwMainThreadID != dwCurrentThreadID )
		AttachThreadInput(dwCurrentThreadID, dwMainThreadID, FALSE);
}


//-----------------------------------------------------------------------------
// push cursor
//-----------------------------------------------------------------------------
VOID WindowWrap::PushCursor(LPCTSTR szCursorName)
{
	SetCursor(szCursorName);
	m_listCursor.push_back(szCursorName);
}


//-----------------------------------------------------------------------------
// pop cursor
//-----------------------------------------------------------------------------
VOID WindowWrap::PopCursor()
{
	m_listCursor.pop_back();
	if( !m_listCursor.empty() )
		SetCursor(m_listCursor.back().c_str());
	else
		SetCursor(_T("arrow"));
}


//-----------------------------------------------------------------------------
// window proc
//-----------------------------------------------------------------------------
LRESULT WindowWrap::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
	case WM_ERASEBKGND:	// 处理窗口被菜单等东西覆盖的重画
		if( m_pRender )
			m_pRender->Present();	
		return false;
	case WM_DESTROY:
		PostQuitMessage(0);	
		break;
	case WM_ACTIVATEAPP:
	case WM_ACTIVATE:
		m_bWindowActive = (wParam != 0);
		if( FALSE == m_bWindowActive )
			::ClipCursor(NULL);	
		break;

	case WM_LBUTTONDOWN:	// 拖动时限制鼠标移动范围
		if( m_bWindowActive )
		{
			RECT rcWindow;	
			::GetClientRect(hWnd, &rcWindow);
			::ClientToScreen(hWnd, (LPPOINT)&rcWindow);
			::ClientToScreen(hWnd, (LPPOINT)&rcWindow + 1);
			::ClipCursor(&rcWindow);
		}
		break;

	case WM_LBUTTONUP:	// 释放先前设定的鼠标移动范围
		if( m_bWindowActive )
			::ClipCursor(NULL);	
		break;

	case WM_MOVE: // 更新窗口坐标系
	case WM_SIZE:
		if( m_bWindowActive && m_pRender )
			m_pRender->UpdateWindowPos();
		break;

	case WM_SETCURSOR:  // 光标由外部设置
		/*::SetCursor(m_hCursorCurrent);*/
		OnWindowsMsg(WM_SETCURSOR, (DWORD)wParam, (DWORD)lParam);
		return true;

    case WM_SYSCOMMAND:
		switch (wParam)
		{
		case SC_TASKLIST:
		case SC_SCREENSAVE:
		case SC_MONITORPOWER:
        case SC_KEYMENU:
        case SC_MOUSEMENU:
			return false;
		case SC_CLOSE:
			PostMessage(hWnd, WM_COMMAND, IDM_FILE_EXIT, 0);
			OnWindowsMsg(WM_QUIT, 0, 0);	// 同时发送一个退出消息
			return false;
		default:
			OnWindowsMsg((DWORD)msg, (DWORD)wParam, (DWORD)lParam);
			break;
		}
		break;

	case WM_SYSKEYUP:
		if( wParam == VK_MENU || wParam == VK_F10 ) // 防止产生SC_KEYMENU系统命令
			return false;
		break;
	}

	//if( m_bWindowActive )
	{
		switch( msg )
		{
		case WM_CHAR:
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MOUSEMOVE:
		case WM_MOUSEWHEEL:
		case WM_SYSKEYDOWN:
		case WM_SYSCHAR:
		case WM_SYSKEYUP:
		case WM_INPUTLANGCHANGE:
		case WM_IME_SETCONTEXT:				
		case WM_INPUTLANGCHANGEREQUEST:
		case WM_VENGINE_USER:
		case WM_COMMAND:
			OnWindowsMsg((DWORD)msg, (DWORD)wParam, (DWORD)lParam);
			break;
		case WM_IME_STARTCOMPOSITION:
		case WM_IME_ENDCOMPOSITION:
		case WM_IME_NOTIFY:
		case WM_IME_COMPOSITION:
			{
				OnWindowsMsg((DWORD)msg, (DWORD)wParam, (DWORD)lParam);
				if( m_bInvisibleIme )
					return false;
			}
			break;
		}
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}






} // namespace vEngine {
