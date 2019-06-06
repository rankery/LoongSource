// LoongQuestEditorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LoongQuestEditor.h"
#include "LoongQuestEditorDlg.h"

#include "QuestMgr.h"
#include "..\WorldDefine\Questdef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define LQE _T("LoongQuestEditor   ")

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
	virtual BOOL OnInitDialog();
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()
BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	((CStatic*)GetDlgItem( IDC_STATIC_VERSION ))->SetWindowTextW( _T("LoongQuestEditor Version 1.3") );
	((CStatic*)GetDlgItem( IDC_STATIC_DATE ))->SetWindowTextW( _T("Copyright (C) 2008年10月20") );
	return TRUE;
}

// CLoongQuestEditorDlg dialog




CLoongQuestEditorDlg::CLoongQuestEditorDlg(CWnd* pParent /*=NULL*/)
: CDialog(CLoongQuestEditorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pCurrentQuest = NULL;
	m_wstrCurrentQuestFileName = _T("");
	m_wstrQuestPath = _T("");
}

void CLoongQuestEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_ctrlTab);
	DDX_Control(pDX, IDC_TREE1, m_ctrlQuestTree);
	DDX_Control(pDX, IDC_BTN_NEW, m_ctrlNewQuest);
	DDX_Control(pDX, IDC_BTN_SAVE, m_ctrlSaveQuest);
	DDX_Control(pDX, IDC_BTN_SAVE_AS, m_ctrlSaveAsQuest);
	DDX_Control(pDX, IDC_BTN_DEL, m_ctrlDeleteQuest);
	DDX_Control(pDX, IDC_BTN_QUEST_PATH, m_ctrlQuestPath);
	DDX_Control(pDX, IDC_BTN_REFRESH, m_ctrlRefreshQuestTree);
	DDX_Control(pDX, IDC_BTN_EXPORT_QUEST_ACCEPT_RELATION, m_ctrlExportQuestAcceptRelation);
	DDX_Control(pDX, IDC_BTN_EXPORT_QUEST_COMPLETE_RELATION, m_ctrlExportQuestCompleteRelation);
}

BEGIN_MESSAGE_MAP(CLoongQuestEditorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_SAVE, &CLoongQuestEditorDlg::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_QUEST_PATH, &CLoongQuestEditorDlg::OnSetQuestPath)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CLoongQuestEditorDlg::OnTvnSelchangedTree1)
	ON_BN_CLICKED(IDC_BTN_NEW, &CLoongQuestEditorDlg::OnNewQuest)
	ON_BN_CLICKED(IDC_BTN_DEL, &CLoongQuestEditorDlg::OnDeleteQuest)
	ON_BN_CLICKED(IDOK, &CLoongQuestEditorDlg::OnOk)
	ON_BN_CLICKED(IDC_BTN_REFRESH, &CLoongQuestEditorDlg::OnRefreshQuesTree)
	ON_MESSAGE(WM_BUTTONPRESSED,ButtonPressed) ////do this
	ON_BN_CLICKED(IDC_BTN_EXPORT_QUEST_ACCEPT_RELATION, &CLoongQuestEditorDlg::OnBtnExportQuestAcceptRelation)
	ON_BN_CLICKED(IDC_BTN_EXPORT_QUEST_COMPLETE_RELATION, &CLoongQuestEditorDlg::OnBtnExportQuestCompleteRelation)
END_MESSAGE_MAP()


// CLoongQuestEditorDlg message handlers

