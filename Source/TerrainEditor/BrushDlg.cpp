// BrushDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "TerrainEditor.h"
#include "BrushDlg.h"
#include ".\brushdlg.h"
#include "PlugTerrainEditor.h"

// CBrushDlg 对话框

IMPLEMENT_DYNAMIC(CBrushDlg, CDialog)
CBrushDlg::CBrushDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBrushDlg::IDD, pParent)
	, m_szInner(_T("3"))
	, m_szOuter(_T("10"))
	, m_szStr(_T("16"))
	, m_bLock(FALSE)
	, m_szFreq(_T("4.0"))
	, m_lerpType(0)
{
}

CBrushDlg::~CBrushDlg()
{
}

void CBrushDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_Inner, m_szInner);
	DDX_Text(pDX, IDC_Outer, m_szOuter);
	DDX_Text(pDX, IDC_Str, m_szStr);
	DDX_Check(pDX, IDC_LOCK, m_bLock);
	DDX_Text(pDX, IDC_Freq, m_szFreq);
	DDX_CBIndex(pDX, IDC_LerpType, m_lerpType);
	DDX_Control(pDX, IDC_LerpType, m_hLerpType);
	DDX_Control(pDX, IDC_ShapeType, m_hShapeType);
}


BEGIN_MESSAGE_MAP(CBrushDlg, CDialog)

ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_ReNoise, OnBnClickedRenoise)
END_MESSAGE_MAP()


// CBrushDlg 消息处理程序

void CBrushDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}

void CBrushDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

BOOL CBrushDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_hLerpType.AddString(_T("无"));
	m_hLerpType.AddString(_T("线性"));
	m_hLerpType.AddString(_T("余弦"));
	m_hLerpType.AddString(_T("两次"));
	m_lerpType=2;
	UpdateData(FALSE);

	m_hShapeType.AddString(_T("圆形"));
	m_hShapeType.AddString(_T("方形"));
	m_hShapeType.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CBrushDlg::GetNoiseSet(float& freq)
{
	UpdateData();
	freq=(float)_tstof(m_szFreq);
}

int CBrushDlg::GetLerpType()
{
	UpdateData();
	return m_lerpType;
}

void CBrushDlg::GetSetting(int& inner,int& outer,int *pStr)
{
	if(!::IsWindow(GetSafeHwnd()))
		return;
	UpdateData();
	inner=_tstoi(m_szInner);
	outer=_tstoi(m_szOuter);
	if(pStr)
		*pStr=_tstoi(m_szStr);
}

void CBrushDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialog::OnCancel();
}

void CBrushDlg::OnBnClickedRenoise()
{
	// TODO: 在此添加控件通知处理程序代码
	g_editor->GetEngine()->ResetNoise();
}

int CBrushDlg::GetShape()
{
	return m_hShapeType.GetCurSel();
}

tagBrushOpt CBrushDlg::GetBrushOpt() const
{
	tagBrushOpt brushOpt;
	return brushOpt;
}

void SetBrushOpt(const tagBrushOpt& brushOpt)
{
	//
}