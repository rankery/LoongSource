#pragma once
#include "afxcmn.h"


// CSkelDlg 对话框
class Cool3D::ResSkeleton;
class CSkelDlg : public CDialog
{
	DECLARE_DYNAMIC(CSkelDlg)

public:
	CSkelDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSkelDlg();

// 对话框数据
	enum { IDD = IDD_SkelDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	

public:
	Cool3D::ResSkeleton *m_pRes;
	virtual BOOL OnInitDialog();
	CTreeCtrl m_skelTree;
	CString m_TagNodes;
};
