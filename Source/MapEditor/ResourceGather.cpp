// ResourceGather.cpp : 实现文件
//

#include "stdafx.h"
#include "MapEditor.h"
#include "ResourceGather.h"
#include "PlugMapEditor.h"
#include "MapObjDlg.h"
#include "..\WorldBase\GameMapEditor\MapObjEdit.h"
#include "..\WorldBase\GameMapEditor\GameMapEditor.h"

// ResourceGather 对话框

IMPLEMENT_DYNAMIC(ResourceGather, CDialog)
ResourceGather::ResourceGather(CWnd* pParent /*=NULL*/)
	: CDialog(ResourceGather::IDD, pParent)
{
	ASSERT(pParent);
	m_pParent = pParent;
}

ResourceGather::~ResourceGather()
{
	m_pParent = NULL;
}

void ResourceGather::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(ResourceGather, CDialog)
	ON_BN_CLICKED(IDC_NewAdd, OnBnClickedNewadd)
	ON_BN_CLICKED(IDC_Save, OnBnClickedSave)
	ON_BN_CLICKED(IDC_Read, OnBnClickedRead)
	ON_BN_CLICKED(IDC_Delete, OnBnClickedDelete)
	ON_WM_DESTROY()
	ON_NOTIFY(HDN_ITEMDBLCLICK, 0, &ResourceGather::OnHdnItemdblclickList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &ResourceGather::OnNMDblclkList1)
END_MESSAGE_MAP()



BOOL ResourceGather::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_ListCtrl.InsertColumn(0, _T("地图对象"), LVCFMT_LEFT,  250);
	m_ListCtrl.InsertColumn(1, _T("地图对象名"), LVCFMT_LEFT,  100);
	m_ListCtrl.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	UpdateList();
	return TRUE;
}
void ResourceGather::UpdateList()
{
	m_ListCtrl.DeleteAllItems();
	int nItem = 0;
	vector<tagResGather>::iterator ite = CMapObjDlg::m_vecRes.begin();
	for(; ite!=CMapObjDlg::m_vecRes.end(); ++ite)
	{
		m_ListCtrl.InsertItem(nItem, (*ite).szObj, 0);

		if((*ite).eType==GMAPOBJ_Npc)
		{
			DWORD dwTypeID = _ttoi((*ite).szObj);
			CreatureDataReader* pCD = g_mapEditor->GetMap()->GetAttDataBase();
			ASSERT(pCD!=NULL);
			map<DWORD, tagCreatureProto> npcMap;
			pCD->GetCreatureMap(npcMap);

			for(map<DWORD,tagCreatureProto>::iterator iter=npcMap.begin();iter!=npcMap.end();++iter)
			{
				tagCreatureProto *pAtt = &iter->second;
				if(pAtt->dwTypeID == dwTypeID)
				{
					m_ListCtrl.SetItemText(nItem,1,pAtt->szName);
					break;
				}
			}
		}
		else if((*ite).eType==GMAPOBJ_Sound)
		{
			m_ListCtrl.SetItemText(nItem,1,_T("音效"));
		}
		else if((*ite).eType==GMAPOBJ_StaticObj)
		{
			m_ListCtrl.SetItemText(nItem,1,_T("静态地物"));
		}
		else if((*ite).eType==GMAPOBJ_DynamicBlock)
		{
			m_ListCtrl.SetItemText(nItem,1,_T("动态障碍物"));
		}
		else if((*ite).eType==GMAPOBJ_MapDoor)
		{
			m_ListCtrl.SetItemText(nItem,1,_T("门"));	
		}
		nItem++;
	}
}

// ResourceGather 消息处理程序

void ResourceGather::OnCancel()
{
	CDialog::OnCancel();
	m_pParent->PostMessage(WM_DELRG_DLG,(WPARAM)this);
	DestroyWindow();
}
void ResourceGather::OnDestroy()
{
	CDialog::OnDestroy();
	delete this;	
}

