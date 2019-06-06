/////////////////////////////////////////////////////////////////////////////
// Name:        mainpage.cpp
// Purpose:     
// Author:      songg
// Modified by: 
// Created:     15/07/2009 09:38:59
// RCS-ID:      
// Copyright:   songg
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif


////@begin includes
////@end includes
#include "StdAfx.h"
#include "wx/wx.h"
#include "MainPage.h"
#include "Loong_ServerMerge.h"
#include "Loong_OutputCtrl.h"
#include "ServerMergeToolApp.h"
////@begin XPM images
////@end XPM images


/*
 * MainPage type definition
 */

IMPLEMENT_DYNAMIC_CLASS( MainPage, wxDialog )


/*
 * MainPage event table definition
 */

BEGIN_EVENT_TABLE( MainPage, wxDialog )
    EVT_BUTTON( ID_BUTTON_MERGE, MainPage::OnButtonClick )
    EVT_BUTTON( ID_BUTTON_EXIT, MainPage::OnButtonExit )
	EVT_CHECKBOX( ID_CHECKBOX_MERGE_TYPE, MainPage::OnCheckBoxMergeType)
////@begin MainPage event table entries
////@end MainPage event table entries

END_EVENT_TABLE()


/*
 * MainPage constructors
 */

MainPage::MainPage()
{
    Init();
}

MainPage::MainPage( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);

	//处理版本信息
	//初始化输出控制
	g_clsOutput.Init(m_itemResult);
	g_clsOutput.Output(_T("数据库版本1.3.0.0!\r\n"));

	//算配置文件路径
	TCHAR tszPath[MAX_PATH];
	tstring strMergeIniPath;
	tstring strCompensationIniPath;
	GetModuleFileName(NULL,tszPath,MAX_PATH);

	//把文件路径处理一下
	strMergeIniPath=tszPath;
	strCompensationIniPath=tszPath;

	//合并配置
	strMergeIniPath=strMergeIniPath.substr(0,strMergeIniPath.rfind('\\'));
	strMergeIniPath += _T('/');
	strMergeIniPath += _T(CONFIG_INI);

	strCompensationIniPath=strCompensationIniPath.substr(0,strCompensationIniPath.rfind('\\'));
	strCompensationIniPath += _T('/');
	strCompensationIniPath += _T(COMPENSATION_INI);

	//看文件是否存在
	BOOL bFileOneRet = PathFileExists(strMergeIniPath.c_str());
	BOOL bFileTwoRet = PathFileExists(strCompensationIniPath.c_str());
	if(bFileOneRet == TRUE && bFileTwoRet == TRUE)
	{
		//把ini文件中的值放入	
		//tagConfigDetail stConfigLoginDB[EM_DB_TYPE_END];	//LoginDB配置
		//tagConfigDetail stConfigLoongDB[EM_DB_TYPE_END];	//LoongDB配置
		Loong_PutINIValueInToMemory(strMergeIniPath.c_str(),strCompensationIniPath.c_str());

		//把得到的数值放到界面儿上
		Loong_ShowDetailOnScreen();

	}
	else
	{
		//没找到文件
		wxMessageBox("未找到ServerMerge.ini或ServerMerge_C.ini文件");
			
	}
	
}


/*
 * MainPage creator
 */

bool MainPage::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin MainPage creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end MainPage creation
    return true;
}


/*
 * MainPage destructor
 */

MainPage::~MainPage()
{
////@begin MainPage destruction

	//读入数据	
	Loong_GetValueFromUserSetting();
	//写ini文件
	Loong_SaveUserSettingToINIFile();

	SAFE_DEL(m_pclsServerMerge);

	SAFE_CLOSE_HANDLE(m_MergeEvent);

	std::multimap<ERoleCompensationType, tagCompensation*>::iterator itLoop = m_mmCompensation.begin();
	for(;itLoop != m_mmCompensation.end(); ++itLoop)
	{
		SAFE_DEL(itLoop->second);
	}
	
////@end MainPage destruction
}


/*
 * Member initialisation
 */

void MainPage::Init()
{
	m_pclsServerMerge = NULL;

	m_MergeEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	//启动work线程
	HANDLE thread_work_handle_ = (HANDLE)_beginthreadex(NULL, 0, (THREADPROC)&MergeWorkerThread, this, 0, NULL);
	if(thread_work_handle_ == NULL)
	{
		CloseHandle(thread_work_handle_);
		thread_work_handle_ = 0;
	}
	//CWinThread *pThread = AfxBeginThread(MergeWorkerThread , this);
	//ASSERT(pThread);
}
/*
 * Control creation for MainPage
 */

