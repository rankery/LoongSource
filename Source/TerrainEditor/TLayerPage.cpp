// TLayerPage.cpp : 实现文件
//

#include "stdafx.h"
#include "TerrainEditor.h"
#include "TLayerPage.h"
#include ".\tlayerpage.h"
#include "LayerAttDlg.h"
#include "PlugTerrainEditor.h"


// TLayerPage 对话框

IMPLEMENT_DYNAMIC(TLayerPage, CPropertyPage)
TLayerPage::TLayerPage()
	: CPropertyPage(TLayerPage::IDD)
{
}

TLayerPage::~TLayerPage()
{
}

void TLayerPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LayerList, m_layerList);
	DDX_Control(pDX, IDC_EditTools, m_editTools);
	DDX_Control(pDX, IDC_ShowLayerCount, m_btnShowLayerCount);
	DDX_Control(pDX, IDC_ShowSubPatchEdge, m_btnShowSubPatchEdge);
}


BEGIN_MESSAGE_MAP(TLayerPage, CPropertyPage)
	ON_BN_CLICKED(IDC_AddLayer, OnBnClickedAddlayer)
	ON_NOTIFY(NM_CLICK, IDC_LayerList, OnNMClickLayerlist)
	ON_BN_CLICKED(IDC_DelLayer, OnBnClickedDellayer)
	ON_BN_CLICKED(IDC_LayerUp, OnBnClickedLayerup)
	ON_BN_CLICKED(IDC_LayerDown, OnBnClickedLayerdown)
	ON_BN_CLICKED(IDC_Clear, OnBnClickedClear)
	ON_NOTIFY(NM_DBLCLK, IDC_LayerList, OnNMDblclkLayerlist)
	ON_BN_CLICKED(IDC_CheckLayer, OnBnClickedChecklayer)
END_MESSAGE_MAP()


// TLayerPage 消息处理程序
const UINT IMG_SIZE=128;
BOOL TLayerPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  在此添加额外的初始化
	int i=-1;
	i=m_layerList.InsertColumn(0,_T("名称"),LVCFMT_LEFT,64,0);
	i=m_layerList.InsertColumn(1,_T("贴图文件"),LVCFMT_LEFT,164,1);
	i=m_layerList.InsertColumn(2,_T("uv序号"),LVCFMT_LEFT,48,2);
	i=m_layerList.InsertColumn(3,_T("地表类型"),LVCFMT_CENTER,64,3);
	
	m_layerList.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	RebuildList();

	CRect rc;
	GetClientRect(rc);
	rc.left=rc.right-IMG_SIZE;
	rc.bottom=rc.top+IMG_SIZE;
	m_bmpDraw.Create(_T("bmpDraw"),
		WS_BORDER|WS_CHILD|WS_VISIBLE|SS_BITMAP|SS_CENTERIMAGE, rc, this);

	m_editTools.ResetContent();
	m_editTools.AddString(_T("Brush"));
	m_editTools.AddString(_T("Noise"));
	m_editTools.AddString(_T("Smooth"));
	m_editTools.SetCurSel(0);

	m_btnShowLayerCount.SetCheck(0);
	m_btnShowSubPatchEdge.SetCheck(0);
   
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void TLayerPage::RebuildList()
{
	m_layerList.DeleteAllItems();
	TerrainEditor *pEd=g_editor->GetEngine();

	int list=0;
	for(UINT i=0;i<TerrainEditor::MAX_MAP_LAYER;i++)
	{
		const tagTMapLayer *pLayer=pEd->GetMapLayer(i);
		if(pLayer!=NULL
			&& pLayer->szName[0]!=0 
			&& pLayer->szTexFile[0]!=0)
		{
			m_layerList.InsertItem(list,pLayer->szName);
			m_layerList.SetItemText(list,1,pLayer->szTexFile);
			CString szUVIndex;
			szUVIndex.Format(_T("%d"),pLayer->uvSetIndex);
			m_layerList.SetItemText(list,2,szUVIndex);
			CString szSurfaceType;
			switch (pLayer->surfaceType)
			{
			case SURFACETYPE_None:
				szSurfaceType=_T("无");
				break;
			case SURFACETYPE_Snow:
				szSurfaceType=_T("雪地");
				break;
			case SURFACETYPE_Block:
				szSurfaceType=_T("石块");
				break;
			case SURFACETYPE_Earth:
				szSurfaceType=_T("土");
				break;
			case SURFACETYPE_Grass:
				szSurfaceType=_T("草地");
				break;
			case SURFACETYPE_Sand:
				szSurfaceType=_T("沙地");
				break;
			case SURFACETYPE_Ice:
				szSurfaceType=_T("冰面");
				break;
			case SURFACETYPE_Metal:
				szSurfaceType=_T("金属");
				break;
			case SURFACETYPE_WOOD:
				szSurfaceType=_T("木头");
				break;
			}
			m_layerList.SetItemText(list,3,szSurfaceType);
			list++;
		}
		else
		{
			m_layerList.InsertItem(list,_T("None"));
		}
	}
}

