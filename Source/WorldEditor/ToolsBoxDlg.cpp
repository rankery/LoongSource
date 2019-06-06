// ToolsBoxDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "ToolsBoxDlg.h"
#include ".\toolsboxdlg.h"
#include "PluginMgr.h"


// CToolsBoxDlg 对话框

IMPLEMENT_DYNAMIC(CToolsBoxDlg, CDialog)
CToolsBoxDlg::CToolsBoxDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CToolsBoxDlg::IDD, pParent)
{
}

CToolsBoxDlg::~CToolsBoxDlg()
{
}

void CToolsBoxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ToolsList, m_list);
}


BEGIN_MESSAGE_MAP(CToolsBoxDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_LBN_SELCHANGE(IDC_ToolsList, OnLbnSelchangeToolslist)
END_MESSAGE_MAP()


// CToolsBoxDlg 消息处理程序

void CToolsBoxDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}

void CToolsBoxDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

void CToolsBoxDlg::OnLbnSelchangeToolslist()
{
	// TODO: 在此添加控件通知处理程序代码
	int sel=m_list.GetCurSel();
	if(sel!=-1)
	{
		PluginMgr::Inst()->SetActive(sel);
	}
}