void MainPage::CreateControls()
{    
////@begin MainPage content construction
    MainPage* itemDialog1 = this;

	wxTextValidator validator(wxFILTER_NUMERIC); 

#if defined(__WXMSW__)
    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    m_itemResult = new wxTextCtrl( itemDialog1, ID_TEXTCTRL_RESULT, wxEmptyString, wxPoint(0, 0), wxSize(300, 500), wxTE_MULTILINE|wxTE_READONLY );
	itemBoxSizer2->Add(m_itemResult, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(itemBoxSizer4, 0, wxGROW, 5);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer4->Add(itemBoxSizer5, 0, wxALIGN_CENTER_HORIZONTAL, 5);

    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer5->Add(itemBoxSizer6, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticBox* itemStaticBoxSizer7Static = new wxStaticBox(itemDialog1, wxID_ANY, _("LoginSrcOne"));
    wxStaticBoxSizer* itemStaticBoxSizer7 = new wxStaticBoxSizer(itemStaticBoxSizer7Static, wxVERTICAL);
    itemBoxSizer6->Add(itemStaticBoxSizer7, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxBoxSizer* itemBoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer7->Add(itemBoxSizer8, 0, wxALIGN_CENTER_HORIZONTAL, 5);

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer8->Add(itemBoxSizer9, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxBoxSizer* itemBoxSizer10 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer9->Add(itemBoxSizer10, 0, wxALIGN_LEFT, 5);

    wxStaticText* itemStaticText11 = new wxStaticText( itemDialog1, wxID_STATIC, _("Ip"), wxDefaultPosition, wxSize(30, -1), 0 );
    itemBoxSizer10->Add(itemStaticText11, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_itemLoginIP[EM_DB_SRC_ONE] = new wxTextCtrl( itemDialog1, ID_SRC1_LOGIN_IP, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, validator );
    itemBoxSizer10->Add(m_itemLoginIP[EM_DB_SRC_ONE], 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    wxBoxSizer* itemBoxSizer13 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer9->Add(itemBoxSizer13, 0, wxALIGN_LEFT, 5);

    wxStaticText* itemStaticText14 = new wxStaticText( itemDialog1, wxID_STATIC, _("User"), wxDefaultPosition, wxSize(30, -1), 0 );
    itemBoxSizer13->Add(itemStaticText14, 0, wxALIGN_CENTER_VERTICAL, 5);

	m_itemLoginUser[EM_DB_SRC_ONE] = new wxTextCtrl( itemDialog1, ID_SRC1_LOGIN_USER, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer13->Add(m_itemLoginUser[EM_DB_SRC_ONE], 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    wxBoxSizer* itemBoxSizer16 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer9->Add(itemBoxSizer16, 0, wxALIGN_LEFT, 5);

    wxStaticText* itemStaticText17 = new wxStaticText( itemDialog1, wxID_STATIC, _("Psd"), wxDefaultPosition, wxSize(30, -1), 0 );
    itemBoxSizer16->Add(itemStaticText17, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_itemLoginPsd[EM_DB_SRC_ONE] = new wxTextCtrl( itemDialog1, ID_SRC1_LOGIN_PSD, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
    itemBoxSizer16->Add(m_itemLoginPsd[EM_DB_SRC_ONE], 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    wxBoxSizer* itemBoxSizer19 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer9->Add(itemBoxSizer19, 0, wxALIGN_LEFT, 5);

    wxStaticText* itemStaticText20 = new wxStaticText( itemDialog1, wxID_STATIC, _("Name"), wxDefaultPosition, wxSize(30, -1), 0 );
    itemBoxSizer19->Add(itemStaticText20, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_itemLoginName[EM_DB_SRC_ONE] = new wxTextCtrl( itemDialog1, ID_SRC1_LOGIN_NAME, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer19->Add(m_itemLoginName[EM_DB_SRC_ONE], 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    wxBoxSizer* itemBoxSizer22 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer9->Add(itemBoxSizer22, 0, wxALIGN_LEFT, 5);

    wxStaticText* itemStaticText23 = new wxStaticText( itemDialog1, wxID_STATIC, _("Port"), wxDefaultPosition, wxSize(30, -1), 0 );
    itemBoxSizer22->Add(itemStaticText23, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_itemLoginPort[EM_DB_SRC_ONE] = new wxTextCtrl( itemDialog1, ID_SRC1_LOGIN_PORT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, validator );
    itemBoxSizer22->Add(m_itemLoginPort[EM_DB_SRC_ONE], 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    wxStaticBox* itemStaticBoxSizer25Static = new wxStaticBox(itemDialog1, wxID_ANY, _("LoongSrcOne"));
    wxStaticBoxSizer* itemStaticBoxSizer25 = new wxStaticBoxSizer(itemStaticBoxSizer25Static, wxVERTICAL);
    itemBoxSizer6->Add(itemStaticBoxSizer25, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxBoxSizer* itemBoxSizer26 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer25->Add(itemBoxSizer26, 0, wxALIGN_CENTER_HORIZONTAL, 5);

    wxBoxSizer* itemBoxSizer27 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer26->Add(itemBoxSizer27, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxBoxSizer* itemBoxSizer28 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer27->Add(itemBoxSizer28, 0, wxALIGN_LEFT, 5);

    wxStaticText* itemStaticText29 = new wxStaticText( itemDialog1, wxID_STATIC, _("Ip"), wxDefaultPosition, wxSize(30, -1), 0 );
    itemBoxSizer28->Add(itemStaticText29, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_itemLoongIP[EM_DB_SRC_ONE] = new wxTextCtrl( itemDialog1, ID_SRC1_LOONG_IP, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, validator );
    itemBoxSizer28->Add(m_itemLoongIP[EM_DB_SRC_ONE], 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    wxBoxSizer* itemBoxSizer31 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer27->Add(itemBoxSizer31, 0, wxALIGN_LEFT, 5);

    wxStaticText* itemStaticText32 = new wxStaticText( itemDialog1, wxID_STATIC, _("User"), wxDefaultPosition, wxSize(30, -1), 0 );
    itemBoxSizer31->Add(itemStaticText32, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_itemLoongUser[EM_DB_SRC_ONE] = new wxTextCtrl( itemDialog1, ID_SRC1_LOONG_USER, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer31->Add(m_itemLoongUser[EM_DB_SRC_ONE], 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    wxBoxSizer* itemBoxSizer34 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer27->Add(itemBoxSizer34, 0, wxALIGN_LEFT, 5);

    wxStaticText* itemStaticText35 = new wxStaticText( itemDialog1, wxID_STATIC, _("Psd"), wxDefaultPosition, wxSize(30, -1), 0 );
    itemBoxSizer34->Add(itemStaticText35, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_itemLoongPsd[EM_DB_SRC_ONE] = new wxTextCtrl( itemDialog1, ID_SRC1_LOONG_PSD, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
    itemBoxSizer34->Add(m_itemLoongPsd[EM_DB_SRC_ONE], 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    wxBoxSizer* itemBoxSizer37 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer27->Add(itemBoxSizer37, 0, wxALIGN_LEFT, 5);

    wxStaticText* itemStaticText38 = new wxStaticText( itemDialog1, wxID_STATIC, _("Name"), wxDefaultPosition, wxSize(30, -1), 0 );
    itemBoxSizer37->Add(itemStaticText38, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_itemLoongName[EM_DB_SRC_ONE] = new wxTextCtrl( itemDialog1, ID_SRC1_LOONG_NAME, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer37->Add(m_itemLoongName[EM_DB_SRC_ONE], 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    wxBoxSizer* itemBoxSizer40 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer27->Add(itemBoxSizer40, 0, wxALIGN_LEFT, 5);

    wxStaticText* itemStaticText41 = new wxStaticText( itemDialog1, wxID_STATIC, _("Port"), wxDefaultPosition, wxSize(30, -1), 0 );
    itemBoxSizer40->Add(itemStaticText41, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_itemLoongPort[EM_DB_SRC_ONE] = new wxTextCtrl( itemDialog1, ID_SRC1_LOONG_PORT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 ,validator);
    itemBoxSizer40->Add(m_itemLoongPort[EM_DB_SRC_ONE], 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    wxBoxSizer* itemBoxSizer43 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer5->Add(itemBoxSizer43, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticBox* itemStaticBoxSizer44Static = new wxStaticBox(itemDialog1, wxID_ANY, _("LoginSrcTwo"));
    wxStaticBoxSizer* itemStaticBoxSizer44 = new wxStaticBoxSizer(itemStaticBoxSizer44Static, wxVERTICAL);
    itemBoxSizer43->Add(itemStaticBoxSizer44, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxBoxSizer* itemBoxSizer45 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer44->Add(itemBoxSizer45, 0, wxALIGN_CENTER_HORIZONTAL, 5);

    wxBoxSizer* itemBoxSizer46 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer45->Add(itemBoxSizer46, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxBoxSizer* itemBoxSizer47 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer46->Add(itemBoxSizer47, 0, wxALIGN_LEFT, 5);

    wxStaticText* itemStaticText48 = new wxStaticText( itemDialog1, wxID_STATIC, _("Ip"), wxDefaultPosition, wxSize(30, -1), 0 );
    itemBoxSizer47->Add(itemStaticText48, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_itemLoginIP[EM_DB_SRC_TWO] = new wxTextCtrl( itemDialog1, ID_SRC2_LOGIN_IP, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, validator );
    itemBoxSizer47->Add(m_itemLoginIP[EM_DB_SRC_TWO], 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    wxBoxSizer* itemBoxSizer50 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer46->Add(itemBoxSizer50, 0, wxALIGN_LEFT, 5);

    wxStaticText* itemStaticText51 = new wxStaticText( itemDialog1, wxID_STATIC, _("User"), wxDefaultPosition, wxSize(30, -1), 0 );
    itemBoxSizer50->Add(itemStaticText51, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_itemLoginUser[EM_DB_SRC_TWO] = new wxTextCtrl( itemDialog1, ID_SRC2_LOGIN_USER, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer50->Add(m_itemLoginUser[EM_DB_SRC_TWO], 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    wxBoxSizer* itemBoxSizer53 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer46->Add(itemBoxSizer53, 0, wxALIGN_LEFT, 5);

    wxStaticText* itemStaticText54 = new wxStaticText( itemDialog1, wxID_STATIC, _("Psd"), wxDefaultPosition, wxSize(30, -1), 0 );
    itemBoxSizer53->Add(itemStaticText54, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_itemLoginPsd[EM_DB_SRC_TWO] = new wxTextCtrl( itemDialog1, ID_SRC2_LOGIN_PSD, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
    itemBoxSizer53->Add(m_itemLoginPsd[EM_DB_SRC_TWO], 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    wxBoxSizer* itemBoxSizer56 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer46->Add(itemBoxSizer56, 0, wxALIGN_LEFT, 5);

    wxStaticText* itemStaticText57 = new wxStaticText( itemDialog1, wxID_STATIC, _("Name"), wxDefaultPosition, wxSize(30, -1), 0 );
    itemBoxSizer56->Add(itemStaticText57, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_itemLoginName[EM_DB_SRC_TWO] = new wxTextCtrl( itemDialog1, ID_SRC2_LOGIN_NAME, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer56->Add(m_itemLoginName[EM_DB_SRC_TWO], 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    wxBoxSizer* itemBoxSizer59 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer46->Add(itemBoxSizer59, 0, wxALIGN_LEFT, 5);

    wxStaticText* itemStaticText60 = new wxStaticText( itemDialog1, wxID_STATIC, _("Port"), wxDefaultPosition, wxSize(30, -1), 0 );
    itemBoxSizer59->Add(itemStaticText60, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_itemLoginPort[EM_DB_SRC_TWO] = new wxTextCtrl( itemDialog1, ID_SRC2_LOGIN_PORT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0,validator );
    itemBoxSizer59->Add(m_itemLoginPort[EM_DB_SRC_TWO], 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    wxStaticBox* itemStaticBoxSizer62Static = new wxStaticBox(itemDialog1, wxID_ANY, _("LoongSrcTwo"));
    wxStaticBoxSizer* itemStaticBoxSizer62 = new wxStaticBoxSizer(itemStaticBoxSizer62Static, wxVERTICAL);
    itemBoxSizer43->Add(itemStaticBoxSizer62, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxBoxSizer* itemBoxSizer63 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer62->Add(itemBoxSizer63, 0, wxALIGN_CENTER_HORIZONTAL, 5);

    wxBoxSizer* itemBoxSizer64 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer63->Add(itemBoxSizer64, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxBoxSizer* itemBoxSizer65 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer64->Add(itemBoxSizer65, 0, wxALIGN_LEFT, 5);

    wxStaticText* itemStaticText66 = new wxStaticText( itemDialog1, wxID_STATIC, _("Ip"), wxDefaultPosition, wxSize(30, -1), 0 );
    itemBoxSizer65->Add(itemStaticText66, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_itemLoongIP[EM_DB_SRC_TWO] = new wxTextCtrl( itemDialog1, ID_SRC2_LOONG_IP, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0,validator );
    itemBoxSizer65->Add(m_itemLoongIP[EM_DB_SRC_TWO], 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    wxBoxSizer* itemBoxSizer68 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer64->Add(itemBoxSizer68, 0, wxALIGN_LEFT, 5);

    wxStaticText* itemStaticText69 = new wxStaticText( itemDialog1, wxID_STATIC, _("User"), wxDefaultPosition, wxSize(30, -1), 0 );
    itemBoxSizer68->Add(itemStaticText69, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_itemLoongUser[EM_DB_SRC_TWO] = new wxTextCtrl( itemDialog1, ID_SRC2_LOONG_USER, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer68->Add(m_itemLoongUser[EM_DB_SRC_TWO], 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    wxBoxSizer* itemBoxSizer71 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer64->Add(itemBoxSizer71, 0, wxALIGN_LEFT, 5);

    wxStaticText* itemStaticText72 = new wxStaticText( itemDialog1, wxID_STATIC, _("Psd"), wxDefaultPosition, wxSize(30, -1), 0 );
    itemBoxSizer71->Add(itemStaticText72, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_itemLoongPsd[EM_DB_SRC_TWO] = new wxTextCtrl( itemDialog1, ID_SRC2_LOONG_PSD, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
    itemBoxSizer71->Add(m_itemLoongPsd[EM_DB_SRC_TWO], 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    wxBoxSizer* itemBoxSizer74 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer64->Add(itemBoxSizer74, 0, wxALIGN_LEFT, 5);

    wxStaticText* itemStaticText75 = new wxStaticText( itemDialog1, wxID_STATIC, _("Name"), wxDefaultPosition, wxSize(30, -1), 0 );
    itemBoxSizer74->Add(itemStaticText75, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_itemLoongName[EM_DB_SRC_TWO] = new wxTextCtrl( itemDialog1, ID_SRC2_LOONG_NAME, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer74->Add(m_itemLoongName[EM_DB_SRC_TWO], 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    wxBoxSizer* itemBoxSizer77 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer64->Add(itemBoxSizer77, 0, wxALIGN_LEFT, 5);

    wxStaticText* itemStaticText78 = new wxStaticText( itemDialog1, wxID_STATIC, _("Port"), wxDefaultPosition, wxSize(30, -1), 0 );
    itemBoxSizer77->Add(itemStaticText78, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_itemLoongPort[EM_DB_SRC_TWO] = new wxTextCtrl( itemDialog1, ID_SRC2_LOONG_PORT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 ,validator);
    itemBoxSizer77->Add(m_itemLoongPort[EM_DB_SRC_TWO], 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    wxBoxSizer* itemBoxSizer80 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer5->Add(itemBoxSizer80, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticBox* itemStaticBoxSizer81Static = new wxStaticBox(itemDialog1, wxID_ANY, _("LoginTar"));
    wxStaticBoxSizer* itemStaticBoxSizer81 = new wxStaticBoxSizer(itemStaticBoxSizer81Static, wxVERTICAL);
    itemBoxSizer80->Add(itemStaticBoxSizer81, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxBoxSizer* itemBoxSizer82 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer81->Add(itemBoxSizer82, 0, wxALIGN_CENTER_HORIZONTAL, 5);

    wxBoxSizer* itemBoxSizer83 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer82->Add(itemBoxSizer83, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxBoxSizer* itemBoxSizer84 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer83->Add(itemBoxSizer84, 0, wxALIGN_LEFT, 5);

    wxStaticText* itemStaticText85 = new wxStaticText( itemDialog1, wxID_STATIC, _("Ip"), wxDefaultPosition, wxSize(30, -1), 0 );
    itemBoxSizer84->Add(itemStaticText85, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_itemLoginIP[EM_DB_TAR] = new wxTextCtrl( itemDialog1, ID_TAR_LOGIN_IP, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, validator );
    itemBoxSizer84->Add(m_itemLoginIP[EM_DB_TAR], 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    wxBoxSizer* itemBoxSizer87 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer83->Add(itemBoxSizer87, 0, wxALIGN_LEFT, 5);

    wxStaticText* itemStaticText88 = new wxStaticText( itemDialog1, wxID_STATIC, _("User"), wxDefaultPosition, wxSize(30, -1), 0 );
    itemBoxSizer87->Add(itemStaticText88, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_itemLoginUser[EM_DB_TAR] = new wxTextCtrl( itemDialog1, ID_TAR_LOGIN_USER, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer87->Add(m_itemLoginUser[EM_DB_TAR], 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    wxBoxSizer* itemBoxSizer90 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer83->Add(itemBoxSizer90, 0, wxALIGN_LEFT, 5);

    wxStaticText* itemStaticText91 = new wxStaticText( itemDialog1, wxID_STATIC, _("Psd"), wxDefaultPosition, wxSize(30, -1), 0 );
    itemBoxSizer90->Add(itemStaticText91, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_itemLoginPsd[EM_DB_TAR] = new wxTextCtrl( itemDialog1, ID_TAR_LOGIN_PSD, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
    itemBoxSizer90->Add(m_itemLoginPsd[EM_DB_TAR], 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    wxBoxSizer* itemBoxSizer93 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer83->Add(itemBoxSizer93, 0, wxALIGN_LEFT, 5);

    wxStaticText* itemStaticText94 = new wxStaticText( itemDialog1, wxID_STATIC, _("Name"), wxDefaultPosition, wxSize(30, -1), 0 );
    itemBoxSizer93->Add(itemStaticText94, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_itemLoginName[EM_DB_TAR] = new wxTextCtrl( itemDialog1, ID_TAR_LOGIN_NAME, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer93->Add(m_itemLoginName[EM_DB_TAR], 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    wxBoxSizer* itemBoxSizer96 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer83->Add(itemBoxSizer96, 0, wxALIGN_LEFT, 5);

    wxStaticText* itemStaticText97 = new wxStaticText( itemDialog1, wxID_STATIC, _("Port"), wxDefaultPosition, wxSize(30, -1), 0 );
    itemBoxSizer96->Add(itemStaticText97, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_itemLoginPort[EM_DB_TAR] = new wxTextCtrl( itemDialog1, ID_TAR_LOGIN_PORT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0,validator );
    itemBoxSizer96->Add(m_itemLoginPort[EM_DB_TAR], 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    wxStaticBox* itemStaticBoxSizer99Static = new wxStaticBox(itemDialog1, wxID_ANY, _("LoongTar"));
    wxStaticBoxSizer* itemStaticBoxSizer99 = new wxStaticBoxSizer(itemStaticBoxSizer99Static, wxVERTICAL);
    itemBoxSizer80->Add(itemStaticBoxSizer99, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxBoxSizer* itemBoxSizer100 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer99->Add(itemBoxSizer100, 0, wxALIGN_CENTER_HORIZONTAL, 5);

    wxBoxSizer* itemBoxSizer101 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer100->Add(itemBoxSizer101, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxBoxSizer* itemBoxSizer102 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer101->Add(itemBoxSizer102, 0, wxALIGN_LEFT, 5);

    wxStaticText* itemStaticText103 = new wxStaticText( itemDialog1, wxID_STATIC, _("Ip"), wxDefaultPosition, wxSize(30, -1), 0 );
    itemBoxSizer102->Add(itemStaticText103, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_itemLoongIP[EM_DB_TAR] = new wxTextCtrl( itemDialog1, ID_TAR_LOONG_IP, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0,validator );
    itemBoxSizer102->Add(m_itemLoongIP[EM_DB_TAR], 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    wxBoxSizer* itemBoxSizer105 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer101->Add(itemBoxSizer105, 0, wxALIGN_LEFT, 5);

    wxStaticText* itemStaticText106 = new wxStaticText( itemDialog1, wxID_STATIC, _("User"), wxDefaultPosition, wxSize(30, -1), 0 );
    itemBoxSizer105->Add(itemStaticText106, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_itemLoongUser[EM_DB_TAR] = new wxTextCtrl( itemDialog1, ID_TAR_LOONG_USER, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer105->Add(m_itemLoongUser[EM_DB_TAR], 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    wxBoxSizer* itemBoxSizer108 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer101->Add(itemBoxSizer108, 0, wxALIGN_LEFT, 5);

    wxStaticText* itemStaticText109 = new wxStaticText( itemDialog1, wxID_STATIC, _("Psd"), wxDefaultPosition, wxSize(30, -1), 0 );
    itemBoxSizer108->Add(itemStaticText109, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_itemLoongPsd[EM_DB_TAR] = new wxTextCtrl( itemDialog1, ID_TAR_LOONG_PSD, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
    itemBoxSizer108->Add(m_itemLoongPsd[EM_DB_TAR], 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    wxBoxSizer* itemBoxSizer111 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer101->Add(itemBoxSizer111, 0, wxALIGN_LEFT, 5);

    wxStaticText* itemStaticText112 = new wxStaticText( itemDialog1, wxID_STATIC, _("Name"), wxDefaultPosition, wxSize(30, -1), 0 );
    itemBoxSizer111->Add(itemStaticText112, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_itemLoongName[EM_DB_TAR] = new wxTextCtrl( itemDialog1, ID_TAR_LOONG_NAME, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer111->Add(m_itemLoongName[EM_DB_TAR], 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    wxBoxSizer* itemBoxSizer114 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer101->Add(itemBoxSizer114, 0, wxALIGN_LEFT, 5);

    wxStaticText* itemStaticText115 = new wxStaticText( itemDialog1, wxID_STATIC, _("Port"), wxDefaultPosition, wxSize(30, -1), 0 );
    itemBoxSizer114->Add(itemStaticText115, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_itemLoongPort[EM_DB_TAR] = new wxTextCtrl( itemDialog1, ID_TAR_LOONG_PORT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0,validator );
    itemBoxSizer114->Add(m_itemLoongPort[EM_DB_TAR], 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);



	wxBoxSizer* itemBoxSizer117 = new wxBoxSizer(wxHORIZONTAL);
	itemBoxSizer5->Add(itemBoxSizer117, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

	wxStaticBox* itemStaticBoxSizer118Static = new wxStaticBox(itemDialog1, wxID_ANY, _("Opt"));
	wxStaticBoxSizer* itemStaticBoxSizer118 = new wxStaticBoxSizer(itemStaticBoxSizer118Static, wxVERTICAL);
	itemBoxSizer117->Add(itemStaticBoxSizer118, 0, wxALIGN_CENTER_VERTICAL, 5);

	wxBoxSizer* itemBoxSizer119 = new wxBoxSizer(wxHORIZONTAL);
	itemStaticBoxSizer118->Add(itemBoxSizer119, 0, wxALIGN_CENTER_HORIZONTAL, 5);

	wxStaticText* itemStaticText120 = new wxStaticText( itemDialog1, wxID_STATIC, wxGetTranslation(wxString() + (wxChar) 0x6E90 + (wxChar) 0x4E8C + (wxChar) 0x4E16 + (wxChar) 0x754C + (wxChar) 0x540D), wxDefaultPosition, wxDefaultSize, 0 );
	itemBoxSizer119->Add(itemStaticText120, 0, wxALIGN_CENTER_VERTICAL, 5);

	wxTextCtrl* itemTextCtrl121 = new wxTextCtrl( itemDialog1, ID_SRC2_WORLD_NAME, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	itemBoxSizer119->Add(itemTextCtrl121, 0, wxALIGN_CENTER_VERTICAL, 5);

	itemBoxSizer119->Add(40, 5, 0, wxALIGN_CENTER_VERTICAL, 5);

	wxCheckBox* itemCheckBox123 = new wxCheckBox( itemDialog1, ID_CHECKBOX_MERGE_TYPE, wxGetTranslation(wxString() + (wxChar) 0x662F + (wxChar) 0x5426 + (wxChar) 0x8DE8 + (wxChar) 0x5927 + (wxChar) 0x533A), wxDefaultPosition, wxDefaultSize, 0 );
	itemCheckBox123->SetValue(false);
	itemBoxSizer119->Add(itemCheckBox123, 0, wxALIGN_CENTER_VERTICAL, 5);

	wxBoxSizer* itemBoxSizer124 = new wxBoxSizer(wxHORIZONTAL);
	itemBoxSizer5->Add(itemBoxSizer124, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

	itemBoxSizer124->Add(150, 5, 0, wxALIGN_CENTER_VERTICAL, 5);

	wxButton* itemButton126 = new wxButton( itemDialog1, ID_BUTTON_MERGE, _("Merge"), wxDefaultPosition, wxDefaultSize, 0 );
	itemBoxSizer124->Add(itemButton126, 0, wxALIGN_CENTER_VERTICAL, 5);

	wxButton* itemButton127 = new wxButton( itemDialog1, ID_BUTTON_EXIT, _("Exit"), wxDefaultPosition, wxDefaultSize, 0 );
	itemBoxSizer124->Add(itemButton127, 0, wxALIGN_CENTER_VERTICAL, 5);

#endif

////@end MainPage content construction
}


/*
 * Should we show tooltips?
 */

bool MainPage::ShowToolTips()
{
    return true;
}

/*
 * Get bitmap resources
 */

wxBitmap MainPage::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin MainPage bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end MainPage bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon MainPage::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin MainPage icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end MainPage icon retrieval
}

/*
/	刷新ui
*/
void MainPage::OnRefreshUI()
{

	m_bSelected = ((wxCheckBox *)FindWindow(ID_CHECKBOX_MERGE_TYPE))->GetValue();
	if(m_bSelected == FALSE)
	{
		//使源2以及目标login库相关配置项无效
		for(INT iLoop = EM_DB_SRC_ONE; iLoop <= EM_DB_TAR; iLoop++ )
		{
			((wxTextCtrl*)m_itemLoginIP[iLoop])->Enable(false);
			((wxTextCtrl*)m_itemLoginUser[iLoop])->Enable(false);
			((wxTextCtrl*)m_itemLoginPsd[iLoop])->Enable(false);
			((wxTextCtrl*)m_itemLoginName[iLoop])->Enable(false);
			((wxTextCtrl*)m_itemLoginPort[iLoop])->Enable(false);
		}
	}
	else
	{
		for(INT iLoop = EM_DB_SRC_ONE; iLoop <= EM_DB_TAR; iLoop++ )
		{
			((wxTextCtrl*)m_itemLoginIP[iLoop])->Enable(true);
			((wxTextCtrl*)m_itemLoginUser[iLoop])->Enable(true);
			((wxTextCtrl*)m_itemLoginPsd[iLoop])->Enable(true);
			((wxTextCtrl*)m_itemLoginName[iLoop])->Enable(true);
			((wxTextCtrl*)m_itemLoginPort[iLoop])->Enable(true);
		}
	}

}


/*
/	是否跨大区
*/
void MainPage::OnCheckBoxMergeType( wxCommandEvent& event )
{
	OnRefreshUI();
}

void MainPage::OnButtonExit( wxCommandEvent& event )
{
	//::PostMessage((HWND__ *)this->GetHandle(),WM_CLOSE,0,0);
	this->EndModal(true);
}
void MainPage::OnButtonClick( wxCommandEvent& event )
{
	////@begin wxEVT_LEFT_DOWN event handler for ID_BUTTON in MainPage.
	// Before editing this code, remove the block markers.


	//读入数据	
	Loong_GetValueFromUserSetting();

	//检查用户设置合法性
	BOOL bRet = Loong_CheckUserSettingValidity();
	if(bRet == FALSE)
	{
		wxMessageBox("您的设置有误!");
	}
	else
	{
		//处理重复合并
		if(P_VALID(m_pclsServerMerge))
		{
			SAFE_DEL(m_pclsServerMerge);
			m_pclsServerMerge = new CLoongServerMerge;
		}
		else
		{
			m_pclsServerMerge = new CLoongServerMerge;
		}

		if( !m_pclsServerMerge->Init(stConfigLoginDB,stConfigLoongDB,
									(EDBMergeType)m_bSelected,m_strSrc2WorldName,m_mmCompensation ))
		{
			//初始化数据库失败
			wxMessageBox("初始化数据库失败");
		}
		else
		{
			//开始合并
			SetEvent(m_MergeEvent);
		}
	}


	event.Skip();
	////@end wxEVT_LEFT_DOWN event handler for ID_BUTTON in MainPage. 
}
/*
* 检查用户输入信息的合法性
*/
BOOL MainPage::Loong_CheckUserSettingValidity()
{
	for(int iLoop = EM_DB_SRC_ONE; iLoop<EM_DB_TYPE_END;iLoop++)
	{
		//跨大区的话检查一下login的设置
		if(m_bSelected == TRUE)
		{
			//检查 login
			if(stConfigLoginDB[iLoop].strIp.empty() == true)
				return FALSE;
			if(stConfigLoginDB[iLoop].strUser.empty() == true)
				return FALSE;
			if(stConfigLoginDB[iLoop].strPsd.empty() == true)
				return FALSE;
			if(stConfigLoginDB[iLoop].strDBName.empty() == true)
				return FALSE;
			if(stConfigLoginDB[iLoop].dwPort == 0)
				return FALSE;
		}

		//检查 loong
		if(stConfigLoongDB[iLoop].strIp.empty() == true)
			return FALSE;
		if(stConfigLoongDB[iLoop].strUser.empty() == true)
			return FALSE;
		if(stConfigLoongDB[iLoop].strPsd.empty() == true)
			return FALSE;
		if(stConfigLoongDB[iLoop].strDBName.empty() == true)
			return FALSE;
		if(stConfigLoongDB[iLoop].dwPort == 0)
			return FALSE;

	}
	return TRUE;
}
/*
* 从用户设置里读取数据放入属性里
*/
void MainPage::Loong_GetValueFromUserSetting()
{
	for(int iLoop = EM_DB_SRC_ONE; iLoop<EM_DB_TYPE_END; iLoop++)
	{
		//login
		stConfigLoginDB[iLoop].strIp = m_itemLoginIP[iLoop]->GetValue();
		stConfigLoginDB[iLoop].strUser = m_itemLoginUser[iLoop]->GetValue();
		stConfigLoginDB[iLoop].strPsd = m_itemLoginPsd[iLoop]->GetValue();
		stConfigLoginDB[iLoop].strDBName = m_itemLoginName[iLoop]->GetValue();
		stConfigLoginDB[iLoop].dwPort = _ttoi(m_itemLoginPort[iLoop]->GetValue());
		//loong
		stConfigLoongDB[iLoop].strIp = m_itemLoongIP[iLoop]->GetValue();
		stConfigLoongDB[iLoop].strUser = m_itemLoongUser[iLoop]->GetValue();
		stConfigLoongDB[iLoop].strPsd = m_itemLoongPsd[iLoop]->GetValue();
		stConfigLoongDB[iLoop].strDBName = m_itemLoongName[iLoop]->GetValue();
		stConfigLoongDB[iLoop].dwPort = _ttoi(m_itemLoongPort[iLoop]->GetValue());
	}
	m_bSelected = ((wxCheckBox *)FindWindow(ID_CHECKBOX_MERGE_TYPE))->GetValue();
	m_strSrc2WorldName = ((wxTextCtrl*)FindWindow(ID_SRC2_WORLD_NAME))->GetValue();

}
/*
* 显示数据
*/
void MainPage::Loong_ShowDetailOnScreen()
{
	TCHAR cTemp[100];
	for(int iLoop = EM_DB_SRC_ONE; iLoop<EM_DB_TYPE_END; iLoop++)
	{
		//login
		m_itemLoginIP[iLoop]->SetValue(stConfigLoginDB[iLoop].strIp.c_str());
		m_itemLoginUser[iLoop]->SetValue(stConfigLoginDB[iLoop].strUser.c_str());
		m_itemLoginPsd[iLoop]->SetValue(stConfigLoginDB[iLoop].strPsd.c_str());
		m_itemLoginName[iLoop]->SetValue(stConfigLoginDB[iLoop].strDBName.c_str());
		_itot(stConfigLoginDB[iLoop].dwPort,cTemp,10);
		m_itemLoginPort[iLoop]->SetValue(cTemp);

		//loong
		m_itemLoongIP[iLoop]->SetValue(stConfigLoongDB[iLoop].strIp.c_str());
		m_itemLoongUser[iLoop]->SetValue(stConfigLoongDB[iLoop].strUser.c_str());
		m_itemLoongPsd[iLoop]->SetValue(stConfigLoongDB[iLoop].strPsd.c_str());
		m_itemLoongName[iLoop]->SetValue(stConfigLoongDB[iLoop].strDBName.c_str());
		_itot(stConfigLoongDB[iLoop].dwPort,cTemp,10);
		m_itemLoongPort[iLoop]->SetValue(cTemp);

	}
	((wxCheckBox *)FindWindow(ID_CHECKBOX_MERGE_TYPE))->SetValue(m_bSelected);
	((wxTextCtrl*)FindWindow(ID_SRC2_WORLD_NAME))->SetValue(m_strSrc2WorldName.c_str());

	OnRefreshUI();
}
/*
* 把配置文件的值存到本类的内存里
*/
void MainPage::Loong_PutINIValueInToMemory(LPCTSTR pcFileOnePath,LPCTSTR pcFileTwoPath)
{
	TCHAR cTemp[100] = {0};
	TCHAR cTemp2[100] = {0};
	TCHAR cTemp3[100] = {0};
	TCHAR cTemp4[100] = {0};
	//src
	for(int iLoop = EM_DB_SRC_ONE; iLoop<=EM_DB_TAR;iLoop++)
	{
		//login
		if(iLoop != EM_DB_TAR)
			swprintf(cTemp,_T("LoginSrc%d"),iLoop+1);
		else
			swprintf(cTemp,_T("LoginTar"));

		GetPrivateProfileStringW(cTemp,_T("IP"),_T("172.0.0.1"),cTemp2,sizeof(cTemp2),pcFileOnePath);
		stConfigLoginDB[iLoop].strIp = cTemp2;

		GetPrivateProfileStringW(cTemp,_T("USER"),_T("root"),cTemp2,sizeof(cTemp2),pcFileOnePath);
		stConfigLoginDB[iLoop].strUser = cTemp2;

		GetPrivateProfileStringW(cTemp,_T("PSD"),_T("123456"),cTemp2,sizeof(cTemp2),pcFileOnePath);
		TCHAR cOutPsd[X_SHORT_NAME] = {0};
		IUTIL->Decrypt(cTemp2,cOutPsd,X_SHORT_NAME,0);
		stConfigLoginDB[iLoop].strPsd = cOutPsd;

		GetPrivateProfileStringW(cTemp,_T("NAME"),_T(""),cTemp2,sizeof(cTemp2),pcFileOnePath);
		stConfigLoginDB[iLoop].strDBName = cTemp2;

		GetPrivateProfileStringW(cTemp,_T("PORT"),_T("3306"),cTemp2,sizeof(cTemp2),pcFileOnePath);
		stConfigLoginDB[iLoop].dwPort = _ttoi(cTemp2);

	}	
	for(int iLoop = EM_DB_SRC_ONE; iLoop<=EM_DB_TAR;iLoop++)
	{
		//loong	
		if(iLoop != EM_DB_TAR)
			swprintf(cTemp,_T("LoongSrc%d"),iLoop+1);
		else
			swprintf(cTemp,_T("LoongTar"));

		GetPrivateProfileStringW(cTemp,_T("IP"),_T("172.0.0.1"),cTemp2,sizeof(cTemp2),pcFileOnePath);
		stConfigLoongDB[iLoop].strIp = cTemp2;

		GetPrivateProfileStringW(cTemp,_T("USER"),_T("root"),cTemp2,sizeof(cTemp2),pcFileOnePath);
		stConfigLoongDB[iLoop].strUser = cTemp2;

		GetPrivateProfileStringW(cTemp,_T("PSD"),_T("123456"),cTemp2,sizeof(cTemp2),pcFileOnePath);
		TCHAR cOutPsd[X_SHORT_NAME] = {0};
		IUTIL->Decrypt(cTemp2,cOutPsd,X_SHORT_NAME,0);
		stConfigLoongDB[iLoop].strPsd = cOutPsd;

		GetPrivateProfileStringW(cTemp,_T("NAME"),_T(""),cTemp2,sizeof(cTemp2),pcFileOnePath);
		stConfigLoongDB[iLoop].strDBName = cTemp2;

		GetPrivateProfileStringW(cTemp,_T("PORT"),_T("3306"),cTemp2,sizeof(cTemp2),pcFileOnePath);
		stConfigLoongDB[iLoop].dwPort = _ttoi(cTemp2);
	}
	GetPrivateProfileStringW(_T("OPT"),_T("MERGETYPE"),_T("0"),cTemp2,sizeof(cTemp2),pcFileOnePath);
	m_bSelected = _ttoi(cTemp2);

	GetPrivateProfileStringW(_T("OPT"),_T("SRC2WORLDNAME"),_T("世界名"),cTemp2,sizeof(cTemp2),pcFileOnePath);
	m_strSrc2WorldName = cTemp2;

	//取玩家补偿 配置信息
	for(INT32 nLoop = ERC_START; nLoop<ERC_END; nLoop++)
	{
	
		if((ERoleCompensationType)nLoop == ERC_CHANGE_ROLE_NAME)
		{
			swprintf(cTemp,_T("COMPENSATION_%d"),nLoop);
			GetPrivateProfileStringW(cTemp,_T("ITEM_NUM"),_T("0"),cTemp2,sizeof(cTemp2),pcFileTwoPath);
			INT32 nItemNum = _ttoi(cTemp2);
			for(INT32 nLoop = 0; nLoop<nItemNum; nLoop++)
			{	
				swprintf(cTemp3,_T("ITEM_ID_%d"),nLoop);
				GetPrivateProfileStringW(cTemp,cTemp3,_T("0"),cTemp4,sizeof(cTemp4),pcFileTwoPath);
				DWORD dwTemp = _ttoi(cTemp4);

				if(dwTemp != 0)
				{
					tagItemCompensation *pAdd = new tagItemCompensation;
					pAdd->dwItemTypeID = dwTemp;
					m_mmCompensation.insert(make_pair((ERoleCompensationType)nLoop, pAdd));
				}

			}
		}
	}
}
/*
* 把用户设置信息写文件
*/
void MainPage::Loong_SaveUserSettingToINIFile()
{
	//退出时把用户设置写成ini存在当前目录
	TCHAR tszPath[MAX_PATH];
	tstring tstrPath;
	GetModuleFileName(NULL,tszPath,MAX_PATH);

	//把文件路径处理一下
	tstrPath=tszPath;
	tstrPath=tstrPath.substr(0,tstrPath.rfind('\\'));
	tstrPath += _T('/');
	tstrPath += _T(CONFIG_INI); //一个自适应类型的路径

//////////////////////////////////////////////////////////////////////////
	FILE *fp;
	fp=_tfopen(tstrPath.c_str(), _T("w+b"));

	wchar_t strUnicode[1];
	strUnicode[0] = wchar_t(0XFEFF);
	fputwc(*strUnicode,fp);

	fclose(fp);

//////////////////////////////////////////////////////////////////////////

	TCHAR cTemp[100];
	TCHAR cTemp2[100];

	//src
	for(int iLoop = EM_DB_SRC_ONE; iLoop<=EM_DB_TAR  && m_bSelected == TRUE;iLoop++)
	{
		//login
		if(iLoop != EM_DB_TAR)
			swprintf(cTemp,_T("LoginSrc%d"),iLoop+1);
		else
			swprintf(cTemp,_T("LoginTar"));

		WritePrivateProfileStringW(cTemp,_T("IP"),stConfigLoginDB[iLoop].strIp.c_str(),tstrPath.c_str());
		WritePrivateProfileStringW(cTemp,_T("USER"),stConfigLoginDB[iLoop].strUser.c_str(),tstrPath.c_str());

		INT32 nPsdLen = stConfigLoginDB[iLoop].strPsd.length();
		//向上8字节对齐
		nPsdLen = nPsdLen + nPsdLen%8;

		TCHAR cInPsd[X_SHORT_NAME] = {0};
		TCHAR cOutPsd[X_SHORT_NAME] = {0};
		_tcsncpy(cInPsd,stConfigLoginDB[iLoop].strPsd.c_str(),stConfigLoginDB[iLoop].strPsd.length());
		IUTIL->Encrypt(cInPsd,cOutPsd,X_SHORT_NAME,0);
		WritePrivateProfileStringW(cTemp,_T("PSD"),cOutPsd,tstrPath.c_str());

		WritePrivateProfileStringW(cTemp,_T("NAME"),stConfigLoginDB[iLoop].strDBName.c_str(),tstrPath.c_str());
		_itot(stConfigLoginDB[iLoop].dwPort,cTemp2,10);
		WritePrivateProfileStringW(cTemp,_T("PORT"),cTemp2,tstrPath.c_str());


	}	
	for(int iLoop = EM_DB_SRC_ONE; iLoop<=EM_DB_TAR;iLoop++)
	{
		//loong	
		if(iLoop != EM_DB_TAR)
			swprintf(cTemp,_T("LoongSrc%d"),iLoop+1);
		else
			swprintf(cTemp,_T("LoongTar"));

		WritePrivateProfileStringW(cTemp,_T("IP"),stConfigLoongDB[iLoop].strIp.c_str(),tstrPath.c_str());
		WritePrivateProfileStringW(cTemp,_T("USER"),stConfigLoongDB[iLoop].strUser.c_str(),tstrPath.c_str());

		TCHAR cInPsd[X_SHORT_NAME] = {0};
		TCHAR cOutPsd[X_SHORT_NAME] = {0};
		_tcsncpy(cInPsd,stConfigLoongDB[iLoop].strPsd.c_str(),stConfigLoongDB[iLoop].strPsd.length());
		IUTIL->Encrypt(cInPsd,cOutPsd,X_SHORT_NAME,0);
		WritePrivateProfileStringW(cTemp,_T("PSD"),cOutPsd,tstrPath.c_str());

		WritePrivateProfileStringW(cTemp,_T("NAME"),stConfigLoongDB[iLoop].strDBName.c_str(),tstrPath.c_str());
		_itot(stConfigLoongDB[iLoop].dwPort,cTemp2,10);
		WritePrivateProfileStringW(cTemp,_T("PORT"),cTemp2,tstrPath.c_str());
	}
	_itot(m_bSelected,cTemp2,10);
	WritePrivateProfileStringW(_T("OPT"),_T("MERGETYPE"),cTemp2,tstrPath.c_str());
	WritePrivateProfileStringW(_T("OPT"),_T("SRC2WORLDNAME"),m_strSrc2WorldName.c_str(),tstrPath.c_str());

}
UINT MergeWorkerThread(LPVOID pParam)
{
	//异常保护
	EXCEPTION_PROTECT;

	MainPage		*pthis;
	pthis	= (MainPage*)pParam;

	for(;;)
	{
		WaitForSingleObject(pthis->m_MergeEvent, INFINITE);

		//临时
		((wxButton *)pthis->FindWindow(ID_BUTTON_MERGE))->Enable(false);
		BOOL bRet = pthis->m_pclsServerMerge->ServerMerge();
		if(bRet == FALSE)
		{
			//处理重复合并
			if(P_VALID(pthis->m_pclsServerMerge))
			{
				SAFE_DEL(pthis->m_pclsServerMerge);
			}
		}
		((wxButton *)pthis->FindWindow(ID_BUTTON_MERGE))->Enable(true);
	}
	return 0;
}