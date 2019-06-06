#pragma once
#include "afxwin.h"


// WayPointWnd 对话框

class WayPointWnd : public CDialog
{
	DECLARE_DYNAMIC(WayPointWnd)

public:
	WayPointWnd(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~WayPointWnd();

// 对话框数据
	enum { IDD = IDD_WayPoint };
	CString GetWayPointName(){return m_strWayPtName;}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_EditWayPoint;
	CString m_strWayPtName;
	afx_msg void OnBnClickedOk();
};
