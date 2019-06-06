#include "stdafx.h"
#include "GlintPushButton.h"

GlintPushButton::GlintPushButton()
{
	m_bShowGlint = false;
	m_bShowInterval = false;
	m_strGlintPic = _T("");
	m_pGlintPic = NULL;
}

GlintPushButton::~GlintPushButton()
{
	
}

BOOL GlintPushButton::Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement)
{
	BOOL bRet = GUIPushButton::Init( pSystem, pFather, pXmlElement );

	m_dwOldTextColor = m_dwTextColor;
	m_dwGlintColor = ~m_dwTextColor;

	return bRet;
}

VOID GlintPushButton::Destroy()
{
	GUIPushButton::Destroy();

	if( P_VALID(m_pGlintPic) )
		m_pRender->DestroyImage( m_pGlintPic );
}

VOID GlintPushButton::Update()
{
	GUIPushButton::Update();
	SetRefresh( TRUE );

	if( !m_bShowGlint )
		return;
		
	DWORD dwCurrentTime = timeGetTime();
	if( (dwCurrentTime - m_dwLastGlint) < 1000 )
	{
		m_bShowInterval = true;
	}
	else if( (dwCurrentTime - m_dwLastGlint) <= 2000 )
	{
		m_bShowInterval = false;
	} 
	else
	{
		m_dwLastGlint = dwCurrentTime;
		m_bShowInterval = true;
	}
}

VOID GlintPushButton::Render()
{
	GUIPushButton::Render();
	
	if(m_bShowGlint)
		DrawGlint();
	else
	{
		SetTextColor( m_dwOldTextColor );
	}
}


void GlintPushButton::SetGlintPic( LPCTSTR strPath )
{
	if( P_VALID(m_pGlintPic) )
		m_pRender->DestroyImage( m_pGlintPic );

	m_strGlintPic = strPath;
	if( m_strGlintPic.empty() )
	{
		m_pGlintPic = NULL;
		return;
	}
		
	m_pGlintPic = m_pRender->CreateImage( m_strGlintPic, m_rcPic );
}

void GlintPushButton::DrawGlint()
{
	if( m_bShowInterval )
	{
		SetTextColor( m_dwGlintColor );
	}
	else
	{
		SetTextColor( m_dwOldTextColor );
	}

	if( P_VALID(m_pGlintPic) )
		m_pRender->Draw( m_pCanvas, m_pGlintPic, m_dwPicColor, m_eAlphaWrite );

}