BOOL CLoongQuestEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	//初始化 QuestMgr
	new QuestMgr;

	//初始化任务数据
	if ( ReadQuestPathFromReg() != TRUE )
	{
		//设置任务路径
		OnSetQuestPath();
	}
	if ( LoadQuest() != TRUE )
	{
		return FALSE;
	}

	//初始化Tab控件
	InitTabCtrl();

	//初始化按钮控件
	m_ctrlNewQuest.MoveWindow( 175, 540, 75, 23 );
	m_ctrlSaveQuest.MoveWindow( 270, 540, 75, 23 );
	//m_ctrlSaveAsQuest.MoveWindow( 365, 540, 75, 23 );
	m_ctrlDeleteQuest.MoveWindow( 460, 540, 75, 23 );
	m_ctrlQuestPath.MoveWindow( 480, 8, 130, 23 );
	m_ctrlRefreshQuestTree.MoveWindow( 365, 540, 75, 23 );
	m_ctrlExportQuestAcceptRelation.MoveWindow( 550, 540, 130, 23 );
	m_ctrlExportQuestCompleteRelation.MoveWindow( 690, 540, 130, 23 );
	//初始化静态文本控件
	wstring tmp;
	wstringstream wss;
	wss << _T("任务总数:") << m_mapQuestXMLFile.size() << endl;
	wss >> tmp;
	((CStatic*)GetDlgItem( IDC_STATIC_QUEST_COUNT ))->SetWindowTextW( tmp.c_str() );
	((CStatic*)GetDlgItem( IDC_STATIC_QUEST_COUNT ))->MoveWindow( 5, 8, 150, 20 );

	((CStatic*)GetDlgItem( IDC_STATIC_QUEST_PATH ))->MoveWindow( 80, 8, 400, 20 );

	tmp = _T("当前任务路径:  ");
	tmp += m_wstrQuestPath;
	((CStatic*)GetDlgItem( IDC_STATIC_QUEST_PATH ))->SetWindowTextW( tmp.c_str() );

	//初始化树形控件
	m_ctrlQuestTree.MoveWindow( 5, 30, 165, 530 );

	//设置对话框的大小
	SetWindowPos( NULL, 0, 0, 845, 600, SWP_NOZORDER|SWP_NOMOVE );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLoongQuestEditorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLoongQuestEditorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLoongQuestEditorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLoongQuestEditorDlg::InitTabCtrl()
{
	//初始化 Tab控件
	m_ctrlTab.Init();

	m_ctrlTab.InsertItem( 0, _T("任务属性") );
	m_ctrlTab.InsertItem( 1, _T("任务接取条件") );
	m_ctrlTab.InsertItem( 2, _T("任务完成条件") );
	m_ctrlTab.InsertItem( 3, _T("任务奖励") );

	//初始化 任务属性 Tab
	//设置 任务基本参数 GroupBox
	m_ctrlTab.CreateGroupBox( _T("任务基本参数"), 1, 0, 205, 180 );
	m_ctrlTab.CreateStatic( _T("ID:"), 2, 0, 0, 5, 18 );
	m_ctrlTab.CreateEdit( FALSE, 3, 0, 0, 75, 15, 120 );
	m_ctrlTab.CreateStatic( _T("名称:"), 4, 0, 0, 5, 48 );
	m_ctrlTab.CreateEdit( FALSE, 5, 0, 0, 75, 45, 120 );
	m_ctrlTab.CreateStatic( _T("前序任务ID:"), 6, 0, 0, 5, 78 );
	m_ctrlTab.CreateEdit( FALSE, 7, 0, 0, 75, 75, 120 );
	m_ctrlTab.CreateStatic( _T("后续任务ID:"), 8, 0, 0, 5, 108 );
	m_ctrlTab.CreateEdit( FALSE, 9, 0, 0, 75, 105, 120 );
	m_ctrlTab.CreateCheckBox( FALSE, _T("后续任务是否自动加载到玩家身上"), 10, 0, 0, 5, 138 );

	//设置 任务接取类型 GroupBox
	m_ctrlTab.CreateGroupBox( _T("任务接取类型"), 11, 0, 205, 50, 0, 0, 190 );
	m_ctrlTab.CreateCheckBox( FALSE, _T("玩家主动接取"), 12, 0, 0, 5, 205 );
	m_ctrlTab.CreateCheckBox( FALSE, _T("物品接取"), 750, 0, 0, 115, 205 );
	m_ctrlTab.CreateCheckBox( FALSE, _T("系统发布"), 13, 0, 0, 5, 220 );

	//设置 任务信息 GroupBox
	m_ctrlTab.CreateGroupBox( _T("任务信息"), 14, 0, 205, 240, 0, 215, 0 );
	m_ctrlTab.CreateStatic( _T("任务条件"), 15, 0, 0, 220, 18 );
	m_ctrlTab.CreateEdit( FALSE, 16, 0, 0, 220, 35, 190, 80 );
	m_ctrlTab.CreateStatic( _T("任务概述"), 17, 0, 0, 220, 125 );
	m_ctrlTab.CreateEdit( FALSE, 18, 0, 0, 220, 140, 190, 80, TRUE );

	//设置 时间 GroupBox
	m_ctrlTab.CreateGroupBox( _T("任务限制时间"), 19, 0, 205,50, 0, 0, 250 );
	m_ctrlTab.CreateEdit( FALSE, 20, 0, 0, 10, 265, 30 );
	m_ctrlTab.CreateStatic( _T("小时"), 21, 0, 0, 45, 268 );
	m_ctrlTab.CreateEdit( FALSE, 22, 0, 0, 78, 265, 30 );
	m_ctrlTab.CreateStatic( _T("分钟"), 23, 0, 0, 113, 268 );
	m_ctrlTab.CreateEdit( FALSE, 24, 0, 0, 145, 265, 30 );
	m_ctrlTab.CreateStatic( _T("秒"), 25, 0, 0, 185, 268 );

	//设置 周期性任务 GroupBox
	m_ctrlTab.CreateGroupBox( _T("周期性任务"), 26, 0, 205,160, 0, 0, 310 );
	m_ctrlTab.CreateCheckBox( FALSE, _T("周期性任务"), 27, 0, 0, 5, 328 );
	m_ctrlTab.CreateGroupBox( _T("周期"), 28, 0, 150,40, 0, 5, 350 );
	m_ctrlTab.CreateRadioBox( FALSE, _T("每周"), 29, 0, 0, 10, 365, WS_GROUP );
	m_ctrlTab.CreateRadioBox( FALSE, _T("每日"), 30, 0, 0, 70, 365 );
	m_ctrlTab.CreateGroupBox( _T("星期"), 31, 0, 180,60, 0, 5, 400 );
	m_ctrlTab.CreateRadioBox( FALSE, _T("一"), 32, 0, 0, 10, 415, WS_GROUP );
	m_ctrlTab.CreateRadioBox( FALSE, _T("二"), 33, 0, 0, 50, 415 );
	m_ctrlTab.CreateRadioBox( FALSE, _T("三"), 34, 0, 0, 90, 415 );
	m_ctrlTab.CreateRadioBox( FALSE, _T("四"), 35, 0, 0, 130, 415 );
	m_ctrlTab.CreateRadioBox( FALSE, _T("五"), 36, 0, 0, 10, 435 );
	m_ctrlTab.CreateRadioBox( FALSE, _T("六"), 37, 0, 0, 60, 435 );
	m_ctrlTab.CreateRadioBox( FALSE, _T("日"), 38, 0, 0, 110, 435 );

	m_ctrlTab.GetDlgItem( 29 )->EnableWindow( FALSE );
	m_ctrlTab.GetDlgItem( 30 )->EnableWindow( FALSE );
	m_ctrlTab.GetDlgItem( 32 )->EnableWindow( FALSE );
	m_ctrlTab.GetDlgItem( 33 )->EnableWindow( FALSE );
	m_ctrlTab.GetDlgItem( 34 )->EnableWindow( FALSE );
	m_ctrlTab.GetDlgItem( 35 )->EnableWindow( FALSE );
	m_ctrlTab.GetDlgItem( 36 )->EnableWindow( FALSE );
	m_ctrlTab.GetDlgItem( 37 )->EnableWindow( FALSE );
	m_ctrlTab.GetDlgItem( 38 )->EnableWindow( FALSE );

	//设置 任务类型 GroupBox
	m_ctrlTab.CreateGroupBox( _T("任务类型"), 39, 0, 100, 200, 0, 435, 0 );
	m_ctrlTab.CreateRadioBox( TRUE, _T("新手任务"), 40, 0, 0, 445, 15, WS_GROUP );
	m_ctrlTab.CreateRadioBox( FALSE, _T("主线任务"), 41, 0, 0, 445, 30 );
	m_ctrlTab.CreateRadioBox( FALSE, _T("活动任务"), 42, 0, 0, 445, 45 );
	m_ctrlTab.CreateRadioBox( FALSE, _T("剧情任务"), 43, 0, 0, 445, 60 );
	m_ctrlTab.CreateRadioBox( FALSE, _T("职业任务"), 44, 0, 0, 445, 75 );
	m_ctrlTab.CreateRadioBox( FALSE, _T("情侣任务"), 45, 0, 0, 445, 90 );
	m_ctrlTab.CreateRadioBox( FALSE, _T("门派任务"), 46, 0, 0, 445, 105 );
	m_ctrlTab.CreateRadioBox( FALSE, _T("奇遇任务"), 47, 0, 0, 445, 120 );
	m_ctrlTab.CreateRadioBox( FALSE, _T("探险任务"), 48, 0, 0, 445, 135 );
	m_ctrlTab.CreateRadioBox( FALSE, _T("教学任务"), 49, 0, 0, 445, 150 );
	m_ctrlTab.CreateRadioBox( FALSE, _T("夫妻任务"), 50, 0, 0, 445, 165 );
	m_ctrlTab.CreateRadioBox( FALSE, _T("技能任务"), 51, 0, 0, 445, 180 );

	//设置 任务物品 GroupBox
	m_ctrlTab.CreateGroupBox( _T("任务物品"), 651, 0, 200, 45, 0, 435, 200 );
	m_ctrlTab.CreateStatic( _T("ID:"), 652, 0, 0, 440, 218 );
	m_ctrlTab.CreateEdit( FALSE, 653, 0, 0, 462, 215, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 654, 0, 0, 565, 218 );
	m_ctrlTab.CreateEdit( FALSE, 655, 0, 0, 595, 215, 30 );

	//设置 接取NPC GroupBox
	m_ctrlTab.CreateGroupBox( _T("接取NPC"), 600, 0, 205, 105, 0, 425, 250 );
	m_ctrlTab.CreateStatic( _T("ID1:"), 601, 0, 0, 433, 268 );
	m_ctrlTab.CreateEdit( FALSE, 602, 0, 0, 455, 265, 70 );
	m_ctrlTab.CreateStatic( _T("ID2:"), 603, 0, 0, 530, 268 );
	m_ctrlTab.CreateEdit( FALSE, 604, 0, 0, 552, 265, 70 );
	m_ctrlTab.CreateStatic( _T("ID3:"), 605, 0, 0, 433, 300 );
	m_ctrlTab.CreateEdit( FALSE, 606, 0, 0, 455, 295, 70 );
	m_ctrlTab.CreateStatic( _T("ID4:"), 607, 0, 0, 530, 300 );
	m_ctrlTab.CreateEdit( FALSE, 608, 0, 0, 552, 295, 70 );
	m_ctrlTab.CreateStatic( _T("ID5:"), 609, 0, 0, 433, 330 );
	m_ctrlTab.CreateEdit( FALSE, 610, 0, 0, 455, 325, 70 );
	m_ctrlTab.CreateStatic( _T("ID6:"), 611, 0, 0, 530, 330 );
	m_ctrlTab.CreateEdit( FALSE, 612, 0, 0, 552, 325, 70 );

	//设置 完成NPC GroupBox
	m_ctrlTab.CreateGroupBox( _T("完成NPC"), 613, 0, 205, 105, 0, 425, 360 );
	m_ctrlTab.CreateStatic( _T("ID1:"), 614, 0, 0, 433, 378 );
	m_ctrlTab.CreateEdit( FALSE, 615, 0, 0, 455, 375, 70 );
	m_ctrlTab.CreateStatic( _T("ID2:"), 616, 0, 0, 530, 378 );
	m_ctrlTab.CreateEdit( FALSE, 617, 0, 0, 552, 375, 70 );
	m_ctrlTab.CreateStatic( _T("ID3:"), 618, 0, 0, 433, 410 );
	m_ctrlTab.CreateEdit( FALSE, 619, 0, 0, 455, 405, 70 );
	m_ctrlTab.CreateStatic( _T("ID4:"), 620, 0, 0, 530, 410 );
	m_ctrlTab.CreateEdit( FALSE, 621, 0, 0, 552, 405, 70 );
	m_ctrlTab.CreateStatic( _T("ID5:"), 622, 0, 0, 433, 440 );
	m_ctrlTab.CreateEdit( FALSE, 623, 0, 0, 455, 435, 70 );
	m_ctrlTab.CreateStatic( _T("ID6:"), 624, 0, 0, 530, 440 );
	m_ctrlTab.CreateEdit( FALSE, 625, 0, 0, 552, 435, 70 );

	//设置 任务参数 GroupBox
	m_ctrlTab.CreateGroupBox( _T("任务参数"), 52, 0, 205,100, 0, 215, 250 );
	m_ctrlTab.CreateStatic( _T("建议玩家个数:"), 53, 0, 0, 225, 268 );
	m_ctrlTab.CreateEdit( FALSE, 54, 0, 0, 360, 265, 50 );
	m_ctrlTab.CreateStatic( _T("每个玩家接取任务次数:"), 55, 0, 0, 225, 295 );
	m_ctrlTab.CreateEdit( FALSE, 56, 0, 0, 360, 290, 50 );
	m_ctrlTab.CreateCheckBox( FALSE, _T("任务可以重复接取"), 57, 0, 0, 225, 325 );

	//设置 任务脚本 GroupBox
	m_ctrlTab.CreateGroupBox( _T("任务脚本"), 58, 0, 205,80, 0, 215, 360 );
	m_ctrlTab.CreateStatic( _T("接取任务时用行的脚本ID:"), 59, 0, 0, 225, 383 );
	m_ctrlTab.CreateEdit( FALSE, 60, 0, 0, 365, 378, 50 );
	m_ctrlTab.CreateStatic( _T("完成任务时用行的脚本ID:"), 61, 0, 0, 225, 413 );
	m_ctrlTab.CreateEdit( FALSE, 62, 0, 0, 365, 410, 50 );

	//初始化 任务接取条件 Tab
	//设置 等级 GroupBox
	m_ctrlTab.CreateGroupBox( _T("等级"), 63, 1, 195, 80 );
	m_ctrlTab.CreateStatic( _T("最小等级:"), 64, 1, 0, 5, 18 );
	m_ctrlTab.CreateEdit( FALSE, 65, 1, 0, 75, 15 );
	m_ctrlTab.CreateStatic( _T("最大等级:"), 66, 1, 0, 5, 48 );
	m_ctrlTab.CreateEdit( FALSE, 67, 1, 0, 75, 48 );

	//设置 声望 GroupBox
	m_ctrlTab.CreateGroupBox( _T("声望"), 68, 1, 195, 125, 0, 0, 85 );
	m_ctrlTab.CreateStatic( _T("声望1:"), 69, 1, 0, 5, 103 );
	m_ctrlTab.CreateEdit( FALSE, 70, 1, 0, 40, 100, 30 );
	m_ctrlTab.CreateStatic( _T("Max:"), 71, 1, 0, 75, 103 );
	m_ctrlTab.CreateEdit( FALSE, 72, 1, 0, 100, 100, 30 );
	m_ctrlTab.CreateStatic( _T("Min:"), 73, 1, 0, 135, 103 );
	m_ctrlTab.CreateEdit( FALSE, 74, 1, 0, 160, 100, 30 );

	m_ctrlTab.CreateStatic( _T("声望2:"), 75, 1, 0, 5, 128 );
	m_ctrlTab.CreateEdit( FALSE, 76, 1, 0, 40, 126, 30 );
	m_ctrlTab.CreateStatic( _T("Max:"), 77, 1, 0, 75, 128 );
	m_ctrlTab.CreateEdit( FALSE, 78, 1, 0, 100, 126, 30 );
	m_ctrlTab.CreateStatic( _T("Min:"), 79, 1, 0, 135, 128 );
	m_ctrlTab.CreateEdit( FALSE, 80, 1, 0, 160, 126, 30 );


	m_ctrlTab.CreateStatic( _T("声望3:"), 81, 1, 0, 5, 153 );
	m_ctrlTab.CreateEdit( FALSE, 82, 1, 0, 40, 151, 30 );
	m_ctrlTab.CreateStatic( _T("Max:"), 83, 1, 0, 75, 153 );
	m_ctrlTab.CreateEdit( FALSE, 84, 1, 0, 100, 151, 30 );
	m_ctrlTab.CreateStatic( _T("Min:"), 85, 1, 0, 135, 153 );
	m_ctrlTab.CreateEdit( FALSE, 86, 1, 0, 160, 151, 30 );

	m_ctrlTab.CreateStatic( _T("声望4:"), 87, 1, 0, 5, 178 );
	m_ctrlTab.CreateEdit( FALSE, 88, 1, 0, 40, 176, 30 );
	m_ctrlTab.CreateStatic( _T("Max:"), 89, 1, 0, 75, 178 );
	m_ctrlTab.CreateEdit( FALSE, 90, 1, 0, 100, 176, 30 );
	m_ctrlTab.CreateStatic( _T("Min:"), 91, 1, 0, 135, 178 );
	m_ctrlTab.CreateEdit( FALSE, 92, 1, 0, 160, 176, 30 );

	//设置 技能 GroupBox
	m_ctrlTab.CreateGroupBox( _T("技能"), 93, 1, 195, 125, 0, 0, 215 );
	m_ctrlTab.CreateStatic( _T("技能1:"), 94, 1, 0, 5, 233 );
	m_ctrlTab.CreateEdit( FALSE, 95, 1, 0, 40, 230, 60 );
	m_ctrlTab.CreateStatic( _T("值:"), 96, 1, 0, 105, 233 );
	m_ctrlTab.CreateEdit( FALSE, 97, 1, 0, 125, 230, 60 );

	m_ctrlTab.CreateStatic( _T("技能2:"), 98, 1, 0, 5, 258 );
	m_ctrlTab.CreateEdit( FALSE, 99, 1, 0, 40, 255, 60 );
	m_ctrlTab.CreateStatic( _T("值:"), 100, 1, 0, 105, 258 );
	m_ctrlTab.CreateEdit( FALSE, 101, 1, 0, 125, 255, 60 );

	m_ctrlTab.CreateStatic( _T("技能3:"), 102, 1, 0, 5, 283 );
	m_ctrlTab.CreateEdit( FALSE, 103, 1, 0, 40, 280, 60 );
	m_ctrlTab.CreateStatic( _T("值:"), 104, 1, 0, 105, 283 );
	m_ctrlTab.CreateEdit( FALSE, 105, 1, 0, 125, 280, 60 );

	m_ctrlTab.CreateStatic( _T("技能4:"), 106, 1, 0, 5, 308 );
	m_ctrlTab.CreateEdit( FALSE, 107, 1, 0, 40, 305, 60 );
	m_ctrlTab.CreateStatic( _T("值:"), 108, 1, 0, 105, 308 );
	m_ctrlTab.CreateEdit( FALSE, 109, 1, 0, 125, 305, 60 );

	//设置 物品 GroupBox
	m_ctrlTab.CreateGroupBox( _T("物品"), 110, 1, 195, 240, 0, 205, 0 );
	m_ctrlTab.CreateStatic( _T("ID1:"), 111, 1, 0, 210, 18 );
	m_ctrlTab.CreateEdit( FALSE, 112, 1, 0, 232, 15, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 113, 1, 0, 335, 18 );
	m_ctrlTab.CreateEdit( FALSE, 114, 1, 0, 365, 15, 30 );

	m_ctrlTab.CreateStatic( _T("ID2:"), 115, 1, 0, 210, 43 );
	m_ctrlTab.CreateEdit( FALSE, 116, 1, 0, 232, 40, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 117, 1, 0, 335, 43 );
	m_ctrlTab.CreateEdit( FALSE, 118, 1, 0, 365, 40, 30 );

	m_ctrlTab.CreateStatic( _T("ID3:"), 119, 1, 0, 210, 68 );
	m_ctrlTab.CreateEdit( FALSE, 120, 1, 0, 232, 65, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 121, 1, 0, 335, 68 );
	m_ctrlTab.CreateEdit( FALSE, 122, 1, 0, 365, 65, 30 );

	m_ctrlTab.CreateStatic( _T("ID4:"), 123, 1, 0, 210, 93 );
	m_ctrlTab.CreateEdit( FALSE, 124, 1, 0, 232, 90, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 125, 1, 0, 335, 93 );
	m_ctrlTab.CreateEdit( FALSE, 126, 1, 0, 365, 90, 30 );

	m_ctrlTab.CreateStatic( _T("ID5:"), 127, 1, 0, 210, 118 );
	m_ctrlTab.CreateEdit( FALSE, 128, 1, 0, 232, 115, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 129, 1, 0, 335, 118 );
	m_ctrlTab.CreateEdit( FALSE, 130, 1, 0, 365, 115, 30 );

	m_ctrlTab.CreateStatic( _T("ID6:"), 131, 1, 0, 210, 143 );
	m_ctrlTab.CreateEdit( FALSE, 132, 1, 0, 232, 140, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 133, 1, 0, 335, 143 );
	m_ctrlTab.CreateEdit( FALSE, 134, 1, 0, 365, 140, 30 );

	m_ctrlTab.CreateStatic( _T("ID7:"), 135, 1, 0, 210, 168 );
	m_ctrlTab.CreateEdit( FALSE, 136, 1, 0, 232, 165, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 137, 1, 0, 335, 168 );
	m_ctrlTab.CreateEdit( FALSE, 138, 1, 0, 365, 165, 30 );

	m_ctrlTab.CreateStatic( _T("ID8:"), 139, 1, 0, 210, 193 );
	m_ctrlTab.CreateEdit( FALSE, 140, 1, 0, 232, 190, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 141, 1, 0, 335, 193 );
	m_ctrlTab.CreateEdit( FALSE, 142, 1, 0, 365, 190, 30 );

	m_ctrlTab.CreateCheckBox( FALSE, _T("删除限制物品"), 160, 1, 0, 210, 215 );

	//设置 触发器 GroupBox
	m_ctrlTab.CreateGroupBox( _T("触发器"), 700, 1, 170, 220, 0, 410, 0 );
	m_ctrlTab.CreateStatic( _T("ID1:"), 701, 1, 0, 415, 18 );
	m_ctrlTab.CreateEdit( FALSE, 702, 1, 0, 437, 15, 100 );

	m_ctrlTab.CreateStatic( _T("ID2:"), 703, 1, 0, 415, 43 );
	m_ctrlTab.CreateEdit( FALSE, 704, 1, 0, 437, 40, 100 );
	
	m_ctrlTab.CreateStatic( _T("ID3:"), 705, 1, 0, 415, 68 );
	m_ctrlTab.CreateEdit( FALSE, 706, 1, 0, 437, 65, 100 );

	m_ctrlTab.CreateStatic( _T("ID4:"), 707, 1, 0, 415, 93 );
	m_ctrlTab.CreateEdit( FALSE, 708, 1, 0, 437, 90, 100 );

	m_ctrlTab.CreateStatic( _T("ID5:"), 709, 1, 0, 415, 118 );
	m_ctrlTab.CreateEdit( FALSE, 710, 1, 0, 437, 115, 100 );

	m_ctrlTab.CreateStatic( _T("ID6:"), 711, 1, 0, 415, 143 );
	m_ctrlTab.CreateEdit( FALSE, 712, 1, 0, 437, 140, 100 );

	m_ctrlTab.CreateStatic( _T("ID7:"), 713, 1, 0, 415, 168 );
	m_ctrlTab.CreateEdit( FALSE, 714, 1, 0, 437, 165, 100 );

	m_ctrlTab.CreateStatic( _T("ID8:"), 715, 1, 0, 415, 193 );
	m_ctrlTab.CreateEdit( FALSE, 716, 1, 0, 437, 190, 100 );

	//设置 人物属性 GroupBox
	m_ctrlTab.CreateGroupBox( _T("人物属性"), 143, 1, 195, 125, 0, 205, 250 );
	m_ctrlTab.CreateStatic( _T("属性1:"),144, 1, 0, 210, 268 );
	m_ctrlTab.CreateEdit( FALSE, 145, 1, 0, 245, 265, 60 );
	m_ctrlTab.CreateStatic( _T("值:"), 146, 1, 0, 315, 268 );
	m_ctrlTab.CreateEdit( FALSE, 147, 1, 0, 335, 265, 60 );

	m_ctrlTab.CreateStatic( _T("属性2:"),148, 1, 0, 210, 293 );
	m_ctrlTab.CreateEdit( FALSE, 149, 1, 0, 245, 290, 60 );
	m_ctrlTab.CreateStatic( _T("值:"), 150, 1, 0, 315, 293 );
	m_ctrlTab.CreateEdit( FALSE, 151, 1, 0, 335, 290, 60 );

	m_ctrlTab.CreateStatic( _T("属性3:"),152, 1, 0, 210, 318 );
	m_ctrlTab.CreateEdit( FALSE, 153, 1, 0, 245, 315, 60 );
	m_ctrlTab.CreateStatic( _T("值:"), 154, 1, 0, 315, 318 );
	m_ctrlTab.CreateEdit( FALSE, 155, 1, 0, 335, 315, 60 );

	m_ctrlTab.CreateStatic( _T("属性4:"),156, 1, 0, 210, 343 );
	m_ctrlTab.CreateEdit( FALSE, 157, 1, 0, 245, 340, 60 );
	m_ctrlTab.CreateStatic( _T("值:"), 158, 1, 0, 315, 343 );
	m_ctrlTab.CreateEdit( FALSE, 159, 1, 0, 335, 340, 60 );

	//设置 职业 GroupBox
	m_ctrlTab.CreateGroupBox( _T("职业"), 161, 1, 125, 40, 0, 0, 360 );
	m_ctrlTab.CreateStatic( _T("ID:"),162, 1, 0, 5, 377 );
	m_ctrlTab.CreateEdit( FALSE, 163, 1, 0, 30, 375, 80 );

	//设置 性别 GroupBox
	m_ctrlTab.CreateGroupBox( _T("性别"), 164, 1, 125, 40, 0, 0, 420 );
	m_ctrlTab.CreateRadioBox( FALSE, _T("男"), 165, 1, 0, 5, 437, WS_GROUP );
	m_ctrlTab.CreateRadioBox( FALSE, _T("女"), 166, 1, 0, 37, 437 );
	m_ctrlTab.CreateRadioBox( FALSE, _T("无限制"), 500, 1, 0, 70, 437 );

	//设置 地图 GroupBox
	m_ctrlTab.CreateGroupBox( _T("地图"), 167, 1, 125, 40, 0, 145, 420);
	m_ctrlTab.CreateStatic( _T("ID:"),168, 1, 0, 150, 437 );
	m_ctrlTab.CreateEdit( FALSE, 169, 1, 0, 170, 435, 80 );

	//设置 夫妻 GroupBox
	m_ctrlTab.CreateGroupBox( _T("夫妻"), 760, 1, 80, 40, 0, 280, 420);
	m_ctrlTab.CreateCheckBox( FALSE, _T("是否结婚"), 761, 1, 0, 285, 435 );	
	
	//设置 情侣 GroupBox
	m_ctrlTab.CreateGroupBox( _T("情侣"), 762, 1, 120, 40, 0, 370, 420);
	m_ctrlTab.CreateStatic( _T("情侣数量:"),763, 1, 0, 375, 437 );
	m_ctrlTab.CreateEdit( FALSE, 764, 1, 0, 430, 435, 50 );

	//初始化 任务完成条件 Tab
	//设置 物品 GroupBox
	m_ctrlTab.CreateGroupBox( _T("物品"), 170, 2, 200, 250 );
	m_ctrlTab.CreateStatic( _T("ID1:"), 171, 2, 0, 5, 18 );
	m_ctrlTab.CreateEdit( FALSE, 172, 2, 0, 27, 15, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 173, 2, 0, 130, 18 );
	m_ctrlTab.CreateEdit( FALSE, 174, 2, 0, 165, 15, 30 );

	m_ctrlTab.CreateStatic( _T("ID2:"), 175, 2, 0, 5, 43 );
	m_ctrlTab.CreateEdit( FALSE, 176, 2, 0, 27, 40, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 177, 2, 0, 130, 43 );
	m_ctrlTab.CreateEdit( FALSE, 178, 2, 0, 165, 40, 30 );

	m_ctrlTab.CreateStatic( _T("ID3:"), 179, 2, 0, 5, 68 );
	m_ctrlTab.CreateEdit( FALSE, 180, 2, 0, 27, 65, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 181, 2, 0, 130, 68 );
	m_ctrlTab.CreateEdit( FALSE, 182, 2, 0, 165, 65, 30 );

	m_ctrlTab.CreateStatic( _T("ID4:"), 183, 2, 0, 5, 93 );
	m_ctrlTab.CreateEdit( FALSE, 184, 2, 0, 27, 90, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 185, 2, 0, 130, 93 );
	m_ctrlTab.CreateEdit( FALSE, 186, 2, 0, 165, 90, 30 );

	m_ctrlTab.CreateStatic( _T("ID5:"), 187, 2, 0, 5, 118 );
	m_ctrlTab.CreateEdit( FALSE, 188, 2, 0, 27, 115, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 189, 2, 0,130, 118 );
	m_ctrlTab.CreateEdit( FALSE, 190, 2, 0, 165, 115, 30 );

	m_ctrlTab.CreateStatic( _T("ID6:"), 191, 2, 0, 5, 143 );
	m_ctrlTab.CreateEdit( FALSE, 192, 2, 0, 27, 140, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 193, 2, 0, 130, 143 );
	m_ctrlTab.CreateEdit( FALSE, 194, 2, 0, 165, 140, 30 );

	m_ctrlTab.CreateStatic( _T("ID7:"), 195, 2, 0, 5, 168 );
	m_ctrlTab.CreateEdit( FALSE, 196, 2, 0, 27, 165, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 197, 2, 0, 130, 168 );
	m_ctrlTab.CreateEdit( FALSE, 198, 2, 0, 165, 165, 30 );

	m_ctrlTab.CreateStatic( _T("ID8:"), 199, 2, 0, 5, 193 );
	m_ctrlTab.CreateEdit( FALSE, 200, 2, 0, 27, 190, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 201, 2, 0, 130, 193 );
	m_ctrlTab.CreateEdit( FALSE, 202, 2, 0, 165, 190, 30 );

	m_ctrlTab.CreateCheckBox( FALSE, _T("搜集完其中一种就可完成"), 203, 2, 0, 5, 215 );
	m_ctrlTab.CreateCheckBox( FALSE, _T("完成任务后删除物品"), 599, 2, 0, 5, 230 );

	//设置 NPC GroupBox
	m_ctrlTab.CreateGroupBox( _T("NPC"), 204, 2, 200, 250, 0, 210, 0 );
	m_ctrlTab.CreateStatic( _T("ID1:"), 205, 2, 0, 215, 18 );
	m_ctrlTab.CreateEdit( FALSE, 206, 2, 0, 237, 15, 100 );
	m_ctrlTab.CreateStatic( _T("ID2:"), 207, 2, 0, 215, 43 );
	m_ctrlTab.CreateEdit( FALSE, 208, 2, 0, 237, 40, 100 );
	m_ctrlTab.CreateStatic( _T("ID3:"), 209, 2, 0, 215, 68 );
	m_ctrlTab.CreateEdit( FALSE, 210, 2, 0, 237, 65, 100 );
	m_ctrlTab.CreateStatic( _T("ID4:"), 211, 2, 0, 215, 93 );
	m_ctrlTab.CreateEdit( FALSE, 212, 2, 0, 237, 90, 100 );
	m_ctrlTab.CreateStatic( _T("ID5:"), 213, 2, 0, 215, 118 );
	m_ctrlTab.CreateEdit( FALSE, 214, 2, 0, 237, 115, 100 );
	m_ctrlTab.CreateStatic( _T("ID6:"), 215, 2, 0, 215, 143 );
	m_ctrlTab.CreateEdit( FALSE, 216, 2, 0, 237, 140, 100 );
	m_ctrlTab.CreateStatic( _T("ID7:"), 217, 2, 0, 215, 168 );
	m_ctrlTab.CreateEdit( FALSE, 218, 2, 0, 237, 165, 100 );
	m_ctrlTab.CreateStatic( _T("ID8:"), 219, 2, 0, 215, 193 );
	m_ctrlTab.CreateEdit( FALSE, 220, 2, 0, 237, 190, 100 );

	m_ctrlTab.CreateCheckBox( FALSE, _T("必须按顺序对话"), 221, 2, 0, 215, 215 );
	m_ctrlTab.CreateCheckBox( FALSE, _T("只和一个NPC对话后就可完成"), 222, 2, 0, 215, 230 );

	//设置 触发器 GroupBox
	m_ctrlTab.CreateGroupBox( _T("触发器"), 223, 2, 170, 250, 0, 420, 0 );
	m_ctrlTab.CreateStatic( _T("ID1:"), 224, 2, 0, 425, 18 );
	m_ctrlTab.CreateEdit( FALSE, 225, 2, 0, 447, 15, 100 );
	m_ctrlTab.CreateStatic( _T("ID2:"), 226, 2, 0, 425, 43 );
	m_ctrlTab.CreateEdit( FALSE, 227, 2, 0, 447, 40, 100 );
	m_ctrlTab.CreateStatic( _T("ID3:"), 228, 2, 0, 425, 68 );
	m_ctrlTab.CreateEdit( FALSE, 229, 2, 0, 447, 65, 100 );
	m_ctrlTab.CreateStatic( _T("ID4:"), 230, 2, 0, 425, 93 );
	m_ctrlTab.CreateEdit( FALSE, 231, 2, 0, 447, 90, 100 );
	m_ctrlTab.CreateStatic( _T("ID5:"), 232, 2, 0, 425, 118 );
	m_ctrlTab.CreateEdit( FALSE, 233, 2, 0, 447, 115, 100 );
	m_ctrlTab.CreateStatic( _T("ID6:"), 234, 2, 0, 425, 143 );
	m_ctrlTab.CreateEdit( FALSE, 235, 2, 0, 447, 140, 100 );
	m_ctrlTab.CreateStatic( _T("ID7:"), 236, 2, 0, 425, 168 );
	m_ctrlTab.CreateEdit( FALSE, 237, 2, 0, 447, 165, 100 );
	m_ctrlTab.CreateStatic( _T("ID8:"), 238, 2, 0, 425, 193 );
	m_ctrlTab.CreateEdit( FALSE, 239, 2, 0, 447, 190, 100 );

	m_ctrlTab.CreateCheckBox( FALSE, _T("只触发一个就可完成"), 240, 2, 0, 425, 215 );

	//设置 怪物数量 GroupBox
	m_ctrlTab.CreateGroupBox( _T("怪物数量"), 241, 2, 210, 210, 0, 0, 255 );
	m_ctrlTab.CreateStatic( _T("ID1:"), 242, 2, 0, 5, 273 );
	m_ctrlTab.CreateEdit( FALSE, 243, 2, 0, 27, 270, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 244, 2, 0, 130, 273 );
	m_ctrlTab.CreateEdit( FALSE, 245, 2, 0, 165, 270, 30 );

	m_ctrlTab.CreateStatic( _T("ID2:"), 246, 2, 0, 5, 298 );
	m_ctrlTab.CreateEdit( FALSE, 247, 2, 0, 27, 295, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 248, 2, 0, 130, 298 );
	m_ctrlTab.CreateEdit( FALSE, 249, 2, 0, 165, 295, 30 );

	m_ctrlTab.CreateStatic( _T("ID3:"), 250, 2, 0, 5, 323 );
	m_ctrlTab.CreateEdit( FALSE, 251, 2, 0, 27, 320, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 252, 2, 0, 130, 323 );
	m_ctrlTab.CreateEdit( FALSE, 253, 2, 0, 165, 320, 30 );

	m_ctrlTab.CreateStatic( _T("ID4:"), 254, 2, 0, 5, 348 );
	m_ctrlTab.CreateEdit( FALSE, 255, 2, 0, 27, 345, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 256, 2, 0, 130, 348 );
	m_ctrlTab.CreateEdit( FALSE, 257, 2, 0, 165, 345, 30 );

	m_ctrlTab.CreateStatic( _T("ID5:"), 258, 2, 0, 5, 373 );
	m_ctrlTab.CreateEdit( FALSE, 259, 2, 0, 27, 370, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 260, 2, 0, 130, 373 );
	m_ctrlTab.CreateEdit( FALSE, 261, 2, 0, 165, 370, 30 );

	m_ctrlTab.CreateStatic( _T("ID6:"), 262, 2, 0, 5, 398 );
	m_ctrlTab.CreateEdit( FALSE, 263, 2, 0, 27, 395, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 264, 2, 0, 130, 398 );
	m_ctrlTab.CreateEdit( FALSE, 265, 2, 0, 165, 395, 30 );

	m_ctrlTab.CreateCheckBox( FALSE, _T("指定怪物等级"), 780, 2, 0, 5, 425 );
	m_ctrlTab.CreateCheckBox( FALSE, _T("杀完指定数量的一种怪物就可完成"), 266, 2, 0, 5, 440 );

	//设置 金钱 GroupBox
	m_ctrlTab.CreateGroupBox( _T("金钱"), 267, 2, 150, 40, 0, 220, 255 );
	m_ctrlTab.CreateStatic( _T("数量:"), 268, 2, 0, 225, 273 );
	m_ctrlTab.CreateEdit( FALSE, 269, 2, 0, 260, 270, 90 );

	//设置 等级 GroupBox
	m_ctrlTab.CreateGroupBox( _T("等级"), 720, 2, 150, 40, 0, 220, 295 );
	m_ctrlTab.CreateStatic( _T("等级:"), 721, 2, 0, 225, 313 );
	m_ctrlTab.CreateEdit( FALSE, 722, 2, 0, 260, 310, 90 );

	//设置 夫妻 GroupBox
	m_ctrlTab.CreateGroupBox( _T("夫妻"), 770, 2, 80, 40, 0, 220, 335);
	m_ctrlTab.CreateCheckBox( FALSE, _T("是否结婚"), 771, 2, 0, 225, 350 );	

	//设置 情侣 GroupBox
	m_ctrlTab.CreateGroupBox( _T("情侣"), 772, 2, 120, 40, 0, 220, 375);
	m_ctrlTab.CreateStatic( _T("情侣数量:"),773, 2, 0, 225, 393 );
	m_ctrlTab.CreateEdit( FALSE, 774, 2, 0, 280, 390, 50 );

	//设置 选择条件 GroupBox
	m_ctrlTab.CreateGroupBox( _T("选择条件"), 273, 2, 240, 40, 0, 220, 420 );
	m_ctrlTab.CreateCheckBox( FALSE, _T("以上所有条件完成一项即可完成整个任务"), 274, 2, 0, 225, 435 );

	
	//设置 地图 GroupBox
	m_ctrlTab.CreateGroupBox( _T("地图"), 275, 2, 150, 165, 0, 380, 255 );
	m_ctrlTab.CreateStatic( _T("ID:"), 276, 2, 0, 385, 273 );
	m_ctrlTab.CreateEdit( FALSE, 277, 2, 0, 415, 270, 100 );
	m_ctrlTab.CreateStatic( _T("X:"), 278, 2, 0, 385, 298 );
	m_ctrlTab.CreateEdit( FALSE, 279, 2, 0, 415, 295, 100 );
	m_ctrlTab.CreateStatic( _T("Y:"), 280, 2, 0, 385, 323 );
	m_ctrlTab.CreateEdit( FALSE, 281, 2, 0, 415, 320, 100 );
	m_ctrlTab.CreateStatic( _T("Z:"), 282, 2, 0, 385, 348 );
	m_ctrlTab.CreateEdit( FALSE, 283, 2, 0, 415, 345, 100 );
	m_ctrlTab.CreateStatic( _T("半径:"), 800, 2, 0, 385, 373 );
	m_ctrlTab.CreateEdit( FALSE, 801, 2, 0, 415, 370, 100 );
	m_ctrlTab.CreateCheckBox( FALSE, _T("这个坐标是否随机生成"), 284, 2, 0, 385, 395 );

	//设置 事件 GroupBox
	m_ctrlTab.CreateGroupBox( _T("事件"), 790, 2, 115, 165, 0, 535, 255 );
	m_ctrlTab.CreateRadioBox( FALSE, _T("使用物品"), 791, 2, 0, 540, 273, WS_GROUP );
	m_ctrlTab.CreateStatic( _T("物品ID:"), 792, 2, 0, 540, 298 );
	m_ctrlTab.CreateEdit( FALSE, 793, 2, 0, 580, 295, 65 );
	m_ctrlTab.CreateRadioBox( FALSE, _T("使用技能"), 794, 2, 0, 540, 320 );
	m_ctrlTab.CreateStatic( _T("技能ID:"), 795, 2, 0, 540, 345 );
	m_ctrlTab.CreateEdit( FALSE, 796, 2, 0, 580, 342, 65 );
	m_ctrlTab.CreateRadioBox( FALSE, _T("待定"), 797, 2, 0, 540, 367 );
	m_ctrlTab.CreateStatic( _T("待定"), 798, 2, 0, 540, 392 );
	m_ctrlTab.CreateEdit( FALSE, 799, 2, 0, 580, 389, 65 );

	//初始化 任务奖励 Tab
	//设置 物品 GroupBox
	m_ctrlTab.CreateGroupBox( _T("物品"), 285, 3, 200, 150 );
	m_ctrlTab.CreateStatic( _T("ID1:"), 286, 3, 0, 5, 18 );
	m_ctrlTab.CreateEdit( FALSE, 287, 3, 0, 27, 15, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 288, 3, 0, 130, 18 );
	m_ctrlTab.CreateEdit( FALSE, 289, 3, 0, 165, 15, 30 );

	m_ctrlTab.CreateStatic( _T("ID2:"), 290, 3, 0, 5, 43 );
	m_ctrlTab.CreateEdit( FALSE, 291, 3, 0, 27, 40, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 292, 3, 0, 130, 43 );
	m_ctrlTab.CreateEdit( FALSE, 293, 3, 0, 165, 40, 30 );

	m_ctrlTab.CreateStatic( _T("ID3:"), 294, 3, 0, 5, 68 );
	m_ctrlTab.CreateEdit( FALSE, 295, 3, 0, 27, 65, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 296, 3, 0, 130, 68 );
	m_ctrlTab.CreateEdit( FALSE, 297, 3, 0, 165, 65, 30 );

	m_ctrlTab.CreateStatic( _T("ID4:"), 298, 3, 0, 5, 93 );
	m_ctrlTab.CreateEdit( FALSE, 299, 3, 0, 27, 90, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 300, 3, 0, 130, 93 );
	m_ctrlTab.CreateEdit( FALSE, 301, 3, 0, 165, 90, 30 );

	m_ctrlTab.CreateStatic( _T("ID5:"), 302, 3, 0, 5, 118 );
	m_ctrlTab.CreateEdit( FALSE, 303, 3, 0, 27, 115, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 304, 3, 0,130, 118 );
	m_ctrlTab.CreateEdit( FALSE, 305, 3, 0, 165, 115, 30 );

	//设置 可选物品 GroupBox
	m_ctrlTab.CreateGroupBox( _T("可选物品"), 306, 3, 200, 150, 0, 210, 0 );
	m_ctrlTab.CreateStatic( _T("ID1:"), 307, 3, 0, 215, 18 );
	m_ctrlTab.CreateEdit( FALSE, 308, 3, 0, 237, 15, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 309, 3, 0, 340, 18 );
	m_ctrlTab.CreateEdit( FALSE, 310, 3, 0, 375, 15, 30 );

	m_ctrlTab.CreateStatic( _T("ID2:"), 311, 3, 0, 215, 43 );
	m_ctrlTab.CreateEdit( FALSE, 312, 3, 0, 237, 40, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 313, 3, 0, 340, 43 );
	m_ctrlTab.CreateEdit( FALSE, 314, 3, 0, 375, 40, 30 );

	m_ctrlTab.CreateStatic( _T("ID3:"), 315, 3, 0, 215, 68 );
	m_ctrlTab.CreateEdit( FALSE, 316, 3, 0, 237, 65, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 317, 3, 0, 340, 68 );
	m_ctrlTab.CreateEdit( FALSE, 318, 3, 0, 375, 65, 30 );

	m_ctrlTab.CreateStatic( _T("ID4:"), 319, 3, 0, 215, 93 );
	m_ctrlTab.CreateEdit( FALSE, 320, 3, 0, 237, 90, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 321, 3, 0, 340, 93 );
	m_ctrlTab.CreateEdit( FALSE, 322, 3, 0, 375, 90, 30 );

	m_ctrlTab.CreateStatic( _T("ID5:"), 323, 3, 0, 215, 118 );
	m_ctrlTab.CreateEdit( FALSE, 324, 3, 0, 237, 115, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 325, 3, 0,340, 118 );
	m_ctrlTab.CreateEdit( FALSE, 326, 3, 0, 375, 115, 30 );

	//设置 声望 GroupBox
	m_ctrlTab.CreateGroupBox( _T("声望"), 327, 3, 180, 150, 0, 420, 0 );
	m_ctrlTab.CreateStatic( _T("声望1:"), 328, 3, 0, 425, 18 );
	m_ctrlTab.CreateEdit( FALSE, 329, 3, 0, 460, 15, 80 );
	m_ctrlTab.CreateStatic( _T("值:"), 330, 3, 0, 543, 18 );
	m_ctrlTab.CreateEdit( FALSE, 331, 3, 0, 560, 15, 36 );

	m_ctrlTab.CreateStatic( _T("声望2:"), 332, 3, 0, 425, 43 );
	m_ctrlTab.CreateEdit( FALSE, 333, 3, 0, 460, 40, 80 );
	m_ctrlTab.CreateStatic( _T("值:"), 334, 3, 0, 543, 43 );
	m_ctrlTab.CreateEdit( FALSE, 335, 3, 0, 560, 40, 36 );

	m_ctrlTab.CreateStatic( _T("声望3:"), 336, 3, 0, 425, 68 );
	m_ctrlTab.CreateEdit( FALSE, 337, 3, 0, 460, 65, 80 );
	m_ctrlTab.CreateStatic( _T("值:"), 338, 3, 0, 543, 68 );
	m_ctrlTab.CreateEdit( FALSE, 339, 3, 0, 560, 65, 36 );

	m_ctrlTab.CreateStatic( _T("声望4:"), 340, 3, 0, 425, 93 );
	m_ctrlTab.CreateEdit( FALSE, 341, 3, 0, 460, 90, 80 );
	m_ctrlTab.CreateStatic( _T("值:"), 342, 3, 0, 543, 93 );
	m_ctrlTab.CreateEdit( FALSE, 343, 3, 0, 560, 90, 36 );

	//设置 属性 GroupBox
	m_ctrlTab.CreateGroupBox( _T("属性"), 344, 3, 200, 150, 0, 0, 160 );	
	m_ctrlTab.CreateStatic( _T("属性1:"), 345, 3, 0, 5, 178 );
	m_ctrlTab.CreateEdit( FALSE, 346, 3, 0, 40, 175, 80 );
	m_ctrlTab.CreateStatic( _T("值:"), 347, 3, 0, 123, 178 );
	m_ctrlTab.CreateEdit( FALSE, 348, 3, 0, 140, 175, 40 );

	m_ctrlTab.CreateStatic( _T("属性2:"), 349, 3, 0, 5, 203 );
	m_ctrlTab.CreateEdit( FALSE, 350, 3, 0, 40, 200, 80 );
	m_ctrlTab.CreateStatic( _T("值:"), 351, 3, 0, 123, 203 );
	m_ctrlTab.CreateEdit( FALSE, 352, 3, 0, 140, 200, 40 );

	m_ctrlTab.CreateStatic( _T("属性3:"), 353, 3, 0, 5, 228 );
	m_ctrlTab.CreateEdit( FALSE, 354, 3, 0, 40, 225, 80 );
	m_ctrlTab.CreateStatic( _T("值:"), 355, 3, 0, 123, 228 );
	m_ctrlTab.CreateEdit( FALSE, 356, 3, 0, 140, 225, 40 );

	m_ctrlTab.CreateStatic( _T("属性4:"), 357, 3, 0, 5, 253 );
	m_ctrlTab.CreateEdit( FALSE, 358, 3, 0, 40, 250, 80 );
	m_ctrlTab.CreateStatic( _T("值:"), 359, 3, 0, 123, 253 );
	m_ctrlTab.CreateEdit( FALSE, 360, 3, 0, 140, 250, 40 );

	//设置 技能 GroupBox
	m_ctrlTab.CreateGroupBox( _T("技能"), 361, 3, 200, 150, 0, 210, 160 );
	m_ctrlTab.CreateStatic( _T("ID1:"), 362, 3, 0, 215, 178 );
	m_ctrlTab.CreateEdit( FALSE, 363, 3, 0, 237, 175, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 364, 3, 0, 340, 178 );
	m_ctrlTab.CreateEdit( FALSE, 365, 3, 0, 375, 175, 30 );

	m_ctrlTab.CreateStatic( _T("ID2:"), 366, 3, 0, 215, 203 );
	m_ctrlTab.CreateEdit( FALSE, 367, 3, 0, 237, 200, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 368, 3, 0, 340, 203 );
	m_ctrlTab.CreateEdit( FALSE, 369, 3, 0, 375, 200, 30 );

	m_ctrlTab.CreateStatic( _T("ID3:"), 370, 3, 0, 215, 228 );
	m_ctrlTab.CreateEdit( FALSE, 371, 3, 0, 237, 225, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 372, 3, 0, 340, 228 );
	m_ctrlTab.CreateEdit( FALSE, 373, 3, 0, 375, 225, 30 );

	m_ctrlTab.CreateStatic( _T("ID4:"), 374, 3, 0, 215, 253 );
	m_ctrlTab.CreateEdit( FALSE, 375, 3, 0, 237, 250, 100 );
	m_ctrlTab.CreateStatic( _T("数量:"), 376, 3, 0, 340, 253 );
	m_ctrlTab.CreateEdit( FALSE, 377, 3, 0, 375, 250, 30 );

	//设置 buff GroupBox
	m_ctrlTab.CreateGroupBox( _T("Buff"), 378, 3, 180, 150, 0, 420, 160 );
	m_ctrlTab.CreateStatic( _T("Buff1:"), 379, 3, 0, 425, 178 );
	m_ctrlTab.CreateEdit( FALSE, 380, 3, 0, 460, 175, 100 );

	m_ctrlTab.CreateStatic( _T("Buff2:"), 381, 3, 0, 425, 203 );
	m_ctrlTab.CreateEdit( FALSE, 382, 3, 0, 460, 200, 100 );

	m_ctrlTab.CreateStatic( _T("Buff3:"), 383, 3, 0, 425, 228 );
	m_ctrlTab.CreateEdit( FALSE, 384, 3, 0, 460, 225, 100 );

	m_ctrlTab.CreateStatic( _T("Buff4:"), 385, 3, 0, 425, 253 );
	m_ctrlTab.CreateEdit( FALSE, 386, 3, 0, 460, 250, 100 );

	//设置 金钱 GroupBox
	m_ctrlTab.CreateGroupBox( _T("金钱"), 387, 3, 200, 40, 0, 0, 320 );
	m_ctrlTab.CreateStatic( _T("数量:"), 388, 3, 0, 5, 338 );
	m_ctrlTab.CreateEdit( FALSE, 389, 3, 0, 40, 335, 140 );

	//设置 经验值 GroupBox
	m_ctrlTab.CreateGroupBox( _T("经验值"), 390, 3, 210, 40, 0, 210, 320 );
	m_ctrlTab.CreateStatic( _T("数量:"), 391, 3, 0, 215, 338 );
	m_ctrlTab.CreateEdit( FALSE, 392, 3, 0, 250, 335, 140 );

	m_ctrlTab.MoveWindow( 175, 30, 665, 505 );

	//设置
}
void CLoongQuestEditorDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
	map< wstring, tagQuestProto* >::iterator it = m_mapQuestData.begin();
	for ( ; it != m_mapQuestData.end(); )
	{
		//注意删除字符指针， 是由_wcsdup产生的
		delete it->second;
		it = m_mapQuestData.erase( it );
	}

	list< tagQuestProto* >::iterator it1 = m_listDeleteQuest.begin();
	for ( ; it1 != m_listDeleteQuest.end(); )
	{
		//注意删除字符指针， 是由_wcsdup产生的
		delete (*it1);
		it1 = m_listDeleteQuest.erase( it1 );
	}
	
	//删除QuestMgr Ptr
	delete sQuestMgr.getSingletonPtr();
}

