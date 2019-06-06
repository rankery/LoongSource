// DxtDlg.h : header file
//

#if !defined(AFX_DXTDLG_H__1B83A7A5_0D78_4A21_ADEF_3159AD027B80__INCLUDED_)
#define AFX_DXTDLG_H__1B83A7A5_0D78_4A21_ADEF_3159AD027B80__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CDxtDlg dialog

class CDxtDlg : public CDialog
{
// Construction
public:
	void CompressFolder(LPCTSTR  path);
	BOOL CompressByNV(LPCTSTR filename);
	LPDIRECT3DDEVICE9 m_pDevice;
	LPDIRECT3D9 m_pD3D;
	void ShowMsg(const char* lpszFormat,...);
	CDxtDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDxtDlg)
	enum { IDD = IDD_DXT_DIALOG };
	CEdit	m_editMsg;
	CString	m_szMsg;
	CString	m_szPath;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDxtDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int m_numError;
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDxtDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnChangeMsg();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	BOOL m_bSubDir;
	afx_msg void OnBnClickedOk();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DXTDLG_H__1B83A7A5_0D78_4A21_ADEF_3159AD027B80__INCLUDED_)
