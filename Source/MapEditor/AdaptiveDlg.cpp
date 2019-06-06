// AdaptiveDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MapEditor.h"
#include "AdaptiveDlg.h"
const UINT TIMER_ID =  10012;

// AdaptiveDlg 对话框

IMPLEMENT_DYNAMIC(AdaptiveDlg, CDialog)
AdaptiveDlg::AdaptiveDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AdaptiveDlg::IDD, pParent)
{
	m_Cool3DView = NULL;
	m_bAdative = false;
	ASSERT(pParent);
	m_pParent = pParent;
}

AdaptiveDlg::~AdaptiveDlg()
{
	m_pParent = NULL;
	m_Cool3DView = NULL;
}

void AdaptiveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_hwndDraw, m_static);
}

BOOL AdaptiveDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetTimer(TIMER_ID, 20, NULL);
	m_static.UpdateWindow();
	return TRUE;
}

//鼠标操作
void AdaptiveDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	GetDlgItem(IDC_STATIC_hwndDraw)->SetFocus();
	m_Cool3DView->OnLButtonDown(nFlags, point);
	CDialog::OnLButtonDown(nFlags, point);
}

void AdaptiveDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_Cool3DView->OnLButtonUp(nFlags, point);
	CDialog::OnLButtonUp(nFlags, point);
}

void AdaptiveDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	m_Cool3DView->OnMouseMove(nFlags, point);
	CDialog::OnMouseMove(nFlags, point);
}

BOOL AdaptiveDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	m_Cool3DView->OnMouseWheel(nFlags, zDelta, pt);
	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

void AdaptiveDlg::OnRButtonDown(UINT nFlags, CPoint point)
{

	m_Cool3DView->OnRButtonDown(nFlags, point);
	CDialog::OnRButtonDown(nFlags, point);
}

void AdaptiveDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	m_Cool3DView->OnRButtonUp(nFlags, point);
	CDialog::OnRButtonUp(nFlags, point);
}

void AdaptiveDlg::OnTimer(UINT nIDEvent)
{
	if(m_Cool3DView!=NULL)
		m_Cool3DView->Render(m_static.GetSafeHwnd(), m_bAdative);
}
void AdaptiveDlg::OnCancel()
{
	m_bAdative = false;
	CDialog::OnCancel();
	m_pParent->PostMessage(WM_DELADAPTIVE_DLG,(WPARAM)this);
	DestroyWindow();
}
void AdaptiveDlg::OnDestroy()
{
	CDialog::OnDestroy();
	delete this;	
}


BEGIN_MESSAGE_MAP(AdaptiveDlg, CDialog) 
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()
