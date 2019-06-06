#pragma once
#include "..\Cool3D\public\propertylist.h"
#include "..\Cool3D\Cool3DEngine.h"
#include "afxwin.h"
using namespace Cool3D;

// TRiverPage 对话框

class TRiverPage : public CPropertyPage
{
	DECLARE_DYNAMIC(TRiverPage)

public:
	TRiverPage();
	virtual ~TRiverPage();

// 对话框数据
	enum { IDD = IDD_PROPPAGE_River };

	BOOL GetSelRiverIndex(UINT& riverIndex);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	void UpdateMtlDesc(tagRiverMtlDesc *pMtl,bool bSave);
	virtual BOOL OnInitDialog();

	CString m_szHeight;
	CPropertyList m_mtlProp;

	afx_msg void OnEnChangeHeight();
	afx_msg void OnBnClickedApplymtl();
	afx_msg void OnBnClickedNewriver();
	afx_msg void OnBnClickedDelriver();
	CListBox m_riverList;

	void BuildRiverList();
public:
	afx_msg void OnLbnSelchangeList1();
	virtual BOOL OnSetActive();
	afx_msg void OnEnChangeShowdist();
	afx_msg void OnEnChangeEditdist();
	CListBox m_editTools;
	CString m_szShowDist;
	CString m_szEditDist;
};
