/////////////////////////////////////////////////////////////////////////////
// Program:     LoongGMTool
// Name:        LoongGMTool.h
// Created:     2009-02-04
/////////////////////////////////////////////////////////////////////////////

#ifndef LOONGGMTOOL_H_
#define LOONGGMTOOL_H_
#include "UserFrame.h"
class MyPageInfo;

//程序入口
class MyApp : public wxApp
{
public:
	virtual bool OnInit();
	virtual int OnExit();
};

//非模式对话框
class MyDialog : public wxDialog, public UserFrame
{
public:
	MyDialog(const wxString& title);

	void OnOk(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);
	//void OnTimer(wxTimerEvent& event);

	//--挂起2秒
	void Suspend();

	DWORD OnLoginEvent(tagUserEvent* pGameEvent);

private:
	//TextCtrl
	wxTextCtrl *m_textName;
	wxTextCtrl *m_textPassword;

	//Button
	wxButton   *m_btnOk;
	wxButton   *m_btnCancel;

	//wxTimer		m_timer;
	DWORD		m_dwConnectTime;
	BOOL		m_bConnect;

private:
	DECLARE_EVENT_TABLE()
};

#define MyFrame_ID 9527
//Frame
class MyFrame : public wxFrame, public UserFrame
{
public:
	MyFrame(wxWindow *parent, const wxString& title, INT nID);
	~MyFrame();
	
	void OnClose(wxCloseEvent& event);
	void OnSelectAll(wxCommandEvent& event);
	void OnSelectCheckListBox(wxCommandEvent& evet);

	//等到设置服务器列表
	wxCheckListBox* GetCheckListBox() { return m_lbox; }
	wxBookCtrlBase* GetBook() {return m_book;}

	void GetCheckListBoxChecked(std::vector<int>& vec);

	DWORD OnServerStatInfoEvent(tagUserEvent* pGameEvent);
	DWORD OnTimeLimited(tagUserEvent* pGameEvent);

private:
	wxPanel		   *m_panel;
	wxBookCtrlBase *m_book;
	wxCheckListBox *m_lbox;
	wxCheckBox     *m_chkSelectAll;
	bool            m_bSelectAll;
	DECLARE_EVENT_TABLE()
};
#endif /* LOONGGMTOOL_H_ */