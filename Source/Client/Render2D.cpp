//-----------------------------------------------------------------------------
//!\file render2d.cpp
//!\author Lyp
//!
//!\date 2008-03-27
//! last 2008-04-19
//!
//!\brief 龙客户端2d界面渲染接口
//!
//!	Copyright (c) 2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "render2d.h"

#include "AudioSys.h"


// #define _DEBUG_MEM_	// 内存泄漏检测
//-----------------------------------------------------------------------------
// construction /destruction
//-----------------------------------------------------------------------------
GUIRender::~GUIRender()
{
	// 删除所有图片
	 m_mapImageHandle.ResetIterator();
	tagGUIImageHandle* pHandle = NULL;
	while( m_mapImageHandle.PeekNext(pHandle) )
	{
		if( P_VALID(pHandle->dwHandle) )
		{
			ResTexture* pResTexture = (ResTexture*)pHandle->dwHandle;
			IDraw2D::Inst()->ReleaseTexture(pResTexture);
		}

		SAFE_DEL(pHandle);
	}

	// 删除所有字体
	m_mapFont.ResetIterator();
	tagGUIFontEx* pFontEx = NULL;
	while( m_mapFont.PeekNext(pFontEx) )
	{
		if( P_VALID(pFontEx->dwHandle) )
		{
			IFont* pIFont = (IFont*)pFontEx->dwHandle;
			SAFE_DEL(pIFont);
		}
		SAFE_DEL(pFontEx);
	}

	SAFE_DEL(m_pDefaultIFont);


#ifdef _DEBUG_MEM_
	__asm int 3;	// 中断程序以便观察m_mapDebug;
#endif

}


//-----------------------------------------------------------------------------
// Init
//-----------------------------------------------------------------------------
VOID GUIRender::Init()
{ 
	m_pVFS = "VFS_System"; 

	// 创建默认字体
	m_pDefaultIFont = Device()->NewFont();
	m_pDefaultIFont->Create(0, 16, 400, _T("Fixsys"));
	m_dwRTSize = 0;
}



//-----------------------------------------------------------------------------
// 读入图片，失败返回0,成功返回句柄(如果strImage.empty()则返回NULL)
// 如果无法读取指定图片则返回可以设置为RenderTarget的空纹理
//-----------------------------------------------------------------------------
tagGUIImage* GUIRender::CreateImage(const tstring str, const tagRect& rc)
{
	if( str.empty() ) 
		return NULL;
	
	tagGUIImage* pImage = new tagGUIImage;
	if( !P_VALID(pImage) )
		return NULL;

#ifdef _DEBUG_MEM_
	m_mapDebug.Add((DWORD)pImage, str);
#endif

	pImage->dwID = m_pUtil->Crc32(str.c_str());
	pImage->rc = rc;
	pImage->ptSize.x = rc.right - rc.left;
	pImage->ptSize.y = rc.bottom - rc.top;

	tagGUIImageHandle* pHandle = m_mapImageHandle.Peek(pImage->dwID);
	if( P_VALID(pHandle) )	// 图片曾经读入
	{
		pHandle->nRefCount++;
		pImage->dwHandle = pHandle->dwHandle;
		if( rc.IsEmpty() )
		{
			pImage->ptSize = pHandle->ptSize;
			pImage->rc = pHandle->ptSize;
		}
		return pImage;
	}

	// 文件不存在，创建RenderTarget
	if( str.find(_T("\\desktop\\"))!=-1)
	{
		IRenderTexture* pRenderTexture = Device()->NewRenderTexture();
		bool bResult = pRenderTexture->Create(pImage->ptSize.x, pImage->ptSize.y, EPF_A8R8G8B8, false, false, 1, false);
		if( !bResult )	// 创建RenderTarget失败
		{
			SAFE_DEL(pRenderTexture);
			SAFE_DEL(pImage);
			return NULL;
		}

		m_dwRTSize += pImage->ptSize.x * pImage->ptSize.y * sizeof(DWORD);

		// 清空render target
		pRenderTexture->Begin();
		Device()->Clear(true, false, false, 0, 1.0f, 0, 0);
		pRenderTexture->End();
		pImage->dwHandle = (DWORD)pRenderTexture;
		pImage->bRenderTarget = TRUE;
		return pImage;
	}

	ResTexture* pResTexture = NULL;
	if( str.find(_T("temp\\")) == 0 )	// temp目录下下载的文件必须以散包方式读取
		pResTexture = IDraw2D::Inst()->NewTexture(str.c_str(), (IFS*)(m_pVFSDefault.operator->()));
	else
		pResTexture = IDraw2D::Inst()->NewTexture(str.c_str(), (IFS*)(m_pVFS.operator->()));

	pImage->dwHandle = (DWORD)pResTexture;
	INT nWidth = pResTexture->GetDeviceTex()->Width();
	INT nHeight = pResTexture->GetDeviceTex()->Height();

	pHandle = new tagGUIImageHandle;
	if( P_VALID(pHandle) )
	{
		pHandle->dwHandle = pImage->dwHandle;
		pHandle->nRefCount = 1;
		pHandle->ptSize.Set(nWidth, nHeight);
		m_mapImageHandle.Add(pImage->dwID, pHandle);
	}

	if( pImage->rc.IsEmpty() )
	{
		pImage->rc.SetLTWH(0, 0, nWidth, nHeight);
		pImage->ptSize.Set(nWidth, nHeight);
	}

	return pImage;
}


