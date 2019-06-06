/////////////////////////////////////////////////////////////////////////////
// Name:        AutoChat.h
// Created:     2009-11-2
// Auth:		rzhang
// Info:		角色维护页面
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "UserFrame.h"

class MyListCtrl;

class AutoChatPage : public wxPanel, public UserFrame
{
public:
	AutoChatPage(wxBookCtrlBase *book, const wxString& label);
	const wxString& GetLabel() { return m_label; }
	void SetMyFrame(MyFrame* frame) { m_pMyFrame = frame; }
	void ClearTextCtrl();

	void InsertShowItem(tstring szWorldName, tstring szTester, BYTE byHour, BYTE byMinute, tstring szContent);
	void ClearShowItem();
	void AutoChatOperationStatus(BOOL);

	//--按键事件
	void OnSetOperation(wxCommandEvent& event);
	void OnUseFileOperation(wxCommandEvent& event);
	void OnCancelOperation(wxCommandEvent& event);
	
	DWORD OnSetEvent(tagUserEvent* pGameEvent);
	DWORD OnCancelEvent(tagUserEvent* pGameEvent){return 0;}

private:
	wxString				m_label;

	wxTextCtrl*				m_text_filepath;
	wxTextCtrl*				m_text_notice;
	wxTextCtrl*				m_text_hour;
	wxTextCtrl*				m_text_min;

	wxButton*				m_setnotice_ok_I;
	wxButton*				m_setnotice_ok_II;
	wxButton*				m_setnotice_cancel;

	MyListCtrl*				m_lctrlStatus;

	MyFrame*				m_pMyFrame;	

	DECLARE_EVENT_TABLE()
};