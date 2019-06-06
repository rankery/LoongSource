#pragma once

//

/////////////////////////////////////////////////////////////////////////////
// CWebView html view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#include <afxhtml.h>

class CWebView : public CHtmlView
{
protected:
	DECLARE_DYNCREATE(CWebView)

	// html Data
public:
	//{{AFX_DATA(CWebView)
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	// Attributes
public:

	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWebView)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();

	virtual void OnAppCmd(LPCTSTR lpszWhere);

	//}}AFX_VIRTUAL

	// Implementation
public:
	CWebView();
	virtual ~CWebView();

	virtual void OnPaint();
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CWebView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnDestroy();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

