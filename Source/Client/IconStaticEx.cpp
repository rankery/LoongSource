#include "stdafx.h"
#include "IconStaticEx.h"

VOID IconStaticEx::DrawText(LPCTSTR szText)
{
	m_strText = szText;
	if( !m_strText.empty() )
	{
		m_pCanvas->rc = m_rcText + m_ptView;
		m_pRender->Text(m_pCanvas, m_strText, m_pFont, m_dwTextColor, m_dwShadeColor, m_eTextAlign, m_bTextWriteAlpha);
	}
	SetRefresh( TRUE );
}