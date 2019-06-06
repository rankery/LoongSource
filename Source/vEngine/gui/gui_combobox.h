//-----------------------------------------------------------------------------
//!\file xui_combo.h
//!\author tidy / Lyp
//!
//!\date 2004-12-08
//! last 2004-12-15
//!
//!\brief 界面系统 combo box 控件
//-----------------------------------------------------------------------------
#pragma once


namespace vEngine {
//-----------------------------------------------------------------------------
// 界面系统combo box控件
//-----------------------------------------------------------------------------
class VENGINE_API GUIComboBox : public GUIWnd
{
public:
	virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
	virtual VOID Destroy();
	virtual VOID SendEvent(tagGUIEvent* pEvent);

	virtual BOOL Resize(tagPoint& ptSize); // 改变控件大小,传入新的大小

	virtual VOID AddChild(GUIWnd* pChild);
	virtual VOID DelChild(GUIWnd* pChild);


	GUIButton* GetButton() { return m_pButton; }
	GUIEditBox* GetEditBox() { return m_pEditBox; }
	GUIListBox* GetListBox() { return m_pListBox; }
	

	GUIComboBox();

protected:
	GUIButton*		m_pButton;
	GUIEditBox*		m_pEditBox;
	GUIListBox*		m_pListBox;

	bool			m_bActive;	// 是否任意子控件被激活

	
	//-----------------------------------------------------------------------------
	// 可编辑属性
	//-----------------------------------------------------------------------------
	BOOL			m_bAutoResize;	// 是否自动调节子控件大小

};


}	// namespace vEngine {
