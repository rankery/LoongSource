// Config.cpp : implementation file
//

#include "stdafx.h"
#include "launcher.h"
#include "Config.h"

#include "var_container.h"
#include "launcherDlg.h"
#include "Process.h"

// CConfig dialog

IMPLEMENT_DYNAMIC(CConfig, CDialog)

CConfig::CConfig(CWnd* pParent /*=NULL*/)
	: CDialog(CConfig::IDD, pParent)
{
	m_pVarContainer = new VarContainer;
}

CConfig::~CConfig()
{
	SAFE_DEL(m_pVarContainer);
}

void CConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestHttpDlg)
	DDX_Control(pDX, IDC_RADIO1, m_ctrlWindow0);
	DDX_Control(pDX, IDC_RADIO2, m_ctrlWindow1);
	DDX_Control(pDX, IDC_RADIO3, m_ctrlWindow2);
	DDX_Control(pDX, IDC_RADIO4, m_ctrlFullScreen);
	DDX_Control(pDX, IDC_STATIC1, m_ctrlTips);


	DDX_Control(pDX, IDC_CHECK1, m_ctrlVerticalSync);		// 垂直同步
	DDX_Control(pDX, IDC_CHECK11, m_ctrlTextureDetail);		// 高精度纹理
	DDX_Control(pDX, IDC_CHECK2, m_ctrlGroundPrecision);	// 高精度地表
	DDX_Control(pDX, IDC_CHECK6, m_ctrlGPUAni);				// GPU动画
	DDX_Control(pDX, IDC_CHECK3, m_ctrlGroundHighLight);	// 地表高亮
	DDX_Control(pDX, IDC_CHECK7, m_ctrlNormalMap);			// 法线贴图
	DDX_Control(pDX, IDC_CHECK12, m_ctrlMusic);				// 音乐
	DDX_Control(pDX, IDC_CHECK16, m_ctrlSound);				// 音效


	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfig, CDialog)
	ON_BN_CLICKED(IDOK, &CConfig::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CConfig::OnBnClickedCancel)
END_MESSAGE_MAP()


BOOL CConfig::OnInitDialog()
{
	BOOL bResult = CDialog::OnInitDialog();
	this->SetWindowText(g_StrTable[_T("GameVideoSetup")]);
	m_ctrlWindow0.SetWindowText(g_StrTable[_T("WindowMode1")]);
	m_ctrlWindow1.SetWindowText(g_StrTable[_T("WindowMode2")]);
	m_ctrlWindow2.SetWindowText(g_StrTable[_T("WindowMode3")]);
	m_ctrlFullScreen.SetWindowText(g_StrTable[_T("WindowMode4")]);
	m_ctrlTips.SetWindowText(g_StrTable[_T("GameVideoTips")]);

	m_ctrlVerticalSync.SetWindowText(g_StrTable[_T("VerticalSync")]);
	m_ctrlTextureDetail.SetWindowText(g_StrTable[_T("TextureDetail")]);
	m_ctrlGroundPrecision.SetWindowText(g_StrTable[_T("GroundPrecision")]);
	m_ctrlGPUAni.SetWindowText(g_StrTable[_T("GPUAni")]);		
	m_ctrlGroundHighLight.SetWindowText(g_StrTable[_T("GroundHighLight")]);
	m_ctrlNormalMap.SetWindowText(g_StrTable[_T("NormalMap")]);
	m_ctrlMusic.SetWindowText(g_StrTable[_T("Music")]);
	m_ctrlSound.SetWindowText(g_StrTable[_T("Sound")]);

	return bResult;
}


// CConfig message handlers
void CConfig::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
	Save();
}

void CConfig::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}


