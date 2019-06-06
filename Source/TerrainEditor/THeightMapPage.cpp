// THeightMapPage.cpp : 实现文件
//

#include "stdafx.h"
#include "Resource.h"
#include "TerrainEditor.h"
#include "THeightMapPage.h"
#include ".\theightmappage.h"
#include "PlugTerrainEditor.h"
#include "TImportDlg.h"
#include "TCreateDlg.h"
#include "UVSetDlg.h"


// THeightMapPage 对话框

IMPLEMENT_DYNAMIC(THeightMapPage, CPropertyPage)
THeightMapPage::THeightMapPage()
	: CPropertyPage(THeightMapPage::IDD)
	, m_szEditToolInfo(_T(""))
	, m_bWireFrame(FALSE)
	, m_szName(_T("default"))
	, m_szBuildPath(_T(".\\Terrain\\"))
	, m_szShowDist(_T(""))
	, m_szEditDist(_T(""))
{
}

THeightMapPage::~THeightMapPage()
{
}

void THeightMapPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EditTools, m_editTools);
	DDX_Text(pDX, IDC_Info, m_szEditToolInfo);
	DDX_Check(pDX, IDC_Wireframe, m_bWireFrame);
	DDX_Text(pDX, IDC_Name, m_szName);
	DDX_Text(pDX, IDC_BuildPath, m_szBuildPath);
	DDX_Control(pDX, IDC_Build, m_btnBuild);
	DDX_Control(pDX, IDC_ShowPatchEdge, m_btnShowEdge);
	DDX_Text(pDX, IDC_ShowDist, m_szShowDist);
	DDX_Text(pDX, IDC_EditDist, m_szEditDist);
}


BEGIN_MESSAGE_MAP(THeightMapPage, CPropertyPage)
	ON_BN_CLICKED(IDC_ImportHightmap, OnBnClickedImporthightmap)
	ON_BN_CLICKED(IDC_Create, OnBnClickedCreate)
	ON_LBN_SELCHANGE(IDC_EditTools, OnLbnSelchangeEdittools)
	ON_BN_CLICKED(IDC_Wireframe, OnBnClickedWireframe)
	ON_BN_CLICKED(IDC_Save, OnBnClickedSave)
	ON_BN_CLICKED(IDC_Load, OnBnClickedLoad)
	ON_BN_CLICKED(IDC_Build, OnBnClickedBuild)
	ON_EN_CHANGE(IDC_ShowDist, OnEnChangeShowdist)
	ON_EN_CHANGE(IDC_EditDist, OnEnChangeEditdist)
	ON_BN_CLICKED(IDC_Mod, OnBnClickedMod)
	ON_BN_CLICKED(IDC_EditUVSet, OnBnClickedEdituvset)
	ON_BN_CLICKED(IDC_ExportHightmap, OnBnClickedExporthightmap)
	ON_BN_CLICKED(IDC_Undo, OnBnClickedUndo)
	ON_BN_CLICKED(IDC_Redo, OnBnClickedRedo)
END_MESSAGE_MAP()


// THeightMapPage 消息处理程序

void THeightMapPage::OnBnClickedImporthightmap()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	BeginMFCRes();
	TImportDlg dlg(m_szName);
	dlg.DoModal();
	EndMFCRes();
}

void THeightMapPage::OnBnClickedCreate()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	BeginMFCRes();
	TCreateDlg dlg(m_szName);
	dlg.DoModal();
	EndMFCRes();
}

BOOL THeightMapPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_editTools.ResetContent();
	for(UINT i=0;i<HEditToolsNum;i++)
	{
		m_editTools.AddString(HEditToolsName[i]);
	}
	m_editTools.SetCurSel(0);

	bool t;
	bool bw=false;
	g_editor->GetEngine()->GetDrawOption(bw,t);
	m_bWireFrame=bw;

	m_szShowDist.Format(_T("%d"),g_editor->GetEngine()->OP_Draw_Dist);
	m_szEditDist.Format(_T("%d"),g_editor->GetEngine()->OP_Edit_Dist);
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void THeightMapPage::OnLbnSelchangeEdittools()
{
	// TODO: 在此添加控件通知处理程序代码
	int sel=m_editTools.GetCurSel();
	if(sel!=-1)
	{
		int mysel=sel;
		ASSERT(mysel>=0 && mysel<HEditToolsNum);
		UpdateData(TRUE);
		m_szEditToolInfo=HEditToolsInfo[mysel];
		UpdateData(FALSE);
	}
}

void THeightMapPage::SetDrawOption()
{
	UpdateData();
	g_editor->GetEngine()->SetDrawOption(m_bWireFrame==TRUE,true);
}

void THeightMapPage::OnBnClickedWireframe()
{
	// TODO: 在此添加控件通知处理程序代码
	SetDrawOption();
}

void THeightMapPage::OnBnClickedSave()
{
	// TODO: 在此添加控件通知处理程序代码
	if(!g_editor->GetEngine()->SaveInfo())
	{
		AfxMessageBox(Cool3D::Kernel::Inst()->GetLastError());
	}
	else
	{
		AfxMessageBox(_T("存盘成功"));
	}
}

void THeightMapPage::OnBnClickedLoad()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	g_editor->GetEngine()->SetName(m_szName);
	if(!g_editor->GetEngine()->LoadInfo())
	{
		AfxMessageBox(Cool3D::Kernel::Inst()->GetLastError());
	}
}

void THeightMapPage::OnBnClickedBuild()
{
	// TODO: 在此添加控件通知处理程序代码	
}

void THeightMapPage::OnEnChangeShowdist()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CPropertyPage::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码

	UpdateData();
	int nShowDist=_tstoi(m_szShowDist);
	g_editor->GetEngine()->OP_Draw_Dist=nShowDist;
}

void THeightMapPage::OnEnChangeEditdist()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CPropertyPage::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码

	UpdateData();
	int nEditDist=_tstoi(m_szEditDist);
	g_editor->GetEngine()->OP_Edit_Dist=nEditDist;
}

void THeightMapPage::OnBnClickedMod()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	BeginMFCRes();
	TCreateDlg dlg(m_szName);
	dlg.m_bMod=true;
	dlg.DoModal();
	EndMFCRes();
}

void THeightMapPage::OnBnClickedEdituvset()
{
	// TODO: 在此添加控件通知处理程序代码
	BeginMFCRes();
	CUVSetDlg dlg;
	dlg.DoModal();
	EndMFCRes();
}

void THeightMapPage::OnBnClickedExporthightmap()
{
	// TODO: 在此添加控件通知处理程序代码

	CFileDialog dlg(FALSE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	_T("BMP文件|*.bmp||"),
	this,0);
	if(IDOK==dlg.DoModal())
	{
		CString szPath = dlg.GetPathName();

		if(g_editor->GetEngine()->ExportHeightMap(szPath))
			AfxMessageBox(_T("导出成功！"));
		else
			AfxMessageBox(_T("导出失败！"));
	}
}

void THeightMapPage::OnBnClickedUndo()
{
	// TODO: 在此添加控件通知处理程序代码
	g_editor->GetEngine()->UndoBrush();
}

void THeightMapPage::OnBnClickedRedo()
{
	// TODO: 在此添加控件通知处理程序代码
	g_editor->GetEngine()->RedoBrush();
}
