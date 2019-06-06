#include "..\fworldeditor\mainfrm.h"
#include "..\fworldeditor\mainfrm.h"
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "Cool3DView.h"

#include "MainFrm.h"
#include ".\mainfrm.h"
#include "Cool3DModels.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}

	CRect rc;
	GetClientRect(rc);
	CenterWindow(NULL);

	return 0;
}

void _ShowNote(const TCHAR* szTxt)
{
	CMainFrame *pM=(CMainFrame *)::AfxGetMainWnd();
	pM->m_wndStatusBar.SetPaneText(1,szTxt);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或
	// 样式
	RECT rc;
	rc.left=0;
	rc.right=1010;
	rc.top=0;
	rc.bottom=650;
	AdjustWindowRect(&rc,cs.style,TRUE);
	cs.x=rc.left;
	cs.y=rc.top;
	cs.cx=rc.right-rc.left;
	cs.cy=rc.bottom-rc.top;

	return TRUE;
}


// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame 消息处理程序


void CMainFrame::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类

	Cool3DModels::Inst()->Destroy();

	CFrameWnd::PostNcDestroy();
}