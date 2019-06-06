// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "CPack.h"

#include "MainFrm.h"
#include "LeftView.h"
#include "CPackView.h"
#include "CPackDoc.h"
#include "ProgressDialog.h"


#include "cpkcore\cpkcore.h"
#include "cpkcore\util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern HWND  g_hWndFileName;
extern HWND  g_hWndProgress;
extern HWND  g_hWndInfo;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_ADD, OnAdd)
	ON_COMMAND(ID_APP_UP, OnAppUp)
	ON_COMMAND(ID_APP_ADD, OnAppAdd)
	ON_COMMAND(ID_APP_EXTRACT, OnAppExtract)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_APP_TIDY, OnAppTidy)
	ON_COMMAND(ID_APP_ANALYSE, OnAppAnalyse)
	ON_COMMAND(ID_EXTRACT, OnExtract)
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI_RANGE(AFX_ID_VIEW_MINIMUM, AFX_ID_VIEW_MAXIMUM, OnUpdateViewStyles)
	ON_COMMAND_RANGE(AFX_ID_VIEW_MINIMUM, AFX_ID_VIEW_MAXIMUM, OnViewStyle)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_SEPARATOR,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	// create splitter window
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
		return FALSE;

	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CLeftView), CSize(180, 100), pContext) ||
		!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CCPackView), CSize(100, 100), pContext))
	{
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}

	return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs


	cs.cx = 950;
	cs.cy = 600;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

CCPackView* CMainFrame::GetRightPane()
{
	CWnd* pWnd = m_wndSplitter.GetPane(0, 1);
	CCPackView* pView = DYNAMIC_DOWNCAST(CCPackView, pWnd);
	return pView;
}


CLeftView* CMainFrame::GetLeftPane()
{
	CWnd* pWnd = m_wndSplitter.GetPane(0, 0);
	CLeftView* pView = DYNAMIC_DOWNCAST(CLeftView, pWnd);
	return pView;
}

void CMainFrame::OnUpdateViewStyles(CCmdUI* pCmdUI)
{
	// TODO: customize or extend this code to handle choices on the
	// View menu.

	CCPackView* pView = GetRightPane(); 

	// if the right-hand pane hasn't been created or isn't a view,
	// disable commands in our range

	if (pView == NULL)
		pCmdUI->Enable(FALSE);
	else
	{
		DWORD dwStyle = pView->GetStyle() & LVS_TYPEMASK;

		// if the command is ID_VIEW_LINEUP, only enable command
		// when we're in LVS_ICON or LVS_SMALLICON mode

		if (pCmdUI->m_nID == ID_VIEW_LINEUP)
		{
			if (dwStyle == LVS_ICON || dwStyle == LVS_SMALLICON)
				pCmdUI->Enable();
			else
				pCmdUI->Enable(FALSE);
		}
		else
		{
			// otherwise, use dots to reflect the style of the view
			pCmdUI->Enable();
			BOOL bChecked = FALSE;

			switch (pCmdUI->m_nID)
			{
			case ID_VIEW_DETAILS:
				bChecked = (dwStyle == LVS_REPORT);
				break;

			case ID_VIEW_SMALLICON:
				bChecked = (dwStyle == LVS_SMALLICON);
				break;

			case ID_VIEW_LARGEICON:
				bChecked = (dwStyle == LVS_ICON);
				break;

			case ID_VIEW_LIST:
				bChecked = (dwStyle == LVS_LIST);
				break;

			default:
				bChecked = FALSE;
				break;
			}

			pCmdUI->SetRadio(bChecked ? 1 : 0);
		}
	}
}


void CMainFrame::OnViewStyle(UINT nCommandID)
{
	// TODO: customize or extend this code to handle choices on the
	// View menu.
	CCPackView* pView = GetRightPane();

	// if the right-hand pane has been created and is a CCPackView,
	// process the menu commands...
	if (pView != NULL)
	{
		DWORD dwStyle = (DWORD)-1;

		switch (nCommandID)
		{
		case ID_VIEW_LINEUP:
			{
				// ask the list control to snap to grid
				CListCtrl& refListCtrl = pView->GetListCtrl();
				refListCtrl.Arrange(LVA_ALIGNTOP);
			}
			break;

		// other commands change the style on the list control
		case ID_VIEW_DETAILS:
			dwStyle = LVS_REPORT;
			break;

		case ID_VIEW_SMALLICON:
			dwStyle = LVS_SMALLICON;
			break;

		case ID_VIEW_LARGEICON:
			dwStyle = LVS_ICON;
			break;

		case ID_VIEW_LIST:
			dwStyle = LVS_LIST;
			break;
		}

		// change the style; window will repaint automatically
		if (dwStyle != (DWORD)-1)
		{
			pView->ModifyStyle(LVS_TYPEMASK, dwStyle);
			CCPackDoc* pDoc = (CCPackDoc*)this->GetActiveDocument();
		
			if(dwStyle == 0)
			{	
				// 回到大图标时可能有问题，所以这里重新更新
				GetRightPane()->UpdateWindow();
				GetRightPane()->GetListCtrl().Arrange(LVA_ALIGNTOP);
			}
			
			pDoc->UpdateAllViews(NULL);
		}
	}
}


void CMainFrame::OnAdd() 
{
	// TODO: Add your command handler code here
	CCPackDoc* pDoc = (CCPackDoc*)this->GetActiveDocument();
	ASSERT(pDoc);

	// 调用文档的添加行为
	pDoc->OnAdd();
}


