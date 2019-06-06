// SelAttDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MapEditor.h"
#include "SelAttDlg.h"
#include ".\selattdlg.h"
#include "PlugMapEditor.h"


// CSelAttDlg 对话框

IMPLEMENT_DYNAMIC(CSelAttDlg, CDialog)
CSelAttDlg::CSelAttDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelAttDlg::IDD, pParent)
{
	m_szTitle=_T("_RuntimeTitle");
	m_sel=0;
	m_dwLastNPCAttID=-1;
}

CSelAttDlg::~CSelAttDlg()
{
}

void CSelAttDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_AttList, m_hList);
}


BEGIN_MESSAGE_MAP(CSelAttDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_AttList, OnLvnItemchangedAttlist)
END_MESSAGE_MAP()


// CSelAttDlg 消息处理程序

BOOL CSelAttDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetWindowText(m_szTitle);
	m_hList.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_hList.InsertColumn(0, _T("属性ID"),	LVCFMT_LEFT,  64);
	m_hList.InsertColumn(1, _T("对象名称"),	LVCFMT_LEFT,  190);

	CreatureDataReader* pDB = g_mapEditor->GetMap()->GetAttDataBase();
	ASSERT(pDB!=NULL);
	
	int lastSel=-1;
	switch(m_objType)
	{
	case MapObjEdit::NPC:
		{
			CString attID;
			map<DWORD,tagCreatureProto> creatureMap;
			pDB->GetCreatureMap(creatureMap);
			int n=0;
			for(map<DWORD,tagCreatureProto>::iterator iter=creatureMap.begin();
				iter!=creatureMap.end(); iter++)
			{
				tagCreatureProto* pAtt = &iter->second;
				attID.Format(_T("%d"),pAtt->dwTypeID);
				m_hList.InsertItem(n,attID);
				m_hList.SetItemText(n,1,pAtt->szName);

				//--是上次选中的那个
				if(pAtt->dwTypeID == m_dwLastNPCAttID)
				{
					lastSel=n;
				}
				n++;
			}
		}
		break;
	}

	if(lastSel!=-1)
	{
		m_hList.SetItemState(lastSel,LVIS_SELECTED|LVIS_DROPHILITED,LVIS_SELECTED|LVIS_DROPHILITED);
		
		CRect rect;
		m_hList.GetItemRect(0, &rect, LVIR_BOUNDS);
		int itemheight=rect.bottom-rect.top;

		CSize size;
		size.cx=0;
		size.cy=lastSel*itemheight;
		m_hList.Scroll(size);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CSelAttDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	POSITION pos=m_hList.GetFirstSelectedItemPosition();
	if(pos==NULL)
	{
		AfxMessageBox(_T("请先选中一个属性!"));
		return;
	}
	int item=m_hList.GetNextSelectedItem(pos);

	 CString szID=m_hList.GetItemText(item,0);
	 m_sel=_tstoi(szID);
	
	OnOK();
}

void CSelAttDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

void CSelAttDlg::readstr(FILE *f,TCHAR *szString)
{
	do
	{
	   if(feof(f))  return;
	   ZeroMemory(szString,sizeof(szString));
	   _fgetts(szString, 255, f);
	} 
	while (((szString[0] == _T('/')) && (szString[1] == _T('/'))) || (szString[0] == _T('\n')));
	return;
}


void CSelAttDlg::OnLvnItemchangedAttlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	POSITION pos=m_hList.GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int item=m_hList.GetNextSelectedItem(pos);
		for(int i=0;i<m_hList.GetItemCount();i++)
		{
			if(i!=item)
				m_hList.SetItemState(i,0, LVIS_SELECTED|LVIS_DROPHILITED);	
		}
		
	}
	
	*pResult = 0;
}
