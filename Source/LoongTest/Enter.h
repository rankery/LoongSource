#if !defined(AFX_ENTER_H__B70743CD_48E2_4DC7_9BD6_58B1428F9DDF__INCLUDED_)
#define AFX_ENTER_H__B70743CD_48E2_4DC7_9BD6_58B1428F9DDF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Enter.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CEnter dialog

class CEnter : public CDialog
{
// Construction
public:



	CEnter(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEnter)
	enum { IDD = IDD_DIALOG1 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEnter)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEnter)
	virtual void OnOK();

	afx_msg void OnTJ();
	afx_msg void OnSelchangeCombo4();
	afx_msg void OnClose();
	afx_msg void OnCancel2();
afx_msg	VOID NetRecvJoinGame(tagNetCmd *pM, DWORD pPrama); 

afx_msg	VOID AAWaitResponse(tagNetCmd *pM, DWORD pPrama);

afx_msg	VOID AAWaitProofResult(tagNetCmd *pM, DWORD pPrama);
afx_msg VOID ShowResult(tagNetCmd * pMsg, DWORD pPrama);

//static DWORD RecvThreadFunc(LPVOID   pParam)
//{
//return 0;
//};



	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedOk2();
	afx_msg void OnBnClickedOk3();
	afx_msg void OnBnClickedOk4();
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton2();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENTER_H__B70743CD_48E2_4DC7_9BD6_58B1428F9DDF__INCLUDED_)