void CLoongQuestEditorDlg::OnBnClickedBtnSave()
{
	// TODO: Add your control notification handler code here
	if ( m_pCurrentQuest != NULL && m_wstrCurrentQuestFileName.empty() != true )
	{
		wstring path = m_mapQuestXMLFile[m_wstrCurrentQuestFileName];
		path += _T("\\") + m_wstrCurrentQuestFileName;
		SaveQuest( m_pCurrentQuest, path.c_str() );
	}
}


void CLoongQuestEditorDlg::SaveQuest( tagQuestProto* pQuest, LPCTSTR szFileName )
{	
	//任务属性 Tab
	//任务基本参数 GroupBox
	pQuest->id = m_ctrlTab.GetItemTextAsDWORD( 3 );
	//wcscpy( pQuest->pName, m_ctrlTab.GetItemText( 5 ).c_str() );
	//wstring tmp = m_ctrlTab.GetItemText( 5 );
	//wcscpy_s( pQuest->pName, tmp.length() + 1, tmp.c_str() );
	memset( pQuest->szName, 0, 36 );
	_tcsncpy( pQuest->szName, m_ctrlTab.GetItemText( 5 ).c_str(), 36 );
	
	pQuest->dwPrevQuestId = m_ctrlTab.GetItemTextAsDWORD( 7 );
	pQuest->dwNextQuestId = m_ctrlTab.GetItemTextAsDWORD( 9 );
	pQuest->bAutoAddQuest = m_ctrlTab.GetItemCheckState( 10 );

	//任务接取类型 --玩家主动接取
	pQuest->bQuestPlayerAccept = m_ctrlTab.GetItemCheckState( 12 );
	//系统发布
	pQuest->bQuestSystemRelease = m_ctrlTab.GetItemCheckState( 13 );
	//物品接取
	pQuest->bQuestItemAccept = m_ctrlTab.GetItemCheckState( 750 );

	//任务信息 GroupBox
	memset( pQuest->szConditions, 0, 36 );
	//CWnd* pWnd = m_ctrlTab.GetWndPtr( 16 );
	//pWnd->GetWindowText( pQuest->szConditions, 36 );
	_tcsncpy( pQuest->szConditions, m_ctrlTab.GetItemText( 16 ).c_str(), 36 );
	
	memset( pQuest->szOverview, 0, 256 );
	_tcsncpy( pQuest->szOverview, m_ctrlTab.GetItemText( 18 ).c_str(), 256 );

	/*tmp = m_ctrlTab.GetItemText( 16 );
	wcscpy_s( pQuest->pConditions, tmp.length() + 1, tmp.c_str() );
	tmp = m_ctrlTab.GetItemText( 18 );
	wcscpy_s( pQuest->pOverview,   tmp.length() + 1, tmp.c_str() );*/

	//时间 GroupBox
	int nHour = m_ctrlTab.GetItemTextAsInt( 20 );
	int nMin = m_ctrlTab.GetItemTextAsInt( 22 );
	int nSec = m_ctrlTab.GetItemTextAsInt( 24 );
	pQuest->dwLimitTime = nHour * 3600 + nMin * 60 + nSec;

	//周期性任务 GroupBox
	//是否是周期性任务
	pQuest->bPeriodic = m_ctrlTab.GetItemCheckState( 27 );
	if ( m_ctrlTab.GetItemCheckState( 29 ) == TRUE )
	{
		//一周
		pQuest->eQuestPeriodic = EQuestPeriodic_WEEK;
	}
	else
	{
		//一月
		pQuest->eQuestPeriodic = EQuestPeriodic_DAY;
	}

	if ( m_ctrlTab.GetItemCheckState( 32 ) == TRUE )
	{
		//周一
		pQuest->eWeek = EWeek_MON;
	}
	else if ( m_ctrlTab.GetItemCheckState( 33 ) == TRUE  )
	{
		//周二
		pQuest->eWeek = EWeek_TUES;
	}
	else if ( m_ctrlTab.GetItemCheckState( 34 ) == TRUE  )
	{
		//周三
		pQuest->eWeek = EWeek_WEDNES;
	}
	else if ( m_ctrlTab.GetItemCheckState( 35 ) == TRUE  )
	{
		//周四
		pQuest->eWeek = EWeek_THURS;
	}
	else if ( m_ctrlTab.GetItemCheckState( 36 ) == TRUE  )
	{
		//周五
		pQuest->eWeek = EWeek_FRI;
	}
	else if ( m_ctrlTab.GetItemCheckState( 37 ) == TRUE  )
	{
		//周六
		pQuest->eWeek = EWeek_SAT;
	}
	else if ( m_ctrlTab.GetItemCheckState( 38 ) == TRUE  )
	{
		//周日
		pQuest->eWeek = EWeek_SUN;
	}

	//任务类型 GroupBox
	if ( m_ctrlTab.GetItemCheckState( 40 ) == TRUE )
	{
		pQuest->eQuestType = EQT_Beginer;
	}
	else if ( m_ctrlTab.GetItemCheckState( 41 ) == TRUE )
	{
		pQuest->eQuestType = EQT_Gut;
	}
	else if ( m_ctrlTab.GetItemCheckState( 42 ) == TRUE )
	{
		pQuest->eQuestType = EQT_Skill;
	}
	else if ( m_ctrlTab.GetItemCheckState( 43 ) == TRUE )
	{
		pQuest->eQuestType = EQT_System;
	}
	else if ( m_ctrlTab.GetItemCheckState( 44 ) == TRUE )
	{
		pQuest->eQuestType = EQT_Commission;
	}
	else if ( m_ctrlTab.GetItemCheckState( 45 ) == TRUE )
	{
		pQuest->eQuestType = EQT_Arrest;
	}
	else if ( m_ctrlTab.GetItemCheckState( 46 ) == TRUE )
	{
		pQuest->eQuestType = EQT_Star;
	}
	else if ( m_ctrlTab.GetItemCheckState( 47 ) == TRUE )
	{
		pQuest->eQuestType = EQT_Guild;
	}
	else if ( m_ctrlTab.GetItemCheckState( 48 ) == TRUE )
	{
		pQuest->eQuestType = EQT_Profession;
	}
	else if ( m_ctrlTab.GetItemCheckState( 49 ) == TRUE )
	{
		pQuest->eQuestType = EQT_Study;
	}
	else if ( m_ctrlTab.GetItemCheckState( 50 ) == TRUE )
	{
		pQuest->eQuestType = EQT_Lovers;
	}
	else if ( m_ctrlTab.GetItemCheckState( 51 ) == TRUE )
	{
		pQuest->eQuestType = EQT_Legend;
	}

	//任务参数 GroupBox
	//建议玩家个数
	pQuest->dwSuggestedPlayers = m_ctrlTab.GetItemTextAsDWORD( 54 );
	//每个玩家接取任务次数
	pQuest->dwDoQuestTimes = m_ctrlTab.GetItemTextAsDWORD( 56 );
	//任务可以重复接取
	pQuest->bRepeatable = m_ctrlTab.GetItemCheckState( 57 );

	//任务脚本 GroupBox
	pQuest->QuestStartScriptID = m_ctrlTab.GetItemTextAsDWORD( 60 );
	pQuest->QuestCompleteScriptID = m_ctrlTab.GetItemTextAsDWORD( 62 );

	//任务物品
	//ID
	pQuest->dwSrcItemID = m_ctrlTab.GetItemTextAsDWORD(653);
	//数量
	pQuest->dwSrcItemCount = m_ctrlTab.GetItemTextAsDWORD(655);

	//任务接取条件 Tab
	//最小等级
	pQuest->dwGetQuestRequiredMinLevel = m_ctrlTab.GetItemTextAsDWORD( 65 );
	//最大等级
	pQuest->dwGetQuestRequiredMaxLevel = m_ctrlTab.GetItemTextAsDWORD( 67 );

	//任务接取NPC
	pQuest->dwAcceptQuestNPCID[0] = m_ctrlTab.GetItemTextAsDWORD(602);	//ID1
	pQuest->dwAcceptQuestNPCID[1] = m_ctrlTab.GetItemTextAsDWORD(604);	//ID2
	pQuest->dwAcceptQuestNPCID[2] = m_ctrlTab.GetItemTextAsDWORD(606);	//ID3
	pQuest->dwAcceptQuestNPCID[3] = m_ctrlTab.GetItemTextAsDWORD(608);	//ID4
	pQuest->dwAcceptQuestNPCID[4] = m_ctrlTab.GetItemTextAsDWORD(610);	//ID5
	pQuest->dwAcceptQuestNPCID[5] = m_ctrlTab.GetItemTextAsDWORD(612);	//ID6

	//任务完成NPC
	pQuest->dwCompleteQuestNPCID[0] = m_ctrlTab.GetItemTextAsDWORD(615);	//ID1
	pQuest->dwCompleteQuestNPCID[1] = m_ctrlTab.GetItemTextAsDWORD(617);	//ID2
	pQuest->dwCompleteQuestNPCID[2] = m_ctrlTab.GetItemTextAsDWORD(619);	//ID3
	pQuest->dwCompleteQuestNPCID[3] = m_ctrlTab.GetItemTextAsDWORD(621);	//ID4
	pQuest->dwCompleteQuestNPCID[4] = m_ctrlTab.GetItemTextAsDWORD(623);	//ID5
	pQuest->dwCompleteQuestNPCID[5] = m_ctrlTab.GetItemTextAsDWORD(625);	//ID6

	//声望 GroupBox
	//声望1
	pQuest->dwGetQuestRequiredRep[0] = m_ctrlTab.GetItemTextAsDWORD( 70 );
	pQuest->dwGetQuestRequiredMaxRepValue[0] = m_ctrlTab.GetItemTextAsDWORD( 72 );
	pQuest->dwGetQuestRequiredMinRepValue[0] = m_ctrlTab.GetItemTextAsDWORD( 74 );

	pQuest->dwGetQuestRequiredRep[1] = m_ctrlTab.GetItemTextAsDWORD( 76 );
	pQuest->dwGetQuestRequiredMaxRepValue[1] = m_ctrlTab.GetItemTextAsDWORD( 78 );
	pQuest->dwGetQuestRequiredMinRepValue[1] = m_ctrlTab.GetItemTextAsDWORD( 80 );

	pQuest->dwGetQuestRequiredRep[2] = m_ctrlTab.GetItemTextAsDWORD( 82 );
	pQuest->dwGetQuestRequiredMaxRepValue[2] = m_ctrlTab.GetItemTextAsDWORD( 84 );
	pQuest->dwGetQuestRequiredMinRepValue[2] = m_ctrlTab.GetItemTextAsDWORD( 86 );

	pQuest->dwGetQuestRequiredRep[3] = m_ctrlTab.GetItemTextAsDWORD( 88 );
	pQuest->dwGetQuestRequiredMaxRepValue[3] = m_ctrlTab.GetItemTextAsDWORD( 90 );
	pQuest->dwGetQuestRequiredMinRepValue[3] = m_ctrlTab.GetItemTextAsDWORD( 92 );

	//技能 GroupBox
	pQuest->dwGetQuestRequiredSkill[0] = m_ctrlTab.GetItemTextAsDWORD( 95 );
	pQuest->dwGetQuestRequiredSkillValue[0] = m_ctrlTab.GetItemTextAsDWORD( 97 );

	pQuest->dwGetQuestRequiredSkill[1] = m_ctrlTab.GetItemTextAsDWORD( 99 );
	pQuest->dwGetQuestRequiredSkillValue[1] = m_ctrlTab.GetItemTextAsDWORD( 101 );

	pQuest->dwGetQuestRequiredSkill[2] = m_ctrlTab.GetItemTextAsDWORD( 103 );
	pQuest->dwGetQuestRequiredSkillValue[2] = m_ctrlTab.GetItemTextAsDWORD( 105 );

	pQuest->dwGetQuestRequiredSkill[3] = m_ctrlTab.GetItemTextAsDWORD( 107 );
	pQuest->dwGetQuestRequiredSkillValue[3] = m_ctrlTab.GetItemTextAsDWORD( 109 );

	//物品 GroupBox
	pQuest->dwGetQuestRequiredItemID[0] = m_ctrlTab.GetItemTextAsDWORD( 112 );
	pQuest->dwGetQuestRequiredItemCount[0] = m_ctrlTab.GetItemTextAsDWORD( 114 );

	pQuest->dwGetQuestRequiredItemID[1] = m_ctrlTab.GetItemTextAsDWORD( 116 );
	pQuest->dwGetQuestRequiredItemCount[1] = m_ctrlTab.GetItemTextAsDWORD( 118 );

	pQuest->dwGetQuestRequiredItemID[2] = m_ctrlTab.GetItemTextAsDWORD( 120 );
	pQuest->dwGetQuestRequiredItemCount[2] = m_ctrlTab.GetItemTextAsDWORD( 122 );

	pQuest->dwGetQuestRequiredItemID[3] = m_ctrlTab.GetItemTextAsDWORD( 124 );
	pQuest->dwGetQuestRequiredItemCount[3] = m_ctrlTab.GetItemTextAsDWORD( 126 );

	pQuest->dwGetQuestRequiredItemID[4] = m_ctrlTab.GetItemTextAsDWORD( 128 );
	pQuest->dwGetQuestRequiredItemCount[4] = m_ctrlTab.GetItemTextAsDWORD( 130 );

	pQuest->dwGetQuestRequiredItemID[5] = m_ctrlTab.GetItemTextAsDWORD( 132 );
	pQuest->dwGetQuestRequiredItemCount[5] = m_ctrlTab.GetItemTextAsDWORD( 134 );

	pQuest->dwGetQuestRequiredItemID[6] = m_ctrlTab.GetItemTextAsDWORD( 136 );
	pQuest->dwGetQuestRequiredItemCount[6] = m_ctrlTab.GetItemTextAsDWORD( 138 );

	pQuest->dwGetQuestRequiredItemID[7] = m_ctrlTab.GetItemTextAsDWORD( 140 );
	pQuest->dwGetQuestRequiredItemCount[7] = m_ctrlTab.GetItemTextAsDWORD( 142 );

	pQuest->bDelRestrictedItem = m_ctrlTab.GetItemCheckState( 160 );

	//人物属性 GroupBox
	pQuest->dwGetQuestRequiredAtt[0] = m_ctrlTab.GetItemTextAsDWORD( 145 );
	pQuest->dwGetQuestRequiredAttValue[0] = m_ctrlTab.GetItemTextAsDWORD( 147 );

	pQuest->dwGetQuestRequiredAtt[1] = m_ctrlTab.GetItemTextAsDWORD( 149 );
	pQuest->dwGetQuestRequiredAttValue[1] = m_ctrlTab.GetItemTextAsDWORD( 151 );

	pQuest->dwGetQuestRequiredAtt[2] = m_ctrlTab.GetItemTextAsDWORD( 153 );
	pQuest->dwGetQuestRequiredAttValue[2] = m_ctrlTab.GetItemTextAsDWORD( 155 );

	pQuest->dwGetQuestRequiredAtt[3] = m_ctrlTab.GetItemTextAsDWORD( 157 );
	pQuest->dwGetQuestRequiredAttValue[3] = m_ctrlTab.GetItemTextAsDWORD( 159 );

	//职业 GroupBox
	pQuest->dwGetQuestRequiredClass = m_ctrlTab.GetItemTextAsDWORD( 163 );

	//性别 GroupBox
	if ( m_ctrlTab.GetItemCheckState( 165 ) == TRUE )
	{
		//男
		pQuest->dwGetQuestRequiredSex = 1;
	}
	else if ( m_ctrlTab.GetItemCheckState( 166 ) == TRUE )
	{
		//女
		pQuest->dwGetQuestRequiredSex = 2;
	}
	else if (m_ctrlTab.GetItemCheckState(500) == TRUE)
	{
		//无限制
		pQuest->dwGetQuestRequiredSex = 0;
	}
	//地图 GroupBox
	pQuest->dwGetQuestRequiredMapID = m_ctrlTab.GetItemTextAsDWORD( 169 );

	//触发器 GroupBox
	pQuest->dwGetQuestReqTriggersID[0] = m_ctrlTab.GetItemTextAsDWORD( 702 );
	pQuest->dwGetQuestReqTriggersID[1] = m_ctrlTab.GetItemTextAsDWORD( 704 );
	pQuest->dwGetQuestReqTriggersID[2] = m_ctrlTab.GetItemTextAsDWORD( 706 );
	pQuest->dwGetQuestReqTriggersID[3] = m_ctrlTab.GetItemTextAsDWORD( 708 );
	pQuest->dwGetQuestReqTriggersID[4] = m_ctrlTab.GetItemTextAsDWORD( 710 );
	pQuest->dwGetQuestReqTriggersID[5] = m_ctrlTab.GetItemTextAsDWORD( 712 );
	pQuest->dwGetQuestReqTriggersID[6] = m_ctrlTab.GetItemTextAsDWORD( 714 );
	pQuest->dwGetQuestReqTriggersID[7] = m_ctrlTab.GetItemTextAsDWORD( 716 );
	
	//夫妻
	pQuest->bGetQuestReqMarried = m_ctrlTab.GetItemCheckState(761);
	//情侣
	pQuest->dwGetQuestReqLoverNum = m_ctrlTab.GetItemTextAsDWORD(764);

	//任务完成条件 Tab
	//物品 GroupBox
	pQuest->dwCompleteQuestRequiredItemID[0] = m_ctrlTab.GetItemTextAsDWORD( 172 );
	pQuest->dwCompleteQuestRequiredItemIDCount[0] = m_ctrlTab.GetItemTextAsDWORD( 174 );

	pQuest->dwCompleteQuestRequiredItemID[1] = m_ctrlTab.GetItemTextAsDWORD( 176 );
	pQuest->dwCompleteQuestRequiredItemIDCount[1] = m_ctrlTab.GetItemTextAsDWORD( 178 );

	pQuest->dwCompleteQuestRequiredItemID[2] = m_ctrlTab.GetItemTextAsDWORD( 180 );
	pQuest->dwCompleteQuestRequiredItemIDCount[2] = m_ctrlTab.GetItemTextAsDWORD( 182 );

	pQuest->dwCompleteQuestRequiredItemID[3] = m_ctrlTab.GetItemTextAsDWORD( 184 );
	pQuest->dwCompleteQuestRequiredItemIDCount[3] = m_ctrlTab.GetItemTextAsDWORD( 186 );

	pQuest->dwCompleteQuestRequiredItemID[4] = m_ctrlTab.GetItemTextAsDWORD( 188 );
	pQuest->dwCompleteQuestRequiredItemIDCount[4] = m_ctrlTab.GetItemTextAsDWORD( 190 );

	pQuest->dwCompleteQuestRequiredItemID[5] = m_ctrlTab.GetItemTextAsDWORD( 192 );
	pQuest->dwCompleteQuestRequiredItemIDCount[5] = m_ctrlTab.GetItemTextAsDWORD( 194 );

	pQuest->dwCompleteQuestRequiredItemID[6] = m_ctrlTab.GetItemTextAsDWORD( 196 );
	pQuest->dwCompleteQuestRequiredItemIDCount[6] = m_ctrlTab.GetItemTextAsDWORD( 198 );

	pQuest->dwCompleteQuestRequiredItemID[7] = m_ctrlTab.GetItemTextAsDWORD( 200 );
	pQuest->dwCompleteQuestRequiredItemIDCount[7] = m_ctrlTab.GetItemTextAsDWORD( 202 );

	pQuest->bRequiredOnlyOneItem =  m_ctrlTab.GetItemCheckState( 203 );
	pQuest->bDeleteItem	= m_ctrlTab.GetItemCheckState(599);

	//NPC GroupBox
	pQuest->dwCompleteQuestRequiredNPC[0] = m_ctrlTab.GetItemTextAsDWORD( 206 );
	pQuest->dwCompleteQuestRequiredNPC[1] = m_ctrlTab.GetItemTextAsDWORD( 208 );
	pQuest->dwCompleteQuestRequiredNPC[2] = m_ctrlTab.GetItemTextAsDWORD( 210 );
	pQuest->dwCompleteQuestRequiredNPC[3] = m_ctrlTab.GetItemTextAsDWORD( 212 );
	pQuest->dwCompleteQuestRequiredNPC[4] = m_ctrlTab.GetItemTextAsDWORD( 214 );
	pQuest->dwCompleteQuestRequiredNPC[5] = m_ctrlTab.GetItemTextAsDWORD( 216 );
	pQuest->dwCompleteQuestRequiredNPC[6] = m_ctrlTab.GetItemTextAsDWORD( 218 );
	pQuest->dwCompleteQuestRequiredNPC[7] = m_ctrlTab.GetItemTextAsDWORD( 220 );

	pQuest->bRequiredOnlyInOrder = m_ctrlTab.GetItemCheckState( 221 );
	pQuest->bRequiredOnlyOneNPC = m_ctrlTab.GetItemCheckState( 222 );

	//触发器 GroupBox
	pQuest->dwCompleteQuestReqTriggersID[0] = m_ctrlTab.GetItemTextAsDWORD( 225 );
	pQuest->dwCompleteQuestReqTriggersID[1] = m_ctrlTab.GetItemTextAsDWORD( 227 );
	pQuest->dwCompleteQuestReqTriggersID[2] = m_ctrlTab.GetItemTextAsDWORD( 229 );
	pQuest->dwCompleteQuestReqTriggersID[3] = m_ctrlTab.GetItemTextAsDWORD( 231 );
	pQuest->dwCompleteQuestReqTriggersID[4] = m_ctrlTab.GetItemTextAsDWORD( 233 );
	pQuest->dwCompleteQuestReqTriggersID[5] = m_ctrlTab.GetItemTextAsDWORD( 235 );
	pQuest->dwCompleteQuestReqTriggersID[6] = m_ctrlTab.GetItemTextAsDWORD( 237 );
	pQuest->dwCompleteQuestReqTriggersID[7] = m_ctrlTab.GetItemTextAsDWORD( 239 );

	pQuest->bRequiredOnlyOneTrigger = m_ctrlTab.GetItemCheckState( 240 );

	//怪物数量 GroupBox
	pQuest->dwCompleteQuestRequiredCreatureID[0] = m_ctrlTab.GetItemTextAsDWORD( 243 );
	pQuest->dwCompleteQuestRequiredCreatureCount[0] = m_ctrlTab.GetItemTextAsDWORD( 245 );

	pQuest->dwCompleteQuestRequiredCreatureID[1] = m_ctrlTab.GetItemTextAsDWORD( 247 );
	pQuest->dwCompleteQuestRequiredCreatureCount[1] = m_ctrlTab.GetItemTextAsDWORD( 249 );

	pQuest->dwCompleteQuestRequiredCreatureID[2] = m_ctrlTab.GetItemTextAsDWORD( 251 );
	pQuest->dwCompleteQuestRequiredCreatureCount[2] = m_ctrlTab.GetItemTextAsDWORD( 253 );

	pQuest->dwCompleteQuestRequiredCreatureID[3] = m_ctrlTab.GetItemTextAsDWORD( 255 );
	pQuest->dwCompleteQuestRequiredCreatureCount[3] = m_ctrlTab.GetItemTextAsDWORD( 257 );

	pQuest->dwCompleteQuestRequiredCreatureID[4] = m_ctrlTab.GetItemTextAsDWORD( 259 );
	pQuest->dwCompleteQuestRequiredCreatureCount[4] = m_ctrlTab.GetItemTextAsDWORD( 261 );

	pQuest->dwCompleteQuestRequiredCreatureID[5] = m_ctrlTab.GetItemTextAsDWORD( 263 );
	pQuest->dwCompleteQuestRequiredCreatureCount[5] = m_ctrlTab.GetItemTextAsDWORD( 265 );

	//怪物等级
	pQuest->bRequiredCreatureLevel   =  m_ctrlTab.GetItemCheckState(780);
	pQuest->bRequiredOnlyOneCreature = m_ctrlTab.GetItemCheckState( 266 );

	//夫妻
	pQuest->bCompleteQuestReqMarried = m_ctrlTab.GetItemCheckState(771);
	//情侣
	pQuest->dwCompleteQuestReqLoverNum = m_ctrlTab.GetItemTextAsDWORD(774);
	//人物等级
	pQuest->dwCompleteQuestReqLevel = m_ctrlTab.GetItemTextAsDWORD(722);
	//金钱 GroupBox
	pQuest->dwCompleteQuestRequiredMoney = m_ctrlTab.GetItemTextAsDWORD( 269 );

	//选择条件 GroupBox
	pQuest->bRequiredOnlyOneCondition = m_ctrlTab.GetItemCheckState( 274 );

	//地图 GroupBox
	pQuest->dwCompleteQuestRequiredMapID 	  = m_ctrlTab.GetItemTextAsDWORD( 277 );
	pQuest->fCompleteQuestRequiredMap_X  	  = m_ctrlTab.GetItemTextAsFloat( 279 );
	pQuest->fCompleteQuestRequiredMap_Y  	  = m_ctrlTab.GetItemTextAsFloat( 281 );
	pQuest->fCompleteQuestRequiredMap_Z  	  = m_ctrlTab.GetItemTextAsFloat( 283 );
	pQuest->fCompleteQuestRequiredMap_Radius  = m_ctrlTab.GetItemTextAsFloat( 801 );
	pQuest->bCompleteQuestRequiredMap_Random  = m_ctrlTab.GetItemCheckState( 284 );

	//事件 GroupBox
	if( m_ctrlTab.GetItemCheckState(791) == TRUE )
	{
		pQuest->eCompleteQuestEventType = EQCET_UseItem;
		pQuest->dwUseItemEvent_Item_ID = m_ctrlTab.GetItemTextAsDWORD( 793 );
	}
	else if( m_ctrlTab.GetItemCheckState(794) == TRUE )
	{	
		pQuest->eCompleteQuestEventType = EQCET_UseSkill;
		pQuest->dwUseItemEvent_Item_ID = m_ctrlTab.GetItemTextAsDWORD( 796 );
	}
	//任务奖励 Tab
	//物品 GroupBox
	pQuest->dwRewItemID[0] = m_ctrlTab.GetItemTextAsDWORD( 287 );
	pQuest->dwRewItemCount[0] = m_ctrlTab.GetItemTextAsDWORD( 289 );

	pQuest->dwRewItemID[1] = m_ctrlTab.GetItemTextAsDWORD( 291 );
	pQuest->dwRewItemCount[1] = m_ctrlTab.GetItemTextAsDWORD( 293 );

	pQuest->dwRewItemID[2] = m_ctrlTab.GetItemTextAsDWORD( 295 );
	pQuest->dwRewItemCount[2] = m_ctrlTab.GetItemTextAsDWORD( 297 );

	pQuest->dwRewItemID[3] = m_ctrlTab.GetItemTextAsDWORD( 299 );
	pQuest->dwRewItemCount[3] = m_ctrlTab.GetItemTextAsDWORD( 301 );

	pQuest->dwRewItemID[4] = m_ctrlTab.GetItemTextAsDWORD( 303 );
	pQuest->dwRewItemCount[4] = m_ctrlTab.GetItemTextAsDWORD( 305 );

	//可选物品 GroupBox
	pQuest->dwRewChoicesItemID[0] = m_ctrlTab.GetItemTextAsDWORD( 308 );
	pQuest->dwRewChoicesItemCount[0] = m_ctrlTab.GetItemTextAsDWORD( 310 );

	pQuest->dwRewChoicesItemID[1] = m_ctrlTab.GetItemTextAsDWORD( 312 );
	pQuest->dwRewChoicesItemCount[1] = m_ctrlTab.GetItemTextAsDWORD( 314 );

	pQuest->dwRewChoicesItemID[2] = m_ctrlTab.GetItemTextAsDWORD( 316 );
	pQuest->dwRewChoicesItemCount[2] = m_ctrlTab.GetItemTextAsDWORD( 318 );

	pQuest->dwRewChoicesItemID[3] = m_ctrlTab.GetItemTextAsDWORD( 320 );
	pQuest->dwRewChoicesItemCount[3] = m_ctrlTab.GetItemTextAsDWORD( 322 );

	pQuest->dwRewChoicesItemID[4] = m_ctrlTab.GetItemTextAsDWORD( 324 );
	pQuest->dwRewChoicesItemCount[4] = m_ctrlTab.GetItemTextAsDWORD( 326 );

	//声望 GroupBox
	pQuest->dwRewRequiredRep[0] = m_ctrlTab.GetItemTextAsDWORD( 329 );
	pQuest->dwRewRequiredRepValue[0] = m_ctrlTab.GetItemTextAsDWORD( 331 );

	pQuest->dwRewRequiredRep[1] = m_ctrlTab.GetItemTextAsDWORD( 333 );
	pQuest->dwRewRequiredRepValue[1] = m_ctrlTab.GetItemTextAsDWORD( 335 );

	pQuest->dwRewRequiredRep[2] = m_ctrlTab.GetItemTextAsDWORD( 337 );
	pQuest->dwRewRequiredRepValue[2] = m_ctrlTab.GetItemTextAsDWORD( 339 );

	pQuest->dwRewRequiredRep[3] = m_ctrlTab.GetItemTextAsDWORD( 341 );
	pQuest->dwRewRequiredRepValue[3] = m_ctrlTab.GetItemTextAsDWORD( 343 );

	//属性 GroupBox
	pQuest->dwRewAtt[0] = m_ctrlTab.GetItemTextAsDWORD( 346 );
	pQuest->dwRewAttValue[0] = m_ctrlTab.GetItemTextAsDWORD( 348 );

	pQuest->dwRewAtt[1] = m_ctrlTab.GetItemTextAsDWORD( 350 );
	pQuest->dwRewAttValue[1] = m_ctrlTab.GetItemTextAsDWORD( 352 );

	pQuest->dwRewAtt[2] = m_ctrlTab.GetItemTextAsDWORD( 354 );
	pQuest->dwRewAttValue[2] = m_ctrlTab.GetItemTextAsDWORD( 356 );

	pQuest->dwRewAtt[3] = m_ctrlTab.GetItemTextAsDWORD( 358 );
	pQuest->dwRewAttValue[3] = m_ctrlTab.GetItemTextAsDWORD( 360 );

	//技能 GroupBox
	pQuest->dwRewSkill[0] = m_ctrlTab.GetItemTextAsDWORD( 363 );
	pQuest->dwRewSkillValue[0] = m_ctrlTab.GetItemTextAsDWORD( 365 );

	pQuest->dwRewSkill[1] = m_ctrlTab.GetItemTextAsDWORD( 367 );
	pQuest->dwRewSkillValue[1] = m_ctrlTab.GetItemTextAsDWORD( 369 );

	pQuest->dwRewSkill[2] = m_ctrlTab.GetItemTextAsDWORD( 371 );
	pQuest->dwRewSkillValue[2] = m_ctrlTab.GetItemTextAsDWORD( 373 );

	pQuest->dwRewSkill[3] = m_ctrlTab.GetItemTextAsDWORD( 375 );
	pQuest->dwRewSkillValue[3] = m_ctrlTab.GetItemTextAsDWORD( 377 );

	//buff GroupBox
	pQuest->dwRewBuff[0] = m_ctrlTab.GetItemTextAsDWORD( 380 );
	pQuest->dwRewBuff[1] = m_ctrlTab.GetItemTextAsDWORD( 382 );
	pQuest->dwRewBuff[2] = m_ctrlTab.GetItemTextAsDWORD( 384 );
	pQuest->dwRewBuff[3] = m_ctrlTab.GetItemTextAsDWORD( 386 );

	//金钱 GroupBox
	pQuest->dwRewMoney = m_ctrlTab.GetItemTextAsDWORD( 389 );

	//经验值 GroupBox
	pQuest->dwRewXP = m_ctrlTab.GetItemTextAsDWORD( 392 );

	sQuestMgr.SaveQuestToXml( pQuest, szFileName );
}

