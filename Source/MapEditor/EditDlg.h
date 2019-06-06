#pragma once


// CEditDlg 对话框

class CEditDlg : public CDialog
{
	DECLARE_DYNAMIC(CEditDlg)

public:
	CEditDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CEditDlg();

// 对话框数据
	enum { IDD = IDD_EDITDLG };
	
	bool		m_bAdd;
	int			m_iSelType;
	void AttachParent(CWnd* pWnd)
	{
		ASSERT(pWnd != NULL);
		m_pParent = pWnd;
	}
protected:
	
	CWnd* m_pParent;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
};