// 用户点击"向上"按钮
void CMainFrame::OnAppUp()
{
	// TODO: Add your command handler code here
	TCHAR szFullName[MAX_PATH];

	CCPackDoc* pDoc = (CCPackDoc*)this->GetActiveDocument();
	ASSERT(pDoc);

	DWORD dwCRC = pDoc->GetCpk()->GetCurDirCRC();
	DWORD dwFatherCRC = pDoc->GetCpk()->GetFatherCRC(dwCRC);

	BOOL bResult = FALSE;
	bResult = pDoc->GetCpk()->GetFullNameFromCRC(szFullName, dwFatherCRC);
	ASSERT(bResult);

	pDoc->GetCpk()->SetCurDir(szFullName);
	pDoc->UpdateAllViews(this->GetRightPane()); // 左边更新会自动带动右边更新

}



void CMainFrame::OnAppAdd() 
{
	// TODO: Add your command handler code here
	this->OnAdd();
}



// 用户点击释放
void CMainFrame::OnAppExtract() 
{
	// TODO: Add your command handler code here
	// 得到CPK包中的当前目录名
	CCPackDoc* pDoc = (CCPackDoc*)this->GetActiveDocument();
	ASSERT(pDoc);

	TCHAR szFullCPKName[MAX_PATH];
	DWORD dwCRC = pDoc->GetCpk()->GetCurDirCRC();
	BOOL bResult = pDoc->GetCpk()->GetFullNameFromCRC(szFullCPKName, dwCRC);
	ASSERT(bResult);

	UINT uSelected = this->GetRightPane()->GetListCtrl().GetSelectedCount();

	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(bi));

	TCHAR szTitle[MAX_PATH];
	if (uSelected == 0)
		_stprintf(szTitle, _T("将释放%s\n, 请选择释放目标目录"), szFullCPKName);
	else
		_stprintf(szTitle, _T("将释放包中%s的%lu个子对象\n请选择释放目标目录"), 
			szFullCPKName, uSelected);

	bi.lpszTitle = szTitle;
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;

	LPITEMIDLIST lpidl = SHBrowseForFolder(&bi);
	if (lpidl == NULL)
		return;

	// 得到用户指定的目标目录
	TCHAR szDestDir[MAX_PATH];
	bResult = SHGetPathFromIDList(lpidl, szDestDir);
	if (bResult == FALSE)
	{
		Msg(_T("Couldn't convert path from id list"));
		ASSERT(0);
	}
	::GlobalFree(lpidl);


	if (uSelected == 0)	// 没有文件被选择，则释放当前目录到指定目录
	{
		INT nResult = Msg(MB_OKCANCEL, 
			_T("没有选择任何文件，将会释放包中%s目录下所有文件和目录到%s"),
			szFullCPKName, szDestDir);

		if (nResult == IDOK)
		{
			// 释放文件
			// 用来显示进度的
			CProgressDialog pd;
			pd.Create(IDD_PROGRESSDIALOG, NULL);
			pd.GetDlgItem(IDC_FILENAME, &g_hWndFileName);
			pd.GetDlgItem(IDC_PROGRESS1, &g_hWndProgress);
			pd.GetDlgItem(IDC_INFO, &g_hWndInfo);
			pd.ShowWindow(SW_SHOW);
			pd.UpdateWindow();
			pd.SetWindowText("释放当前目录下的所有文件");

			bResult = pDoc->GetCpk()->ExtractFile(szDestDir, szFullCPKName);
			if (FALSE == bResult)
			{
				Msg(_T("Couldn't Extract %s to %s"), szFullCPKName, szDestDir);
				ASSERT(0);
			}
		}
	}
	else
	{
		// 用来显示进度的
		CProgressDialog pd;
		pd.Create(IDD_PROGRESSDIALOG, NULL);
		pd.GetDlgItem(IDC_FILENAME, &g_hWndFileName);
		pd.GetDlgItem(IDC_PROGRESS1, &g_hWndProgress);
		pd.GetDlgItem(IDC_INFO, &g_hWndInfo);
		pd.ShowWindow(SW_SHOW);
		pd.UpdateWindow();
		pd.SetWindowText("释放当前目录下选定的文件");

		POSITION pos = this->GetRightPane()
			->GetListCtrl().GetFirstSelectedItemPosition();

		TCHAR szTemp[MAX_PATH];
		while (pos)
		{
			int nItem = this->GetRightPane()
				->GetListCtrl().GetNextSelectedItem(pos);

			lstrcpy(szTemp, szFullCPKName);
			if (lstrlen(szTemp) > 0)	// 根目录，就不要加"\"
				lstrcat(szTemp, _T("\\"));
			
			// zsTemp == FullName
			lstrcat(szTemp, this->GetRightPane()->GetListCtrl().GetItemText(nItem, 0));

			// 释放文件
			bResult = pDoc->GetCpk()->ExtractFile(szDestDir, szTemp);
			if (FALSE == bResult)
			{
				Msg(_T("Couldn't Extract %s to %s"), szTemp, szDestDir);
				ASSERT(0);
			}
		}
	}
}


void CMainFrame::OnFileOpen() 
{
	// TODO: Add your command handler code here
	
}

void CMainFrame::OnAppTidy() 
{
	// TODO: Add your command handler code here
	
}


// 对整个包进行分析
void CMainFrame::OnAppAnalyse() 
{
	// TODO: Add your command handler code here
	
}



// 菜单中的释放指令
void CMainFrame::OnExtract() 
{
	// TODO: Add your command handler code here
	OnAppExtract();
	
}
