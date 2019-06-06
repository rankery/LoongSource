//-----------------------------------------------------------------------------
//!\file GameInputMap.h
//!\author Lyp
//!
//!\date 2008-10-09
//! last 2008-10-09
//!
//!\brief game input map
//!
//!	Copyright (c) 1985-2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once


//-----------------------------------------------------------------------------
//!\brief 键盘命令注册
//!
//-----------------------------------------------------------------------------
class GameInputMap : public InputMap
{
public:
	virtual BOOL LoadGUI(LPCTSTR szWindowName, tagPoint& ptPos);
	virtual VOID UnloadGUI();
	virtual VOID Update();

	BOOL LoadFromFile();
	BOOL SaveToFile();

	VOID SetAllEnable(BOOL bEnable=TRUE) { m_bEnable = bEnable; }

protected:
	TObjRef<GameFrameMgr>		m_pFrameMgr;

	GUIButton*					m_pWndCancle;
	GUIButton*					m_pWndClose;
	GUIButton*					m_pWndDefault;
	GUIPushButton*				m_pWndPage1;
	GUIPushButton*				m_pWndPage2;
	GUIPushButton*				m_pWndPage3;

	GUIStatic*					m_pText;
	BOOL						m_bEnable;
	INT							m_nCurrentTextRow;			//当前listbox显示的是第几行


	BOOL GUIEventHandler(tagGUIEvent* pEvent);
	virtual VOID ShowKeys();

	VOID UpdateSetup();	// 设置时的更新

	BOOL Save();
	VOID Recover();
	VOID RecoverDefault();

	VOID CheckRepeatSet(tagKeyMapInfo* pInfoFind);	// 检查是否已重复设置的按键，如果有，就清除
};