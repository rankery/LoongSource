////////////////////////////////////////
// Program:     RTClient
// Name:        RTClient.h
// Created:     2009-03-20
////////////////////////////////////////
#pragma once

class PageAssist;

//程序入口
class MyApp : public wxApp
{
public:
	virtual bool OnInit();
	virtual int OnExit();
};

//对话框
class LoginDialog : public wxDialog
{
public:
    LoginDialog(const wxString& title);
    
    void OnOk(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);

	void Suspend();
	void Clear();

	wxString GetUserName();
	wxString GetUserPassword();
private:
	//TextCtrl
    //用户名
	wxTextCtrl *name_;
    //密码
	wxTextCtrl *password_;

	//Button
    //登录
	wxButton   *login_;
    //退出
	wxButton   *cancel_;

private:
	DECLARE_EVENT_TABLE()
};

//Frame
class ShowPage;
//class GmMgrPage;

class MyFrame : public wxFrame
{
public:
	MyFrame(wxWindow *parent, const wxString& title);
	~MyFrame();
	
    //处理退出事件
	void Close();
	void OnClose(wxCloseEvent& event);
	void update_ui();

	void GotoFirstPage();
	//void ShowPage(bool bShow_1, bool bShow_2, bool bShow_3, bool bShow_4);
	//void ShowGMMgrPage(bool bShow);
	//void ShowMultiButtons(bool bShow);
	//void ShowChangeMaxNum(bool bShow);

	PageAssist* GetPageAssistPtr() { return page_assist_; }
		
private:
    //容器
	wxPanel			*panel_;
    wxBookCtrlBase	*book_;
	ShowPage		*open_close_page_;
	//GmMgrPage				*gm_mgr_page_;
	PageAssist		*page_assist_;
	DECLARE_EVENT_TABLE()
};

extern LoginDialog*	g_pLoginDialog;
extern MyFrame*		g_pMainFrame;

