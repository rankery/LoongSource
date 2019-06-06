#pragma once
#include "afxwin.h"


// CAttachMdlDlg 对话框

class CAttachMdlDlg : public CDialog
{
	DECLARE_DYNAMIC(CAttachMdlDlg)

public:
	CAttachMdlDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAttachMdlDlg();

// 对话框数据
	enum { IDD = IDD_AttachMdl };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedMdlfile();
	afx_msg void OnBnClickedOk();
	CString m_szMdlPath;
	CString m_szTagNodeName;
};
