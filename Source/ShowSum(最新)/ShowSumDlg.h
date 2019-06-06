// ShowSumDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "resource.h"
#include "Mutex.h"

// CShowSumDlg dialog
class CShowSumDlg : public CDialog
{
// Construction
public:
	CShowSumDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SHOWSUM_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
public:
	//void UpdateShowItem(INT nIndex, LPCTSTR, LPCTSTR, INT);
	void UpdateUI();
	//CListCtrl GetList() { return m_listItem; }

// Implementation
protected:
	HICON m_hIcon;	
	void OnClose();
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnUpdateDataMessage(WPARAM wparam, LPARAM lparam);

private:
	CListCtrl	m_listItem;
	//CShowList	m_SumValue;
	Mutex		m_lock;
	INT			m_nSum;
public:
	afx_msg LRESULT OnBnClickedButton1(WPARAM wparam, LPARAM lparam);
	// fack number
	double m_lfRate;
	afx_msg void OnBnClickedButton1();
};

//extern CShowSumDlg g_ShowPageDlg;
