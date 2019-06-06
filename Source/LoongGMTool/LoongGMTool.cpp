/////////////////////////////////////////////////////////////////////////////
// Program:     LoongGMTool
// Name:        LoongGMTool.cpp
// Created:     2009-02-04
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wx/wx.h"
#include "wx/busyinfo.h"
#include "wx/panel.h"
#include "wx/bookctrl.h"
#include "LoongGMTool.h"
#include "common.h"

#include "DoublePage.h"
#include "PmdPage.h"
#include "YxjPage.h"
#include "ZdxhPage.h"
#include "GmMgrPage.h"
#include "RolePage.h"
#include "AutoChat.h"
#include "AccountPage.h"
#include "resource.h"
#include "NetSessionMgr.h"
#include "UserFrameMgr.h"
#include "DataMgr.h"
#include "ItemProtoData.h"
#include "..\ServerDefine\gm_tool_define.h"
#include "..\ServerDefine\msg_rt_c.h"

#define TIMER_ID 2000

//程序入口WinMain
IMPLEMENT_APP(MyApp)

//----------------------------------------------------------------------------
// MyApp 
//----------------------------------------------------------------------------
bool MyApp::OnInit()
{
	if(!wxApp::OnInit())
		return false;

	// 初始化配置文件
	if( !g_StrTable.Init(NULL, _T("loonggmtool.xml")) )
	{
		IMSG(_T("Init loonggmtool.xml Fail"));
		return false;
	}

	// 初始化网络
	new NetSessionMgr;
	if( !sNetSessionMgrPtr->Init() )
	{
		IMSG(_T("Init NetSession Fail"));
		return false;;
	}

	new UserFrameMgr;
	new DataMgr;
	ItemProtoData::Inst()->LoadFromFile();
	
	// 创建接收网络消息线程
	sNetSessionMgrPtr->CreateThread();

	//登录
	MyDialog *dialog = new MyDialog(wxT("LoongGMTool"));
	dialog->Show(true);

	return true;
}

int MyApp::OnExit()
{
	if( P_VALID(sNetSessionMgrPtr) )
	{
		delete sNetSessionMgrPtr;
	}

	if( P_VALID(sUserFrameMgrPgr) )
	{
		delete sUserFrameMgrPgr;
	}
	if( P_VALID(sDataMgrPtr) )
	{
		delete sDataMgrPtr;
	}
	return 0;
}

//----------------------------------------------------------------------------
// MyDialog
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(MyDialog, wxDialog)
	EVT_BUTTON(wxID_OK, MyDialog::OnOk)
	EVT_BUTTON(wxID_CANCEL, MyDialog::OnCancel)
	//EVT_TIMER(TIMER_ID, MyDialog::OnTimer)
	EVT_CLOSE(MyDialog::OnClose)
END_EVENT_TABLE()

MyDialog::MyDialog(const wxString& title) :wxDialog(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(300,150))
//,m_timer(this, TIMER_ID)
{
	wxSizer *sizeTop = new wxBoxSizer(wxVERTICAL);

	wxSizer *sizeRow = NULL;
	sizeRow = CreateSizerWithTextAndLabel(this, g_StrTable[_T("Account")], wxID_ANY, &m_textName);
	sizeTop->Add(sizeRow, 0, wxALL | wxGROW, CONTROL_BORDER);
	sizeRow = CreateSizerWithTextAndLabel(this, g_StrTable[_T("PassWord")], wxID_ANY, &m_textPassword, true);
	sizeTop->Add(sizeRow, 0, wxALL | wxGROW, CONTROL_BORDER);

	m_btnOk		= new wxButton(this, wxID_OK, g_StrTable[_T("OK")]);
	m_btnCancel = new wxButton(this, wxID_CANCEL, g_StrTable[_T("Cancel")]);
	wxSizer *sizeBtn = new wxBoxSizer(wxHORIZONTAL);
	sizeBtn->Add(m_btnOk, 0, wxALL, CONTROL_BORDER);
	sizeBtn->Add(m_btnCancel, 0, wxALL, CONTROL_BORDER);

	sizeTop->Add(sizeBtn, 0, wxALIGN_CENTER);

	//m_timer.Start(200);
	m_bConnect = false;

	SetSizer(sizeTop);
	sUserFrameMgrPgr->RegisterEventHandle(_T("tagLogin"), (FRAMEEVENTPROC)m_Trunk.sfp1(&UserFrame::OnLoginEvent));
}

