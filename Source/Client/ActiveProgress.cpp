#include "StdAfx.h"
#include "ActiveProgress.h"

ActiveProgress::ActiveProgress() :
m_dwInitForePicColor( 0xffffffff ),
m_dwGlintStartTime( 0 ),
m_dwGlintCycleTime( 1000 ),
m_dwLastUpdataTime( 0 ),
m_bGlint( FALSE ),
m_byGlintEndAlpha( 0 )
{
}
ActiveProgress::~ActiveProgress()
{
}

BOOL ActiveProgress::Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement)
{
	if( !GUIProgress::Init( pSystem, pFather, pXmlElement ) )
		return FALSE;

	m_dwInitForePicColor = m_dwForePicColor;

	return TRUE;
}

VOID ActiveProgress::Update()
{
	DWORD dwCurrTime = timeGetTime();
	DWORD dwTimeDelta = dwCurrTime - m_dwLastUpdataTime;

	if( m_bGlint )
	{
		DWORD dwPassTime = dwCurrTime - m_dwGlintStartTime;
		DWORD dwHalfCycleTime = m_dwGlintCycleTime / 2;
		BYTE byCurrAlpha = 0;
		if( 1 == ( dwPassTime / dwHalfCycleTime ) % 2 )
			byCurrAlpha = BYTE( dwPassTime % dwHalfCycleTime / (FLOAT)dwHalfCycleTime * ( GetAlphaARGB( m_dwInitForePicColor ) - m_byGlintEndAlpha ) ) + m_byGlintEndAlpha;
		else
			byCurrAlpha = BYTE( ( dwHalfCycleTime - dwPassTime % dwHalfCycleTime ) / (FLOAT)dwHalfCycleTime * ( GetAlphaARGB( m_dwInitForePicColor ) - m_byGlintEndAlpha ) ) + m_byGlintEndAlpha;

		SetAlphaARGB( m_dwForePicColor, byCurrAlpha );
		SetRefresh();
	}

	m_dwLastUpdataTime = dwCurrTime;

	GUIProgress::Update();
}

VOID ActiveProgress::Glint( const BOOL bGlint, const BYTE byGlintEndAlpha/* = 0*/ )
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
		m_dwForePicColor = m_dwInitForePicColor;
	}

	m_bGlint = bGlint;
	SetRefresh();
}
