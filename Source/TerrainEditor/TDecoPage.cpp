// TDecoPage.cpp : 实现文件
//

#include "stdafx.h"
#include "TerrainEditor.h"
#include "TDecoPage.h"
#include ".\tdecopage.h"
#include "PlugTerrainEditor.h"


// TDecoPage 对话框

IMPLEMENT_DYNAMIC(TDecoPage, CPropertyPage)
TDecoPage::TDecoPage()
	: CPropertyPage(TDecoPage::IDD)
	, m_szShowSize(_T(""))
{
}

TDecoPage::~TDecoPage()
{
}

void TDecoPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LayerList, m_layerList);
	DDX_Control(pDX, IDC_LayerProp, m_layerProp);
	DDX_Text(pDX, IDC_ShowSize, m_szShowSize);
}


BEGIN_MESSAGE_MAP(TDecoPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BtnAdd, OnBnClickedBtnadd)
	ON_BN_CLICKED(IDC_BtnRemove, OnBnClickedBtnremove)
	ON_LBN_SELCHANGE(IDC_LayerList, OnLbnSelchangeLayerlist)
	ON_BN_CLICKED(IDC_BtnApply, OnBnClickedBtnapply)
	ON_BN_CLICKED(IDC_BtnImportMap, OnBnClickedBtnimportmap)
	ON_EN_CHANGE(IDC_ShowSize, OnEnChangeShowsize)
	ON_BN_CLICKED(IDC_RefreshView, OnBnClickedRefreshview)
END_MESSAGE_MAP()


// TDecoPage 消息处理程序

void TDecoPage::OnBnClickedBtnadd()
{
	// TODO: 在此添加控件通知处理程序代码
	TCHAR szName[256];
	_stprintf(szName,_T("%d"),GetTickCount());
	g_editor->GetEngine()->AddDecoLayer(szName);

	BuildLayerList();
}

void TDecoPage::OnBnClickedBtnremove()
{
	// TODO: 在此添加控件通知处理程序代码
	CString szName=GetSelLayerName();
	if(szName.GetLength() > 0)
	{
		g_editor->GetEngine()->RemoveDecoLayer(szName);
		BuildLayerList();
	}
}

CString TDecoPage::GetSelLayerName()
{
	int sel=m_layerList.GetCurSel();
	if(sel==-1)
		return _T("");
	else
	{
		CString ret;
		m_layerList.GetText(sel,ret);
		return ret;
	}
}

void TDecoPage::BuildLayerList()
{
	int sel=m_layerList.GetCurSel();
	m_layerList.ResetContent();
	for(int i=0;i<TerrainEditor::MAX_DECO_LAYER;i++)
	{
		tagTDecoLayer *pLayer=g_editor->GetEngine()->GetDecoLayer(i);
		if(pLayer!=NULL &&
			pLayer->szName[0]!=0)
		{
			m_layerList.AddString(pLayer->szName);
		}
	}
	if(sel!=-1)
		m_layerList.SetCurSel(sel);
}

void TDecoPage::UpdateLayerProp(tagTDecoLayer *pLayer,bool bSave)
{
	if(pLayer==NULL)
		return;

	if(!bSave)//如果不是存数据,则重建list
		m_layerProp.ResetContent();

	int p=0;
	m_layerProp.UpdateVal(_T("名称"),pLayer->szName,sizeof(pLayer->szName),p++,bSave,false);
	m_layerProp.UpdateVal(_T("消隐系数"),pLayer->fadeOut,p++,bSave);

	m_layerProp.UpdateVal(_T("类型"),pLayer->type,p++,bSave);
	m_layerProp.UpdateVal(_T("文件名"),pLayer->szFileName,sizeof(pLayer->szFileName),p++,bSave,true);
	m_layerProp.UpdateVal(_T("x大小"),pLayer->size.x,p++,bSave);
	m_layerProp.UpdateVal(_T("y大小"),pLayer->size.y,p++,bSave);
	m_layerProp.UpdateVal(_T("z大小"),pLayer->size.z,p++,bSave);
	m_layerProp.UpdateVal(_T("大小随机系数"),pLayer->sizeRandRate,p++,bSave);
	m_layerProp.UpdateVal(_T("Alpha mask"),pLayer->alphaRef,p++,bSave);
	m_layerProp.UpdateVal(_T("x旋转随机"),pLayer->rotateRand.x,p++,bSave);
	m_layerProp.UpdateVal(_T("y旋转随机"),pLayer->rotateRand.y,p++,bSave);
	m_layerProp.UpdateVal(_T("z旋转随机"),pLayer->rotateRand.z,p++,bSave);
	m_layerProp.UpdateVal(_T("振幅"),pLayer->wave,p++,bSave);
	m_layerProp.UpdateVal(_T("波速"),pLayer->speed,p++,bSave);

	m_layerProp.UpdateVal(_T("材质"),pLayer->diffuse,p++,bSave);
}

BOOL TDecoPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  在此添加额外的初始化
	BuildLayerList();

	m_szShowSize.Format(_T("%d"),TerrainEditor::OP_Deco_Draw_Size);
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void TDecoPage::OnLbnSelchangeLayerlist()
{
	// TODO: 在此添加控件通知处理程序代码
	CString szSelName=GetSelLayerName();
	if(szSelName.GetLength() > 0)
	{
		tagTDecoLayer *pLayer=g_editor->GetEngine()->GetDecoLayer(szSelName);
		if(pLayer)
			UpdateLayerProp(pLayer,false);
		else
			AfxMessageBox(_T("Layer not found!"));
	}
}

void TDecoPage::OnBnClickedBtnapply()
{
	// TODO: 在此添加控件通知处理程序代码
	CString szSelName=GetSelLayerName();
	if(szSelName.GetLength() > 0)
	{
		tagTDecoLayer *pLayer=g_editor->GetEngine()->GetDecoLayer(szSelName);
		if(pLayer)
		{
			UpdateLayerProp(pLayer,true);
			BuildLayerList();//name可以被修改
			g_editor->GetEngine()->PostEditChangeDecoLayer(szSelName,true);
		}
		else
			AfxMessageBox(_T("Layer not found!"));
	}
}

void TDecoPage::OnBnClickedBtnimportmap()
{
	// TODO: 在此添加控件通知处理程序代码
	CString szSelName=GetSelLayerName();
	if(szSelName.GetLength() <= 0)
	{
		AfxMessageBox(_T("请先选中一个层!"));
		return;
	}

	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("BMP文件|*.bmp|TGA文件|*.tga||"),
		this,0);
	if(IDOK==dlg.DoModal())
	{
		CString szImgPath=dlg.GetPathName();
		g_editor->GetEngine()->ImportDensityMap(szSelName,szImgPath,
			g_editor->GetCamera());
	}
}

bool TDecoPage::IfEnableNoise()
{
	if(IsDlgButtonChecked(IDC_EnableNoise)>0)
		return true;
	else
		return false;
}
void TDecoPage::OnEnChangeShowsize()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CPropertyPage::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData();
	TerrainEditor::OP_Deco_Draw_Size=_tstoi(m_szShowSize);
}

void TDecoPage::OnBnClickedRefreshview()
{
	// TODO: 在此添加控件通知处理程序代码
	g_editor->GetEngine()->RefreshDecoView(g_editor->GetCamera());
}

BOOL TDecoPage::OnSetActive()
{
	// TODO: 在此添加专用代码和/或调用基类
	BuildLayerList();

	return CPropertyPage::OnSetActive();
}