void CLoongQuestEditorDlg::UpdateTabCtrl( tagQuestProto* pQuest )
{
	//任务属性 Tab
	//任务基本参数 GroupBox
	m_ctrlTab.SetItemText( 3, pQuest->dwQuestId );
	m_ctrlTab.SetItemText( 5, pQuest->szName );
	m_ctrlTab.SetItemText( 7, pQuest->dwPrevQuestId );
	m_ctrlTab.SetItemText( 9, pQuest->dwNextQuestId );
	m_ctrlTab.SetItemCheckState( 10, pQuest->bAutoAddQuest );

	//任务接取类型--玩家主动接取
	m_ctrlTab.SetItemCheckState( 12, pQuest->bQuestPlayerAccept );
	//系统发布
	m_ctrlTab.SetItemCheckState( 13, pQuest->bQuestSystemRelease );
	//物品接取
	m_ctrlTab.SetItemCheckState( 750, pQuest->bQuestItemAccept );
	
	//任务信息 GroupBox
	m_ctrlTab.SetItemText( 16, pQuest->szConditions );
	m_ctrlTab.SetItemText( 18, pQuest->szOverview );

	//时间 GroupBox
	DWORD dwHour = pQuest->dwLimitTime / 3600;
	DWORD dwMin = ( pQuest->dwLimitTime % 3600 ) / 60;
	DWORD dwSec = ( pQuest->dwLimitTime % 3600 ) % 60;
	m_ctrlTab.SetItemText( 20, dwHour );
	m_ctrlTab.SetItemText( 22, dwMin );
	m_ctrlTab.SetItemText( 24, dwSec );

	//周期性任务 GroupBox
	//是否是周期性任务
	m_ctrlTab.SetItemCheckState( 27, pQuest->bPeriodic );
	if ( pQuest->bPeriodic == TRUE )
	{
		//激活周期选项
		m_ctrlTab.GetDlgItem(28)->EnableWindow(TRUE);
		m_ctrlTab.GetDlgItem(29)->EnableWindow(TRUE);
		m_ctrlTab.GetDlgItem(30)->EnableWindow(TRUE);

	}
	if ( pQuest->eQuestPeriodic == EQuestPeriodic_WEEK )
	{
		//一周
		m_ctrlTab.SetItemCheckState( 29, TRUE );
		m_ctrlTab.SetItemCheckState( 30, FALSE );
		//激活星期选项
		m_ctrlTab.GetDlgItem(31)->EnableWindow(TRUE);
		m_ctrlTab.GetDlgItem(32)->EnableWindow(TRUE);
		m_ctrlTab.GetDlgItem(33)->EnableWindow(TRUE);
		m_ctrlTab.GetDlgItem(34)->EnableWindow(TRUE);
		m_ctrlTab.GetDlgItem(35)->EnableWindow(TRUE);
		m_ctrlTab.GetDlgItem(36)->EnableWindow(TRUE);
		m_ctrlTab.GetDlgItem(37)->EnableWindow(TRUE);
		m_ctrlTab.GetDlgItem(38)->EnableWindow(TRUE);
	}
	else if ( pQuest->eQuestPeriodic = EQuestPeriodic_DAY )
	{
		//一日
		m_ctrlTab.SetItemCheckState( 29, FALSE );
		m_ctrlTab.SetItemCheckState( 30, TRUE );
	}
	else if ( pQuest->eQuestPeriodic = EQuestPeriodic_NOT )
	{
		//都不是
		m_ctrlTab.SetItemCheckState( 29, FALSE );
		m_ctrlTab.SetItemCheckState( 30, FALSE );
	}
	//全部置成非选中状态
	for ( int i = 0; i < 7; ++i )
	{
		m_ctrlTab.SetItemCheckState( 32 + i, FALSE );
	}
	switch( pQuest->eWeek )
	{
	case EWeek_MON:
		m_ctrlTab.SetItemCheckState( 32, TRUE );
		break;
	case EWeek_TUES:
		m_ctrlTab.SetItemCheckState( 33, TRUE );
		break;
	case EWeek_WEDNES:
		m_ctrlTab.SetItemCheckState( 34, TRUE );
		break;
	case EWeek_THURS:
		m_ctrlTab.SetItemCheckState( 35, TRUE );
		break;
	case EWeek_FRI:
		m_ctrlTab.SetItemCheckState( 36, TRUE );
		break;
	case EWeek_SAT:
		m_ctrlTab.SetItemCheckState( 37, TRUE );
		break;
	case EWeek_SUN:
		m_ctrlTab.SetItemCheckState( 38, TRUE );
		break;
	}

	//全部置成非选中状态
	for ( int i = 0; i < 12; ++i )
	{
		m_ctrlTab.SetItemCheckState( 40 + i, FALSE );
	}
	switch ( pQuest->eQuestType )
	{
	case EQT_Beginer:
		m_ctrlTab.SetItemCheckState( 40, TRUE );
		break;
	case EQT_Gut:
		m_ctrlTab.SetItemCheckState( 41, TRUE );
		break;
	case EQT_Skill:
		m_ctrlTab.SetItemCheckState( 42, TRUE );
		break;
	case EQT_System:
		m_ctrlTab.SetItemCheckState( 43, TRUE );
		break;
	case EQT_Commission:
		m_ctrlTab.SetItemCheckState( 44, TRUE );
		break;
	case EQT_Arrest:
		m_ctrlTab.SetItemCheckState( 45, TRUE );
		break;
	case EQT_Star:
		m_ctrlTab.SetItemCheckState( 46, TRUE );
		break;
	case EQT_Guild:
		m_ctrlTab.SetItemCheckState( 47, TRUE );
		break;
	case EQT_Profession:
		m_ctrlTab.SetItemCheckState( 48, TRUE );
		break;
	case EQT_Study:
		m_ctrlTab.SetItemCheckState( 49, TRUE );
		break;
	case EQT_Lovers:
		m_ctrlTab.SetItemCheckState( 50, TRUE );
		break;
	case EQT_Legend:
		m_ctrlTab.SetItemCheckState( 51, TRUE );
		break;
	}

	//任务参数 GroupBox
	//建议玩家个数
	m_ctrlTab.SetItemText( 54, pQuest->dwSuggestedPlayers );
	//每个玩家接取任务次数
	m_ctrlTab.SetItemText( 56, pQuest->dwDoQuestTimes );
	//任务可以重复接取
	m_ctrlTab.SetItemCheckState( 57, pQuest->bRepeatable );

	//任务脚本 GroupBox
	m_ctrlTab.SetItemText( 60, pQuest->QuestStartScriptID );
	m_ctrlTab.SetItemText( 62, pQuest->QuestCompleteScriptID );
	
	//任务接取NPC
	m_ctrlTab.SetItemText(602,pQuest->dwAcceptQuestNPCID[0]);//ID1
	m_ctrlTab.SetItemText(604,pQuest->dwAcceptQuestNPCID[1]);//ID2
	m_ctrlTab.SetItemText(606,pQuest->dwAcceptQuestNPCID[2]);//ID3
	m_ctrlTab.SetItemText(608,pQuest->dwAcceptQuestNPCID[3]);//ID4
	m_ctrlTab.SetItemText(610,pQuest->dwAcceptQuestNPCID[4]);//ID5
	m_ctrlTab.SetItemText(612,pQuest->dwAcceptQuestNPCID[5]);//ID6

	//任务完成NPC
	m_ctrlTab.SetItemText(615,pQuest->dwCompleteQuestNPCID[0]);//ID1
	m_ctrlTab.SetItemText(617,pQuest->dwCompleteQuestNPCID[1]);//ID2
	m_ctrlTab.SetItemText(619,pQuest->dwCompleteQuestNPCID[2]);//ID3
	m_ctrlTab.SetItemText(621,pQuest->dwCompleteQuestNPCID[3]);//ID4
	m_ctrlTab.SetItemText(623,pQuest->dwCompleteQuestNPCID[4]);//ID5
	m_ctrlTab.SetItemText(625,pQuest->dwCompleteQuestNPCID[5]);//ID6

	//任务物品
	//ID
	m_ctrlTab.SetItemText( 653, pQuest->dwSrcItemID );
	//数量
	m_ctrlTab.SetItemText( 655, pQuest->dwSrcItemCount);

	//任务接取条件 Tab
	//最小等级
	m_ctrlTab.SetItemText( 65, pQuest->dwGetQuestRequiredMinLevel );
	//最大等级
	m_ctrlTab.SetItemText( 67, pQuest->dwGetQuestRequiredMaxLevel );

	//声望 GroupBox
	//声望
	for ( int i = 0; i < 4; ++i )
	{
		m_ctrlTab.SetItemText( 70 + i * 6, pQuest->dwGetQuestRequiredRep[i] );
		m_ctrlTab.SetItemText( 72 + i * 6, pQuest->dwGetQuestRequiredMaxRepValue[i] );
		m_ctrlTab.SetItemText( 74 + i * 6, pQuest->dwGetQuestRequiredMinRepValue[i] );
	}

	//技能 GroupBox
	for ( int i = 0; i < 4; ++i )
	{
		m_ctrlTab.SetItemText( 95 + i * 4, pQuest->dwGetQuestRequiredSkill[i] );
		m_ctrlTab.SetItemText( 97 + i * 4, pQuest->dwGetQuestRequiredSkillValue[i] );
	}

	//物品 GroupBox
	for ( int i = 0; i < 8; ++i )
	{
		m_ctrlTab.SetItemText( 112 + i * 4, pQuest->dwGetQuestRequiredItemID[i] );
		m_ctrlTab.SetItemText( 114 + i * 4, pQuest->dwGetQuestRequiredItemCount[i] );
	}

	m_ctrlTab.SetItemCheckState( 160, pQuest->bDelRestrictedItem );

	//人物属性 GroupBox
	for ( int i = 0; i < 4; ++i )
	{
		m_ctrlTab.SetItemText( 145 + i * 4, pQuest->dwGetQuestRequiredAtt[i] );
		m_ctrlTab.SetItemText( 147 + i * 4, pQuest->dwGetQuestRequiredAttValue[i] );
	}

	//触发器
	for( int i = 0; i < QUEST_TRIGGERS_COUNT ; ++i )
	{
		m_ctrlTab.SetItemText( 702 + i * 2, pQuest->dwGetQuestReqTriggersID[i]);
	}
	
	//夫妻
	m_ctrlTab.SetItemCheckState( 761, pQuest->bGetQuestReqMarried );
	//情侣
	m_ctrlTab.SetItemText( 764, pQuest->dwGetQuestReqLoverNum );
	
	//职业 GroupBox
	m_ctrlTab.SetItemText( 163, pQuest->dwGetQuestRequiredClass );

	switch( pQuest->dwGetQuestRequiredSex )
	{
	case 1:
		//男
		m_ctrlTab.SetItemCheckState( 165, TRUE );
		m_ctrlTab.SetItemCheckState( 166, FALSE );
		m_ctrlTab.SetItemCheckState( 500, FALSE );
		break;
	case 2:
		//女
		m_ctrlTab.SetItemCheckState( 165, FALSE );
		m_ctrlTab.SetItemCheckState( 166, TRUE );
		m_ctrlTab.SetItemCheckState( 500, FALSE );
		break;
	case 0:
		//无限制
		m_ctrlTab.SetItemCheckState( 500, TRUE );
		m_ctrlTab.SetItemCheckState( 165, FALSE );
		m_ctrlTab.SetItemCheckState( 166, FALSE );
		break;
	}

	//地图 GroupBox
	m_ctrlTab.SetItemText( 169, pQuest->dwGetQuestRequiredMapID );

	//任务完成条件 Tab
	//物品 GroupBox
	for ( int i = 0; i < 8; ++i )
	{
		m_ctrlTab.SetItemText( 172 + i * 4, pQuest->dwCompleteQuestRequiredItemID[i] );
		m_ctrlTab.SetItemText( 174 + i * 4, pQuest->dwCompleteQuestRequiredItemIDCount[i] );
	}
	m_ctrlTab.SetItemCheckState( 203, pQuest->bRequiredOnlyOneItem );
	m_ctrlTab.SetItemCheckState( 599, pQuest->bDeleteItem );

	//NPC GroupBox
	for ( int i = 0; i < 8; ++i )
	{
		m_ctrlTab.SetItemText( 206 + i * 2, pQuest->dwCompleteQuestRequiredNPC[i] );
	}
	m_ctrlTab.SetItemCheckState( 221, pQuest->bRequiredOnlyInOrder );
	m_ctrlTab.SetItemCheckState( 222, pQuest->bRequiredOnlyOneNPC );

	//触发器 GroupBox
	for ( int i = 0; i < 8; ++i )
	{
		m_ctrlTab.SetItemText( 225 + i * 2, pQuest->dwCompleteQuestReqTriggersID[i] );
	}
	m_ctrlTab.SetItemCheckState( 240, pQuest->bRequiredOnlyOneTrigger );

	//怪物数量 GroupBox
	for ( int i = 0; i < 6; ++i )
	{
		m_ctrlTab.SetItemText( 243 + i * 4, pQuest->dwCompleteQuestRequiredCreatureID[i] );
		m_ctrlTab.SetItemText( 245 + i * 4, pQuest->dwCompleteQuestRequiredCreatureCount[i] );
	}
	m_ctrlTab.SetItemCheckState( 266, pQuest->bRequiredOnlyOneCreature );
	m_ctrlTab.SetItemCheckState( 780, pQuest->bRequiredCreatureLevel );

	//夫妻
	m_ctrlTab.SetItemCheckState( 771, pQuest->bCompleteQuestReqMarried );
	
	//情侣
	m_ctrlTab.SetItemText( 774, pQuest->dwCompleteQuestReqLoverNum );

	//等级
	m_ctrlTab.SetItemText( 722, pQuest->dwCompleteQuestReqLevel );

	//金钱 GroupBox
	m_ctrlTab.SetItemText( 269, pQuest->dwCompleteQuestRequiredMoney );

	//选择条件 GroupBox
	m_ctrlTab.SetItemCheckState( 274, pQuest->bRequiredOnlyOneCondition );

	//地图 GroupBox
	m_ctrlTab.SetItemText( 277, pQuest->dwCompleteQuestRequiredMapID );
	m_ctrlTab.SetItemText( 279, pQuest->fCompleteQuestRequiredMap_X );
	m_ctrlTab.SetItemText( 281, pQuest->fCompleteQuestRequiredMap_Y );
	m_ctrlTab.SetItemText( 283, pQuest->fCompleteQuestRequiredMap_Z );
	m_ctrlTab.SetItemText( 801, pQuest->fCompleteQuestRequiredMap_Radius );
	m_ctrlTab.SetItemCheckState( 284, pQuest->bCompleteQuestRequiredMap_Random );

	//任务事件
	switch ( pQuest->eCompleteQuestEventType )
	{
		//使用物品
		case EQCET_UseItem:
			m_ctrlTab.SetItemCheckState( 791, TRUE );
			//物品ID
			m_ctrlTab.SetItemText( 793, pQuest->dwUseItemEvent_Item_ID );
			break;
		//使用技能
		case EQCET_UseSkill:
			m_ctrlTab.SetItemCheckState( 794, TRUE );
			//技能ID
			m_ctrlTab.SetItemText( 796, pQuest->dwUseSkillEvent_Skill_ID );
			break;
		default:
			m_ctrlTab.SetItemCheckState( 791, FALSE );
			m_ctrlTab.SetItemCheckState( 794, FALSE );
			m_ctrlTab.SetItemCheckState( 797, FALSE );
	}
	
	//任务奖励 Tab
	//物品 GroupBox
	for ( int i = 0; i < 5; ++i )
	{
		m_ctrlTab.SetItemText( 287 + i * 4, pQuest->dwRewItemID[i] );
		m_ctrlTab.SetItemText( 289 + i * 4, pQuest->dwRewItemCount[i] );
	}

	//可选物品 GroupBox
	for ( int i = 0; i < 5; ++i )
	{
		m_ctrlTab.SetItemText( 308 + i * 4, pQuest->dwRewChoicesItemID[i] );
		m_ctrlTab.SetItemText( 310 + i * 4, pQuest->dwRewChoicesItemCount[i] );
	}

	//声望 GroupBox
	for ( int i = 0; i < 4; ++i )
	{
		m_ctrlTab.SetItemText( 329 + i * 4, pQuest->dwRewRequiredRep[i] );
		m_ctrlTab.SetItemText( 331 + i * 4, pQuest->dwRewRequiredRepValue[i] );
	}

	//属性 GroupBox
	for ( int i = 0; i < 4; ++i )
	{
		m_ctrlTab.SetItemText( 346 + i * 4, pQuest->dwRewAtt[i] );
		m_ctrlTab.SetItemText( 348 + i * 4, pQuest->dwRewAttValue[i] );
	}

	//技能 GroupBox
	for ( int i = 0; i < 4; ++i )
	{
		m_ctrlTab.SetItemText( 363 + i * 4, pQuest->dwRewSkill[i] );
		m_ctrlTab.SetItemText( 365 + i * 4, pQuest->dwRewSkillValue[i] );
	}

	//buff GroupBox
	for ( int i = 0; i < 4; ++i )
	{
		m_ctrlTab.SetItemText( 380 + i * 2, pQuest->dwRewBuff[i] );
	}

	//金钱 GroupBox
	m_ctrlTab.SetItemText( 389, pQuest->dwRewMoney );

	//经验值 GroupBox
	m_ctrlTab.SetItemText( 392, pQuest->dwRewXP );
}
void CLoongQuestEditorDlg::OnSetQuestPath()
{
	//让用户指定任务文件目录
	BROWSEINFO bi;
	ZeroMemory( &bi, sizeof(BROWSEINFO) );
	LPMALLOC pMalloc;
	LPITEMIDLIST pidl = SHBrowseForFolder( &bi );

	TCHAR path[MAX_PATH];
	if ( pidl != NULL )
	{
		SHGetPathFromIDList( pidl, path );
		//设置任务路径
		m_wstrQuestPath = path;
		if(SUCCEEDED(SHGetMalloc(&pMalloc)))//pidl指向的对象用完应该释放
		{
			pMalloc->Free(pidl);
			pMalloc->Release();
		}
	}
	else
	{
		//当前任务路径为空时，提示用户设置
		if ( m_wstrQuestPath.empty() == true )
		{
			MessageBox( _T("选择任务路径为空,请设置一个任务路径"), _T("提示"), MB_OK);
			return;
		}
	}

	//将任务存到注册表中
	if ( WriteQuestPathToReg() != TRUE )
	{
		MessageBox( _T("写入注册表失败！"), _T("提示"), MB_OK);
	}

	//更新显示内容
	wstring tmp = _T("当前任务路径:");
	tmp += m_wstrQuestPath;
	((CStatic*)GetDlgItem( IDC_STATIC_QUEST_PATH ))->SetWindowTextW( tmp.c_str() );

	//刷新QuestTree
	if ( LoadQuest() != TRUE )
	{
		return;
	}
}	

