/////////////////////////////////////////////////////////////////////////////
// Name:        GmMgrPage.h
// Created:     2009-05-20
// Info:		GM管理页面
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "UserFrame.h"

class MyListCtrl;

class GmMgrPage : public wxPanel, public UserFrame
{
public:
	GmMgrPage(wxBookCtrlBase *book, const wxString& label);
	const wxString& GetLabel() { return m_label; }

	void SetMyFrame(MyFrame* frame) { m_pMyFrame = frame; }

	//--创建,修改,删除
	void OnGMCreateOperation(wxCommandEvent& event);
	void OnGMModifyOperation(wxCommandEvent& event);
	void OnGMDeleteOperation(wxCommandEvent& event);

	DWORD OnGmMgrPageEvent(tagUserEvent* pGameEvent);

private:
	wxString				m_label;

	wxTextCtrl*				m_text_account;				//创建
	wxTextCtrl*				m_text_psw;
	wxComboBox*				m_combox_Pr_cre;
	wxButton*				m_btn_create;

	wxTextCtrl*				m_text_account_modify;		//修改
	wxComboBox*				m_combox_Pr_modify;
	wxButton*				m_btn_modify;
	
	wxTextCtrl*				m_text_account_del;			//删除
	wxButton*				m_btn_del;
	
	MyFrame*				m_pMyFrame;	

	DECLARE_EVENT_TABLE()
};