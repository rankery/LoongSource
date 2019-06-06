// BlendTrackDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Cool3DView.h"
#include "BlendTrackDlg.h"


// CBlendTrackDlg dialog

IMPLEMENT_DYNAMIC(CBlendTrackDlg, CDialog)

CBlendTrackDlg::CBlendTrackDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBlendTrackDlg::IDD, pParent)
	, m_szUpperSkeAniName(_T(""))
	, m_szLowerSkeAniName(_T(""))
	, m_szAllSkeAniName(_T(""))
	, m_fUpperWeight(1.0f)
	, m_LowerWeight(1.0f)
	, m_fAllWeight(1.0f)
{

}

CBlendTrackDlg::~CBlendTrackDlg()
{
}

void CBlendTrackDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_UpperSkeAniName, m_szUpperSkeAniName);
	DDX_Text(pDX, IDC_LowerSkeAniName, m_szLowerSkeAniName);
	DDX_Text(pDX, IDC_AllSkeAniName, m_szAllSkeAniName);
	DDX_Text(pDX, IDC_UpperWeight, m_fUpperWeight);
	DDX_Text(pDX, IDC_LowerWeight, m_LowerWeight);
	DDX_Text(pDX, IDC_AllWeight, m_fAllWeight);
}


BEGIN_MESSAGE_MAP(CBlendTrackDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CBlendTrackDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CBlendTrackDlg message handlers

void CBlendTrackDlg::OnBnClickedOk()
{
	UpdateData();

	OnOK();
}
