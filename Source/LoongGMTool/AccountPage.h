/////////////////////////////////////////////////////////////////////////////
// Name:        RolePage.h
// Created:     2009-05-20
// Info:		角色维护页面
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "UserFrame.h"

class MyListCtrl;

class AccountPage : public wxPanel, public UserFrame
{
public:
	AccountPage(wxBookCtrlBase *book, const wxString& label);
	const wxString& GetLabel() { return m_label; }

	void SetMyFrame(MyFrame* frame) { m_pMyFrame = frame; }

	//void InsertShowItem();
	//void ClearShowItem();
	//void AutoChatOperationStatus(BOOL);

	//--按键事件
	void OnAccountSuspendOperation(wxCommandEvent& event);
	void OnAccountUnsuspendOperation(wxCommandEvent& event);
	void OnAccountFileSuspendOperation(wxCommandEvent& event);
	void OnAccountFileUnsuspendOperation(wxCommandEvent& event);
	void OnIPBlacklistAdd(wxCommandEvent& event);
	void OnIPBlacklistDel(wxCommandEvent& event);

	DWORD OnAccountSuspendEvent(tagUserEvent* pGameEvent);
	DWORD OnIPBlacklistEvent(tagUserEvent* pGameEvent);
	
private:
	wxString				m_label;
	wxTextCtrl*				m_text_account;
	wxTextCtrl*				m_text_sectionname;
	wxTextCtrl*				m_text_worldname;
	wxButton*				m_account_suspend;
	wxButton*				m_account_unsuspend;

	wxTextCtrl*				m_text_accountfile;
	wxTextCtrl*				m_text_sectionnameI;
	wxTextCtrl*				m_text_worldnameI;
	wxButton*				m_account_suspendI;
	wxButton*				m_account_unsuspendI;

	wxTextCtrl*				m_text_IP;
	wxTextCtrl*				m_text_sectionname_I;
	wxButton*				m_add_blacklist;
	wxButton*				m_del_blacklist;

	MyListCtrl*				m_lctrl_result;
	MyFrame*				m_pMyFrame;	

	DECLARE_EVENT_TABLE()
};