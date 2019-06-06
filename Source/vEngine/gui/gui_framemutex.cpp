//-----------------------------------------------------------------------------
//!\file gui_framemutex.cpp
//!\author tidy / Lyp
//!
//!\date 2005-4-13
//! last 2007-5-15
//!
//!\brief 界面系统 framemutex 控件
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "gui_framemutex.h"
#include "gui_event_define.h"

namespace vEngine {
//-----------------------------------------------------------------------------
// 将事件反馈给回调函数或者向上传递,返回TRUE表示消息被外部回调函数接受
//-----------------------------------------------------------------------------
VOID GUIFrameMutex::SendEvent(tagGUIEvent* pEvent)
{
	GUIWnd* pWnd = m_pSystem->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return;

	if( EGUIE_FrameMutex == pEvent->eEvent )
	{
		if( this == pWnd->GetFather() && 1 == pEvent->dwParam1 )
		{
			tagGUIEvent event(this, EGUIE_FrameMutex, NULL);
			event.dwParam1 = 0;

			//通知其他子空件
			std::list<GUIWnd*>::iterator it = m_listChild.begin();
			for(; it != m_listChild.end(); ++it)
			{
				if( *it != pWnd )
					(*it)->OnEvent(&event);
			}
		}
	}

	return GUIWnd::SendEvent(pEvent);
}


//-----------------------------------------------------------------------------
//设置某个子控件按下
//-----------------------------------------------------------------------------
VOID GUIFrameMutex::SetPush(GUIWnd *pWnd)
{
	if( FALSE == P_VALID(pWnd) )
		return;

	if( pWnd->GetFather() != this )
		return;

	tagGUIEvent event(this, EGUIE_FrameMutex, NULL);
	event.dwParam1 = 1;
	pWnd->OnEvent(&event);
	event.dwParam1 = 0;

	//通知其他子空件
	std::list<GUIWnd*>::iterator it;
	for(it = m_listChild.begin(); it != m_listChild.end(); ++it)
	{
		if( *it != pWnd )
			(*it)->OnEvent(&event);
	}
}

}	// namespace vEngine {
