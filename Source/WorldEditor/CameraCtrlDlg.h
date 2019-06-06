#pragma once
#include "EditorCamera.h"
#include "afxcmn.h"

// CCameraCtrlDlg 对话框

class CCameraCtrlDlg : public CDialog
{
	DECLARE_DYNAMIC(CCameraCtrlDlg)

public:
	CCameraCtrlDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCameraCtrlDlg();
	virtual BOOL OnInitDialog();
	float GetSlider() { return (float)m_slider.GetPos(); }

// 对话框数据
	enum { IDD = IDD_CameraCtrl };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	int m_nX;
	int m_nY;
	EditorCamera	*m_pCamera;
	afx_msg void OnBnClickedButtonGoto();
	CSliderCtrl m_slider;
};
