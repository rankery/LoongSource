// AttachMdlDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Cool3DView.h"
#include "AttachMdlDlg.h"
#include ".\attachmdldlg.h"


// CAttachMdlDlg 对话框

IMPLEMENT_DYNAMIC(CAttachMdlDlg, CDialog)
CAttachMdlDlg::CAttachMdlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAttachMdlDlg::IDD, pParent)
	, m_szMdlPath(_T(""))
	, m_szTagNodeName(_T(""))
{
}

CAttachMdlDlg::~CAttachMdlDlg()
{
}

void CAttachMdlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MdlPath, m_szMdlPath);
	DDX_Text(pDX, IDC_TagNodeName, m_szTagNodeName);
}


BEGIN_MESSAGE_MAP(CAttachMdlDlg, CDialog)
	ON_BN_CLICKED(IDC_MdlFile, OnBnClickedMdlfile)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CAttachMdlDlg 消息处理程序

void CAttachMdlDlg::OnBnClickedMdlfile()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		NULL,
		this,0);
	if(IDOK==dlg.DoModal())
	{
		UpdateData();
		m_szMdlPath=dlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CAttachMdlDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	if(m_szMdlPath.IsEmpty())
	{
		AfxMessageBox(_T("文件路径不能为空"));
		return;
	}
	if(m_szTagNodeName.IsEmpty())
	{
		AfxMessageBox(_T("挂载点名称不能为空"));
		return;
	}

	OnOK();
}