BOOL CLoongQuestEditorDlg::ReadQuestPathFromReg()
{
	HKEY hKey, hLQE, hPath;

	//打开注册表相应键位
	long ret = ::RegOpenKeyEx( HKEY_CURRENT_USER, _T("software"), 0, KEY_READ, &hKey );
	if ( ret != ERROR_SUCCESS )
	{
		::RegCloseKey( hKey );
		return FALSE;
	}

	ret = ::RegOpenKeyEx( hKey, _T("LoongQuestEditor"), 0, KEY_READ, &hLQE );
	if ( ret != ERROR_SUCCESS )
	{
		::RegCloseKey( hKey );
		return FALSE;
	}

	ret = ::RegOpenKeyEx( hLQE, _T("Path"), 0, KEY_READ, &hPath );
	if ( ret != ERROR_SUCCESS )
	{
		::RegCloseKey( hKey );
		return FALSE;
	}

	//读取 任务路径
	TCHAR path[MAX_PATH];
	DWORD dwType = REG_SZ;
	DWORD cbData = MAX_PATH * sizeof(TCHAR);
	ret = ::RegQueryValueEx( hPath, _T("Path"), NULL, &dwType, (BYTE*)path, &cbData );
	if ( ret != ERROR_SUCCESS )
	{
		::RegCloseKey( hKey );
		return FALSE;
	}

	if ( path[0] == '\0' )
	{
		::RegCloseKey( hKey );
		return FALSE;
	}
	m_wstrQuestPath = path;

	//关闭注册表
	::RegCloseKey( hKey );

	return TRUE;
}


