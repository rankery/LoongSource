#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "..\Cool3D\Cool3DEngine.h"
using namespace Cool3D;

// TLayerPage 对话框

class TLayerPage : public CPropertyPage
{
	DECLARE_DYNAMIC(TLayerPage)

public:
	TLayerPage();
	virtual ~TLayerPage();

// 对话框数据
	enum { IDD = IDD_PROPPAGE_LAYER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	CListCtrl m_layerList;

	void RebuildList();
	ImageGDI	m_tileImg;
public:
	CString GetSelLayerName();//得到选中的layer的名称
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedAddlayer();
	virtual BOOL OnSetActive();
	CStatic m_bmpDraw;
	afx_msg void OnNMClickLayerlist(NMHDR *pNMHDR, LRESULT *pResult);
	CListBox m_editTools;
	afx_msg void OnBnClickedDellayer();
	afx_msg void OnBnClickedLayerup();
	afx_msg void OnBnClickedLayerdown();
	afx_msg void OnBnClickedClear();
	afx_msg void OnNMDblclkLayerlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedChecklayer();
	CButton m_btnShowLayerCount;
	CButton m_btnShowSubPatchEdge;
};
