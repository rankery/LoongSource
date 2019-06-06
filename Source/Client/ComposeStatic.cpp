#include "StdAfx.h"
#include "ComposeStatic.h"
ComposeStatic::ComposeStatic(void)
{
}

ComposeStatic::~ComposeStatic(void)
{
}

BOOL ComposeStatic::Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement)
{
	BOOL bRet = GUIStatic::Init(pSystem, pFather, pXmlElement);
	return bRet;
}

VOID ComposeStatic::Destroy()
{
	GUIStatic::Destroy();
}

VOID ComposeStatic::Update()
{
	GUIStatic::Update();
	float deltaTime =  Cool3D::Kernel::Inst()->GetDeltaTime();
}

VOID ComposeStatic::Render()
{
	if( !IsRefresh() )
		return GUIWnd::Render();

	DrawImg();

	DrawText();

	// µ÷ÓÃ»ùÀàäÖÈ¾
	GUIWnd::Render();
}

VOID ComposeStatic::OnEvent(tagGUIEvent* pEvent)
{
	switch(pEvent->eEvent)
	{
	case EGUISCB_Drop:
		{
			if( this->IsInvisible() )
				return;
			tagPoint pt ( LOWORD( pEvent->dwParam4 ), HIWORD( pEvent->dwParam4 ) );
			if( this->PointInWnd( pt ) )
			{
				if(EDragType(pEvent->dwParam1)==EDT_Item
					)
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

BOOL ComposeStatic::OnInputMessage(tagGUIInputMsg* pMsg)
{
	switch( pMsg->dwType )
	{
	//case WM_LBUTTONDOWN:// Êó±ê×ó¼ü²Ù×÷ÎªÍÏ¶¯
	//	{
	//		GUIWnd* wnd = m_pSystem->GetMousePoint();
	//		if( wnd == this ) 
	//		{
	//			tagGUIEvent event(this, EGUIE_Drag, pMsg);
	//			event.dwParam4 = MAKELONG(pMsg->pt.x, pMsg->pt.y);
	//			SendEvent(&event);
	//		}
	//	}
	//	break;
	case WM_MOUSEMOVE:
		{
			return GUIStatic::OnInputMessage(pMsg);
		}
		break;

	}
	return GUIStatic::OnInputMessage(pMsg);
}

VOID ComposeStatic::DrawText()
{
	// äÖÈ¾ÎÄ×Ö
	if( !m_strText.empty() )
	{
		m_pCanvas->rc = m_rcText + m_ptView;
		m_pRender->Text(m_pCanvas, m_strText, m_pFont, m_dwTextColor, m_dwShadeColor, m_eTextAlign, m_bTextWriteAlpha);
	}
}

VOID ComposeStatic::DrawImg()
{
	// äÖÈ¾µ×Í¼
	m_pCanvas->rc.SetLTWH(m_ptView, m_ptSize);
	m_pRender->Draw(m_pCanvas, m_pPic, m_dwPicColor, m_eAlphaWrite);
}

void ComposeStatic::SetPic( const tstring& strPicName )
{
	if( P_VALID(m_pPic) )
		m_pRender->DestroyImage(m_pPic);

	m_strPic = strPicName;
	m_pPic = m_pRender->CreateImage(m_strPic, m_rcPic);
}