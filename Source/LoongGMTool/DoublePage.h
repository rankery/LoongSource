/////////////////////////////////////////////////////////////////////////////
// Name:        DoublePage.h
// Created:     2009-02-04
// Info:		Ë«±¶Ò³Ãæ
/////////////////////////////////////////////////////////////////////////////
#ifndef DOUBLEPAGE_H_
#define DOUBLEPAGE_H_
#include "UserFrame.h"

class MyListCtrl;

//----------------------------------------------------------------------------
// Ë«±¶
//----------------------------------------------------------------------------
class DoublePage : public wxPanel, public UserFrame
{
	friend class MyFrame;
public:
	DoublePage(wxBookCtrlBase *book, const wxString& label);
	const wxString& GetLabel() { return m_label; }

	void SetMyFrame(MyFrame* frame) { m_pMyFrame = frame; }

	//void InsertIPData();
	void InsertShowItem(tstring szWorldName, tstring szTester, int nType, DWORD dwRate, tagDWORDTime dwStartTime, tagDWORDTime dwEndTime);
	void ClearShowItem();
	void DoubleOperationStatus(BOOL);
	void OnDoubleOperation(wxCommandEvent& event);
	void OnCancelDoubleOperation(wxCommandEvent& event);
	//DWORD OnDoublePageEvent(tagUserEvent* pGameEvent);

	void ClearCtrlText();

private:
	wxString m_label;

	//¿Ø¼þ
	wxComboBox*		m_comboBox;
	wxTextCtrl*		m_textLastTime;
	wxTextCtrl*		m_textRate;
	wxTextCtrl*		m_textIP;
	wxButton*		m_btnOk;
	wxButton*		m_bCancel;
	//wxButton*		m_btnRunScript;
	//wxButton*		m_btnCheckHistoryLog;
	wxButton*		m_btnAdd;
	//MyListCtrl*     m_lctrlIp;
	MyListCtrl*     m_lctrlServerStatus;

	wxTextCtrl*		m_text_year;
	wxTextCtrl*		m_text_month;
	wxTextCtrl*		m_text_day;
	wxTextCtrl*		m_text_hour;
	wxTextCtrl*		m_text_min;

	MyFrame*				m_pMyFrame;	

	DECLARE_EVENT_TABLE()
};

#endif /* DOUBLEPAGE_H_ */