void TLayerPage::OnBnClickedAddlayer()
{
	// TODO: 在此添加控件通知处理程序代码
	BeginMFCRes();
	CLayerAttDlg dlg;
	if(IDOK==dlg.DoModal())
	{
		TerrainEditor *pEd=g_editor->GetEngine();
		int uvSetIndex=_tstoi(dlg.m_szUVSetIndex);
		bool ok=pEd->AddLayer(dlg.m_szName,dlg.m_szTex,uvSetIndex,dlg.m_typeIndex);
		if(!ok)
		{
			AfxMessageBox(Kernel::Inst()->GetLastError(),MB_OK);
		}
		else
			RebuildList();
	}
	EndMFCRes();
}

BOOL TLayerPage::OnSetActive()
{
	// TODO: 在此添加专用代码和/或调用基类
	RebuildList();

	return CPropertyPage::OnSetActive();
}

CString TLayerPage::GetSelLayerName()
{
	CString ret;
	POSITION pos = m_layerList.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		int nItem = m_layerList.GetNextSelectedItem(pos);
		ret=m_layerList.GetItemText(nItem,0);
	}

	if(ret==_T("None"))
		ret.Empty();
	
	return ret;
}

void TLayerPage::OnNMClickLayerlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	POSITION pos = m_layerList.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		int nItem = m_layerList.GetNextSelectedItem(pos);
		CString szFile=m_layerList.GetItemText(nItem,1);
		if(!szFile.IsEmpty())
		{
			m_tileImg.Destroy();
			m_tileImg.LoadFromFile(szFile);
			m_tileImg.Scale(IMG_SIZE,IMG_SIZE,EIS_Fastest);
			
			m_bmpDraw.SetBitmap(m_tileImg.GetHBmp());
		}
	}

	*pResult = 0;
}

void TLayerPage::OnBnClickedDellayer()
{
	// TODO: 在此添加控件通知处理程序代码
	CString selLayerName=GetSelLayerName();
	if(selLayerName.IsEmpty())//没有选中
		return;

	//--提示,确认
	CString msg;
	msg.Format(_T("确定要删除[%s]图层吗?"),selLayerName);
	if(IDYES!=::AfxMessageBox(msg,MB_YESNO|MB_ICONWARNING))
		return;

	//--
	bool del=g_editor->GetEngine()->RemoveLayer(selLayerName);
	if(!del)
	{
		CString error;
		error.Format(_T("删除[%s]图层失败."),selLayerName);
		::AfxMessageBox(error);
	}
	else
		RebuildList();
}

void TLayerPage::OnBnClickedLayerup()
{
	// TODO: 在此添加控件通知处理程序代码
	CString selLayerName=GetSelLayerName();
	if(selLayerName.IsEmpty())//没有选中
		return;
	g_editor->GetEngine()->MoveLayer(selLayerName,-1);
	RebuildList();
}

void TLayerPage::OnBnClickedLayerdown()
{
	// TODO: 在此添加控件通知处理程序代码
	CString selLayerName=GetSelLayerName();
	if(selLayerName.IsEmpty())//没有选中
		return;
	g_editor->GetEngine()->MoveLayer(selLayerName,1);
	RebuildList();
}

void TLayerPage::OnBnClickedClear()
{
	// TODO: 在此添加控件通知处理程序代码
	if(IDYES!=::AfxMessageBox(_T("要删除所有的图层吗?"),MB_YESNO|MB_ICONWARNING))
		return;
	g_editor->GetEngine()->ClearLayers();
	RebuildList();
}

void TLayerPage::OnNMDblclkLayerlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	CString szSelLayer=GetSelLayerName();
	if(szSelLayer.IsEmpty())
		return;

	tagTMapLayer *pLayer=g_editor->GetEngine()->FindMapLayer(szSelLayer);
	if(pLayer)
	{
		BeginMFCRes();
		CLayerAttDlg dlg;
		dlg.m_bMod=true;
		dlg.m_szName=pLayer->szName;
		dlg.m_szTex=pLayer->szTexFile;
		dlg.m_szUVSetIndex.Format(_T("%d"),pLayer->uvSetIndex);
		dlg.m_typeIndex=pLayer->surfaceType;
		if(IDOK==dlg.DoModal())
		{
			int uvSetIndex=_tstoi(dlg.m_szUVSetIndex);
			pLayer->uvSetIndex=uvSetIndex;
			pLayer->surfaceType=dlg.m_typeIndex;
			RebuildList();
		}
		EndMFCRes();
	}
}

void TLayerPage::OnBnClickedChecklayer()
{
}
