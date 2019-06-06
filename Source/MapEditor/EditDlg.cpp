// EditDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MapEditor.h"
#include ".\editdlg.h"
#include ".\mapobjdlg.h"
#include ".\PlugMapEditor.h"

// CEditDlg 对话框

IMPLEMENT_DYNAMIC(CEditDlg, CDialog)
CEditDlg::CEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditDlg::IDD, pParent)
{
	m_pParent = NULL;
	m_bAdd = false;
	m_iSelType = -1;
}

CEditDlg::~CEditDlg()
{
}

void CEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEditDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_BN_CLICKED(IDC_CANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_OK, OnBnClickedOk)
END_MESSAGE_MAP()


// CEditDlg 消息处理程序

void CEditDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if(nID == SC_CLOSE)
	{
		ShowWindow(SW_HIDE);
		m_bAdd = false;
	}else
		CDialog::OnSysCommand(nID, lParam);
}

void CEditDlg::OnBnClickedOk()
{
	/*UpdateData();
	CString szBuffer;
	tagNpcDesc NpcDesc;
	tagStaticObjDesc StaticObjDesc;


	switch(m_iSelType)
	{
	case CB_ERR:
		AfxMessageBox("请先选择一个类型");
		return;
	case 0:
		GetDlgItem(IDC_EDIT_MAPID)->GetWindowText(szBuffer);
		NpcDesc.libID = atoi(szBuffer);
		GetDlgItem(IDC_EDIT_ATTID)->GetWindowText(szBuffer);
		NpcDesc.attNpc.attID = atoi(szBuffer);

		if(NULL == g_mapEditor->GetDataBase()->FindNpcAtt(atoi(szBuffer)))
			goto ErrorHint;
#pragma warning(disable:4244)
		GetDlgItem(IDC_EDIT_XPOS)->GetWindowText(szBuffer);
		NpcDesc.vecPos.x  = atof(szBuffer);


		GetDlgItem(IDC_EDIT_YPOS)->GetWindowText(szBuffer);
		NpcDesc.vecPos.y  = atof(szBuffer);


		GetDlgItem(IDC_EDIT_ZPOS)->GetWindowText(szBuffer);
		NpcDesc.vecPos.z  = atof(szBuffer);


		GetDlgItem(IDC_EDIT_DIR)->GetWindowText(szBuffer);
		NpcDesc.fDir = atof(szBuffer);


		GetDlgItem(IDC_EDIT_SPEED)->GetWindowText(szBuffer);
		NpcDesc.fSpeed = atof(szBuffer);

#pragma warning(default:4244)
		break;
	case 1:
		GetDlgItem(IDC_EDIT_MAPID)->GetWindowText(szBuffer);
		StaticObjDesc.libID = atoi(szBuffer);
		GetDlgItem(IDC_EDIT_ATTID)->GetWindowText(szBuffer);
		StaticObjDesc.attStaticObj.attID = atoi(szBuffer);

		if(NULL== g_mapEditor->GetDataBase()->FindStaticObjAtt(atoi(szBuffer)))
			goto ErrorHint;

		GetDlgItem(IDC_EDIT_XPOS)->GetWindowText(szBuffer);
		StaticObjDesc.vecPos.x = atof(szBuffer);
		GetDlgItem(IDC_EDIT_YPOS)->GetWindowText(szBuffer);
		StaticObjDesc.vecPos.y = atof(szBuffer);
		GetDlgItem(IDC_EDIT_ZPOS)->GetWindowText(szBuffer);
		StaticObjDesc.vecPos.z = atof(szBuffer);
		GetDlgItem(IDC_EDIT_DIR)->GetWindowText(szBuffer);
		StaticObjDesc.fDir = atof(szBuffer);
		break;
	default:
ErrorHint:
		AfxMessageBox("不合法的属性ID!");
		return;
	}

	


	CMapObjDlg* pWnd = (CMapObjDlg*)m_pParent;
	pWnd->UpdateList(&NpcDesc,&StaticObjDesc, m_bAdd);
	ShowWindow(SW_HIDE);
	m_bAdd = false;
*/
}

void CEditDlg::OnBnClickedCancel()
{
	ShowWindow(SW_HIDE);
}