/////////////////////////////////////////////////////////////////////////////
// Name:        RolePage.h
// Created:     2009-05-20
// Info:		角色维护页面
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "UserFrame.h"

class MyListCtrl;

class RolePage : public wxPanel, public UserFrame
{
public:
	RolePage(wxBookCtrlBase *book, const wxString& label);
	const wxString& GetLabel() { return m_label; }

	void SetMyFrame(MyFrame* frame) { m_pMyFrame = frame; }

	//--按键事件
	void OnRoleResumeOperation(wxCommandEvent& event);
	void OnRoleProModifyOperation(wxCommandEvent& event);
	void OnRoleItemProOperation(wxCommandEvent& event);
	void OnRoleItemDelOperation(wxCommandEvent& event);
	void OnRoleItemResOperation(wxCommandEvent& event);
	void OnRoleClearBaibaoOperation(wxCommandEvent& event);

	DWORD OnRolePageEvent(tagUserEvent* pGameEvent);

private:
	wxString				m_label;
	wxTextCtrl*				m_text_delrole;
	wxTextCtrl*				m_text_rolename;
	wxButton*				m_btn_okresume;

	wxTextCtrl*				m_text_prorole;
	wxTextCtrl*				m_text_level;
	wxTextCtrl*				m_text_exp;
	wxTextCtrl*				m_text_skillnum;
	wxTextCtrl*				m_text_money;
	wxTextCtrl*				m_text_yuanbao;
	wxButton*				m_btn_okmodipro;

	wxTextCtrl*				m_text_itemrole;
	wxComboBox*				m_combox_item;
	wxTextCtrl*				m_text_num;
	wxTextCtrl*				m_text_quality;
	wxButton*				m_btn_okitem;

	wxTextCtrl*				m_text_itemownername;
	//wxTextCtrl*				m_text_itemdelid;
	wxTextCtrl*				m_text_Item64IDStr;
	wxTextCtrl*				m_text_baibaoownername;
	wxButton*				m_btn_okitemdel;
	wxButton*				m_btn_clearbaibao;

	//wxTextCtrl*				m_text_resumeitemid;
	wxTextCtrl*				m_text_resumeitemidstr;
	wxTextCtrl*				m_text_targetrolename;
	wxButton*				m_btn_okitemresume;

	MyFrame*				m_pMyFrame;	

	DECLARE_EVENT_TABLE()
};