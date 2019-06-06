// DownloadInfo.cpp : implementation file
//

#include "stdafx.h"
#include "launcher.h"
#include "DownloadInfo.h"


// DownloadInfo dialog

IMPLEMENT_DYNAMIC(DownloadInfo, CDialog)

DownloadInfo::DownloadInfo(CWnd* pParent /*=NULL*/)
	: CDialog(DownloadInfo::IDD, pParent)
{

}

DownloadInfo::~DownloadInfo()
{
}

void DownloadInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestHttpDlg)
	DDX_Control(pDX, IDC_EDIT1, m_ctrlEdit);
	//}}AFX_DATA_MAP
}


BOOL DownloadInfo::OnInitDialog()
{
	BOOL bResult = CDialog::OnInitDialog();
	this->SetWindowText(g_StrTable[_T("AutoUpdateInfo")]);

	return bResult;
}


BEGIN_MESSAGE_MAP(DownloadInfo, CDialog)
END_MESSAGE_MAP()


// DownloadInfo message handlers
