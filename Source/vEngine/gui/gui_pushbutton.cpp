//-----------------------------------------------------------------------------
//!\file gui_pushbutton.cpp
//!\author Lyp
//!
//!\date 2004-12-08
//! last 2004-12-15
//!
//!\brief 界面系统 push button 控件
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "gui_static.h"
#include "gui_button.h"
#include "gui_pushbutton.h"
#include "gui_event_define.h"

namespace vEngine {
//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL GUIPushButton::Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement)
{
	BOOL bResult = GUIButton::Init(pSystem, pFather, pXmlElement);

	if( pFather->GetClassName() == _T("CGUIFrameMutex") )
		m_bRadioButton = TRUE;
	else
		m_bRadioButton = FALSE;

	return bResult;
}


//-----------------------------------------------------------------------------
// 基本消息处理
//-----------------------------------------------------------------------------
BOOL GUIPushButton::OnInputMessage(tagGUIInputMsg* pMsg)
{
	if( m_bDisable )
	{
		if( pMsg->dwType == WM_LBUTTONDOWN || pMsg->dwType == WM_LBUTTONDBLCLK )
		{
			if( m_pSystem->GetMousePoint() == this ) // if( PointInWnd(pMsg->pt) )
			{
				m_pRender->PlaySound(_T("button_disable"), 0.7f);
			}
		}
		return GUIStatic::OnInputMessage(pMsg);
	}

	switch( pMsg->dwType )
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		if( m_pSystem->GetMousePoint() == this )
		{
			BOOL bChange = FALSE;
			switch(m_eState)
			{
			case EGUIBS_PushDown:
				if( !m_bRadioButton )
				{
					m_eState = EGUIBS_HighLight;
					bChange = TRUE;
				}
				break;
			case EGUIBS_HighLight:
				m_eState = EGUIBS_PushDown;
				m_pRender->PlaySound(_T("button_down"), 0.7f);
				bChange = TRUE;
				break;
			}

			if( bChange )
			{
				tagGUIEvent event(this, EGUIE_PushButtonDown, pMsg, (INT)m_eState);
				SendEvent(&event);
				event.eEvent = EGUIE_FrameMutex;	// 模拟RadioButton事件
				event.dwParam1 = 1;
				this->SendEvent(&event);
				SetRefresh();
			}
		}
		break;
	
	case WM_MOUSEMOVE:
		if( PointInWnd(pMsg->pt) && m_eState == EGUIBS_PopUp )
		{
			m_eState = EGUIBS_HighLight;
			SetRefresh();
		}

		if( !PointInWnd(pMsg->pt) && m_eState == EGUIBS_HighLight )
		{
			m_eState = EGUIBS_PopUp;
			SetRefresh();
		}
		break;
	}

	return GUIStatic::OnInputMessage(pMsg);
}


//-----------------------------------------------------------------------------
// 接受事件
//-----------------------------------------------------------------------------
VOID GUIPushButton::OnEvent(tagGUIEvent* pEvent)
{
	GUIWnd* pWnd = m_pSystem->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return;

	// PushButton处于FrameMutex中，相当于RadioButton
	if( !m_bDisable && pWnd == m_pFather )
	{
		if( EGUIE_FrameMutex == pEvent->eEvent )
		{
			if( 0 == pEvent->dwParam1 )
				m_eState = EGUIBS_PopUp;
			else
				m_eState = EGUIBS_PushDown;
			SetRefresh();
		}
	}

	return GUIStatic::OnEvent(pEvent);
}


//-----------------------------------------------------------------------------
// 设置按键状态
//-----------------------------------------------------------------------------
VOID GUIPushButton::SetState(EGUIButtonState eState, BOOL bSendMutexEvent)
{
	if( eState == EGUIBS_PopUp && m_pSystem->GetMousePoint() == this )
		eState = EGUIBS_HighLight;

	m_eState = eState;
	tagGUIEvent event(this, EGUIE_PushButtonDown, NULL, (INT)m_eState);
	SendEvent(&event);
	
	if( bSendMutexEvent )
	{
		event.eEvent = EGUIE_FrameMutex;	// 模拟RadioButton事件
		event.dwParam1 = 1;
		SendEvent(&event);
	}
	SetRefresh();
}


}	// namespace vEngine {
