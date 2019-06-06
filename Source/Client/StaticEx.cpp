#include "StdAfx.h"
#include "StaticEx.h"

StaticEx::StaticEx(void) : bisHighLight(FALSE)
{
}

StaticEx::~StaticEx(void)
{
}

VOID StaticEx::Render()
{
	// 画高亮框
	if( bisHighLight )
	{
		// 活动边框
		FLOAT fColor = abs(255.0f*sinf((FLOAT)timeGetTime()/200.0f));
		DWORD dwColorTemp = ((DWORD)fColor) | (((DWORD)fColor) << 8);
		DWORD dwColor = 0xff000000 | dwColorTemp;
		m_pCanvas->rc.SetLTWH(m_ptView, m_ptSize);
		m_pCanvas->rc.left -= 2;
		m_pCanvas->rc.top -= 2;
		m_pCanvas->rc.right += 1;
		m_pCanvas->rc.bottom += 1;
		m_pRender->Line(m_pCanvas, dwColor, TRUE);
	}

	// 渲染基类
	GUIStatic::Render();
}

VOID StaticEx::SetPicFile( const tstring& strFileName )
{
	if (P_VALID(m_pPic))
		m_pRender->DestroyImage(m_pPic);

	m_strPic = strFileName;

	m_pPic = m_pRender->CreateImage(m_strPic, m_rcPic);

    SetRefresh(TRUE);
}

VOID StaticEx::SetFontHeight( INT hValue )
{
	m_nFontHeight = hValue;
	m_pRender->DestroyFont(m_pFont);
	m_pFont = m_pRender->CreateFont(m_strFont, m_nFontWidth, m_nFontHeight, m_nFontWeight, !m_bNoAntiAliase);
	if( !P_VALID(m_pFont) )	// 尝试使用父控件字体
		m_pFont = m_pRender->CloneFont(m_pFather->GetFont());
}
