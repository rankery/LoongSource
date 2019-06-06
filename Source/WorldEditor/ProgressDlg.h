#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "..\Cool3D\EditorSupport\IUICallBack.h"
#include "TextProgressCtrl.h"


// CProgressDlg 对话框

class CProgressDlg : 
	public CDialog,
    public IUICallBack
{
	DECLARE_DYNAMIC(CProgressDlg)

public:
	CProgressDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CProgressDlg();

// 对话框数据
	enum { IDD = IDD_Progress };

	void Create(int max,int min);

	virtual void AddMsg(const TCHAR* szFormat,...);
	virtual void SetProgress(int curVal,int maxVal);

	const TCHAR* GetMapName();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnNMCustomdrawProgress(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	CTextProgressCtrl m_ctrl;

public:
	CString m_szInfo;
private:
	CEdit m_edit;
protected:
	CString m_szMapName;
public:
	afx_msg void OnBnClickedBuild();
	afx_msg void OnBnClickedExit();
protected:
	BOOL m_bTrn;
	BOOL m_bNav;
	BOOL m_bMap;
	BOOL m_bRiver;
	BOOL m_bLightMap;
	BOOL m_bCompress;
	BOOL m_bCalcLight;
	BOOL m_bRadiosity;
	int m_nRadiosityTimes;
	BOOL m_bBlur;
	int m_nBlurTimes;
	BOOL m_bApplyToCombine;
	BOOL m_bApplyToDeco;
	BOOL m_bAmbientOcclusion;
	BOOL m_bColliderData;
	BOOL m_bNPCNavMap;
	BOOL m_bNPCNavGraph;
	BOOL m_bPlayerNavMap;

	BOOL m_bMiniMap;
	int m_nMiniMapSize;
public:
	float m_fNPCPassHeight;
	float m_fNPCNavGraphPassHeight;
	BOOL m_bSmallAO;
};
