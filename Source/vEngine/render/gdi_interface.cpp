//-----------------------------------------------------------------------------
// File: gdi_interface
// Desc: gdi interface
// Auth: Lyp
// Date: 2007/1/30
// Last: 2007/1/30
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "render.h"	// 基类
#include "gdi_interface.h"

// 使用其它部件
#include "..\variable\var_container.h"

namespace vEngine {


//-----------------------------------------------------------------------------
// construction / destruction
//-----------------------------------------------------------------------------
GDIRender::GDIRender()
{
	m_hWnd = 0;
	m_hdcBackBuff = 0;
	m_hbmpBackBuff = 0;
	m_nBitsPerPel = 32;

	m_hDllModule = LoadLibrary(_T("Msimg32.dll"));
	if( m_hDllModule )
		m_fpAlphaBlend = (fpAlphaBlend*)GetProcAddress(m_hDllModule, "AlphaBlend");
}

GDIRender::~GDIRender()
{
	this->Destroy();
	if( m_hDllModule )
		FreeLibrary(m_hDllModule);
}


//-----------------------------------------------------------------------------
// init direct draw
//-----------------------------------------------------------------------------
BOOL GDIRender::Init(INT nWidth, INT nHeight)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;

	// get handle of main window
	m_hWnd = (HWND)m_pGlobeVar->GetDword(_T("HWND"));

	// 调节窗口客户区为要求的大小
	RECT rcInt;	// rcInt相当于rcClient
	RECT rcExt;	// rcExt相当于rcWindow

	// 计算边框的大小
	::GetWindowRect(m_hWnd, &rcExt);
	::GetClientRect(m_hWnd, &rcInt);
	INT x = (rcExt.right-rcExt.left) - (rcInt.right-rcInt.left);
	INT y = (rcExt.bottom-rcExt.top) - (rcInt.bottom-rcInt.top);

	// 新的窗口大小
	SetWindowPos(m_hWnd, HWND_TOP, 0, 0, nWidth+x, nHeight+y, SWP_SHOWWINDOW);

	// 得到窗口显示区的实际坐标
	::GetClientRect(m_hWnd, (LPRECT)&m_rcWindow);
	::ClientToScreen(m_hWnd, (LPPOINT)&m_rcWindow);
	::ClientToScreen(m_hWnd, (LPPOINT)&m_rcWindow + 1);

	m_hdcBackBuff = CreateCompatibleDC(0); 
 
	// Create a compatible bitmap for hdcScreen. 
	m_hbmpBackBuff = CreateBitmap(nWidth, nHeight, 1, m_nBitsPerPel, NULL); 
	if( !SelectObject(m_hdcBackBuff, m_hbmpBackBuff) )
	{
		DeleteObject(m_hbmpBackBuff);
		m_nBitsPerPel = 16;
		m_hbmpBackBuff = CreateBitmap(nWidth, nHeight, 1, m_nBitsPerPel, NULL); 
		if( !SelectObject(m_hdcBackBuff, m_hbmpBackBuff) )
		{
			DeleteObject(m_hbmpBackBuff);
			m_nBitsPerPel = 8;
			m_hbmpBackBuff = CreateBitmap(nWidth, nHeight, 1, m_nBitsPerPel, NULL); 
			if( !SelectObject(m_hdcBackBuff, m_hbmpBackBuff) )
			{
				ERR( _T("Error in bitmap selection\n") );
				return FALSE;
			}
		}
	}

	SetBitmapDimensionEx(m_hbmpBackBuff, nWidth, nHeight, NULL);
	SetBkColor( m_hdcBackBuff, RGB(0,0,0) );
	return TRUE;
}


//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
VOID GDIRender::Destroy()
{
	::DeleteObject(m_hbmpBackBuff);
	::DeleteDC(m_hdcBackBuff);
}

// handle exception -- added by Jay 20091125
static BOOL SafeStretchBlt(__in HDC hdcDest, __in int xDest, __in int yDest, __in int wDest, __in int hDest, __in HDC hdcSrc, __in int xSrc, __in int ySrc, __in int wSrc, __in int hSrc, __in DWORD rop)
{
	static INT nErrorCount = 0;
	__try{
		return ::StretchBlt( hdcDest, xDest, yDest, wDest, hDest,hdcSrc, xSrc, ySrc, wSrc, hSrc, rop);
	}
	__except(1)//continue
	{		
		nErrorCount++;	
		if(nErrorCount < 5) // first
		{
			TObjRef<Log> logObj;
			logObj->Write(_T("Exception cought in SafeStretchBlt Error Count(%d)"), nErrorCount);
		}
	}	
	return FALSE;
}


