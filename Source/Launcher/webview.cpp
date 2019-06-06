// NetView.cpp : implementation file
//

#include "stdafx.h"
#include "webView.h"

/////////////////////////////////////////////////////////////////////////////
// CWebView

IMPLEMENT_DYNCREATE(CWebView, CHtmlView)

CWebView::CWebView()
{
	//{{AFX_DATA_INIT(CWebView)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CWebView::~CWebView()
{
}

void CWebView::DoDataExchange(CDataExchange* pDX)
{
	CHtmlView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWebView)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWebView, CHtmlView)
	//{{AFX_MSG_MAP(CWebView)
	//}}AFX_MSG_MAP
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEACTIVATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWebView diagnostics

#ifdef _DEBUG
void CWebView::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CWebView::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWebView message handlers

void CWebView::PostNcDestroy() 
{
}
//-------------------------------------------------------------------------------------------
//自绘渲染
//-------------------------------------------------------------------------------------------
void CWebView::OnPaint()
{

	CHtmlView::OnPaint();
}

BOOL CWebView::OnEraseBkgnd(CDC* pDC)
{
	return CWnd::OnEraseBkgnd(pDC);
}

int CWebView::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

void CWebView::OnDestroy()
{
	CWnd::OnDestroy(); // bypass CView doc/frame stuff
}


void CWebView::OnAppCmd(LPCTSTR lpszWhere)
{
	// default: do nothing
}


