#pragma once
#include "afxwin.h"


// CSelUpperSkeAniDlg 对话框

class CSelUpperSkeAniDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelUpperSkeAniDlg)

public:
	CSelUpperSkeAniDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSelUpperSkeAniDlg();

// 对话框数据
	enum { IDD = IDD_SELUPPERSKEANIDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CString m_szUpperSkeAniName;
	CString m_szLowerSkeAniName;
	BOOL m_bIsOneTrack;
	CEdit m_UpperSkeAniEdit;
};
