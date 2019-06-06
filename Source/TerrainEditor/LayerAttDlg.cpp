// LayerAttDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TerrainEditor.h"
#include "LayerAttDlg.h"
#include ".\layerattdlg.h"
#include "PlugTerrainEditor.h"


// CLayerAttDlg 对话框

IMPLEMENT_DYNAMIC(CLayerAttDlg, CDialog)
CLayerAttDlg::CLayerAttDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLayerAttDlg::IDD, pParent)
	, m_szName(_T(""))
	, m_szTex(_T(""))
	, m_szUVSetIndex(_T("0"))
	, m_typeIndex(-1)
{
	m_bMod=false;
}

CLayerAttDlg::~CLayerAttDlg()
{
}

void CLayerAttDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_Name, m_szName);
	DDX_Text(pDX, IDC_Tex, m_szTex);
	DDX_Text(pDX, IDC_UVSetIndex, m_szUVSetIndex);
	DDX_Control(pDX, IDC_SURFACETYPE, m_typeList);
}


BEGIN_MESSAGE_MAP(CLayerAttDlg, CDialog)
	ON_BN_CLICKED(IDC_FileDlg, OnBnClickedFiledlg)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CLayerAttDlg 消息处理程序

void CLayerAttDlg::OnBnClickedFiledlg()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	_T("BMP文件,TGA文件,DDS文件|*.bmp;*.tga;*.dds||"),
	this,0);
	if(IDOK==dlg.DoModal())
	{
		UpdateData(TRUE);
		m_szTex=dlg.GetPathName();
		CString szPath=Kernel::Inst()->GetWorkPath();
		szPath.MakeLower();
		m_szTex.MakeLower();
		m_szTex.Replace(szPath,_T(""));
		
		TCHAR szName[256]={0};
		_tsplitpath(m_szTex,NULL,NULL,szName,NULL);
		m_szName=szName;

		UpdateData(FALSE);
	}
}

void CLayerAttDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	int uvSetIndex=_tstoi(m_szUVSetIndex);
	if(uvSetIndex>=TRN_MAX_UV_SET)
	{
		AfxMessageBox(_T("UV序号越界"));
		return;
	}

	m_typeIndex=m_typeList.GetCurSel();
	OnOK();
}

BOOL CLayerAttDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_typeList.InsertString(SURFACETYPE_None,_T("无"));
	m_typeList.InsertString(SURFACETYPE_Snow,_T("雪地"));
	m_typeList.InsertString(SURFACETYPE_Block,_T("石块"));
	m_typeList.InsertString(SURFACETYPE_Earth,_T("土"));
	m_typeList.InsertString(SURFACETYPE_Grass,_T("草地"));
	m_typeList.InsertString(SURFACETYPE_Sand,_T("沙地"));
	m_typeList.InsertString(SURFACETYPE_Ice,_T("冰面"));
	m_typeList.InsertString(SURFACETYPE_Metal,_T("金属"));
	m_typeList.InsertString(SURFACETYPE_WOOD,_T("木头"));

	if(m_bMod)
	{
		GetDlgItem(IDC_Name)->EnableWindow(FALSE);
		GetDlgItem(IDC_Tex)->EnableWindow(FALSE);
		m_typeList.SetCurSel(m_typeIndex);

		UpdateData(FALSE);
	}
	else
	{
		m_typeList.SetCurSel(SURFACETYPE_Earth);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
