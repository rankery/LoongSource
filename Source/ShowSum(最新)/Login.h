#pragma once


// CLogin dialog

class CLogin : public CDialog
{
	DECLARE_DYNAMIC(CLogin)

public:
	CLogin(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLogin();

// Dialog Data
	enum { IDD = IDD_LOGIN };

	static CString getAccount() { return m_strAccount;}
	static CString getPwd() {return m_strPwd;}

protected:
	static CString m_strAccount;
	static CString m_strPwd;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();	
};
