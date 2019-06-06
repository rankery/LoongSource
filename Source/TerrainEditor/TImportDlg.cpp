// TImportDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TerrainEditor.h"
#include "TImportDlg.h"
#include ".\timportdlg.h"
#include "PlugTerrainEditor.h"

#include "..\Cool3D\Cool3DEngine.h"
using namespace Cool3D;

// TImportDlg 对话框

IMPLEMENT_DYNAMIC(TImportDlg, CDialog)
TImportDlg::TImportDlg(const TCHAR* szTerrainName,CWnd* pParent /*=NULL*/)
	: CDialog(TImportDlg::IDD, pParent)
	, m_szFile(_T(""))
	, m_szXScale(_T("50.0"))
	, m_szZScale(_T("50.0"))
	, m_szYScale(_T("32.0"))
	, m_szUScale(_T("8.0"))
	, m_szVScale(_T("8.0"))
{
	m_szTerrainName=szTerrainName;
}

TImportDlg::~TImportDlg()
{
}

void TImportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_File, m_szFile);
	DDX_Text(pDX, IDC_XScale, m_szXScale);
	DDX_Text(pDX, IDC_ZScale, m_szZScale);
	DDX_Text(pDX, IDC_HScale, m_szYScale);
	DDX_Text(pDX, IDC_uScale, m_szUScale);
	DDX_Text(pDX, IDC_vScale, m_szVScale);
}


BEGIN_MESSAGE_MAP(TImportDlg, CDialog)
	ON_BN_CLICKED(IDC_FileDlg, OnBnClickedFiledlg)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// TImportDlg 消息处理程序

void TImportDlg::OnBnClickedFiledlg()
{
	// TODO: 在此添加控件通知处理程序代码
	
	ASSERT(g_editor!=NULL);
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	_T("BMP文件|*.bmp|TGA文件|*.tga||"),
	this,0);
	if(IDOK==dlg.DoModal())
	{
		UpdateData(TRUE);
		m_szFile=dlg.GetPathName();
		UpdateData(FALSE);
	}
	
}

void TImportDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	g_editor->GetEngine()->SetName(m_szTerrainName);
	bool ok=g_editor->GetEngine()->ImportHeightMap(m_szFile,
		(float)_tstof(m_szXScale),
		(float)_tstof(m_szZScale),
		(float)_tstof(m_szYScale),
		Vector2((float)_tstof(m_szUScale),(float)_tstof(m_szVScale)));
	if(!ok)
	{
		::AfxMessageBox(Cool3D::Kernel::Inst()->GetLastError());
	}
	else
	{
		OnOK();
	}
}
