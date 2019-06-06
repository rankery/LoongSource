/////////////////////////////////////////////////////////////////////////////
// Program:     RTClient
// Name:        GmMgrPage.h
// Created:     2009-06-25
// Info:		开启和关闭服务器
/////////////////////////////////////////////////////////////////////////////

#ifndef GMMGRPAGE_H_
#define GMMGRPAGE_H_

#include "PageAssist.h"

class GmMgrPage : public wxPanel
{
public:
	GmMgrPage(wxBookCtrlBase* book, const wxString label);

	const wxString& GetLabel() { return label_; }
	void SetPageAssist(PageAssist* pPageAssit) { page_assist_ = pPageAssit; }

	//--创建,修改,删除
	void OnGMCreateOperation(wxCommandEvent& event);
	void OnGMModifyOperation(wxCommandEvent& event);
	void OnGMDeleteOperation(wxCommandEvent& event);
	void OnGMQueryOperation(wxCommandEvent& event);

private:
	//页面标题
	wxString label_;

	wxTextCtrl*				m_text_account;				//创建
	wxTextCtrl*				m_text_psw;
	//wxComboBox*				m_combox_Pr_cre;
	wxButton*				m_btn_create;

	wxTextCtrl*				m_text_account_modify;		//修改
	wxTextCtrl*				m_text_bind_ip;
	//wxComboBox*				m_combox_Pr_modify;
	wxButton*				m_btn_modify;

	wxTextCtrl*				m_text_account_del;			//删除
	wxButton*				m_btn_del;

	wxButton*				m_btn_query;				//查询

	PageAssist*				page_assist_;

	DECLARE_EVENT_TABLE()
};

#endif /* OPENAANDCLOSESERVERPAGE_H_ */

