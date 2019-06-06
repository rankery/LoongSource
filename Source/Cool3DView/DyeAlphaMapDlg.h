#pragma once


// CDyeAlphaMapDlg 对话框

class CDyeAlphaMapDlg : public CDialog
{
	DECLARE_DYNAMIC(CDyeAlphaMapDlg)

public:
	CDyeAlphaMapDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDyeAlphaMapDlg();

// 对话框数据
	enum { IDD = IDD_DyeAlphaMap };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_szBaseTexPath;
	afx_msg void OnBnClickedBrowse();
	afx_msg void OnBnClickedOk();
	int m_nAlphaTexWidth;
	int m_nAlphaTexHeight;
	float m_fLumMask;
};
