#pragma once
#include "afxwin.h"
#include "..\Cool3D\public\propertylist.h"
#include "..\Cool3D\Cool3DEngine.h"
using namespace Cool3D;



// TDecoPage 对话框

class TDecoPage : public CPropertyPage
{
	DECLARE_DYNAMIC(TDecoPage)

public:
	TDecoPage();
	virtual ~TDecoPage();

// 对话框数据
	enum { IDD = IDD_PROPPAGE_DECO };

	CString GetSelLayerName();
	bool IfEnableNoise();
protected:
	void BuildLayerList();
	void UpdateLayerProp(tagTDecoLayer *pLayer,bool bSave);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	CListBox		m_layerList;
	CPropertyList	m_layerProp;
	afx_msg void OnBnClickedBtnadd();
	afx_msg void OnBnClickedBtnremove();
	virtual BOOL OnInitDialog();
	afx_msg void OnLbnSelchangeLayerlist();
	afx_msg void OnBnClickedBtnapply();
public:
	afx_msg void OnBnClickedBtnimportmap();
	afx_msg void OnEnChangeShowsize();
protected:
	CString m_szShowSize;
public:
	afx_msg void OnBnClickedRefreshview();
	virtual BOOL OnSetActive();
};
