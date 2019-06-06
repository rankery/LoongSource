//-----------------------------------------------------------------------------
//!\file gui_pushbutton.h
//!\author Lyp
//!
//!\date 2007-04-20
//! last 2007-04-20
//!
//!\brief 界面系统 push button 控件
//-----------------------------------------------------------------------------
#pragma once


namespace vEngine {
//-----------------------------------------------------------------------------
// 界面系统 push button 控件
//-----------------------------------------------------------------------------
class VENGINE_API GUIPushButton : public GUIButton
{
public:
	virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
	virtual BOOL OnInputMessage(tagGUIInputMsg* pMsg);
	virtual VOID OnEvent(tagGUIEvent* pEvent);

	// 设置按键状态
	VOID SetState(EGUIButtonState eState, BOOL bSendMutexEvent=TRUE);

protected:
	BOOL m_bRadioButton;
};



}	// namespace vEngine {
