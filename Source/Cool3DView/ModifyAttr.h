#pragma once


// CModifyAttr dialog

class CModifyAttr : public CDialog
{
	DECLARE_DYNAMIC(CModifyAttr)

public:
	CModifyAttr(CWnd* pParent = NULL);   // standard constructor
	virtual ~CModifyAttr();

// Dialog Data
	enum { IDD = IDD_ModifyAttr };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	int m_iLightmapSize;
};