BOOL CLoongQuestEditorDlg::WriteQuestPathToReg()
{
	HKEY hKey, hLQE, hPath;
	long ret = ::RegOpenKeyEx( HKEY_CURRENT_USER, _T("software"), 0, KEY_READ|KEY_WRITE, &hKey );
	if ( ret != ERROR_SUCCESS )
	{
		return FALSE;
	}
	DWORD dw;
	if ( ::RegCreateKeyEx( hKey, _T("LoongQuestEditor"), 0, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL, &hLQE, &dw) != ERROR_SUCCESS )
	{
		::RegCloseKey( hKey );
		return FALSE;
	}

	if ( ::RegCreateKeyEx( hLQE, _T("Path"), 0, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL, &hPath, &dw) != ERROR_SUCCESS )
	{
		::RegCloseKey( hLQE );
		::RegCloseKey( hKey );
		return FALSE;
	}

	//查询路径
	//定义数据类型
	DWORD dwType = REG_SZ;
	//定义数据长度
	DWORD cbData = (DWORD)(m_wstrQuestPath.length() + 1) * sizeof(TCHAR);
	ret = ::RegSetValueEx( hPath, _T("Path"), NULL, dwType, (BYTE*)m_wstrQuestPath.c_str(), cbData );
	if ( ret != ERROR_SUCCESS )
	{
		::RegCloseKey( hPath );
		::RegCloseKey( hLQE );
		::RegCloseKey( hKey );
		return FALSE;
	}

	//关闭注册表
	::RegCloseKey( hPath );
	::RegCloseKey( hLQE );
	::RegCloseKey( hKey );

	return TRUE;
}