//-----------------------------------------------------------------------------
// render
//-----------------------------------------------------------------------------
BOOL GDIRender::Render()
{
	static DWORD dwLastTick = timeGetTime();
	static INT nFrameCounter = 0;
	static TCHAR sFpsBuf[64]={0};

	if( m_hWnd == 0 )
		return FALSE;

	if( m_bShowFPS )
	{
		++nFrameCounter;
		DWORD dwCurrentTick = timeGetTime();
		if( dwCurrentTick - dwLastTick >= 200 )	// 200 毫秒刷新1次
		{
			nFrameCounter = nFrameCounter * 1000 / (dwCurrentTick - dwLastTick);
			wsprintf( sFpsBuf, _T("fps%d"), nFrameCounter );
			nFrameCounter = 0;
			dwLastTick = dwCurrentTick;
		}

		SetBkMode( m_hdcBackBuff, OPAQUE );
		SetBkColor( m_hdcBackBuff, RGB(255,255,255));
		SetTextColor( m_hdcBackBuff, RGB(0,0,0) );
		TextOut( m_hdcBackBuff, 0, 0, sFpsBuf, lstrlen(sFpsBuf) );
	}

	HDC hdc = ::GetDC(m_hWnd);
	if( hdc )
	{
		SafeStretchBlt( hdc, 0, 0, m_rcWindow.right - m_rcWindow.left, m_rcWindow.bottom - m_rcWindow.top,
			m_hdcBackBuff, 0, 0, m_nWidth, m_nHeight, SRCCOPY);
		/*::StretchBlt( hdc, 0, 0, m_rcWindow.right - m_rcWindow.left, m_rcWindow.bottom - m_rcWindow.top,
			m_hdcBackBuff, 0, 0, m_nWidth, m_nHeight, SRCCOPY);*/
		::ReleaseDC(m_hWnd, hdc);			
	}

	return TRUE;
}


//-----------------------------------------------------------------------------
// UpdateWindowPos
//-----------------------------------------------------------------------------
VOID GDIRender::UpdateWindowPos()
{
	if( m_hWnd )
	{
		::GetClientRect(m_hWnd, (LPRECT)&m_rcWindow);
		::ClientToScreen(m_hWnd, (LPPOINT)&m_rcWindow);
		::ClientToScreen(m_hWnd, (LPPOINT)&m_rcWindow + 1 );
	}
	else
		return;

	HDC hdc = ::GetDC(m_hWnd);
	if( hdc )
	{
		StretchBlt( hdc, 0, 0, m_rcWindow.right - m_rcWindow.left, m_rcWindow.bottom - m_rcWindow.top,
			m_hdcBackBuff, 0, 0, m_nWidth, m_nHeight, SRCCOPY);
		::ReleaseDC(m_hWnd, hdc);
	}
}



//-----------------------------------------------------------------------------
// CreateSurface
//-----------------------------------------------------------------------------
DWORD GDIRender::CreateSurface(INT nWidth, INT nHeight, BOOL bAlpha, BOOL bTarget)
{
	HDC hdcScreen = CreateDC(_T("DISPLAY"), NULL, NULL, NULL); 
	HDC hdc = CreateCompatibleDC(hdcScreen); 

	// Create a compatible bitmap for hdcScreen. 
	HBITMAP hbmp = CreateBitmap(nWidth, nHeight, 1, m_nBitsPerPel, NULL); 
	if( hbmp == 0 )
	{
		::DeleteDC(hdcScreen);
		::DeleteDC(hdc);
		return GT_INVALID;
	}
 
	// Select the bitmaps into the compatible DC. 
	if( !SelectObject(hdc, hbmp) )
	{
		DeleteObject(hbmp);
		::DeleteDC(hdcScreen);
		::DeleteDC(hdc);
		return GT_INVALID;
	}

	SetBitmapDimensionEx(hbmp, nWidth, nHeight, NULL);
	SetTextColor( hdc, RGB(255,255,255) );
	SetBkColor( hdc, RGB(0,0,0) );
	::DeleteDC(hdcScreen);
	return (DWORD)hdc; 
}


