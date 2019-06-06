//-----------------------------------------------------------------------------
//!\file d3d_interface.cpp
//!\author Lyp
//!
//!\date 2002-03-08
//! last 2005-11-18
//!
//!\brief d3d interface
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "render.h"	// 基类
#include "d3d_interface.h"
#include "..\console\console_gui.h"

// 使用其它部件
#include "..\variable\var_container.h"

namespace vEngine {
//-----------------------------------------------------------------------------
// Construction/distruction
//-----------------------------------------------------------------------------
D3DRender::D3DRender() 
{
	m_hWnd               = 0L;
	m_pDD                = NULL;
	m_pD3D               = NULL;
    m_pd3dDevice         = NULL;
    m_pddsFrontBuffer    = NULL;
    m_pddsBackBuffer     = NULL;

	ZeroMemory(&m_PixelFormat, sizeof(DDPIXELFORMAT));
	ZeroMemory(&m_PixelFormat32, sizeof(DDPIXELFORMAT));
	ZeroMemory(&m_rcWindow, sizeof(RECT));

	m_pLastTexture		= NULL;		// 优化用
	m_pLastTarget		= NULL;

	// 临时顶点中不需改动的值
	for( INT n=0; n<4; n++)
	{
		m_Mesh4[n].sz = m_Mesh4[n].rhw = .5f;
		m_Mesh4[n].specular = 0xffffffff;
	}
}

D3DRender::~D3DRender() 
{
	Destroy();
}


//-----------------------------------------------------------------------------
// 建立显示器, pGUID 指向Driver的 GUID
//-----------------------------------------------------------------------------
BOOL D3DRender::Init(INT nWidth, INT nHeight)
{
	m_hWnd		= (HWND)m_pGlobeVar->GetDword(_T("HWND"));
	m_nWidth	= nWidth;
	m_nHeight	= nHeight;

    // DDraw stuff begins here
    if(FAILED( DirectDrawCreateEx(NULL, (VOID**)&m_pDD, IID_IDirectDraw7, NULL) ))
	{
		IMSG(_T("could not create DirectDraw object\r\n"));
		return FALSE;
	}

	if(FAILED( CreateWindowedDisplay(nWidth, nHeight) ))
	{
		IMSG(_T("could not create Display\r\n"));
		return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 清除部件
//-----------------------------------------------------------------------------
VOID D3DRender::Destroy()
{
	
	SAFE_RELEASE(m_pd3dDevice);
	SAFE_RELEASE(m_pD3D);
	SAFE_RELEASE(m_pddsBackBuffer);
	SAFE_RELEASE(m_pddsFrontBuffer);
	
	if( m_pDD )
	{
		m_pDD->SetCooperativeLevel(m_hWnd, DDSCL_NORMAL);
		m_pDD->RestoreDisplayMode();
	}

	SAFE_RELEASE(m_pDD);
}



//-----------------------------------------------------------------------------
// 建立窗口显示方式
//-----------------------------------------------------------------------------
HRESULT D3DRender::CreateWindowedDisplay(INT nWidth, INT nHeight)
{
    // Set cooperative level
    HRESULT hr = m_pDD->SetCooperativeLevel(m_hWnd, DDSCL_NORMAL);
    if( FAILED(hr) )
	{
		IMSG(_T("could not set cooperative level to normal\r\n"));
		return hr;
	}

	LPDIRECTDRAWCLIPPER pcClipper;
    
    // Create the primary surface
    DDSURFACEDESC2 ddsd;
    ZeroMemory (&ddsd, sizeof (ddsd));
    ddsd.dwSize         = sizeof (ddsd);
    ddsd.dwFlags        = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	hr = m_pDD->CreateSurface(&ddsd, &m_pddsFrontBuffer, NULL);
    if( FAILED(hr) )
	{
		IMSG(_T("could not create front buffer\r\n"));
		return hr;
	}

    // Create the backbuffer surface
    ddsd.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;    
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;	// DDSCAPS_BACKBUFFER 
    ddsd.dwWidth        = (DWORD)nWidth;
    ddsd.dwHeight       = (DWORD)nHeight;
	hr = m_pDD->CreateSurface( &ddsd, &m_pddsBackBuffer, NULL );
    if( FAILED(hr) )
	{
		IMSG(_T("could not create back buffer\r\n"));
		return hr;
	}

	hr = m_pDD->CreateClipper (0, &pcClipper, NULL);
	if( FAILED(hr) )
	{
		IMSG(_T("could not create clipper object\r\n"));
		return hr;
	}
    
	hr = pcClipper->SetHWnd(0, m_hWnd);
    if( FAILED (hr) )
    {
        pcClipper->Release();
        IMSG(_T("could not set HWnd for clipper object\r\n"));
		return hr;
    }

	hr = m_pddsFrontBuffer->SetClipper(pcClipper);
    if (FAILED (hr))
    {
        pcClipper->Release();
        IMSG(_T("could not attach clipper to front buffer\r\n"));
		return hr;
    }

    // Done with clipper
    pcClipper->Release();

	hr = m_pDD->QueryInterface(IID_IDirect3D7, (VOID**)&m_pD3D);
    if( FAILED(hr) )
	{
		IMSG(_T("could not find IID_IDirect3D7 interface\r\n"));
		return hr;
	}
        
	hr = m_pD3D->CreateDevice(IID_IDirect3DTnLHalDevice, m_pddsBackBuffer, &m_pd3dDevice);
	if (FAILED(hr))
    {
		// LxC: Add the following code.
		// It seems the IID_IDirect3DTnLHalDevice COM instance could not be instantiated.
		hr = m_pD3D->CreateDevice(IID_IDirect3DHALDevice, m_pddsBackBuffer, &m_pd3dDevice);
		if (FAILED(hr))
		{
			IMSG(_T("could not find 3D hardware\r\n"));
			return hr;
		}
    }
    
    hr = m_pd3dDevice->SetRenderTarget(m_pddsBackBuffer, 0L);
	if( FAILED(hr) )
	{
		IMSG(_T("could not set render render target\r\n"));
		return hr;
	}

	this->Clear((DWORD)m_pddsBackBuffer, 0, 0);

	hr = this->GetCaps(true);
	if( FAILED(hr) )
	{
		IMSG(_T("could not get hardware ability information\r\n"));
		return hr;
	}

	// 设置基本的 State
    SetD3DState();
	
	/*
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
*/
    return S_OK;
}




//-----------------------------------------------------------------------------
// 建立各种DirectDraw空表面
//-----------------------------------------------------------------------------
DWORD D3DRender::CreateSurface(INT nWidth, INT nHeight, BOOL bAlpha, BOOL bTarget)
{
    DDSURFACEDESC2 ddsd;
	ZeroMemory (&ddsd, sizeof(ddsd));
	if( bTarget )
	{
		ddsd.dwSize          = sizeof(ddsd);
		ddsd.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;//|DDSD_PIXELFORMAT|DDSD_TEXTURESTAGE;    
		ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_3DDEVICE;	// DDSCAPS_BACKBUFFER 
		ddsd.dwWidth        = (DWORD)nWidth;
		ddsd.dwHeight       = (DWORD)nHeight;
		//ddsd.ddpfPixelFormat = bAlpha ? m_PixelFormat32 : m_PixelFormat;
	}
	else
	{
		ddsd.dwSize          = sizeof(ddsd);
		ddsd.dwFlags         = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT|DDSD_TEXTURESTAGE;
		ddsd.ddsCaps.dwCaps  = DDSCAPS_TEXTURE;
		ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
		ddsd.dwWidth = (DWORD)nWidth;
		ddsd.dwHeight = (DWORD)nHeight;
		ddsd.ddpfPixelFormat = bAlpha ? m_PixelFormat32 : m_PixelFormat;
	}

	DWORD dwReturn = 0;
    HRESULT hr = m_pDD->CreateSurface(&ddsd, (LPDIRECTDRAWSURFACE7*)&dwReturn, NULL);
	this->Clear(dwReturn, 0, 0xffffffff);
	return dwReturn;
}



//-----------------------------------------------------------------------------
// release surface
//-----------------------------------------------------------------------------
VOID D3DRender::ReleaseSurface(DWORD dwSurfaceHandle)
{
	LPDIRECTDRAWSURFACE7 pdds = (LPDIRECTDRAWSURFACE7)dwSurfaceHandle;
	SAFE_RELEASE(pdds);
}



//-----------------------------------------------------------------------------
//param dwSurfaceHandle=0代表要取得后台缓冲的HDC
//-----------------------------------------------------------------------------
DWORD D3DRender::GetDC(DWORD dwSurfaceHandle)
{
	HDC hDC = 0;
	if( !P_VALID(dwSurfaceHandle) )
	{
		if( DD_OK == m_pddsBackBuffer->GetDC(&hDC) )
			return (DWORD)hDC;
		else
			return GT_INVALID;
	}
	
	if( DD_OK == ((LPSURFACE)dwSurfaceHandle)->GetDC(&hDC) )
		return (DWORD)hDC; 
	else 
		return GT_INVALID;	
}


//-----------------------------------------------------------------------------
// release dc
//-----------------------------------------------------------------------------
VOID D3DRender::ReleaseDC(DWORD dwHandle, DWORD dwDC)
{
	ASSERT(P_VALID(dwDC));

	if( !P_VALID(dwHandle) )
	{
		m_pddsBackBuffer->ReleaseDC((HDC)dwDC);
		return;
	}

	((LPSURFACE)dwHandle)->ReleaseDC((HDC)dwDC);
}




//-----------------------------------------------------------------------------
// 使用指定颜色清除一块区域
//-----------------------------------------------------------------------------
VOID D3DRender::Clear(DWORD dwHandle, const tagRect* prc, DWORD dwColor)
{
	LPSURFACE pDest = m_pddsBackBuffer;
	if( P_VALID(dwHandle) ) 
		pDest = (LPSURFACE)dwHandle;

	D3DVIEWPORT7 view;
	if( !P_VALID(prc) && !P_VALID(dwHandle) )
	{
		if( m_pddsBackBuffer != m_pLastTarget )
		{
			m_pd3dDevice->SetRenderTarget(m_pddsBackBuffer, 0);
			view.dwX = view.dwY = 0;
			view.dwWidth = m_nWidth;
			view.dwHeight = m_nHeight;
			view.dvMaxZ = view.dvMinZ = 1.0f;
			m_pd3dDevice->SetViewport(&view);
			m_pLastTarget = m_pddsBackBuffer;
		}
		m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, dwColor, 1.0f, 0L);
		m_pd3dDevice->BeginScene();
		return;
	}

	if( pDest != m_pLastTarget )
	{
		m_pd3dDevice->SetRenderTarget(pDest, 0);
		DDSURFACEDESC2	ddsd;
		ddsd.dwSize = sizeof(ddsd);
		pDest->GetSurfaceDesc(&ddsd);
		view.dwX = view.dwY = 0;
		view.dwWidth = ddsd.dwWidth;
		view.dwHeight = ddsd.dwHeight;
		view.dvMaxZ = view.dvMinZ = 1.0f;
		m_pd3dDevice->SetViewport(&view);
		m_pLastTarget = pDest;
	}
	INT nCount = prc ? 1 : 0;
	m_pd3dDevice->Clear(nCount, (LPD3DRECT)prc, D3DCLEAR_TARGET, dwColor, 1.0f, 0);
	
}




//-----------------------------------------------------------------------------
// 将窗口client坐标转换为窗口view坐标(view坐标为逻辑坐标)
//-----------------------------------------------------------------------------
VOID D3DRender::ClientToView(tagPoint& pt)
{
	if( ( m_rcWindow.right == m_rcWindow.left )
		|| ( m_rcWindow.bottom == m_rcWindow.top ) )
		return;	// 防止被0除

	pt.x = pt.x * m_nWidth / (m_rcWindow.right - m_rcWindow.left);
	pt.y = pt.y * m_nHeight / (m_rcWindow.bottom - m_rcWindow.top);
}



//-----------------------------------------------------------------------------
// draw
//-----------------------------------------------------------------------------
VOID D3DRender::Draw(DWORD dwDestHandle, DWORD dwSrcHandle, const tagRect* prcDest, 
					  const tagRect* prcSrc, DWORD dwColor)
{

	LPSURFACE pDest = P_VALID(dwDestHandle) ? (LPSURFACE)dwDestHandle : m_pddsBackBuffer;
	LPSURFACE pSrc = (LPSURFACE)dwSrcHandle;

	DDSURFACEDESC2	ddsd;
	ddsd.dwSize = sizeof(ddsd);
	pSrc->GetSurfaceDesc(&ddsd);

	float u1 = (float)prcSrc->left / ddsd.dwWidth;
	float u2 = (float)prcSrc->right / ddsd.dwWidth;
	float v1 = (float)prcSrc->top / ddsd.dwHeight;
	float v2 = (float)prcSrc->bottom / ddsd.dwHeight;

	float x1 = (float)prcDest->left-0.5f; 
	float y1 = (float)prcDest->top-0.5f;
	float x2 = (float)prcDest->right-0.5f; 
	float y2 = (float)prcDest->bottom-0.5f;

	// 建立临时三维网格
	SetTLVertex(&m_Mesh4[0], x1, y1, u1, v1, dwColor);
	SetTLVertex(&m_Mesh4[1], x2, y1, u2, v1, dwColor);
	SetTLVertex(&m_Mesh4[2], x1, y2, u1, v2, dwColor);
	SetTLVertex(&m_Mesh4[3], x2, y2, u2, v2, dwColor);	

	// 作图
	if( pSrc != m_pLastTexture )
	{
		m_pd3dDevice->SetTexture(0, pSrc);
		m_pLastTexture = pSrc;
	}

	if( pDest != m_pLastTarget )
	{
		m_pd3dDevice->SetRenderTarget(pDest, 0);
		pDest->GetSurfaceDesc(&ddsd);
		D3DVIEWPORT7 view;
		view.dwX = 0;
		view.dwY = 0;
		view.dwWidth = ddsd.dwWidth;
		view.dwHeight = ddsd.dwHeight;
		view.dvMaxZ = view.dvMinZ = 1.0f;
		m_pd3dDevice->SetViewport(&view);
		m_pLastTarget = pDest;
	}

	m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, m_Mesh4, 4, NULL);
}



//-----------------------------------------------------------------------------
// 更新屏幕
//-----------------------------------------------------------------------------
BOOL D3DRender::Render()
{
	static DWORD dwLastTick = timeGetTime();
	static INT nFrameCounter = 0;
	static TCHAR sFpsBuf[64]={0};

	if( !P_VALID(m_pd3dDevice) )
		return FALSE;

	m_pd3dDevice->EndScene();

	if( m_bShowFPS )
	{
		++nFrameCounter;
		DWORD dwCurrentTick = timeGetTime();
		if( dwCurrentTick - dwLastTick >= 200 )	// 100 毫秒刷新1次
		{
			nFrameCounter = nFrameCounter * 1000 / (dwCurrentTick - dwLastTick);
			wsprintf( sFpsBuf, _T("fps%d"), nFrameCounter );
			nFrameCounter = 0;
			dwLastTick = dwCurrentTick;
		}

		HDC hdc;
		if( m_pddsBackBuffer->GetDC(&hdc) == DD_OK )
		{
			SetBkMode( hdc, OPAQUE );
			SetTextColor( hdc, RGB(0,0,0) );
			TextOut( hdc, 0, 0, sFpsBuf, lstrlen(sFpsBuf) );
			m_pddsBackBuffer->ReleaseDC( hdc );
		}
	}

	HRESULT hr = m_pddsFrontBuffer->Blt((LPRECT)&m_rcWindow, m_pddsBackBuffer,
		NULL, DDBLT_DONOTWAIT, NULL );

	if( hr == DDERR_SURFACELOST )
	{
		m_pddsFrontBuffer->Restore();
		m_pddsBackBuffer->Restore();
		return FALSE;
	}

	return TRUE;
}





//-----------------------------------------------------------------------------
// UpdateWindowPos
//-----------------------------------------------------------------------------
VOID D3DRender::UpdateWindowPos()
{
	HWND hWnd = (HWND)m_pGlobeVar->GetDword(_T("HWND"));
	if( hWnd )
	{
		::GetClientRect(hWnd, (LPRECT)&m_rcWindow);
		::ClientToScreen(hWnd, (LPPOINT)&m_rcWindow);
		::ClientToScreen(hWnd, (LPPOINT)&m_rcWindow + 1 );
	}

	if( P_VALID(m_pddsFrontBuffer) )
		m_pddsFrontBuffer->Blt((LPRECT)&m_rcWindow, m_pddsBackBuffer, NULL, DDBLT_WAIT, NULL);
}


//-----------------------------------------------------------------------------
// 获取硬件支援信息， 并记录纹理格式
//-----------------------------------------------------------------------------
HRESULT D3DRender::GetCaps(bool bRunTest)
{
	// 寻找合适的16位色格式
	TEXTURESEARCHINFO tsi;

	// 检查是否支持32位色纹理
	tsi.pddpf				= &m_PixelFormat;
	tsi.dwDesiredBPP		= 32;
	tsi.dwAlphaBitMask		= 0;
	HRESULT hr = m_pd3dDevice->EnumTextureFormats (TextureSearchCallback, &tsi);
	if( FAILED (hr) )
		return hr;

	// 检查是否支持32位色纹理
	tsi.pddpf				= &m_PixelFormat32;
	tsi.dwDesiredBPP		= 32;
	tsi.dwAlphaBitMask		= 0xff000000;
	return m_pd3dDevice->EnumTextureFormats (TextureSearchCallback, &tsi);
}



//-----------------------------------------------------------------------------
// 设置常用的纹理以及渲染状态
//-----------------------------------------------------------------------------
VOID D3DRender::SetD3DState()
{
	// 设置基本的 State
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,	D3DTOP_MODULATE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1,	D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	// 线性过滤参数
    m_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_POINT);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFG_POINT);
	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_DITHERENABLE, FALSE);
    
	// 关闭纹理透视纠正
	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, FALSE);
    
	// 关闭 Z 缓冲
    m_pd3dDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_FALSE);
	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, FALSE);
	   
       
    // Set diffuse blending for alpha set in vertices. 
    m_pd3dDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
    m_pd3dDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pd3dDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// 关闭硬件光源支持
    m_pd3dDevice->SetRenderState(D3DRENDERSTATE_LIGHTING, FALSE);

	// 打开Alpha测试(avoids drawing pixels with less than a certain	alpha)
    m_pd3dDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, TRUE);
    m_pd3dDevice->SetRenderState(D3DRENDERSTATE_ALPHAREF, 0x08);
    m_pd3dDevice->SetRenderState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_CLIPPING, FALSE);

	 

}



//-----------------------------------------------------------------------------
// Name: TextureSearchCallback()
// Desc: 纹理列举回调函数
//-----------------------------------------------------------------------------
HRESULT CALLBACK D3DRender::TextureSearchCallback(DDPIXELFORMAT* pddpf,VOID* param)
{
	if( NULL==pddpf || NULL==param )	
		return DDENUMRET_OK;

    TEXTURESEARCHINFO* ptsi = (TEXTURESEARCHINFO*)param;

	// Check if we found a good match
    if (pddpf->dwRGBBitCount == ptsi->dwDesiredBPP)
    {
		if (ptsi->dwAlphaBitMask == pddpf->dwRGBAlphaBitMask)
		{
			memcpy( ptsi->pddpf, pddpf, sizeof(DDPIXELFORMAT) );
			return DDENUMRET_CANCEL;
		}
    }

    return DDENUMRET_OK;
}



} // namespace vEngine {
