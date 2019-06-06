#pragma once
#include "..\Cool3D\public\propertylist.h"


// CUVSetDlg 对话框

class CUVSetDlg : public CDialog
{
	DECLARE_DYNAMIC(CUVSetDlg)

public:
	CUVSetDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUVSetDlg();

// 对话框数据
	enum { IDD = IDD_UVSet };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	CPropertyList m_hUVList;

	void UpdateUVList(bool bSave);
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
