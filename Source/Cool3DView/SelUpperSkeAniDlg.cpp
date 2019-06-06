// SelUpperSkeAniDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Cool3DView.h"
#include "SelUpperSkeAniDlg.h"
#include ".\selupperskeanidlg.h"


// CSelUpperSkeAniDlg 对话框

IMPLEMENT_DYNAMIC(CSelUpperSkeAniDlg, CDialog)
CSelUpperSkeAniDlg::CSelUpperSkeAniDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelUpperSkeAniDlg::IDD, pParent)
	, m_szUpperSkeAniName(_T(""))
	, m_szLowerSkeAniName(_T(""))
	, m_bIsOneTrack(FALSE)
{
}

CSelUpperSkeAniDlg::~CSelUpperSkeAniDlg()
{
}

void CSelUpperSkeAniDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_UpperSkeAniName, m_szUpperSkeAniName);
	DDX_Text(pDX, IDC_LowerSkeAniName, m_szLowerSkeAniName);
	DDX_Check(pDX, IDC_CHECKALL, m_bIsOneTrack);
	DDX_Control(pDX, IDC_UpperSkeAniName, m_UpperSkeAniEdit);
}


BEGIN_MESSAGE_MAP(CSelUpperSkeAniDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CSelUpperSkeAniDlg 消息处理程序

void CSelUpperSkeAniDlg::OnBnClickedOk()
{
	UpdateData();
	if (m_bIsOneTrack)
	{
		if (m_szLowerSkeAniName.IsEmpty())
		{
			AfxMessageBox(_T("下身动画名称不能为空"));
			return;
		}
	}
	else if (m_szUpperSkeAniName.IsEmpty() && m_szLowerSkeAniName.IsEmpty())
	{
		AfxMessageBox(_T("必须输入一个动画名称"));
		return;
	}

	OnOK();
}
