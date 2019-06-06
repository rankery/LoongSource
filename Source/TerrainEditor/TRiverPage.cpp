// TRiverPage.cpp : 实现文件
//
#include "stdafx.h"
#include "TerrainEditor.h"
#include "TRiverPage.h"
#include ".\triverpage.h"
#include "PlugTerrainEditor.h"
#include "..\Cool3D\EditorSupport\TRiverEditor.h"

// TRiverPage 对话框

IMPLEMENT_DYNAMIC(TRiverPage, CPropertyPage)
TRiverPage::TRiverPage()
	: CPropertyPage(TRiverPage::IDD)
	, m_szHeight(_T("100"))
	, m_szShowDist(_T(""))
	, m_szEditDist(_T(""))
{
	//
}

TRiverPage::~TRiverPage()
{
}

void TRiverPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_Height, m_szHeight);
	DDX_Control(pDX, IDC_Mtl, m_mtlProp);
	DDX_Control(pDX, IDC_LIST1, m_riverList);
	DDX_Control(pDX, IDC_EditTools, m_editTools);
	DDX_Text(pDX, IDC_ShowDist, m_szShowDist);
}


BEGIN_MESSAGE_MAP(TRiverPage, CPropertyPage)
	ON_EN_CHANGE(IDC_Height, OnEnChangeHeight)
	ON_BN_CLICKED(IDC_ApplyMtl, OnBnClickedApplymtl)
	ON_BN_CLICKED(IDC_NewRiver, OnBnClickedNewriver)
	ON_BN_CLICKED(IDC_DelRiver, OnBnClickedDelriver)
	ON_LBN_SELCHANGE(IDC_LIST1, OnLbnSelchangeList1)
	ON_EN_CHANGE(IDC_ShowDist, &TRiverPage::OnEnChangeShowdist)
END_MESSAGE_MAP()


// TRiverPage 消息处理程序

BOOL TRiverPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  在此添加额外的初始化
	BuildRiverList();

	m_editTools.ResetContent();
	m_editTools.AddString(_T("Brush"));
	m_editTools.AddString(_T("Noise"));
	m_editTools.AddString(_T("Smooth"));
	m_editTools.SetCurSel(0);

	m_szShowDist.Format(_T("%d"),g_editor->GetEngine()->GetRiverEditor()->OP_Draw_Dist);
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL TRiverPage::GetSelRiverIndex( UINT& riverIndex )
{
	if(m_riverList.m_hWnd==NULL)
		return FALSE;

	int selIndex=m_riverList.GetCurSel();
	if(selIndex==CB_ERR)
		return FALSE;
	riverIndex=selIndex;
	return TRUE;
}

void TRiverPage::UpdateMtlDesc(tagRiverMtlDesc *pMtl,bool bSave)
{
	if(!bSave)
		m_mtlProp.ResetContent();
	int p=0;
	m_mtlProp.UpdateVal(_T("透明度"),pMtl->alpha,p++,bSave);
	m_mtlProp.UpdateVal(_T("镜面系数"),pMtl->power,p++,bSave);
	m_mtlProp.UpdateVal(_T("水面贴图名称"),pMtl->szWaterTex,sizeof(pMtl->szWaterTex),p++,bSave,true);
	m_mtlProp.UpdateVal(_T("水面贴图帧数"),pMtl->waterFrm,p++,bSave);
	m_mtlProp.UpdateVal(_T("水面贴图FPS"),pMtl->waterFps,p++,bSave);
	m_mtlProp.UpdateVal(_T("水面贴图uTilesize"),pMtl->uTileSize,p++,bSave);
	m_mtlProp.UpdateVal(_T("水面贴图vTilesize"),pMtl->vTileSize,p++,bSave);
	m_mtlProp.UpdateVal(_T("纹理扰动贴图"),pMtl->szBumpMap,sizeof(pMtl->szBumpMap),p++,bSave,true);
	//m_mtlProp.UpdateVal(_T("反射贴图帧数"),pMtl->bumpFrm,p++,bSave);
	m_mtlProp.UpdateVal(_T("扰动贴图滚动速度U"),pMtl->bumpUVScroll.x,p++,bSave);
	m_mtlProp.UpdateVal(_T("扰动贴图滚动速度V"),pMtl->bumpUVScroll.y,p++,bSave);
	m_mtlProp.UpdateVal(_T("反射图混合比例"),pMtl->blendColor.A,p++,bSave);
	m_mtlProp.UpdateVal(_T("反射图混合颜色"),pMtl->blendColor,p++,bSave);
	m_mtlProp.UpdateVal(_T("纹理扰动系数"),pMtl->bumpScale,p++,bSave);
	m_mtlProp.UpdateVal(_T("反射效果镜面系数"),pMtl->reflectPower,p++,bSave);
	m_mtlProp.UpdateVal(_T("反射效果高光缩放"),pMtl->reflectSpecularScale,p++,bSave);
	m_mtlProp.UpdateVal(_T("水下雾起始高度"),pMtl->underWaterFogStart,p++,bSave);
	m_mtlProp.UpdateVal(_T("水下雾结束高度"),pMtl->underWaterFogEnd,p++,bSave);
	m_mtlProp.UpdateVal(_T("水下雾颜色"),(Color4f&)pMtl->underWaterFogColor,p++,bSave);
	m_mtlProp.UpdateVal(_T("漫反射材质"),pMtl->diffuse,p++,bSave);
	m_mtlProp.UpdateVal(_T("环境光材质"),pMtl->ambient,p++,bSave);
	m_mtlProp.UpdateVal(_T("高光材质"),pMtl->specular,p++,bSave);

	m_mtlProp.UpdateVal(_T("开启反射、折射、高光效果"),pMtl->bEnableEffect,p++,bSave);
	m_mtlProp.UpdateVal(_T("关闭边缘过渡"),pMtl->bCloseEdgeFade,p++,bSave);
}

