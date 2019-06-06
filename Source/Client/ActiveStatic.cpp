#include "StdAfx.h"
#include "ActiveStatic.h"

ActiveStatic::ActiveStatic() :
m_bHaveMouseKeyDown( FALSE ),
m_dwInitPicColor( 0xffffffff ),
m_dwGlintStartTime( 0 ),
m_dwGlintCycleTime( 1000 ),
m_dwLastUpdataTime( 0 ),
m_bGlint( FALSE ),
m_byGlintEndAlpha( 0 )
{

}
ActiveStatic::~ActiveStatic()
{

}
BOOL ActiveStatic::Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement)
{
	if( !GUIStatic::Init( pSystem, pFather, pXmlElement ) )
		return FALSE;

	m_dwInitPicColor = m_dwPicColor;

	return TRUE;
}

BOOL ActiveStatic::OnInputMessage(tagGUIInputMsg* pMsg)
{
	switch( pMsg->dwType )
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		if( m_pSystem->GetMousePoint() == this ) // if( PointInWnd(pMsg->pt) )
			m_bHaveMouseKeyDown = true;
		break;

	case WM_MOUSEMOVE:
		break;

	case WM_LBUTTONUP:
		if( m_bHaveMouseKeyDown && PointInWnd(pMsg->pt) )
		{
			tagGUIEvent event(this, EGUIE_Click, pMsg);
			SendEvent(&event);
		}
		m_bHaveMouseKeyDown = false;
		break;
	}

	return GUIStatic::OnInputMessage(pMsg);
}

VOID ActiveStatic::OnEvent(tagGUIEvent* pEvent)
{
	switch(pEvent->eEvent)
	{
	case EGUISCB_Drop:
		{
			tagPoint pt( LOWORD( pEvent->dwParam4 ), HIWORD( pEvent->dwParam4 ) );
			if( PointInWnd( pt ) )
			{
				if( EDT_Item == (EDragType)pEvent->dwParam1 )
				{
					tagGUIEvent event(this,(EGUIEvent)(EOnEventSendDropGUI_Item));
					event.dwParam1 = pEvent->dwParam1;
					event.dwParam2 = pEvent->dwParam2;
					event.dwParam3 = pEvent->dwParam3;
					SendEvent(&event);
				}
			}
		}
		break;
	}
}

VOID ActiveStatic::Update()
{
	DWORD dwCurrTime = timeGetTime();
	DWORD dwTimeDelta = dwCurrTime - m_dwLastUpdataTime;

	if( m_bGlint )
	{
		DWORD dwPassTime = dwCurrTime - m_dwGlintStartTime;
		DWORD dwHalfCycleTime = m_dwGlintCycleTime / 2;
		BYTE byCurrAlpha = 0;
		if( 1 == ( dwPassTime / dwHalfCycleTime ) % 2 )
			byCurrAlpha = BYTE( dwPassTime % dwHalfCycleTime / (FLOAT)dwHalfCycleTime * ( GetAlphaARGB( m_dwInitPicColor ) - m_byGlintEndAlpha ) ) + m_byGlintEndAlpha;
		else
			byCurrAlpha = BYTE( ( dwHalfCycleTime - dwPassTime % dwHalfCycleTime ) / (FLOAT)dwHalfCycleTime * ( GetAlphaARGB( m_dwInitPicColor ) - m_byGlintEndAlpha ) ) + m_byGlintEndAlpha;

		SetAlphaARGB( m_dwPicColor, byCurrAlpha );
		SetRefresh();
	}

	m_dwLastUpdataTime = dwCurrTime;

	GUIStatic::Update();
}

BOOL ActiveStatic::SetPic( tstring strFilename)
{
	if( strFilename == m_strPic )
		return TRUE;

	if( P_VALID( m_pPic ) )
		m_pRender->DestroyImage( m_pPic );

	m_pPic = m_pRender->CreateImage( strFilename, m_rcPic );

	if( !P_VALID( m_pPic ) )
		return FALSE;

	if( !P_VALID( m_pPic->dwHandle ) )
		return FALSE;

	SetRefresh();

	return TRUE;
}


VOID ActiveStatic::Glint( const BOOL bGlint, const BYTE byGlintEndAlpha/* = 0*/ )
{
	if( bGlint == m_bGlint )
		return;

	if( bGlint )
	{
		m_dwGlintStartTime = timeGetTime();
		m_byGlintEndAlpha = byGlintEndAlpha;
	}
	else
	{
		m_dwPicColor = m_dwInitPicColor;
	}

	m_bGlint = bGlint;
	SetRefresh();
}

VOID ActiveStatic::SetColorGray( const BOOL bGray )
{
	if( m_bGlint )
		Glint( FALSE );

	DWORD dwNewColor = m_dwInitPicColor;
	if( bGray )
		dwNewColor = m_dwInitPicColor >> 2;

	if( m_dwPicColor != dwNewColor )
	{
		m_dwPicColor = dwNewColor;
		SetRefresh();
	}
}
