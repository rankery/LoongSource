// TVisPage.cpp : 实现文件
//

#include "stdafx.h"
#include "TerrainEditor.h"
#include "TVisPage.h"
#include "PlugTerrainEditor.h"


// TVisPage 对话框

IMPLEMENT_DYNAMIC(TVisPage, CPropertyPage)
TVisPage::TVisPage()
	: CPropertyPage(TVisPage::IDD)
	, m_terrainPow(8)
{
}

TVisPage::~TVisPage()
{
}

void TVisPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_Diffuse, m_diffuse);
	DDX_Control(pDX, IDC_Ambient, m_ambient);
	DDX_Control(pDX, IDC_Specular, m_specular);
	DDX_Text(pDX, IDC_TerrainPow, m_terrainPow);
	DDV_MinMaxUInt(pDX, m_terrainPow, 1, 128);
}


BEGIN_MESSAGE_MAP(TVisPage, CPropertyPage)
	ON_BN_CLICKED(IDC_Diffuse, &TVisPage::OnBnClickedDiffuse)
	ON_BN_CLICKED(IDC_Ambient, &TVisPage::OnBnClickedAmbient)
	ON_BN_CLICKED(IDC_Specular, &TVisPage::OnBnClickedSpecular)
	ON_EN_CHANGE(IDC_TerrainPow, &TVisPage::OnEnChangeTerrainpow)
END_MESSAGE_MAP()


// TVisPage 消息处理程序

void TVisPage::OnBnClickedDiffuse()
{
	// TODO: Add your control notification handler code here
	CColorDialog dlg(m_diffuse.GetColor(),0,this);
	if(IDOK==dlg.DoModal())
	{
		COLORREF c=dlg.GetColor();
		m_diffuse.SetColor(c);
		Color4f diffuse(1,GetRValue(c)/255.0f,GetGValue(c)/255.0f,GetBValue(c)/255.0f);
		g_editor->GetEngine()->SetMtlDiffuse(diffuse);
	}
}

void TVisPage::OnBnClickedAmbient()
{
	// TODO: Add your control notification handler code here
	CColorDialog dlg(m_ambient.GetColor(),0,this);
	if(IDOK==dlg.DoModal())
	{
		COLORREF c=dlg.GetColor();
		m_ambient.SetColor(c);
		Color4f ambient(1,GetRValue(c)/255.0f,GetGValue(c)/255.0f,GetBValue(c)/255.0f);
		g_editor->GetEngine()->SetMtlAmbient(ambient);
	}
}

void TVisPage::OnBnClickedSpecular()
{
	// TODO: Add your control notification handler code here
	CColorDialog dlg(m_specular.GetColor(),0,this);
	if(IDOK==dlg.DoModal())
	{
		COLORREF c=dlg.GetColor();
		m_specular.SetColor(c);
		Color4f specular(1,GetRValue(c)/255.0f,GetGValue(c)/255.0f,GetBValue(c)/255.0f);
		g_editor->GetEngine()->SetMtlSpecular(specular);
	}
}

BOOL TVisPage::OnInitDialog()
{
	CDialog::OnInitDialog();

	COLORREF initColor;
	Color4f color=g_editor->GetEngine()->GetMtlDiffuse();
	initColor=RGB(color.R*255.0f,color.G*255.0f,color.B*255.0f);
	m_diffuse.SetColor(initColor);
	color=g_editor->GetEngine()->GetMtlAmbient();
	initColor=RGB(color.R*255.0f,color.G*255.0f,color.B*255.0f);
	m_ambient.SetColor(initColor);
	color=g_editor->GetEngine()->GetMtlSpecular();
	initColor=RGB(color.R*255.0f,color.G*255.0f,color.B*255.0f);
	m_specular.SetColor(initColor);

	m_terrainPow=(UINT)g_editor->GetEngine()->GetMtlPower();

	UpdateData(FALSE);

	return TRUE;
}
void TVisPage::OnEnChangeTerrainpow()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here

	UpdateData();
	g_editor->GetEngine()->SetMtlPower((float)m_terrainPow);
}
