// SetChopFieldType.cpp : 实现文件
//

#include "stdafx.h"
#include "MapEditor.h"
#include "SetChopFieldType.h"


// CSetChopFieldType 对话框

IMPLEMENT_DYNAMIC(CSetChopFieldType, CDialog)
CSetChopFieldType::CSetChopFieldType(CWnd* pParent /*=NULL*/)
	: CDialog(CSetChopFieldType::IDD, pParent)
{
}

CSetChopFieldType::~CSetChopFieldType()
{
}

void CSetChopFieldType::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBOBOX_CHOPFIELDTYPE, m_ChopFieldType);
}


BEGIN_MESSAGE_MAP(CSetChopFieldType, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CSetChopFieldType 消息处理程序
BOOL CSetChopFieldType::OnInitDialog()
{
	CDialog::OnInitDialog();
	//初始化对象类型列表
	m_ChopFieldType.InsertString(0,_T("树木"));
	m_ChopFieldType.InsertString(1,_T("麦田"));	
	m_ChopFieldType.SetCurSel(0);

	return TRUE;
}

int CSetChopFieldType::GetChopFieldType()
{
	if(m_szChopFieldType==_T("树木"))
		return 0;
	else if(m_szChopFieldType==_T("麦田"))
		return 1;

	return -1;
}

void CSetChopFieldType::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChopFieldType.GetWindowText(m_szChopFieldType);
	OnOK();
}
