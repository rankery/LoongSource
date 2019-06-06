// WayPointWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "MapEditor.h"
#include "WayPointWnd.h"
#include ".\waypointwnd.h"


// WayPointWnd 对话框

IMPLEMENT_DYNAMIC(WayPointWnd, CDialog)
WayPointWnd::WayPointWnd(CWnd* pParent /*=NULL*/)
	: CDialog(WayPointWnd::IDD, pParent)
	, m_strWayPtName(_T(""))
{
}

WayPointWnd::~WayPointWnd()
{
}

void WayPointWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_WAYPOINTNAME, m_EditWayPoint);
	DDX_Text(pDX, IDC_EDIT_WAYPOINTNAME, m_strWayPtName);
}


BEGIN_MESSAGE_MAP(WayPointWnd, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// WayPointWnd 消息处理程序

void WayPointWnd::OnBnClickedOk()
{
	((CEdit*)(GetDlgItem(IDC_EDIT_WAYPOINTNAME)))->GetWindowText(m_strWayPtName);
	OnOK();
}