BOOL CLoongQuestEditorDlg::LoadQuest()
{
	//在Load之前，清空map中的数据
	m_mapQuestXMLFile.clear();

	//遍历任务路径下的所有文件夹和文件
	FindAllQuestXMLFile( m_wstrQuestPath.c_str() );

	//读取任务文件
	LoadQuestDataFromXML();

	//更新Tree控件
	UpdateTreeCtrl();

	return TRUE;
}

void CLoongQuestEditorDlg::FindAllQuestXMLFile( LPCTSTR szQuestPath )
{
	CFileFind finder;

	wstring wstrPath = szQuestPath;
	wstrPath += _T("\\*.*");
	BOOL bRet = finder.FindFile( wstrPath.c_str() );

	while( bRet )
	{
		bRet = finder.FindNextFileW();
		if ( finder.IsDots() == TRUE )
		{
			continue;
		}
		if ( finder.IsDirectory() == TRUE )
		{
			//目录。继续递归
			wstring tmp = (LPCTSTR)finder.GetFileName();
			tmp = wstring(szQuestPath) + _T("\\") + tmp;
			FindAllQuestXMLFile( tmp.c_str() );
		}	
		else
		{
			wstring tmp = (LPCTSTR)finder.GetFileName();
			size_t pos = tmp.rfind( _T(".") );
			if ( pos != string::npos )
			{
				//只要 xml 文件
				if ( tmp.substr( pos + 1 ) == _T("xml") )
				{
					m_mapQuestXMLFile.insert( make_pair( tmp, szQuestPath ) );
				}
			}
		}
	}

	finder.Close();
}

void CLoongQuestEditorDlg::UpdateTreeCtrl()
{
	//清空原来数据
	m_ctrlQuestTree.DeleteAllItems();

	//插入数据
	HTREEITEM hQuest, hLast = NULL;
	hQuest = m_ctrlQuestTree.InsertItem( _T("Quest"), TVI_ROOT );

	wstring wstrDir;

	map< wstring, wstring >::iterator it = m_mapQuestXMLFile.begin();
	for ( ; it != m_mapQuestXMLFile.end(); ++it )
	{
		if ( it->second != wstrDir )
		{
			//插入文件夹
			wstrDir = it->second;
			//去掉前面路径名，只留文件夹名称
			size_t pos = wstrDir.rfind( _T("\\") );
			hLast = m_ctrlQuestTree.InsertItem( (wstrDir.substr(pos + 1)).c_str(), hQuest );
		}

		//插入文件
		if ( hLast != NULL )
		{
			m_ctrlQuestTree.InsertItem( (it->first).c_str(), hLast );
		}
	}

	hQuest = m_ctrlQuestTree.GetRootItem();
	m_ctrlQuestTree.Expand( hQuest, TVE_EXPAND );

	//更新任务计数
	wstring tmp;
	wstringstream wss;
	wss << _T("任务总数:") << m_mapQuestXMLFile.size() << endl;
	wss >> tmp;
	((CStatic*)GetDlgItem( IDC_STATIC_QUEST_COUNT ))->SetWindowTextW( tmp.c_str() );
	
	tmp = _T("当前任务路径:");
	tmp += m_wstrQuestPath;
	((CStatic*)GetDlgItem( IDC_STATIC_QUEST_PATH ))->SetWindowTextW( tmp.c_str() );

}