void ResourceGather::OnBnClickedNewadd()
{
	POSITION pos=m_ListCtrl.GetFirstSelectedItemPosition();
	if(pos==NULL)
	{
		AfxMessageBox(_T("请先选中一个地图对象!"));
		return;
	}
	int item=m_ListCtrl.GetNextSelectedItem(pos);
	CString szID=m_ListCtrl.GetItemText(item,0);
	vector<tagResGather>::iterator it = CMapObjDlg::m_vecRes.begin();
	for(; it!=CMapObjDlg::m_vecRes.end(); ++it)
	{
		if(_tcscmp((*it).szObj, szID.GetBuffer()) == 0)
			m_tagRG = (*it);
	}
	//OnOK();
	m_pParent->PostMessage(WM_NEWOBJ,(WPARAM)this);
}

void ResourceGather::OnBnClickedSave()
{
	int nNum = 0;
	nNum = (int)CMapObjDlg::m_vecRes.size();
	CFileDialog dlg(TRUE, NULL, NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR, 
		_T("*.rg|*.rg||"),this, 0); 
	if(IDOK == dlg.DoModal())
	{
		CString szFileName = dlg.GetPathName();
		FILE* fp = NULL;
		if(_tfopen_s(&fp,szFileName.GetBuffer(),_T("wb"))!=0)
			return ;
		fwrite(&nNum, sizeof(int), 1, fp);
		vector<tagResGather>::iterator it = CMapObjDlg::m_vecRes.begin();
		for(; it!=CMapObjDlg::m_vecRes.end(); ++it)
		{
			fwrite(&(*it), sizeof(tagResGather), 1, fp);
		}
		fclose(fp);
	}
}

void ResourceGather::OnBnClickedRead()
{
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT| OFN_NOCHANGEDIR,
		_T("rg文件|*.rg||"),this,0);
	if(IDOK == dlg.DoModal())
	{
		CString szFileName = dlg.GetPathName();
		FILE* fp = NULL;
		if(_tfopen_s(&fp,szFileName.GetBuffer(),_T("rb"))!=0)
			return;
		int nNum = 0;
		fread(&nNum, sizeof(int), 1, fp);
		vector<tagResGather> vecRes;
		for(int i=0; i<nNum; ++i)
		{
			tagResGather tagRG;
			fread(&tagRG, sizeof(tagResGather), 1, fp);
			vecRes.push_back(tagRG);
		}
		fclose(fp);
		vector<tagResGather>::iterator ittemp = vecRes.begin();
		vector<tagResGather>::iterator it;
		for(; ittemp!=vecRes.end(); ++ittemp)
		{
			it = CMapObjDlg::m_vecRes.begin();
			bool bHad = false;
			for(; it!=CMapObjDlg::m_vecRes.end(); ++it)
			{
				if(_tcscmp((*ittemp).szObj, (*it).szObj) == 0)
				{
					bHad = true;
					break;
				}
			}
			if(!bHad)
				CMapObjDlg::m_vecRes.push_back((*ittemp));
		}
		UpdateList();
	}
}

void ResourceGather::OnBnClickedDelete()
{
	POSITION pos=m_ListCtrl.GetFirstSelectedItemPosition();
	if(pos==NULL)
	{
		AfxMessageBox(_T("请先选中一个地图对象!"));
		return;
	}
	int item=m_ListCtrl.GetNextSelectedItem(pos);
	CString szID=m_ListCtrl.GetItemText(item,0);
	vector<tagResGather>::iterator it = CMapObjDlg::m_vecRes.begin();
	for(; it!=CMapObjDlg::m_vecRes.end(); ++it)
	{
		if(_tcscmp(szID.GetBuffer(), (*it).szObj) == 0)
		{
			CMapObjDlg::m_vecRes.erase(it);
			break;
		}
	}
	UpdateList();
}

void ResourceGather::OnHdnItemdblclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	
	*pResult = 0;
}

void ResourceGather::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	POSITION pos = m_ListCtrl.GetFirstSelectedItemPosition();
	if(pos != NULL)
	{
		int nItem = m_ListCtrl.GetNextSelectedItem(pos);
		m_szMdlPath = m_ListCtrl.GetItemText(nItem,0);
		CString mdlName = m_ListCtrl.GetItemText(nItem,1);
		if(mdlName==_T("静态地物") || mdlName==_T("动态障碍物") || mdlName==_T("门"))
			m_bShow = true;
		else 
			m_bShow = false;
		m_pParent->PostMessage(WM_SHOWMODEL,(WPARAM)this);
	}
	*pResult = 0;
}