DWORD MyDialog::OnLoginEvent(tagUserEvent* pGameEvent)
{
	tagLoginEvent* pEvt = (tagLoginEvent*) pGameEvent;
	if( pEvt->strEventName==_T("tagLogin") && pEvt->dwErrorCode==2 )
	{
		MyFrame* frame = (MyFrame*)FindWindow(MyFrame_ID);
		if( !P_VALID(frame) )
		{
			new MyFrame(this, wxT("LoongGMTool"), MyFrame_ID);		
		}	
	}
	else if( pEvt->dwErrorCode == 0 )
	{
		MyFrame* frame = (MyFrame*)FindWindow(MyFrame_ID);
		if( P_VALID(frame) )
		{
			frame->CenterOnScreen();
			frame->Show();
			this->Hide();
		}
	}
	else if( pEvt->dwErrorCode!=0 )
	{
		sNetSessionMgrPtr->StopTryingConnect();
		wxMessageBox(g_StrTable[_T("UserOrPswError")], wxT(""), wxOK|wxCENTRE, this);
	}
	return 0;
}

//void MyDialog::OnTimer(wxTimerEvent& event)
//{
//
//}

void MyDialog::OnOk(wxCommandEvent &event)
{	
	while(true)
	{
		// 连接上发送用户名和密码
		if( sNetSessionMgrPtr->IsConnected() )
		{
			sNetSessionMgrPtr->SetLoginState(ELS_Connect_Success);	
			tstring strName = m_textName->GetValue();
			tstring strPsw = m_textPassword->GetValue();
			tagNRTC_Challenge msg;
			_sntprintf( msg.szAccountName, X_SHORT_NAME, strName.c_str() );
			msg.dwPsdCrc = Crc32( strPsw.c_str() );
			sNetSessionMgrPtr->SendMsg( &msg, msg.dwSize );

			tagLoginEvent evt(_T("tagLogin"));
			evt.dwErrorCode = 2;
			sUserFrameMgrPgr->SendEvent(&evt);
			break;
		}

		// 仍然在尝试连接
		if( sNetSessionMgrPtr->IsTryingToConnect() )	
			continue;	

		// 连接失败
		if( sNetSessionMgrPtr->GetLoginState() == ELS_Connect )	
		{
			m_dwConnectTime = timeGetTime();
			wxBusyInfo info(g_StrTable[_T("ConnectFail")], this);
			sNetSessionMgrPtr->SetLoginState(ELS_Connect_Fail);
			this->Suspend();
			sNetSessionMgrPtr->SetLoginState(ELS_NULL);
			break;
		}

		tstring strIP = g_StrTable[_T("IP")];
		INT nPort = _tstoi(g_StrTable[_T("Port")]);

		// 尝试连接
		sNetSessionMgrPtr->TryToConnect( g_Util.UnicodeToUnicode8(strIP.c_str()), nPort );
		wxBusyInfo info(g_StrTable[_T("Connecting")], this);
		sNetSessionMgrPtr->SetLoginState(ELS_Connect);
	}
}

void MyDialog::Suspend()
{
	for( int i = 0; i < 18; i++ )
	{
		wxTheApp->Yield();
	}
	wxSleep(2);
}

void MyDialog::OnCancel(wxCommandEvent& event)
{
	Destroy();
}

void MyDialog::OnClose(wxCloseEvent& event)
{
	Destroy();
}

//----------------------------------------------------------------------------
// MyFrame
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
	EVT_CLOSE(MyFrame::OnClose)
	EVT_CHECKBOX(LoongGMTool_SelectAll, MyFrame::OnSelectAll)
	EVT_CHECKLISTBOX(LoongGMTool_ServerList, MyFrame::OnSelectCheckListBox)
END_EVENT_TABLE()