void TRiverPage::OnEnChangeHeight()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CPropertyPage::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UINT riverIndex;
	if( GetSelRiverIndex(riverIndex) )
	{
		TerrainEditor *pTE=g_editor->GetEngine();
		TRiverEditor *pRE=pTE->GetRiverEditor();
		if(pRE)
		{
			CString szHeight;
			GetDlgItem(IDC_Height)->GetWindowText(szHeight);
			int h=_tstoi(szHeight);
			pRE->ApplyHeight2River(riverIndex,h);
		}
	}
}

void TRiverPage::OnBnClickedApplymtl()
{
	// TODO: 在此添加控件通知处理程序代码

	UINT riverIndex;
	if( GetSelRiverIndex(riverIndex) )
	{
		TerrainEditor *pTE=g_editor->GetEngine();
		TRiverEditor *pRE=pTE->GetRiverEditor();
		if(pRE)
		{
			UpdateMtlDesc(pRE->GetRiverMtlDescByIndex(riverIndex),true);

			pRE->ApplyMtl2River(riverIndex);
		}
	}
}

void TRiverPage::OnBnClickedNewriver()
{
	// TODO: 在此添加控件通知处理程序代码
	CString szHeight;
	GetDlgItem(IDC_Height)->GetWindowText(szHeight);
	int h=_tstoi(szHeight);
	g_editor->GetEngine()->NewRiver(h);

	BuildRiverList();
}

void TRiverPage::OnBnClickedDelriver()
{
	// TODO: 在此添加控件通知处理程序代码
	UINT riverIndex;
	if( GetSelRiverIndex(riverIndex) )
	{
		g_editor->GetEngine()->DelRiver(riverIndex);
		BuildRiverList();
	}
}

void TRiverPage::BuildRiverList()
{
	m_riverList.ResetContent();
	for(int i=0;i<g_editor->GetEngine()->GetRiverNum();i++)
	{
		CString szTmp;
		szTmp.Format(_T("%02d"),i);
		m_riverList.AddString(szTmp);
	}
	m_riverList.SetCurSel(0);
}

void TRiverPage::OnLbnSelchangeList1()
{
	// TODO: 在此添加控件通知处理程序代码
	UINT riverIndex;
	if( GetSelRiverIndex(riverIndex) )
	{
		TerrainEditor *pTE=g_editor->GetEngine();
		TRiverEditor *pRE=pTE->GetRiverEditor();
		if(pRE)
		{
			m_szHeight.Format(_T("%.2d"),pRE->GetRiverHeightByIndex(riverIndex));
			UpdateData(FALSE);

			UpdateMtlDesc(pRE->GetRiverMtlDescByIndex(riverIndex),false);
		}
	}
}

BOOL TRiverPage::OnSetActive()
{
	// TODO: 在此添加专用代码和/或调用基类
	BuildRiverList();

	return CPropertyPage::OnSetActive();
}

void TRiverPage::OnEnChangeShowdist()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here

	UpdateData();
	int nShowDist=_tstoi(m_szShowDist);
	g_editor->GetEngine()->GetRiverEditor()->OP_Draw_Dist=nShowDist;
}
