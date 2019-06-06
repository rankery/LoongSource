// FilterSelect.cpp : 实现文件
//

#include "stdafx.h"
#include "MapEditor.h"
#include "FilterSelect.h"
#include "PlugMapEditor.h"
#include ".\filterselect.h"

// FilterSelect 对话框

IMPLEMENT_DYNAMIC(FilterSelect, CDialog)
FilterSelect::FilterSelect(CWnd* pParent /*=NULL*/)
	: CDialog(FilterSelect::IDD, pParent)
{
	m_IsNpc=false;
	m_IsDynaBlock=false;
	m_IsStaticObj=false;
	m_IsMapDoor=false;
	m_IsQuad=false;
	m_IsTrigger = false;
	m_IsWayPoint = false;
	m_IsMapTriggerEffect = false;
}

FilterSelect::~FilterSelect()
{
}

void FilterSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL FilterSelect::OnInitDialog()
{
	CDialog::OnInitDialog();
	if(g_mapEditor->GetCMapObjDlg()->m_IsNpc)
		((CButton*)(GetDlgItem(IDC_CHECK_NPC)))->SetCheck(BST_CHECKED);
	if(g_mapEditor->GetCMapObjDlg()->m_IsDynaBlock)
		((CButton*)(GetDlgItem(IDC_CHECK_DynamicBlock)))->SetCheck(BST_CHECKED);
	if(g_mapEditor->GetCMapObjDlg()->m_IsMapDoor)
		((CButton*)(GetDlgItem(IDC_CHECK_Door)))->SetCheck(BST_CHECKED);
	if(g_mapEditor->GetCMapObjDlg()->m_IsStaticObj)
		((CButton*)(GetDlgItem(IDC_CHECK_StaticObj)))->SetCheck(BST_CHECKED);
	if(g_mapEditor->GetCMapObjDlg()->m_IsQuad)
		((CButton*)(GetDlgItem(IDC_CHECK_Quad)))->SetCheck(BST_CHECKED);
	if(g_mapEditor->GetCMapObjDlg()->m_IsLock)
		((CButton*)(GetDlgItem(IDC_ObjLock)))->SetCheck(BST_CHECKED);
	if(g_mapEditor->GetCMapObjDlg()->m_IsFlag)
		((CButton*)(GetDlgItem(IDC_ObjFlag)))->SetCheck(BST_CHECKED);
	if(g_mapEditor->GetCMapObjDlg()->m_IsMapRect)
		((CButton*)(GetDlgItem(IDC_CHECK_MAPRECT)))->SetCheck(BST_CHECKED);
	if(g_mapEditor->GetCMapObjDlg()->m_IsTrigger)
		((CButton*)(GetDlgItem(IDC_CHECK2_Trigger)))->SetCheck(BST_CHECKED);
	if(g_mapEditor->GetCMapObjDlg()->m_IsWayPoint)
		((CButton*)(GetDlgItem(IDC_CHECK4_WayPoint)))->SetCheck(BST_CHECKED);
	if(g_mapEditor->GetCMapObjDlg()->m_IsMapTriggerEffect)
		((CButton*)(GetDlgItem(IDC_CHECK_TriggerEffect)))->SetCheck(BST_CHECKED);
	return TRUE;
}


BEGIN_MESSAGE_MAP(FilterSelect, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// FilterSelect 消息处理程序

void FilterSelect::OnBnClickedOk()
{
	m_IsNpc = ((CButton*)(GetDlgItem(IDC_CHECK_NPC)))->GetCheck()==1;
	m_IsDynaBlock = ((CButton*)(GetDlgItem(IDC_CHECK_DynamicBlock)))->GetCheck()==1;
	m_IsStaticObj = ((CButton*)(GetDlgItem(IDC_CHECK_StaticObj)))->GetCheck()==1;
	m_IsQuad = ((CButton*)(GetDlgItem(IDC_CHECK_Quad)))->GetCheck()==1;
	m_IsLock = ((CButton*)(GetDlgItem(IDC_ObjLock)))->GetCheck()==1;
	m_IsFlag = ((CButton*)(GetDlgItem(IDC_ObjFlag)))->GetCheck()==1;
	m_IsMapRect = ((CButton*)(GetDlgItem(IDC_CHECK_MAPRECT)))->GetCheck()==1;
	m_IsTrigger = ((CButton*)(GetDlgItem(IDC_CHECK2_Trigger)))->GetCheck()==1;
	m_IsWayPoint = ((CButton*)(GetDlgItem(IDC_CHECK4_WayPoint)))->GetCheck()==1;
	m_IsMapDoor = ((CButton*)(GetDlgItem(IDC_CHECK_Door)))->GetCheck()==1;
	m_IsMapTriggerEffect = ((CButton*)(GetDlgItem(IDC_CHECK_TriggerEffect)))->GetCheck()==1;
	OnOK();
}
