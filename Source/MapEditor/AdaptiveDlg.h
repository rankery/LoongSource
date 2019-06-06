#pragma once
#include "..\Cool3D\EditorSupport\SimpleView.h"
#include "afxwin.h"

// AdaptiveDlg 对话框
#define WM_DELADAPTIVE_DLG WM_USER+101
class AdaptiveDlg : public CDialog
{
	DECLARE_DYNAMIC(AdaptiveDlg)

public:
	AdaptiveDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~AdaptiveDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_Adaptive };
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);

	virtual void OnCancel();
	virtual void OnDestroy();
	void SetSimapleView(SimpleView* Cool3Dview, bool bActive){m_Cool3DView=Cool3Dview;m_bAdative=bActive;}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	
	DECLARE_MESSAGE_MAP()
public:
	SimpleView		*m_Cool3DView;
	CStatic			m_static;
	bool			m_bAdative;
	CWnd*			m_pParent;
};
