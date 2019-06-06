//-----------------------------------------------------------------------------
// File: Display
// Desc: ddraw interface
// Auth: Lyp
// Date: 2003/11/13
// Last: 2204/03/17
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "render.h"

// 使用其它部件
#include "..\variable\var_container.h"

namespace vEngine {
//-----------------------------------------------------------------------------
// construction / destruction
//-----------------------------------------------------------------------------
IRender::IRender()
{
	m_bShowFullScreenMenu = FALSE;
	m_bShowFPS = TRUE;

	m_nWidth = 0;
	m_nHeight = 0;
}

IRender::~IRender()
{
}


VOID IRender::SetShowFPS(BOOL bShow)
{
	m_bShowFPS = bShow;
}


VOID IRender::SwitchFullScreenMenu()
{

	HWND hWnd = (HWND)m_pGlobeVar->GetDword(_T("HWND"));
	if( !hWnd )
		return;

	m_bShowFullScreenMenu = !m_bShowFullScreenMenu;
	if( m_bShowFullScreenMenu )
	{
		SetWindowLong(hWnd, GWL_STYLE, WS_POPUP| WS_VISIBLE);
		SetWindowPos(hWnd,HWND_TOP,0,0,0,0,
			SWP_FRAMECHANGED|SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE);
	}
	else
	{
		SetWindowLong( hWnd, GWL_STYLE, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE );
		SetWindowPos(hWnd,HWND_TOP,0,0,0,0,SWP_FRAMECHANGED|SWP_NOMOVE|SWP_NOSIZE);
	}
}


} // namespace vEngine {

