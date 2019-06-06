#pragma once
#include "afxwin.h"


// CClassList 对话框

class CClassList : public CDialog
{
	DECLARE_DYNAMIC(CClassList)

public:
	CClassList(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CClassList();

// 对话框数据
	enum { IDD = IDD_ClassList };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_hList;
	CString	m_szSelectClass;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
