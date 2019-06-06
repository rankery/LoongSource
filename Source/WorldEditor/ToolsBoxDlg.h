#pragma once
#include "afxwin.h"


// CToolsBoxDlg 对话框

class CToolsBoxDlg : public CDialog
{
	DECLARE_DYNAMIC(CToolsBoxDlg)

public:
	CToolsBoxDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CToolsBoxDlg();

// 对话框数据
	enum { IDD = IDD_ToolsBox };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CListBox m_list;
	afx_msg void OnLbnSelchangeToolslist();
};