MyFrame::MyFrame(wxWindow *parent, const wxString& title, INT nID) : wxFrame(parent, nID, title, wxPoint(0, 0), wxSize(980, 600), 
												  wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE|wxCLIP_CHILDREN|wxTAB_TRAVERSAL)
{
	m_panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN);
	wxSizer *sizeAll = new wxBoxSizer(wxHORIZONTAL);

	//布局-左
	wxSizer* sizerLeft = new wxBoxSizer(wxVERTICAL);

	//服务器列表
	wxStaticBox *box = new wxStaticBox(m_panel, wxID_ANY, g_StrTable[_T("Server")]);
	wxSizer* sizerLeftBox = new wxStaticBoxSizer(box, wxVERTICAL);
	int flags = 0;
	flags = wxLB_MULTIPLE | wxLB_HSCROLL;
	m_lbox = new wxCheckListBox(m_panel, LoongGMTool_ServerList, wxDefaultPosition, wxDefaultSize, 0, NULL, flags);
	sizerLeftBox->Add(m_lbox, 1, wxGROW | wxALL, CONTROL_BORDER);

	sUserFrameMgrPgr->RegisterEventHandle(_T("tagServerStatInfoEvent"), (FRAMEEVENTPROC)m_Trunk.sfp1(&UserFrame::OnServerStatInfoEvent));
	sUserFrameMgrPgr->RegisterEventHandle(_T("tagTimeLimited"), (FRAMEEVENTPROC)m_Trunk.sfp1(&UserFrame::OnTimeLimited));
	
	m_chkSelectAll = CreateCheckBoxAndAddToSizer(m_panel, sizerLeftBox, g_StrTable[_T("SelectAll")], LoongGMTool_SelectAll);
	sizerLeft->Add(sizerLeftBox, 1, wxGROW|wxALL, CONTROL_BORDER);
	sizerLeft->SetMinSize(170, 540);
	//布局-右
	wxSizer *sizeRight = new wxBoxSizer(wxHORIZONTAL);

	int style = wxNO_FULL_REPAINT_ON_RESIZE|wxCLIP_CHILDREN;
	m_book = new wxBookCtrl(m_panel,wxID_ANY, wxDefaultPosition, wxDefaultSize, style);

	GmMgrPage* gmmgr_page = new GmMgrPage(m_book, g_StrTable[_T("GMMgrPage")]);
	if( P_VALID(gmmgr_page) ) gmmgr_page->SetMyFrame(this);

	AccountPage* account_page = new AccountPage(m_book, g_StrTable[_T("AccountPage")]);
	if( P_VALID(account_page) ) account_page->SetMyFrame(this);
	
	RolePage* role_page = new RolePage(m_book, g_StrTable[_T("RolePage")]);
	if( P_VALID(role_page) ) role_page->SetMyFrame(this);
	
	DoublePage* double_page = new DoublePage(m_book, g_StrTable[_T("DoublePage")]);
	if( P_VALID(double_page) )	double_page->SetMyFrame(this);
	
	ZdxhPage* zdxh_page = new ZdxhPage(m_book, g_StrTable[_T("ZdxhPage")]);
	if( P_VALID(zdxh_page) ) zdxh_page->SetMyFrame(this);
	
	YxjPage* yxj_page = new YxjPage(m_book, g_StrTable[_T("YxjPage")]);
	if( P_VALID(yxj_page) ) yxj_page->SetMyFrame(this);
	
	AutoChatPage* autochat_page = new AutoChatPage(m_book, g_StrTable[_T("AutoChatPage")]);
	if ( P_VALID(autochat_page) ) autochat_page->SetMyFrame(this);	

	m_book->AddPage(gmmgr_page, gmmgr_page->GetLabel(), false);
	m_book->AddPage(account_page, account_page->GetLabel(), false);
	m_book->AddPage(role_page, role_page->GetLabel(), false);
	m_book->AddPage(double_page, double_page->GetLabel(), false);
	//m_book->AddPage(pmd_page, pmd_page->GetLabel(), false);
	m_book->AddPage(zdxh_page, zdxh_page->GetLabel(), false);
	m_book->AddPage(yxj_page, yxj_page->GetLabel(), false);
	m_book->AddPage(autochat_page, autochat_page->GetLabel(), false);
	sizeRight->Add(m_book, 1, wxALL| wxGROW, CONTROL_BORDER);
	
	sizeAll->Add(sizerLeft, 0, wxGROW|wxALL, CONTROL_BORDER);
	sizeAll->Add(sizeRight, 1, wxGROW|wxALL, CONTROL_BORDER);
	m_panel->SetSizer(sizeAll);
	m_bSelectAll = false;
}

