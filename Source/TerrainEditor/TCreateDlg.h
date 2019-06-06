#pragma once


// TCreateDlg 对话框

class TCreateDlg : public CDialog
{
	DECLARE_DYNAMIC(TCreateDlg)

public:
	TCreateDlg(const TCHAR* szTerrainName,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~TCreateDlg();

// 对话框数据
	enum { IDD = IDD_Create };

protected:
	CString	m_szTerrainName;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_szWidth;
	CString m_szHeight;
	CString m_szXScale;
	CString m_szZScale;
	CString m_szYScale;
	afx_msg void OnBnClickedOk();
	CString m_szInitVal;
	BOOL m_bNoise;
	CString m_szUScale;
	CString m_szVScale;

	bool m_bMod;
	virtual BOOL OnInitDialog();
};
