#pragma once


// TImportDlg 对话框

class TImportDlg : public CDialog
{
	DECLARE_DYNAMIC(TImportDlg)

public:
	TImportDlg(const TCHAR* szTerrainName,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~TImportDlg();

// 对话框数据
	enum { IDD = IDD_IMPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	CString	m_szTerrainName;

	DECLARE_MESSAGE_MAP()
public:
	CString m_szFile;
	CString m_szXScale;
	CString m_szZScale;
	CString m_szYScale;
	afx_msg void OnBnClickedFiledlg();
	afx_msg void OnBnClickedOk();
	CString m_szUScale;
	CString m_szVScale;
};
