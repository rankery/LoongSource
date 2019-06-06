#include "stdafx.h"
#include "TalentButton.h"
#include "GameClientDefine.h"

TalentButton::TalentButton()
{
	m_pIcon = NULL;
	m_pAlphaTex = NULL;
}

TalentButton::~TalentButton()
{

}

VOID TalentButton::Render()
{
	if( !IsRefresh() )
		return GUIWnd::Render();
	
	// äÖÈ¾µ×Í¼
	m_pCanvas->rc.SetLTWH(m_ptView, m_ptSize);
	m_pRender->Draw(m_pCanvas, m_pPic, m_dwPicColor, EGUIAW_Add);

	// äÖÈ¾Í¼±ê
	tagRect rc = m_pCanvas->rc - m_pCanvas->ptOffset;
	IDraw2D::Inst()->Draw( (RECT*)&rc, 
		(const RECT*)&m_pIcon->rc, 
		(const RECT*)&m_pAlphaTex->rc, 
		(ResTexture*)(m_pIcon->dwHandle), 
		(ResTexture*)(m_pAlphaTex->dwHandle),
		ETFilter_LINEAR, m_dwPicColor );

	return GUIWnd::Render();
}

BOOL TalentButton::OnInputMessage( tagGUIInputMsg* pMsg )
{
	switch( pMsg->dwType )
	{
	case WM_LBUTTONUP:// ×ó¼üµã»÷	
		{
			if( m_pSystem->GetMousePoint() == this )
			{
				tagGUIEvent event(this, EGUIE_Click);
				SendEvent(&event);
			}
		}
		break;
	}
	return GUIStatic::OnInputMessage(pMsg);
}

VOID TalentButton::SetIcon( const tstring str)
{
	if(P_VALID(m_pIcon))
		m_pRender->DestroyImage(m_pIcon);
	
	m_pIcon = m_pRender->CreateImage(str, m_rcPic);
}

VOID TalentButton::SetAlphaTex( const tstring str )
{
	if(P_VALID(m_pAlphaTex))
		m_pRender->DestroyImage(m_pAlphaTex);

	m_pAlphaTex = m_pRender->CreateImage(str, m_rcPic);
}

VOID TalentButton::Destroy()
{
	m_pRender->DestroyImage(m_pIcon);
	m_pRender->DestroyImage(m_pAlphaTex);

	GUIStatic::Destroy();
}