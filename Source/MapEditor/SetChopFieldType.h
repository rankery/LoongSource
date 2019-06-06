#pragma once


// CSetChopFieldType 对话框

class CSetChopFieldType : public CDialog
{
	DECLARE_DYNAMIC(CSetChopFieldType)

public:
	CSetChopFieldType(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetChopFieldType();

// 对话框数据
	enum { IDD = IDD_DIALOG_CHOPFIELDTYPE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	CString   m_szChopFieldType;
public:
	virtual BOOL OnInitDialog();
	int GetChopFieldType();

	CComboBox m_ChopFieldType;
	afx_msg void OnBnClickedOk();
};