//-----------------------------------------------------------------------------
// destroy image
//-----------------------------------------------------------------------------
VOID GUIRender::DestroyImage(tagGUIImage* pImage)
{
	if( !P_VALID(pImage) )
		return;

	tagGUIImageHandle* pHandle = m_mapImageHandle.Peek(pImage->dwID);
	if( P_VALID(pHandle) )
	{
		pHandle->nRefCount--;
		if( pHandle->nRefCount <= 0 )
		{
			if( P_VALID(pHandle->dwHandle) )
			{
				ResTexture* pResTexture = (ResTexture*)pHandle->dwHandle;
				IDraw2D::Inst()->ReleaseTexture(pResTexture);
			}

			m_mapImageHandle.Erase(pImage->dwID);
			SAFE_DEL(pHandle);
		}
	}

#ifdef _DEBUG_MEM_
	m_mapDebug.Erase((DWORD)pImage);
#endif

	SAFE_DEL(pImage);
}


//-----------------------------------------------------------------------------
// 建立字体
//-----------------------------------------------------------------------------
tagGUIFont* GUIRender::CreateFont(const tstring str, INT nWidth, INT nHeight, INT nWeight
								  , BOOL bAntiAliase)
{
	if( str.empty() )
		return NULL;

	tstringstream ss;
	ss << str << nWidth << nHeight << nWeight << bAntiAliase;
	DWORD dwID = m_pUtil->Crc32(ss.str().c_str());

	tagGUIFontEx* pFont = m_mapFont.Peek(dwID);
	if( !P_VALID(pFont) )
	{
		IFont* pIFont = Device()->NewFont();
		pIFont->Create(nWidth, nHeight, nWeight, str.c_str(), 0, bAntiAliase);
		
		pFont = new tagGUIFontEx;
		pFont->dwID = dwID;
		pFont->dwHandle = (DWORD)pIFont;
		pFont->nRefCount = 1;
		m_mapFont.Add(dwID, pFont);
		return pFont;
	}
	else
	{
		pFont->nRefCount++;
		return pFont;
	}

	return 0;
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
VOID GUIRender::DestroyFont(tagGUIFont* pFont)
{
	if( !P_VALID(pFont) )
		return;

	tagGUIFontEx* pFontEx = (tagGUIFontEx*)pFont;
	pFontEx->nRefCount--;
	if( pFontEx->nRefCount <= 0 )
	{
		if( P_VALID(pFontEx->dwHandle) )
		{
			IFont* pIFont = (IFont*)pFontEx->dwHandle;
			SAFE_DEL(pIFont);
		}
		
		m_mapFont.Erase(pFontEx->dwID);
		SAFE_DEL(pFontEx);
	}
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
tagGUIFont* GUIRender::CloneFont(tagGUIFont* pFont)
{
	if( !P_VALID(pFont) )
		return NULL;

	tagGUIFontEx* pFontEx = (tagGUIFontEx*)pFont;
	pFontEx->nRefCount++;
	return pFontEx;
}


//-----------------------------------------------------------------------------
// 如果!P_VALID(pDest->dwHandle)则渲染到后台缓冲, 如果!P_VALID(pSrc)则画色块
//-----------------------------------------------------------------------------
VOID GUIRender::Draw(const tagGUIImage* pDest, const tagGUIImage* pSrc, DWORD dwColor, 
					 EGUIAlphaWrite eAlphaWrite, FLOAT z)
{
	if( P_VALID(pSrc) )
	{
		if( !P_VALID(pDest->dwHandle) )	// 画到后台缓冲
		{
			if( pSrc->bRenderTarget )
			{
				IDraw2D::Inst()->Draw((const RECT*)&pDest->rc, (const RECT*)&pSrc->rc, 
				(IRenderTexture*)(pSrc->dwHandle), dwColor, ETFilter_POINT, 0, z);
				//TCHAR szPath[256];
				//_stprintf(szPath, _T("d:\\%d.tga"), pSrc->dwHandle);
				//((IRenderTexture*)pSrc->dwHandle)->WriteToFile(szPath);
			}
			else
				IDraw2D::Inst()->Draw((const RECT*)&pDest->rc, (const RECT*)&pSrc->rc, 
					(ResTexture*)pSrc->dwHandle, dwColor);
		}
		else
		{
			tagRect rc = pDest->rc;
			rc = rc - pDest->ptOffset;
			if( pSrc->bRenderTarget )
			{
				IDraw2D::Inst()->Draw((const RECT*)&rc, (const RECT*)&pSrc->rc, (IRenderTexture*)pSrc->dwHandle, dwColor);
			}
			else
			{
				if( eAlphaWrite == EGUIAW_Write )
					IDraw2D::Inst()->Draw((const RECT*)&rc, (const RECT*)&pSrc->rc, (ResTexture*)pSrc->dwHandle, dwColor);
				else if( eAlphaWrite == EGUIAW_NoWrite )
					IDraw2D::Inst()->Draw((const RECT*)&rc, (const RECT*)&pSrc->rc, (ResTexture*)pSrc->dwHandle, dwColor, ETFilter_POINT, 0, IDraw2D::EAWT_NoWrite);
				else 
					IDraw2D::Inst()->Draw((const RECT*)&rc, (const RECT*)&pSrc->rc, (ResTexture*)pSrc->dwHandle, dwColor, ETFilter_POINT, 0, IDraw2D::EAWT_Add);
			}
		}
	}
	else
	{
		if( !(dwColor & 0xff000000) )	// 全透明不用画
			return;

		if( !P_VALID(pDest->dwHandle) )	// 画到后台缓冲
		{
			Device()->Clear(true, false, false, dwColor, 1.0f, 0, (RECT*)&pDest->rc);
		}
		else
		{
			tagRect rc = pDest->rc;
			rc = rc - pDest->ptOffset;
			Device()->Clear(true, false, false, dwColor, 1.0f, 0, (RECT*)&rc);
		}
	}
}



//-----------------------------------------------------------------------------
// draw text
//-----------------------------------------------------------------------------
VOID GUIRender::Text(const tagGUIImage* pDest, const tstring& szText, 
							tagGUIFont* pFont, DWORD dwColor, DWORD dwShadeColor,
							EGUITextAlign eAlign, BOOL bWriteAlpha)
{
	if( szText.empty() )
		return;

	IFont* pIFont = NULL;
	if( !P_VALID(pFont) )
		pIFont = m_pDefaultIFont;
	else
		pIFont = (IFont*)pFont->dwHandle;
	
	tagRect rc = pDest->rc;
	if( P_VALID(pDest->dwHandle) )	// 画到后台缓冲
		rc = rc - pDest->ptOffset;

	UINT uFormat = DT_LEFT|DT_TOP|DT_WORDBREAK;	// 样式
	INT nIncX = 1, nIncY = 1;
	switch( eAlign )
	{
	case EGUITA_CenterTop:		uFormat = DT_CENTER|DT_TOP|DT_WORDBREAK; break;	
	case EGUITA_RightTop:		uFormat = DT_RIGHT|DT_TOP|DT_WORDBREAK; break;	
	case EGUITA_LeftCenter:		uFormat = DT_LEFT|DT_VCENTER|DT_SINGLELINE; break;	
	case EGUITA_Center:			uFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE; break;		
	case EGUITA_RightCenter:	uFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE; break;	
	case EGUITA_LeftBottom:		uFormat = DT_LEFT|DT_BOTTOM|DT_SINGLELINE; break;	
	case EGUITA_CenterBottom:	uFormat = DT_CENTER|DT_BOTTOM|DT_SINGLELINE; break;
	case EGUITA_RightBottom:	uFormat = DT_RIGHT|DT_BOTTOM|DT_SINGLELINE; break;
	default: break;
	}

	if( dwShadeColor )
	{
		DWORD dwRealShadeColor = dwShadeColor|0xff000000;
		tagRect	rcShade;
		if( (dwShadeColor & 0xff000000) == 0 )
		{
			rcShade = rc;
			rcShade.left+=nIncX;rcShade.top+=nIncY;
			pIFont->DrawText(szText.c_str(), (LPRECT)&rcShade, uFormat, dwRealShadeColor, bWriteAlpha);
		}
		else
		{
			rcShade = rc;
			rcShade.left-=nIncX;
			rcShade.right-=nIncX;
			pIFont->DrawText(szText.c_str(), (LPRECT)&rcShade, uFormat, dwRealShadeColor, bWriteAlpha);
			rcShade = rc;
			rcShade.left+=nIncX;
			rcShade.right+=nIncX;
			pIFont->DrawText(szText.c_str(), (LPRECT)&rcShade, uFormat, dwRealShadeColor, bWriteAlpha);
			rcShade = rc;
			rcShade.top-=nIncY;
			rcShade.bottom-=nIncY;
			pIFont->DrawText(szText.c_str(), (LPRECT)&rcShade, uFormat, dwRealShadeColor, bWriteAlpha);
			rcShade = rc;
			rcShade.top+=nIncY;
			rcShade.bottom+=nIncY;
			pIFont->DrawText(szText.c_str(), (LPRECT)&rcShade, uFormat, dwRealShadeColor, bWriteAlpha);
		}
	}

	pIFont->DrawText(szText.c_str(), (LPRECT)&rc, uFormat, dwColor, bWriteAlpha);
}


//-----------------------------------------------------------------------------
// 画线
//-----------------------------------------------------------------------------
VOID GUIRender::Line(const tagGUIImage* pDest, DWORD dwColor, BOOL bBox)
{
	tagRect rc = pDest->rc;
	if( P_VALID(pDest->dwHandle) )	// 画到后台缓冲
		rc = rc - pDest->ptOffset;

	if( !bBox )
		IDraw2D::Inst()->DrawLine(rc.left, rc.top, rc.right, rc.bottom, dwColor);
	else
	{
		IDraw2D::Inst()->DrawLine(rc.left, rc.top, rc.left, rc.bottom, dwColor);
		IDraw2D::Inst()->DrawLine(rc.left, rc.top, rc.right, rc.top, dwColor);
		IDraw2D::Inst()->DrawLine(rc.right, rc.top, rc.right, rc.bottom, dwColor);
		IDraw2D::Inst()->DrawLine(rc.left, rc.bottom, rc.right, rc.bottom, dwColor);
	}
}



//-----------------------------------------------------------------------------
/// 将窗口client坐标转换为窗口view坐标(view坐标为逻辑坐标)
//-----------------------------------------------------------------------------
VOID GUIRender::ClientToView(tagPoint& pt)
{
	if( ( m_rcWindow.right == m_rcWindow.left )
		|| ( m_rcWindow.bottom == m_rcWindow.top ) )
		return;	// 防止被0除

	pt.x = pt.x * Device()->GetClientWidth() / (m_rcWindow.right - m_rcWindow.left);
	pt.y = pt.y * Device()->GetClientHeight() / (m_rcWindow.bottom - m_rcWindow.top);
}


//-----------------------------------------------------------------------------
// 刷新窗口坐标
//-----------------------------------------------------------------------------
VOID GUIRender::UpdateWindowPos()
{
	HWND hWnd = (HWND)TObjRef<VarContainer>()->GetDword(_T("HWND"));
	if( hWnd )
	{
		::GetClientRect(hWnd, (LPRECT)&m_rcWindow);
		::ClientToScreen(hWnd, (LPPOINT)&m_rcWindow);
		::ClientToScreen(hWnd, (LPPOINT)&m_rcWindow + 1 );
	}

}

//-----------------------------------------------------------------------------
// 刷新屏幕
//-----------------------------------------------------------------------------
VOID GUIRender::Present()
{
	//-->remove by bwen
	// Cool3D::Device的Create和Present不可以在不同线程调用，否则会引起异常
	//if(Kernel::Inst()->IsEngineCreated() && Kernel::Inst()->IsDeviceCreated())
	//	Device()->Present();
	//<--
}


//-----------------------------------------------------------------------------
// 得到文本大小
//-----------------------------------------------------------------------------
VOID GUIRender::GetTextSize(const LPCTSTR szText, tagGUIFont* pFont, tagPoint& ptSize)
{
	SIZE sz;
	IFont* pIFont = NULL;
	if( !P_VALID(pFont) )
		pIFont = m_pDefaultIFont;
	else
		pIFont = (IFont*)pFont->dwHandle;

	pIFont->MeasureText(szText, sz);
	ptSize.x = sz.cx;
	ptSize.y = sz.cy;
}


//-----------------------------------------------------------------------------
// 使用新的Render Target
//-----------------------------------------------------------------------------
VOID GUIRender::PushRenderTarget( const tagGUIImage* pDest )
{
	if( P_VALID(pDest->dwHandle) )
	{
		IRenderTexture* pRenderTexture = (IRenderTexture*)pDest->dwHandle;
		pRenderTexture->Begin();
	}
}


//-----------------------------------------------------------------------------
// 恢复原来的Render Target
//-----------------------------------------------------------------------------
VOID GUIRender::PopRenderTarget( const tagGUIImage* pDest )
{
	if( P_VALID(pDest->dwHandle) )
	{
		IRenderTexture* pRenderTexture = (IRenderTexture*)pDest->dwHandle;
		pRenderTexture->End();

	}
}


//-----------------------------------------------------------------------------
// 清除Render Target
//-----------------------------------------------------------------------------
VOID GUIRender::ClearRenderTarget()
{
	Device()->Clear(true, false, false, 0, 1.0f, 0, 0);
}


//-----------------------------------------------------------------------------
// 直接创建新的Render Target
//-----------------------------------------------------------------------------
DWORD GUIRender::CreateRenderTarget(const tagRect& rc)
{
	LONG x = rc.right - rc.left;
	LONG y = rc.bottom - rc.top;
	IRenderTexture* pRenderTexture = Device()->NewRenderTexture();
	bool bResult = pRenderTexture->Create(x, y, EPF_A8R8G8B8, false, false, 1, false);
	if( !bResult )	// 创建RenderTarget失败
	{
		SAFE_DEL(pRenderTexture);
		return GT_INVALID;
	}

	// 清空render target
	pRenderTexture->Begin();
	Device()->Clear(true, false, false, 0, 1.0f, 0, 0);
	pRenderTexture->End();
	m_dwRTSize += x*y*sizeof(DWORD);
	return (DWORD)pRenderTexture;
}


//-----------------------------------------------------------------------------
// 直接释放Render Target
//-----------------------------------------------------------------------------
VOID GUIRender::ReleaseRenderTarget(DWORD dwHandle)
{
	if( !GT_VALID(dwHandle) )
		return;

	IRenderTexture* pTexture = (IRenderTexture*)dwHandle;
	m_dwRTSize -= pTexture->Width()*pTexture->Height()*sizeof(DWORD);
	SAFE_DEL(pTexture);
}


//-----------------------------------------------------------------------------
// 设置剪裁区域
//-----------------------------------------------------------------------------
VOID GUIRender::SetScissorRect( tagRect* pRect )
{
	Device()->SetScissorRect((RECT*)pRect);
}


//-----------------------------------------------------------------------------
// 播放界面声音
//-----------------------------------------------------------------------------
VOID GUIRender::PlaySound( LPCTSTR szName, FLOAT fVolume/*=1.0f*/ )
{
	// 先排除空字符
	tstring str = szName;
	m_pUtil->CutSpaceOfStringHeadAndTail(str);

	if( !str.empty() )
		m_pAudioSys->Play2DSound(szName, fVolume);
}