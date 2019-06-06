// ProgressDialog.cpp : implementation file
//

#include "stdafx.h"
#include "CPack.h"
#include "ProgressDialog.h"
#include ".\progressdialog.h"

#include "cpkcore\cpkcore.h"
#include "memmgr\memmgr.h"
#include "cpkcore\disciomgr.h"
#include "filtermgr\filtermgr.h"
#include "CPack.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgressDialog dialog

extern HWND  g_hWndFileName;
extern HWND  g_hWndProgress;
extern HWND  g_hWndInfo;


CProgressDialog::CProgressDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProgressDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CProgressDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgressDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgressDialog, CDialog)
	//{{AFX_MSG_MAP(CProgressDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgressDialog message handlers

BOOL CProgressDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_FILENAME, &g_hWndFileName);
	GetDlgItem(IDC_PROGRESS1, &g_hWndProgress);
	GetDlgItem(IDC_INFO, &g_hWndInfo);
	
	CenterWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
