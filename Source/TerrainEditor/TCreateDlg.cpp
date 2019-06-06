// TCreateDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TerrainEditor.h"
#include "TCreateDlg.h"
#include ".\tcreatedlg.h"
#include "PlugTerrainEditor.h"


// TCreateDlg 对话框

IMPLEMENT_DYNAMIC(TCreateDlg, CDialog)
TCreateDlg::TCreateDlg(const TCHAR* szTerrainName,CWnd* pParent /*=NULL*/)
	: CDialog(TCreateDlg::IDD, pParent)
	, m_szWidth(_T("256"))
	, m_szHeight(_T("256"))
	, m_szXScale(_T("50"))
	, m_szZScale(_T("50"))
	, m_szYScale(_T("1.0"))
	, m_szInitVal(_T("40"))
	, m_bNoise(TRUE)
	, m_szUScale(_T("8.0"))
	, m_szVScale(_T("8.0"))
{
	m_szTerrainName=szTerrainName;
	m_bMod=false;
}

TCreateDlg::~TCreateDlg()
{
}

void TCreateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_Width, m_szWidth);
	DDX_Text(pDX, IDC_Height, m_szHeight);
	DDX_Text(pDX, IDC_XScale, m_szXScale);
	DDX_Text(pDX, IDC_ZScale, m_szZScale);
	DDX_Text(pDX, IDC_HScale, m_szYScale);
	DDX_Text(pDX, IDC_InitVal, m_szInitVal);
	DDX_Check(pDX, IDC_Noise, m_bNoise);
	DDX_Text(pDX, IDC_uScale, m_szUScale);
	DDX_Text(pDX, IDC_vScale, m_szVScale);
}


BEGIN_MESSAGE_MAP(TCreateDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// TCreateDlg 消息处理程序

void TCreateDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	TerrainEditor* pEngine=g_editor->GetEngine();
	int w=_tstoi(m_szWidth);
	int h=_tstoi(m_szHeight);
	float xScale=(float)_tstof(m_szXScale);
	float zScale=(float)_tstof(m_szZScale);
	float yScale=(float)_tstof(m_szYScale);
	Vector2 uvScale=Vector2((float)_tstof(m_szUScale),(float)_tstof(m_szVScale));

	if(m_bMod)
	{
		pEngine->ModAtt(xScale,zScale,yScale);
	}
	else
	{
		pEngine->SetName(m_szTerrainName);
		if(m_bNoise)
		{
			float freq=4.0f;
			g_editor->GetBushDlg()->GetNoiseSet(freq);
			pEngine->CreateByNoise(freq,w,h,xScale,zScale,yScale,uvScale);
		}
		else
		{
			int v=_tstoi(m_szInitVal);
			if(v<0)
				v=0;
			if(v>255)
				v=255;

			pEngine->Create(v,w,h,xScale,zScale,yScale,uvScale);
		}
	}//endof else

	OnOK();
}

BOOL TCreateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	if(m_bMod)
	{
		GetDlgItem(IDC_Width)->EnableWindow(FALSE);
		GetDlgItem(IDC_Height)->EnableWindow(FALSE);
		GetDlgItem(IDC_InitVal)->EnableWindow(FALSE);
		GetDlgItem(IDC_Noise)->EnableWindow(FALSE);
		GetDlgItem(IDC_uScale)->EnableWindow(FALSE);
		GetDlgItem(IDC_vScale)->EnableWindow(FALSE);

		//--
		TerrainEditor* pEngine=g_editor->GetEngine();
		if(pEngine->IsHeightmapCreated())
		{
			m_szWidth.Format(_T("%d"),pEngine->GetHMapWidth());
			m_szHeight.Format(_T("%d"),pEngine->GetHMapHeight());
			m_szXScale.Format(_T("%.2f"),pEngine->GetHMapXScale());
			m_szZScale.Format(_T("%.2f"),pEngine->GetHMapZScale());
			m_szYScale.Format(_T("%.2f"),pEngine->GetHMapYScale());
			m_szInitVal=_T("--");
			m_szUScale.Format(_T("%.2f"),pEngine->GetLayerUVScale(0).x);
			m_szVScale.Format(_T("%.2f"),pEngine->GetLayerUVScale(0).y);

			UpdateData(FALSE);
		}


	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
