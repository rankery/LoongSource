#include "stdafx.h"
#include "RoleHeadTipsStatic.h"

RoleHeadTipsStatic::RoleHeadTipsStatic( void )
{
}

BOOL RoleHeadTipsStatic::Init( GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement )
{
	BOOL bResult = GUIStatic::Init(pSystem, pFather, pXmlElement);

	m_bUnClickable = TRUE;	// 不接受输入
	return bResult;
}

VOID RoleHeadTipsStatic::Destroy()
{
	GUIStatic::Destroy();
}

VOID RoleHeadTipsStatic::Render()
{
	if( !m_bInvisible )
		return GUIStatic::Render();
}


BOOL RoleHeadTipsStatic::ResizeFont( const TCHAR* szInfo, INT nWidth, INT nHeight, INT nWeight )
{
	if(m_nFontWidth==nWidth && m_nFontHeight==nHeight)
	{
		return FALSE;
	}

	if( P_VALID(m_pFont) )
		m_pRender->DestroyFont(m_pFont);
	m_pFont = m_pRender->CreateFont(g_StrTable[_T("Font_MSYH")], nWidth, nHeight, nWeight);
	// 尝试使用父控件字体
	if( !P_VALID(m_pFont) )	
		m_pFont = m_pRender->CloneFont(m_pFather->GetFont());

	m_nFontWidth = nWidth;
	m_nFontHeight = nHeight;

	return TRUE;
}

