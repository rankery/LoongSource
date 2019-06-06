// ClassList.cpp : 实现文件
//

#include "stdafx.h"
#include "SFXEditor.h"
#include "ClassList.h"
#include ".\classlist.h"
#include "SFXPlug.h"


// CClassList 对话框

IMPLEMENT_DYNAMIC(CClassList, CDialog)
CClassList::CClassList(CWnd* pParent /*=NULL*/)
	: CDialog(CClassList::IDD, pParent)
{
}

CClassList::~CClassList()
{
}

void CClassList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ClassList, m_hList);
}


BEGIN_MESSAGE_MAP(CClassList, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CClassList 消息处理程序

BOOL CClassList::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	const TCHAR* const szSuperClass=_T("EffectBase");
	const TCHAR* const szPathSuperClass=_T("EffectPathCtrl");
	for(UINT i=0;i<RTTFactory::Inst()->GetNumClass();i++)
	{
		RTTInfo* pClass=const_cast<RTTInfo*>(RTTFactory::Inst()->GetClass(i));
		if(pClass->IsKindOf(szSuperClass)
			&& _tcsicmp(pClass->m_szClassName,szSuperClass)!=0)//排除最基类
		{
			if(pClass->IsKindOf(szPathSuperClass))
			{
				if(_tcsicmp(pClass->m_szClassName,szPathSuperClass)!=0)
				{
					CString szTmp;//"路径: ";
					szTmp+=pClass->m_szClassName;
					m_hList.AddString(szTmp);
				}
			}
			else
			{
				CString szTmp;//="效果: ";
				szTmp+=pClass->m_szClassName;
				m_hList.AddString(szTmp);
			}
		}
	}
	m_hList.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CClassList::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	int sel=m_hList.GetCurSel();
	if(sel==-1)
	{
		AfxMessageBox(_T("请在列表中选择一个类型"));
		return;
	}

	m_hList.GetText(sel,m_szSelectClass);

	OnOK();
}
