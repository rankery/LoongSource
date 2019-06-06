#include "StdAfx.h"
#include "IconStaticHL.h"

IconStaticHL::IconStaticHL()
{
	m_pHighLightPic = NULL;
}

IconStaticHL::~IconStaticHL(void)
{
}

BOOL IconStaticHL::Init( GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement )
{
	BOOL bRet = IconStatic::Init( pSystem, pFather, pXmlElement );
	return bRet;
}

VOID IconStaticHL::Destroy()
{
	if( P_VALID(m_pHighLightPic) )
		m_pRender->DestroyImage( m_pHighLightPic );
	
	IconStatic::Destroy();
}

VOID IconStaticHL::Render()
{
	// 调用基类渲染
	IconStatic::Render();
	
	if( IsRefresh() )
		DrawHighLight();
}

BOOL IconStaticHL::OnInputMessage( tagGUIInputMsg* pMsg )
{
	switch( pMsg->dwType )
	{
	case WM_LBUTTONUP:
		{
			tagGUIEvent event(this, EGUIE_Click);
			SendEvent(&event);
		}
		break;
	default:
		break;
	}

	return IconStatic::OnInputMessage(pMsg);
}

void IconStaticHL::SetHighLightPic( LPCTSTR szPath )
{
	if( P_VALID(m_pHighLightPic) )
		m_pRender->DestroyImage( m_pHighLightPic );

	m_pHighLightPic = NULL;
	
	if( !P_VALID(szPath) || _tcslen( szPath ) == 0 )
		return;

	m_pHighLightPic = m_pRender->CreateImage( szPath, m_rcPic );	// 读入图片
}

void IconStaticHL::DrawHighLight()
{
	if( P_VALID(m_pHighLightPic) )
		m_pRender->Draw( m_pCanvas, m_pHighLightPic, m_dwPicColor, m_eAlphaWrite );
}