// 窗口方式配置
VOID CConfig::Load(LPCTSTR szFileName)
{
	m_ctrlWindow0.SetCheck(BST_UNCHECKED);
	m_ctrlWindow1.SetCheck(BST_UNCHECKED);
	m_ctrlWindow2.SetCheck(BST_UNCHECKED);
	m_ctrlFullScreen.SetCheck(BST_UNCHECKED);

	m_ctrlVerticalSync.SetCheck(BST_UNCHECKED);
	m_ctrlTextureDetail.SetCheck(BST_UNCHECKED);
	m_ctrlGroundPrecision.SetCheck(BST_UNCHECKED);
	m_ctrlGPUAni.SetCheck(BST_UNCHECKED);	
	m_ctrlGroundHighLight.SetCheck(BST_UNCHECKED);
	m_ctrlNormalMap.SetCheck(BST_UNCHECKED);	
	m_ctrlMusic.SetCheck(BST_UNCHECKED);
	m_ctrlSound.SetCheck(BST_UNCHECKED);

	DWORD dwValue = 1;
	m_strFileName = szFileName;
	BOOL bResult = m_pVarContainer->Load(NULL, szFileName);
	if( !bResult )
		return;

	dwValue = m_pVarContainer->GetDword(_T("Windowed"));
	switch(dwValue)
	{
	case 1:
		m_ctrlWindow0.SetCheck(BST_CHECKED); 
		break;
	case 2:
		m_ctrlWindow1.SetCheck(BST_CHECKED); 
		break;
	case 3:
		m_ctrlWindow2.SetCheck(BST_CHECKED); 
		break;
	default:
		m_ctrlFullScreen.SetCheck(BST_CHECKED); 
		break;
	}

	m_ctrlVerticalSync.SetCheck(m_pVarContainer->GetDword(_T("VerSynch")));
	m_ctrlGPUAni.SetCheck(m_pVarContainer->GetDword(_T("HWSkin")));
	m_ctrlGroundHighLight.SetCheck(m_pVarContainer->GetDword(_T("TerrainSpecular")));
	m_ctrlNormalMap.SetCheck(m_pVarContainer->GetDword(_T("NormalMap")));
	m_ctrlMusic.SetCheck(m_pVarContainer->GetDword(_T("Music")));
	m_ctrlSound.SetCheck(m_pVarContainer->GetDword(_T("SoundEffect")));

	dwValue = m_pVarContainer->GetDword(_T("TexDetail"));
	if( dwValue == 0 )
		m_ctrlTextureDetail.SetCheck(BST_CHECKED);

	dwValue = m_pVarContainer->GetDword(_T("TerrainDetail"));
	if( dwValue == 3 )
		m_ctrlGroundPrecision.SetCheck(BST_CHECKED);

}

VOID CConfig::Save()
{
	DWORD dwValue = 0;
	if( BST_CHECKED == m_ctrlWindow0.GetCheck() )
		dwValue = 1;
	if( BST_CHECKED == m_ctrlWindow1.GetCheck() )
		dwValue = 2;
	if( BST_CHECKED == m_ctrlWindow2.GetCheck() )
		dwValue = 3;
	if( BST_CHECKED == m_ctrlFullScreen.GetCheck() )
		dwValue = 0;

	m_pVarContainer->Set(dwValue, _T("value Windowed"));

	if( BST_CHECKED == m_ctrlVerticalSync.GetCheck() )
		m_pVarContainer->Set(1UL, _T("value VerSynch"));
	else
		m_pVarContainer->Set(0UL, _T("value VerSynch"));

	if( BST_CHECKED == m_ctrlGPUAni.GetCheck() )
		m_pVarContainer->Set(1UL, _T("value HWSkin"));
	else
		m_pVarContainer->Set(0UL, _T("value HWSkin"));

	if( BST_CHECKED == m_ctrlGroundHighLight.GetCheck() )
		m_pVarContainer->Set(1UL, _T("value TerrainSpecular"));
	else
		m_pVarContainer->Set(0UL, _T("value TerrainSpecular"));

	if( BST_CHECKED == m_ctrlNormalMap.GetCheck() )
		m_pVarContainer->Set(1UL, _T("value NormalMap"));
	else
		m_pVarContainer->Set(0UL, _T("value NormalMap"));

	if( BST_CHECKED == m_ctrlMusic.GetCheck() )
		m_pVarContainer->Set(1UL, _T("value Music"));
	else
		m_pVarContainer->Set(0UL, _T("value Music"));

	if( BST_CHECKED == m_ctrlSound.GetCheck() )
		m_pVarContainer->Set(1UL, _T("value SoundEffect"));
	else
		m_pVarContainer->Set(0UL, _T("value SoundEffect"));


	if( BST_CHECKED == m_ctrlTextureDetail.GetCheck() )
		m_pVarContainer->Set(0UL, _T("value TexDetail"));
	else
		m_pVarContainer->Set(2UL, _T("value TexDetail"));

	if( BST_CHECKED == m_ctrlGroundPrecision.GetCheck() )
		m_pVarContainer->Set(3UL, _T("value TerrainDetail"));
	else
		m_pVarContainer->Set(2UL, _T("value TerrainDetail"));

	m_pVarContainer->Save(m_strFileName);
}