MyFrame::~MyFrame()
{
	
}

void MyFrame::OnClose(wxCloseEvent &event)
{
	Destroy();
	GetParent()->Destroy();
}

void MyFrame::OnSelectAll(wxCommandEvent& event)
{
	int count = m_lbox->GetCount();
	if (count <= 0)
		return;
	if (m_bSelectAll == true)
	{
		for (int i = 0; i < count; i++)
		{
			m_lbox->Check(i, false);
		}
		m_bSelectAll = false;
	}
	else
	{
		for (int i = 0; i < count; i++)
		{
			m_lbox->Check(i, true);
			tstring strSerName = m_lbox->GetString( (unsigned int)i );
			tagSWId id;
			sDataMgrPtr->GetSectionAndWorldID(strSerName, &id);
			if( sDataMgrPtr->JudgeWorldListState(id.section_id, id.world_id) != EWS_Well )  
			{
				TCHAR szBuff[128] = {0};
				_sntprintf( szBuff, 128, g_StrTable[_T("ServerNoWell")], strSerName.c_str() );
				wxMessageBox(szBuff, wxT(""), wxOK|wxCENTRE, this);
				m_lbox->Check( (unsigned int)i, false );
			}

		}
		m_bSelectAll = true;
	}
	
}

void MyFrame::GetCheckListBoxChecked( std::vector<int>& vec )
{
	int count = m_lbox->GetCount();
	if (count <= 0)
		return;
	std::vector<int> vecTemp;
	for (int i = 0; i < count; i++)
	{
		if(m_lbox->IsChecked(i))
		{
			vecTemp.push_back(i);
		}
	}
	vec = vecTemp;
}

void MyFrame::OnSelectCheckListBox( wxCommandEvent& evet )
{
	int nSelected = evet.GetSelection();
	if( m_lbox->IsChecked( (unsigned int)nSelected ) )
	{
		wxString cmd_String = evet.GetString();
		tstring strSerName = cmd_String.wx_str();
		tagSWId id;
		sDataMgrPtr->GetSectionAndWorldID(strSerName, &id);
		if( sDataMgrPtr->JudgeWorldListState(id.section_id, id.world_id) != EWS_Well )  
		{
			TCHAR szBuff[128] = {0};
			_sntprintf( szBuff, 128, g_StrTable[_T("ServerNoWell")], strSerName.c_str() );
			wxMessageBox(szBuff, wxT(""), wxOK|wxCENTRE, this);
			m_lbox->Check( (unsigned int)nSelected, false );
		}
	}
}

DWORD MyFrame::OnServerStatInfoEvent( tagUserEvent* pGameEvent )
{
	//--设置游戏世界
	if( P_VALID(m_lbox) )
	{
		std::list<tagWorld*>* world = sDataMgrPtr->GetWorldDataList();
		std::list<tagWorld*>::iterator it = world->begin();
		for( ; it!=world->end(); it++ )
		{
			m_lbox->Append((*it)->world_name);
		}
	}
	return 0;
}

