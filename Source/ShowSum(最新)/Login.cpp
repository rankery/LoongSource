// Login.cpp : implementation file
//

#include "stdafx.h"
#include "ShowSum.h"
#include "Login.h"

CString CLogin::m_strAccount(_T(""));
CString CLogin::m_strPwd(_T(""));
// CLogin dialog

IMPLEMENT_DYNAMIC(CLogin, CDialog)

CLogin::CLogin(CWnd* pParent /*=NULL*/)
	: CDialog(CLogin::IDD, pParent)
{

}

CLogin::~CLogin()
{
}

void CLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strAccount);
	DDV_MaxChars(pDX, m_strAccount, 32);
	DDX_Text(pDX, IDC_EDIT2, m_strPwd);
	DDV_MaxChars(pDX, m_strPwd, 32);
}


BEGIN_MESSAGE_MAP(CLogin, CDialog)
	ON_BN_CLICKED(IDOK, &CLogin::OnBnClickedOk)
END_MESSAGE_MAP()


// CLogin message handlers

void CLogin::OnBnClickedOk()
{
	UpdateData();
	if(m_strAccount.IsEmpty())
	{
		AfxMessageBox(_T("Account is required"));
		return;
	}
	if(m_strPwd.IsEmpty())
	{
		AfxMessageBox(_T("Password is required"));
		return;
	}
	OnOK();
}
