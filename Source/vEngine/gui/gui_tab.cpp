//-----------------------------------------------------------------------------
//!\file gui_tab.cpp
//!\author tidy / Lyp
//!
//!\date 2005-4-13
//! last 2005-4-21
//!
//!\brief 界面系统 tab 控件
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "gui_tab.h"
#include "gui_event_define.h"

namespace vEngine {
//-----------------------------------------------------------------------------
// 将事件反馈给回调函数或者向上传递,返回TRUE表示消息被外部回调函数接受
//-----------------------------------------------------------------------------
VOID GUITab::SendEvent(tagGUIEvent* pEvent)
{
	GUIWnd* pWnd = m_pSystem->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return;

	if( EGUIE_PushButtonDown == pEvent->eEvent )
	{
		if( pWnd == m_pRadioButton )
		{
			if( P_VALID(m_pWnd) )
				m_pWnd->SetInvisible(false);

			tagGUIEvent event(this, EGUIE_FrameMutex, NULL);
			event.dwParam1 = 1;
			SendEvent(&event);			
		}
	}

	return GUIWnd::SendEvent(pEvent);
}


//-----------------------------------------------------------------------------
// on event
//-----------------------------------------------------------------------------
VOID GUITab::OnEvent(tagGUIEvent* pEvent)
{
	GUIWnd* pWnd = m_pSystem->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return;

	if( pWnd != m_pFather )
		return GUIWnd::OnEvent(pEvent);

	if( EGUIE_FrameMutex == pEvent->eEvent )
	{
		if( pEvent->dwParam1 == 0 ) 
		{
			if( P_VALID(m_pWnd) )
				m_pWnd->SetInvisible(true);
			if( m_pRadioButton != (GUIWnd*)GT_INVALID )
			{
				tagGUIEvent event(this, EGUIE_FrameMutex, NULL);
				event.dwParam1 = 0;
				OnEvent(&event);					
			}
		}
		else 
		{
			if( P_VALID(m_pWnd) )
				m_pWnd->SetInvisible(false);
			if( GT_INVALID != (INT)m_pRadioButton )
			{
				tagGUIEvent event(this, EGUIE_FrameMutex, NULL);
				event.dwParam1 = 1;
				OnEvent(&event);					
			}
		}
	}

	return GUIWnd::OnEvent(pEvent);
}

//-----------------------------------------------------------------------------
// 被设置为焦点
//-----------------------------------------------------------------------------
GUIWnd* GUITab::OnActive()
{
	if( (INT)m_pWnd != GT_INVALID )
		return m_pWnd->OnActive();
	else
		return GUIWnd::OnActive();
}



//-----------------------------------------------------------------------------
// 添加子窗口
//-----------------------------------------------------------------------------
VOID GUITab::AddChild(GUIWnd* pChild)
{
	if( 0 == pChild->GetClassName().compare(_T("CGUIPushButton")) )
		m_pRadioButton = (GUIWnd*)pChild;
	else
		m_pWnd = pChild;

	GUIWnd::AddChild(pChild);
}


//-----------------------------------------------------------------------------
// 删除子窗口
//-----------------------------------------------------------------------------
VOID GUITab::DelChild(GUIWnd* pChild)
{
	if( pChild == m_pRadioButton )
		m_pRadioButton = (GUIWnd*)GT_INVALID;
	if( pChild == m_pWnd )
		m_pWnd = (GUIWnd*)GT_INVALID;

	GUIWnd::DelChild(pChild);
}



}	// namespace vEngine {
