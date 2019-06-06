//-----------------------------------------------------------------------------
//!\file gui_tab.h
//!\author tidy / Lyp
//!
//!\date 2005-4-13
//! last 2005-4-21
//!
//!\brief 界面系统 tab 控件
//-----------------------------------------------------------------------------
#pragma once


namespace vEngine {
//-----------------------------------------------------------------------------
// 界面系统tab控件
//-----------------------------------------------------------------------------
class VENGINE_API GUITab : public GUIWnd
{
public:
	virtual VOID SendEvent(tagGUIEvent* pEvent);
	virtual VOID OnEvent(tagGUIEvent* pEvent);
	virtual GUIWnd* OnActive();

	virtual VOID AddChild(GUIWnd* pChild);	// 添加子窗口
	virtual VOID DelChild(GUIWnd* pChild);	// 删除子窗口

	GUITab():m_pWnd((GUIWnd*)GT_INVALID),m_pRadioButton((GUIWnd*)GT_INVALID){}

protected:
	GUIWnd*	m_pRadioButton;
	GUIWnd*	m_pWnd;		//另外一个控件(可以是复合控件)

};


}	// namespace vEngine {