void CLoongQuestEditorDlg::LoadQuestDataFromXML()
{
	//清空m_mapQuestData原来的数据
	map< wstring, tagQuestProto* >::iterator it1 = m_mapQuestData.begin();
	for ( ; it1 != m_mapQuestData.end(); )
	{
		delete it1->second;
		it1 = m_mapQuestData.erase( it1 );
	}

	map< wstring, wstring >::iterator it = m_mapQuestXMLFile.begin();
	for ( ; it != m_mapQuestXMLFile.end(); )
	{
		//生成 任务文件完整路径
		wstring wstrPath;
		wstrPath += it->second + _T("\\") + it->first;

		tagQuestProto* pQuest = new tagQuestProto;
		ZeroMemory( pQuest, sizeof(tagQuestProto) );
		//Load成功后，再加入列表
		if (sQuestMgr.LoadQuestFromXml( pQuest, wstrPath.c_str() ) == TRUE)
		{
			m_mapQuestData.insert( make_pair( it->first, pQuest ) );
			++it;
		}
		else
		{
			//读取任务文件不成功，则删除该文件,注意也要删除pQuest，否则会内存泄漏
			it = m_mapQuestXMLFile.erase(it);
			delete pQuest;
			pQuest = NULL;
		}
	}
}
void CLoongQuestEditorDlg::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM hItem = m_ctrlQuestTree.GetSelectedItem();
	HTREEITEM hRoot = m_ctrlQuestTree.GetRootItem();
	HTREEITEM hParent = m_ctrlQuestTree.GetParentItem( hItem );
	//判断是不是根节点
	if ( hItem != hRoot && hItem != NULL )
	{
		//判断是不是子节点，就是任务文件名称
		//如果父节点不是根节点，且存在，就是文件名此子节点
		if ( hParent != hRoot )
		{
			wstring name = (LPCTSTR)m_ctrlQuestTree.GetItemText( hItem );
			m_pCurrentQuest = m_mapQuestData[name];
			if ( m_pCurrentQuest != NULL )
			{
				//更新 Tab 控件数据
				UpdateTabCtrl( m_pCurrentQuest );
				//更新
				m_wstrCurrentQuestFileName = name;
				wstring tmp = LQE + name;
				SetWindowText( tmp.c_str() );

			}
		}
	}

	*pResult = 0;
}

void CLoongQuestEditorDlg::OnNewQuest()
{
	//当前没有任务文件被打开
	if ( m_pCurrentQuest == NULL && m_wstrCurrentQuestFileName.empty() == true )
	{
		CFileDialog dlg( FALSE, _T("xml"), NULL, OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
			_T( "任务文件 (*.xml)|*.xml|All Files (*.*)|*.*||;"), this);

		//设置当前打开目录为任务路径
		dlg.m_ofn.lpstrInitialDir = m_wstrQuestPath.c_str();
		dlg.m_ofn.lpstrTitle = _T("新建任务");
		if ( dlg.DoModal() == IDOK )
		{
			//得到文件名字
			wstring name = (LPCTSTR)dlg.GetFileName();
			wstring path = (LPCTSTR)dlg.GetPathName();
			//如果新建任务文件所在目录与设置的任务目录不一样，则不允许新建任务，返回并提示
			if ( m_wstrQuestPath != path.substr( 0, m_wstrQuestPath.length() ) )
			{
				//提示退出
				wstringstream wss;
				wstring wstr;
				wss << _T("请将新建任务文件放置于任务路径当中，当前的任务路径为") << m_wstrQuestPath << endl;
				wss >> wstr;
				wss.str(_T(""));

				MessageBox( wstr.c_str(), _T("提示") );
				return;
			}

			m_wstrCurrentQuestFileName = name;
			m_pCurrentQuest = new tagQuestProto;
			ZeroMemory( m_pCurrentQuest, sizeof(tagQuestProto) );
			/*m_pCurrentQuest->pName = _wcsdup( _T("") );
			m_pCurrentQuest->pOverview = _wcsdup( _T("") );
			m_pCurrentQuest->pConditions = _wcsdup( _T("") );*/
			
			string::size_type pos = path.rfind( _T("\\") );
			m_mapQuestXMLFile.insert( make_pair( m_wstrCurrentQuestFileName, path.substr(0, pos) ) );
			m_mapQuestData.insert( make_pair( m_wstrCurrentQuestFileName, m_pCurrentQuest ) );

			//更新Tree 控件
			UpdateTreeCtrl();
			UpdateTabCtrl( m_pCurrentQuest );
		}
	}
	else if ( m_pCurrentQuest != NULL && m_wstrCurrentQuestFileName.empty() != true )
	{
		//当前有任务文件被打开

		//先保存当前打开的任务数据
		wstring path = m_mapQuestXMLFile[m_wstrCurrentQuestFileName];
		path += _T("\\") + m_wstrCurrentQuestFileName;
		SaveQuest( m_pCurrentQuest, path.c_str() );

		//新建任务
		CFileDialog dlg( FALSE, _T("xml"), NULL, OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
			_T( "任务文件 (*.xml)|*.xml|All Files (*.*)|*.*||;"), this);

		//设置当前打开目录为任务路径
		dlg.m_ofn.lpstrInitialDir = m_wstrQuestPath.c_str();
		dlg.m_ofn.lpstrTitle = _T("新建任务");
		if ( dlg.DoModal() == IDOK )
		{
			//得到文件名字
			wstring name = (LPCTSTR)dlg.GetFileName();
			wstring path = (LPCTSTR)dlg.GetPathName();
			//如果新建任务文件所在目录与设置的任务目录不一样，则不允许新建任务，返回并提示
			if ( m_wstrQuestPath != path.substr( 0, m_wstrQuestPath.length() ) )
			{
				//提示退出
				wstringstream wss;
				wstring wstr;
				wss << _T("请将新建任务文件放置于任务路径当中，当前的任务路径为") << m_wstrQuestPath << endl;
				wss >> wstr;
				wss.str(_T(""));

				MessageBox( wstr.c_str(), _T("提示") );
				return;
			}

			m_wstrCurrentQuestFileName = name;
			m_pCurrentQuest = new tagQuestProto;
			ZeroMemory( m_pCurrentQuest, sizeof(tagQuestProto) );

			string::size_type pos = path.rfind( _T("\\") );
			m_mapQuestXMLFile.insert( make_pair( m_wstrCurrentQuestFileName, path.substr(0, pos) ) );
			m_mapQuestData.insert( make_pair( m_wstrCurrentQuestFileName, m_pCurrentQuest ) );

			//更新Tree 控件
			UpdateTreeCtrl();
			UpdateTabCtrl( m_pCurrentQuest );
		}	
	}
}

void CLoongQuestEditorDlg::OnDeleteQuest()
{
	//当前有任务文件被打开
	if ( m_pCurrentQuest != NULL && m_wstrCurrentQuestFileName.empty() != true )
	{
		wstring wstrpath;
		map< wstring, wstring >::iterator it;
		it = m_mapQuestXMLFile.find( m_wstrCurrentQuestFileName );
		if ( it != m_mapQuestXMLFile.end() )
		{
			wstrpath = it->second + _T("\\") + m_wstrCurrentQuestFileName;
			m_mapQuestXMLFile.erase( it );
			m_listDeleteQuestFile.push_back( wstrpath );
		}

		map< wstring, tagQuestProto* >::iterator it1;
		it1 = m_mapQuestData.find( m_wstrCurrentQuestFileName );
		if ( it1 != m_mapQuestData.end() )
		{
			m_listDeleteQuest.push_back( it1->second );
			m_mapQuestData.erase( it1 );
			m_pCurrentQuest = NULL;
		}
		//更新Tree 控件
		wstring tmp = LQE;
		SetWindowText( tmp.c_str() );
		UpdateTreeCtrl();
		tagQuestProto quest;
		ZeroMemory( &quest, sizeof(tagQuestProto) );
		UpdateTabCtrl( &quest );

		
		TCHAR ToBuff[MAX_PATH] = { 0 };
		//wcscpy_s( ToBuff, MAX_PATH, (*it2).c_str() );
		lstrcpy( ToBuff, wstrpath.c_str() );
		
		//删除文件到回收站
		SHFILEOPSTRUCT FileOp;//定义SHFILEOPSTRUCT结构对象
		FileOp.hwnd = NULL; 
		FileOp.wFunc=FO_DELETE; //执行文件删除操作;
		FileOp.pFrom= ToBuff;
		FileOp.pTo=	NULL;
		FileOp.fFlags=FOF_ALLOWUNDO; //此标志使删除文件备份到Windows回收站
		FileOp.hNameMappings=NULL;
		FileOp.lpszProgressTitle=_T("删除任务文件");
		SHFileOperation(&FileOp);
	}
}

void CLoongQuestEditorDlg::OnOk()
{
	// TODO: Add your control notification handler code here
	//OnOK();
}

void CLoongQuestEditorDlg::OnRefreshQuesTree()
{
	if ( LoadQuest() != TRUE )
	{
		return;
	}
}

LRESULT CLoongQuestEditorDlg::ButtonPressed(WPARAM w, LPARAM l)
{
	//WPARAM w is the nID (Control ID) of the Button we pressed
	//设置 周期性任务 GroupBox
	int nID = (int)w;
	if ( nID == 27 || nID == 29 || nID == 30 )
	{
		if ( m_ctrlTab.GetItemCheckState( 27 ) == TRUE )
		{
			m_ctrlTab.GetDlgItem( 29 )->EnableWindow( TRUE );
			m_ctrlTab.GetDlgItem( 30 )->EnableWindow( TRUE );
			if ( m_ctrlTab.GetItemCheckState( 29 ) == TRUE )
			{
				m_ctrlTab.GetDlgItem( 32 )->EnableWindow( TRUE );
				m_ctrlTab.GetDlgItem( 33 )->EnableWindow( TRUE );
				m_ctrlTab.GetDlgItem( 34 )->EnableWindow( TRUE );
				m_ctrlTab.GetDlgItem( 35 )->EnableWindow( TRUE );
				m_ctrlTab.GetDlgItem( 36 )->EnableWindow( TRUE );
				m_ctrlTab.GetDlgItem( 37 )->EnableWindow( TRUE );
				m_ctrlTab.GetDlgItem( 38 )->EnableWindow( TRUE );

				m_ctrlTab.SetItemCheckState(32, TRUE );
				m_ctrlTab.SetItemCheckState(33, FALSE );
				m_ctrlTab.SetItemCheckState(34, FALSE );
				m_ctrlTab.SetItemCheckState(35, FALSE );
				m_ctrlTab.SetItemCheckState(36, FALSE );
				m_ctrlTab.SetItemCheckState(37, FALSE );
				m_ctrlTab.SetItemCheckState(38, FALSE );
			}
			else if ( m_ctrlTab.GetItemCheckState( 30 ) == TRUE )
			{
				m_ctrlTab.GetDlgItem( 32 )->EnableWindow( FALSE );
				m_ctrlTab.GetDlgItem( 33 )->EnableWindow( FALSE );
				m_ctrlTab.GetDlgItem( 34 )->EnableWindow( FALSE );
				m_ctrlTab.GetDlgItem( 35 )->EnableWindow( FALSE );
				m_ctrlTab.GetDlgItem( 36 )->EnableWindow( FALSE );
				m_ctrlTab.GetDlgItem( 37 )->EnableWindow( FALSE );
				m_ctrlTab.GetDlgItem( 38 )->EnableWindow( FALSE );

				m_ctrlTab.SetItemCheckState(32, FALSE );
				m_ctrlTab.SetItemCheckState(33, FALSE );
				m_ctrlTab.SetItemCheckState(34, FALSE );
				m_ctrlTab.SetItemCheckState(35, FALSE );
				m_ctrlTab.SetItemCheckState(36, FALSE );
				m_ctrlTab.SetItemCheckState(37, FALSE );
				m_ctrlTab.SetItemCheckState(38, FALSE );
			}
			
		}
		else
		{
			m_ctrlTab.GetDlgItem( 29 )->EnableWindow( FALSE );
			m_ctrlTab.GetDlgItem( 30 )->EnableWindow( FALSE );
			m_ctrlTab.GetDlgItem( 32 )->EnableWindow( FALSE );
			m_ctrlTab.GetDlgItem( 33 )->EnableWindow( FALSE );
			m_ctrlTab.GetDlgItem( 34 )->EnableWindow( FALSE );
			m_ctrlTab.GetDlgItem( 35 )->EnableWindow( FALSE );
			m_ctrlTab.GetDlgItem( 36 )->EnableWindow( FALSE );
			m_ctrlTab.GetDlgItem( 37 )->EnableWindow( FALSE );
			m_ctrlTab.GetDlgItem( 38 )->EnableWindow( FALSE );
		}

	}
	return 0;
}

BOOL CLoongQuestEditorDlg::SaveAcceptQuestNPCXml( LPCTSTR szFileName )
{
	//key可以重复
	multimap< DWORD, DWORD > mapQuestAcceptRelation;
	

	DWORD dwQuestID;
	DWORD dwNPCID;

	map< wstring, tagQuestProto* >::iterator it = m_mapQuestData.begin();
	for( ; it != m_mapQuestData.end(); ++it )
	{
		tagQuestProto* pQuest = NULL;
		pQuest = it->second;
		dwQuestID = pQuest->dwQuestId;

		for ( int i = 0; i < 6; ++i )
		{
			dwNPCID = pQuest->dwAcceptQuestNPCID[i];
			if ( dwNPCID != 0 )
			{
				mapQuestAcceptRelation.insert( make_pair( dwNPCID, pQuest->dwQuestId ) );
			}
		}
	}

	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "UTF-8", "" );
	doc.LinkEndChild( decl );

	TiXmlElement* root = new TiXmlElement( "accept-quest-relation" );
	doc.LinkEndChild( root );

	multimap< DWORD, DWORD >::iterator it2 = mapQuestAcceptRelation.begin();
	for ( ; it2 != mapQuestAcceptRelation.end(); ++it2 )
	{
		TiXmlElement* entry;
		entry = new TiXmlElement( "entry" );
		root->LinkEndChild( entry );
		entry->SetAttribute( "NPCID",  it2->first );
		entry->SetAttribute( "QuestID",  it2->second );
	}

	string strFileName = WChar2Ansi( szFileName );
	doc.SaveFile( strFileName.c_str() );

	return TRUE;

}

BOOL CLoongQuestEditorDlg::SaveCompleteQuestNPCXml( LPCTSTR szFileName )
{
	//key可以重复
	multimap< DWORD, DWORD > mapQuestCompleteRelation;


	DWORD dwQuestID;
	DWORD dwNPCID;

	map< wstring, tagQuestProto* >::iterator it = m_mapQuestData.begin();
	for( ; it != m_mapQuestData.end(); ++it )
	{
		tagQuestProto* pQuest = NULL;
		pQuest = it->second;
		dwQuestID = pQuest->dwQuestId;

		for ( int i = 0; i < 6; ++i )
		{
			dwNPCID = pQuest->dwCompleteQuestNPCID[i];
			if ( dwNPCID != 0 )
			{
				mapQuestCompleteRelation.insert( make_pair( dwNPCID, pQuest->dwQuestId ) );
			}
		}
	}

	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "UTF-8", "" );
	doc.LinkEndChild( decl );

	TiXmlElement* root = new TiXmlElement( "complete-quest-relation" );
	doc.LinkEndChild( root );

	multimap< DWORD, DWORD >::iterator it2 = mapQuestCompleteRelation.begin();
	for ( ; it2 != mapQuestCompleteRelation.end(); ++it2 )
	{
		TiXmlElement* entry;
		entry = new TiXmlElement( "entry" );
		root->LinkEndChild( entry );
		entry->SetAttribute( "NPCID",  it2->first );
		entry->SetAttribute( "QuestID",  it2->second );
	}

	string strFileName = WChar2Ansi( szFileName );
	doc.SaveFile( strFileName.c_str() );

	return TRUE;
}

void CLoongQuestEditorDlg::OnBtnExportQuestAcceptRelation()
{
	//选择文件
	CFileDialog dlg( FALSE, _T("xml"), NULL, OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		_T( "任务文件 (*.xml)|*.xml|All Files (*.*)|*.*||;"), this);

	//设置当前打开目录为任务路径
	dlg.m_ofn.lpstrTitle = _T("选择接取NPC导出文件");
	if ( dlg.DoModal() == IDOK )
	{
		//得到文件路径全称
		wstring path = (LPCTSTR)dlg.GetPathName();

		SaveAcceptQuestNPCXml(path.c_str());
	}
}

void CLoongQuestEditorDlg::OnBtnExportQuestCompleteRelation()
{
	//选择文件
	CFileDialog dlg( FALSE, _T("xml"), NULL, OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		_T( "任务文件 (*.xml)|*.xml|All Files (*.*)|*.*||;"), this);

	//设置当前打开目录为任务路径
	dlg.m_ofn.lpstrTitle = _T("选择完成NPC导出文件");
	if ( dlg.DoModal() == IDOK )
	{
		//得到文件路径全称
		wstring path = (LPCTSTR)dlg.GetPathName();

		SaveCompleteQuestNPCXml(path.c_str());
	}
}
