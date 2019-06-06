/////////////////////////////////////////////////////////////////////////////
// Program:     RTClient
// Name:        RTClient.cpp
// Created:     2009-03-20
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wx/progdlg.h"
#include "SumShower.h"
#include "ShowPage.h"
//#include "GmMgrPage.h"
#include "UIWords.h"

#include "GameServerMgr.h"
#include "PageAssist.h"
#include "data_define.h"

//程序入口WinMain
IMPLEMENT_APP(MyApp)

//全局对象指针
LoginDialog*	g_pLoginDialog;
MyFrame*		g_pMainFrame;

//----------------------------------------------------------------------------
// MyApp 
//----------------------------------------------------------------------------
bool MyApp::OnInit()
{
	if(!wxApp::OnInit())
		return false;
	
	//初始化Log
	//SharedAppenderPtr _append(new DailyRollingFileAppender(_T("\\RTClientLog\\RTClient.log"), DAILY, true, 5));
	//_append->setName(_T("RTClient"));
	////std::auto_ptr<PatternLayout> _layout(new PatternLayout(_T("%d{%m/%d/%y %H:%M:%S}  - %m [%l]%n")));
	//_append->setLayout( std::auto_ptr<PatternLayout>(new PatternLayout(_T("%D{%m/%d/%y %H:%M:%S}  - %m [%l]%n"))) );
	//Logger::getRoot().addAppender(_append);
	
	//初始化GameServerMgr
	new GameServerMgr;
	if (sGameServerMgr.Init() == false)
	{
		LOG("初始化GameServerMgr 失败")
		return false;
	}
	else
	{
		LOG("初始化GameServerMgr 成功")
	}
	
	g_pLoginDialog = new LoginDialog(wxT("Login"));
	if (g_pLoginDialog == NULL)
	{
		LOG("new LoginDialog  失败")
		return false;
	}
	//显示Frame
	g_pMainFrame = new MyFrame((wxWindow*)g_pLoginDialog, w_SumShow);
	if (g_pMainFrame == NULL)
	{
		LOG("new MyFrame  失败")
		return false;
	}

	//开启处理消息线程
	sGameServerMgr.HandleMessages();

	//显示登录窗口
	g_pLoginDialog->Show(true);

	return true;
}

//执行清理动作
int MyApp::OnExit()
{
	//清理GameServerMgr
	if(sGameServerMgr.getSingletonPtr() != NULL)
	{
		delete sGameServerMgr.getSingletonPtr();
	}
	return 0;
}

//----------------------------------------------------------------------------
// LoginDialog
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(LoginDialog, wxDialog)
	EVT_BUTTON(wxID_OK, LoginDialog::OnOk)
	EVT_BUTTON(wxID_CANCEL, LoginDialog::OnCancel)
	EVT_CLOSE(LoginDialog::OnClose)
END_EVENT_TABLE()

LoginDialog::LoginDialog(const wxString& title) :wxDialog(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(300,150))
{
	wxSizer *sizeTop = new wxBoxSizer(wxVERTICAL);

	wxSizer *sizeRow = NULL;
	sizeRow = CreateSizerWithTextAndLabel(this, w_UserName, wxID_ANY, &name_);
	sizeTop->Add(sizeRow, 0, wxALL | wxGROW, CONTROL_BORDER);
	sizeRow = CreateSizerWithTextAndLabel(this, w_Password, wxID_ANY, &password_, true);
	sizeTop->Add(sizeRow, 0, wxALL | wxGROW, CONTROL_BORDER);

	login_		= new wxButton(this, wxID_OK, w_Login);
	cancel_     = new wxButton(this, wxID_CANCEL, w_Logout);
	wxSizer *sizeBtn = new wxBoxSizer(wxHORIZONTAL);
	sizeBtn->Add(login_, 0, wxALL, CONTROL_BORDER);
	sizeBtn->Add(cancel_, 0, wxALL, CONTROL_BORDER);

	sizeTop->Add(sizeBtn, 0, wxALIGN_CENTER);

	SetSizer(sizeTop);
	//*name_ << wxT("root");
	//*password_ << wxT("root");
	//sizeTop->Fit(this);
	//sizeTop->SetSizeHints(this);
}
wxString LoginDialog::GetUserName()
{
	if (name_ != NULL)
	{
		return name_->GetValue();
	}
	else
	{
		return wxT("");
	}
}

wxString LoginDialog::GetUserPassword()
{
	if (password_ != NULL)
	{
		return password_->GetValue();
	}
	else
	{
		return wxT("");
	}
}