DWORD MyFrame::OnTimeLimited( tagUserEvent* pGameEvent )
{
	if( pGameEvent->strEventName == _T("tagTimeLimited") )
	{
		tagTimeLimited* pEvt = (tagTimeLimited*)pGameEvent;
		switch( pEvt->dwErrorCode )
		{
		case E_GMTool_Success:
			{
				TCHAR szBuff[X_SHORT_NAME] = {0};
				tstring strWorldName = sDataMgrPtr->GetWorldNameByWorldID(pEvt->dwWorldID);
				if( pEvt->dwType == ETLM_Double )
				{
					_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("OpenDouble0")], strWorldName.c_str() );
				}
				else if( pEvt->dwType == ETLM_RightNotice )
				{
					_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("YxjNotice0")], strWorldName.c_str() );
				}
				else if( pEvt->dwType == ETLM_AutoNotice )
				{
					_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("AutoCast0")], strWorldName.c_str() );	
				}
				else if( pEvt->dwType == ETLM_AutoChatNotice )
				{
					_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("AutoChatNotice0")], strWorldName.c_str() );
				}
				wxMessageBox(szBuff, wxT(""), wxOK|wxCENTRE, this);
			}
			break;
		case E_GMTool_TimeSetError:
			{
				TCHAR szBuff[X_SHORT_NAME] = {0};
				tstring strWorldName = sDataMgrPtr->GetWorldNameByWorldID(pEvt->dwWorldID);
				if( pEvt->dwType == ETLM_Double )
				{
					_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("OpenDouble2")], strWorldName.c_str() );
					wxBookCtrlBase* pbook = GetBook();
					DoublePage* pPage = (DoublePage*)pbook->GetPage(3);
					pPage->DoubleOperationStatus(FALSE);
				}
				else if( pEvt->dwType == ETLM_RightNotice )
				{
					_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("YxjNotice2")], strWorldName.c_str() );
					wxBookCtrlBase* pbook = GetBook();
					YxjPage* pPage = (YxjPage*)pbook->GetPage(5);
					pPage->YxjOperationStatus(FALSE);
				}
				else if( pEvt->dwType == ETLM_AutoNotice )
				{
					_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("AutoCast2")], strWorldName.c_str() );
					wxBookCtrlBase* pbook = GetBook();
					ZdxhPage* pPage = (ZdxhPage*)pbook->GetPage(4);
					pPage->ZdxhOperationStatus(FALSE);
				}
				else if( pEvt->dwType == ETLM_AutoChatNotice )
				{
					_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("AutoChatNotice1")], strWorldName.c_str() );
					wxBookCtrlBase* pbook = GetBook();
					AutoChatPage* pPage = (AutoChatPage*)pbook->GetPage(6);
					pPage->AutoChatOperationStatus(FALSE);
				}
				wxMessageBox(szBuff, wxT(""), wxOK|wxCENTRE, this);
			}
			break;
		case E_GMTool_Err:
			{
				TCHAR szBuff[X_SHORT_NAME] = {0};
				tstring strWorldName = sDataMgrPtr->GetWorldNameByWorldID(pEvt->dwWorldID);
				if( pEvt->dwType == ETLM_Double )
				{
					_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("OpenDouble1")], strWorldName.c_str() );
					wxBookCtrlBase* pbook = GetBook();
					DoublePage* pPage = (DoublePage*)pbook->GetPage(3);
					pPage->DoubleOperationStatus(FALSE);
				}
				else if( pEvt->dwType == ETLM_RightNotice )
				{
					_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("YxjNotice1")], strWorldName.c_str() );
					wxBookCtrlBase* pbook = GetBook();
					YxjPage* pPage = (YxjPage*)pbook->GetPage(5);
					pPage->YxjOperationStatus(FALSE);
				}
				else if( pEvt->dwType == ETLM_AutoNotice )
				{
					_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("AutoCast1")], strWorldName.c_str() );	
					wxBookCtrlBase* pbook = GetBook();
					ZdxhPage* pPage = (ZdxhPage*)pbook->GetPage(4);
					pPage->ZdxhOperationStatus(FALSE);
				}
				else if( pEvt->dwType == ETLM_AutoChatNotice )
				{
					_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("AutoChatNotice2")], strWorldName.c_str() );
					wxBookCtrlBase* pbook = GetBook();
					AutoChatPage* pPage = (AutoChatPage*)pbook->GetPage(6);
					pPage->AutoChatOperationStatus(FALSE);
				}
				wxMessageBox(szBuff, wxT(""), wxOK|wxCENTRE, this);
			}
			break;
		}
	}

	return 0;
}
