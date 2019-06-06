// ShowSumDlg.cpp : implementation file
//

#include "stdafx.h"
#include "time.h"
#include "../ServerDefine/msg_gm_tool_c.h"
#include "ShowSum.h"
#include "ShowSumDlg.h"
#include "GMServerMgr.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern GMServerMgr g_GMServerMgr;

// CShowSumDlg dialog

//CShowSumDlg g_ShowPageDlg;

CShowSumDlg::CShowSumDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShowSumDlg::IDD, pParent)
	, m_lfRate(0.0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CShowSumDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listItem);
	DDX_Text(pDX, IDC_EDIT1, m_lfRate);
	DDV_MinMaxDouble(pDX, m_lfRate, 0.01, 1000000);
}

BEGIN_MESSAGE_MAP(CShowSumDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CShowSumDlg::OnLvnItemchangedList1)
	ON_MESSAGE(WM_UPDATEDATA, &CShowSumDlg::OnUpdateDataMessage)
	ON_MESSAGE(WM_UPDATESUM, &CShowSumDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON1, &CShowSumDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CShowSumDlg message handlers

BOOL CShowSumDlg::OnInitDialog()
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
	m_listItem.InsertColumn(0, _T("Section Name"), LVCFMT_CENTER, 80);
	m_listItem.InsertColumn(1, _T("Server Name"), LVCFMT_CENTER, 80);
	m_listItem.InsertColumn(2, _T("Online"), LVCFMT_CENTER, 60);
	m_listItem.InsertColumn(3, _T("Online Limit"), LVCFMT_CENTER, 70);
	m_listItem.InsertColumn(4, _T("Word State"), LVCFMT_CENTER, 80);
	m_listItem.InsertColumn(5, _T("DB State"), LVCFMT_CENTER, 80);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CShowSumDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CShowSumDlg::OnPaint()
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
HCURSOR CShowSumDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CShowSumDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

CString FormatWorldState(GameWorld::EWorldStatus state)
{
	switch(state)
	{
	case GameWorld::EWS_Well:
		return _T("WELL");
	case GameWorld::EWS_InitNotDone:
		return _T("NOTDONE");
	case GameWorld::EWS_SystemError:
		return _T("SYSERROR");
	case GameWorld::EWS_ProofError:
		return _T("PROOFERROR");
	case GameWorld::EWS_DBBlock:
		return _T("DBBLOCK");
	}
	return _T("UNKNOWN");
}

void CShowSumDlg::UpdateUI()
{
	m_listItem.DeleteAllItems();

	m_nSum = 0;
	INT nRowCount = 0;
	TMap<INT, GMServer*> tmpGMServerMap = g_GMServerMgr.GetGMServerMap();

	m_lock.Acquire();

	TMap<INT, GMServer*>::TMapIterator it_1 = tmpGMServerMap.Begin();
	GMServer* pGMServer = NULL;

	while ( tmpGMServerMap.PeekNext(it_1, pGMServer) )
	{
		if (!P_VALID(pGMServer)) return;
		
		TMap<INT, Section*> tmpSectionMap = pGMServer->GetSectionMap();
		TMap<INT, Section*>::TMapIterator it_2 = tmpSectionMap.Begin();
		Section* pSection = NULL;

		while( tmpSectionMap.PeekNext( it_2, pSection ) )
		{
			if (!P_VALID(pSection)) return;

			TMap<INT, GameWorld*> tmpWorldMap = pSection->GetWorldMap();
			TMap<INT, GameWorld*>::TMapIterator it_3 = tmpWorldMap.Begin();
			GameWorld* pWorld = NULL;

			while( tmpWorldMap.PeekNext( it_3, pWorld ))
			{
				if (!P_VALID(pWorld)) return;

				INT nRow = m_listItem.InsertItem( nRowCount, NULL );
				m_listItem.SetItemText( nRow, 0, pSection->GetSectionName() );
				m_listItem.SetItemText( nRow, 1, pWorld->GetWorldName() );

				CString szOnline;
				INT nTmp = pWorld->GetOnline();
				szOnline.Format(_T("%d"), nTmp);
				m_listItem.SetItemText( nRow, 2, szOnline );
				nTmp = pWorld->GetOnlineLimit();
				szOnline.Format(_T("%d"), nTmp);
				m_listItem.SetItemText( nRow, 3, szOnline );
				m_listItem.SetItemText( nRow, 4, FormatWorldState(pWorld->GetWorldStatus()) );
				m_listItem.SetItemText( nRow, 5, FormatWorldState(pWorld->GetDBStatus()) );
				m_nSum += nTmp;
				nRowCount++;
			}
		}
	}
	
	g_GMServerMgr.SetSum(m_nSum);
	m_lock.Release();

	PostMessage( WM_UPDATESUM, 0, 0 );
}

LRESULT CShowSumDlg::OnUpdateDataMessage(WPARAM wparam, LPARAM lparam)
{
	UpdateUI();
	return 0;
}

void CShowSumDlg::OnClose()
{
	return;
}

LRESULT CShowSumDlg::OnBnClickedButton1(WPARAM wparam, LPARAM lparam)
{
	// TODO: Add your control notification handler code here
	TCHAR szSum[30] = {0};
	_stprintf( szSum, _T("Online Sum: %d"), g_GMServerMgr.GetSum() );
	SetWindowText( szSum );
	return 0;
}

void CShowSumDlg::OnBnClickedButton1()
{
	UpdateData();

	if(m_lfRate <=0.0) return;

	tagNGMTS_GMSetFackNumer send; 
	send.fFackRate = (float)m_lfRate;

	TMap<INT, GMServer*> tmpGMServerMap = g_GMServerMgr.GetGMServerMap();

	m_lock.Acquire();

	TMap<INT, GMServer*>::TMapIterator it_1 = tmpGMServerMap.Begin();
	GMServer* pGMServer = NULL;

	while ( tmpGMServerMap.PeekNext(it_1, pGMServer) )
	{
		if (!P_VALID(pGMServer)) continue;		
		pGMServer->SendMsg(&send, send.dwSize);
	}

	m_lock.Release();
}