void LoginDialog::OnOk(wxCommandEvent &event)
{	
	PageAssist* page_assist = NULL;
	if (g_pMainFrame == NULL)
	{
		this->Suspend();
		return;
	}

	page_assist = g_pMainFrame->GetPageAssistPtr();
	if (page_assist == NULL)
	{
		LOG("page_assist  指针为空")
		return;
	}
	UserInfo user_info;
	user_info.name = GetUserName();
	user_info.password = GetUserPassword();
	user_info.privilege = 0;
	page_assist->SetUserInfo(&user_info);

	//连接RTServer
	wxBusyInfo info(w_PlsWait, this);

	sGameServerMgr.ConnectRTServer();
}

void LoginDialog::OnCancel(wxCommandEvent& event)
{
	Destroy();
}

void LoginDialog::OnClose(wxCloseEvent& event)
{
	Destroy();
}

void LoginDialog::Suspend()
{
	for( int i = 0; i < 18; i++ )
	{
		wxTheApp->Yield();
	}
	wxSleep(2);
}

void LoginDialog::Clear()
{
	name_->Clear();
	password_->Clear();
}

//----------------------------------------------------------------------------
// MyFrame
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
	EVT_CLOSE(MyFrame::OnClose)
END_EVENT_TABLE()

MyFrame::MyFrame(wxWindow *parent, const wxString& title) : wxFrame(parent, wxID_ANY, title, 
                                    wxPoint(0, 0), wxSize(980, 600), wxDEFAULT_FRAME_STYLE |     wxNO_FULL_REPAINT_ON_RESIZE|wxCLIP_CHILDREN|wxTAB_TRAVERSAL)
{
	panel_ = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN);
	wxSizer *sizeAll = new wxBoxSizer(wxHORIZONTAL);

	int style = wxNO_FULL_REPAINT_ON_RESIZE|wxCLIP_CHILDREN;
	book_ = new wxBookCtrl(panel_,wxID_ANY, wxDefaultPosition, wxDefaultSize, style);

	open_close_page_		= new ShowPage(book_, w_OpenCloseServer);
	//gm_mgr_page_			= new GmMgrPage(book_, w_GMManager);

	book_->AddPage(open_close_page_, open_close_page_->GetLabel(), false);
	//book_->AddPage(gm_mgr_page_, gm_mgr_page_->GetLabel(), false);

	sizeAll->Add(book_, 1, wxALL| wxGROW, CONTROL_BORDER);
	
	panel_->SetSizer(sizeAll);

	//初始化PageAssist
	page_assist_ = new PageAssist;

	//设置PageAssist指针
	sGameServerMgr.SetPageAssistPtr(page_assist_);
	//gm_mgr_page_->SetPageAssist(page_assist_);

	//设置各个页面的Update回调函数指针
	page_assist_->SetShowPageFunc(open_close_page_, &ShowPage::update_ui);
}

MyFrame::~MyFrame()
{
	//清理PageAssist
	if(page_assist_ != NULL)
	{
		delete page_assist_;
	}
}

void MyFrame::OnClose(wxCloseEvent &event)
{
	Destroy();
	GetParent()->Destroy();
}

void MyFrame::update_ui()
{
	DWORD dwSum = open_close_page_->GetSum();
	TCHAR szSum[10] = {0};
	_itot(dwSum, szSum, 10);
	open_close_page_->ChangeLabel(szSum);
	book_->SetPageText(0, open_close_page_->GetLabel());
	
}

void MyFrame::Close()
{
	Destroy();
	GetParent()->Destroy();
}

void MyFrame::GotoFirstPage()
{
	if (book_ != NULL)
	{
		book_->ChangeSelection(0);
	}
}

//void MyFrame::ShowGMMgrPage( bool bShow )
//{
//	if (book_ == NULL)
//		return;
//
//	int page = book_->GetPageCount();
//	if (bShow)
//	{
//		if (page > 1)
//		{
//			return;
//		}
//
//		book_->AddPage(gm_mgr_page_, gm_mgr_page_->GetLabel(), false);
//	}
//	else
//	{
//		if (page > 1)
//		{
//			book_->RemovePage(1); 
//		}
//	}
//}

//void MyFrame::ShowPage(bool bShow_1, bool bShow_2, bool bShow_3, bool bShow_4)
//{
//	if (book_ == NULL)
//		return;
//
//	ShowPage* pPage = (ShowPage*)(book_->GetPage(0));
//
//	if (pPage != NULL)
//	{
//		pPage->ShowButtons(bShow_1,bShow_2,bShow_3,bShow_4);
//	}
//}