//-----------------------------------------------------------------------------
// release surface
//-----------------------------------------------------------------------------
VOID GDIRender::ReleaseSurface(DWORD dwSurfaceHandle)
{
	HGDIOBJ hbmp = ::GetCurrentObject((HDC)dwSurfaceHandle, OBJ_BITMAP);

	::DeleteObject(hbmp);
	::DeleteDC((HDC)dwSurfaceHandle);
}



//-----------------------------------------------------------------------------
//param dwSurfaceHandle=0代表要取得后台缓冲的HDC
//-----------------------------------------------------------------------------
DWORD GDIRender::GetDC(DWORD dwSurfaceHandle)
{
	if( 0 == dwSurfaceHandle )
		return (DWORD)m_hdcBackBuff;
	
	return dwSurfaceHandle;	
}


//-----------------------------------------------------------------------------
// release dc
//-----------------------------------------------------------------------------
VOID GDIRender::ReleaseDC(DWORD dwSurfaceHandle, DWORD dwDC)
{
	// 不需要此函数
	return;
}


//-----------------------------------------------------------------------------
// clear
//-----------------------------------------------------------------------------
VOID GDIRender::Clear(DWORD dwHandle, const tagRect* prc, DWORD dwColor)
{
	dwColor = ARGB2GDI(dwColor);

	HBRUSH hBrush = CreateSolidBrush(dwColor);
	HDC hdc = (HDC)dwHandle;
	if( !P_VALID(hdc) )
		hdc = m_hdcBackBuff;

	if( P_VALID(prc) )
	{
		HRGN hRgn = ::CreateRectRgn(prc->left, prc->top, prc->right, prc->bottom);
		FillRgn(hdc, hRgn, hBrush);
		DeleteObject(hRgn);
	}
	else
	{
		SIZE size;
		HGDIOBJ bmp = ::GetCurrentObject(hdc, OBJ_BITMAP);
		GetBitmapDimensionEx((HBITMAP)bmp, &size);
		HRGN hRgn = CreateRectRgn(0, 0, size.cx, size.cy);
		FillRgn(hdc, hRgn, hBrush);
		DeleteObject(hRgn);
	}

	DeleteObject(hBrush);

}


//-----------------------------------------------------------------------------
// draw
//-----------------------------------------------------------------------------
VOID GDIRender::Draw(DWORD dwDestHandle, DWORD dwSrcHandle, const tagRect* prcDest, 
					  const tagRect* prcSrc, DWORD dwColor)
{
	if( !P_VALID(dwDestHandle)  )
		dwDestHandle = (DWORD)m_hdcBackBuff;
	
	BLENDFUNCTION blend;
	blend.BlendOp = AC_SRC_OVER;
	blend.BlendFlags = 0;
	blend.SourceConstantAlpha = 255;
	//blend.AlphaFormat = AC_SRC_ALPHA;
	blend.AlphaFormat =0x00;// 如果用AC_SRC_ALPHA，必须要求是32位的;

	if( m_fpAlphaBlend )
	(m_fpAlphaBlend)( (HDC)dwDestHandle, prcDest->left, prcDest->top, 
		prcDest->right-prcDest->left, prcDest->bottom-prcDest->top,
		(HDC)dwSrcHandle, prcSrc->left, prcSrc->top, 
		prcSrc->right-prcSrc->left, prcSrc->bottom-prcSrc->top, blend );
}


//-----------------------------------------------------------------------------
// 将窗口client坐标转换为窗口view坐标(view坐标为逻辑坐标)
//-----------------------------------------------------------------------------
VOID GDIRender::ClientToView(tagPoint& pt)
{
	if( ( m_rcWindow.right == m_rcWindow.left )
		|| ( m_rcWindow.bottom == m_rcWindow.top ) )
		return;	// 防止被0除

	pt.x = pt.x * m_nWidth / (m_rcWindow.right - m_rcWindow.left);
	pt.y = pt.y * m_nHeight / (m_rcWindow.bottom - m_rcWindow.top);
}


}	// namespace vEngine {
