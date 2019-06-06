//-----------------------------------------------------------------------------
//!\file gui_framemutex.h
//!\author tidy / Lyp
//!
//!\date 2005-4-13
//! last 2007-5-15
//!
//!\brief 界面系统 framemutex 控件
//-----------------------------------------------------------------------------
#pragma once


namespace vEngine {
//-----------------------------------------------------------------------------
// 界面系统FrameMutex控件
//-----------------------------------------------------------------------------
class VENGINE_API GUIFrameMutex : public GUIWnd
{
public:
	virtual VOID SendEvent(tagGUIEvent* pEvent);
	VOID SetPush(GUIWnd *pWnd);
};


}	// namespace vEngine {
