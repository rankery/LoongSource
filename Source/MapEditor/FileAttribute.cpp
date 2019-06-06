// FileAttribute.cpp : 实现文件
//

#include "stdafx.h"
#include "MapEditor.h"
#include "FileAttribute.h"
#include ".\PlugMapEditor.h"
#include ".\fileattribute.h"

// FileAttribute 对话框

IMPLEMENT_DYNAMIC(FileAttribute, CDialog)
FileAttribute::FileAttribute(CWnd* pParent /*=NULL*/)
	: CDialog(FileAttribute::IDD, pParent)
	, m_szSkyBoxTopTex(_T(""))
	, m_szSkyBoxFrontRightTex(_T(""))
	, m_szSkyBoxBackLeftTex(_T(""))
	, m_dist(800)
{
}

FileAttribute::~FileAttribute()
{
}

void FileAttribute::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SUNPOSX, m_EditSunPosX);
	DDX_Control(pDX, IDC_EDIT_SUNPOSY, m_EditSunPosY);
	DDX_Control(pDX, IDC_EDIT_SUNPOSZ, m_EditSunPosZ);
	DDX_Control(pDX, IDC_BUTTON_SUNCOL, m_btnSunCol);
	DDX_Control(pDX, IDC_BUTTON_ECHOCOL, m_btnAmbientCol);
	DDX_Control(pDX, IDC_BUTTON_Specular, m_btnSpecular);
	DDX_Control(pDX, IDC_BUTTON_DynamicDiff, m_btnDynamicDiffCol);
	DDX_Control(pDX, IDC_BUTTON_DynamicAmb, m_btnDynamicAmbCol);
	DDX_Control(pDX, IDC_BUTTON_DynamicSpec, m_btnDynamicSpecCol);
	DDX_Control(pDX, IDC_BUTTON4_SkyCol, m_btnSkyCol);
	DDX_Control(pDX, IDC_SkyShadeCol, m_btnSkyShadeCol);
	DDX_Check(pDX, IDC_CHECK_RenderSkyShade, m_bRenderSkyShade);
	DDX_Text(pDX, IDC_EDIT10, m_szSkyBoxTopTex);
	DDX_Text(pDX, IDC_EDIT11, m_szSkyBoxFrontRightTex);
	DDX_Text(pDX, IDC_EDIT12, m_szSkyBoxBackLeftTex);
	DDX_Control(pDX, IDC_EDIT_XSIZE, m_editSkyBoxX);
	DDX_Control(pDX, IDC_EDIT_YSIZE, m_editSkyBoxY);
	DDX_Control(pDX, IDC_EDIT_ZSIZE, m_editSkyBoxZ);
	DDX_Control(pDX, IDC_EDIT_OffX, m_EditSkyBoxOffX);
	DDX_Control(pDX, IDC_EDIT_OffY, m_EditSkyBoxOffY);
	DDX_Control(pDX, IDC_EDIT_OffZ, m_EditSkyBoxOffZ);
	DDX_Control(pDX, IDC_EDIT_Modulus, m_editSunModulus);
	DDX_Control(pDX, IDC_EDIT7, m_editSkyYaw);
	DDX_Control(pDX, IDC_BUTTON_FogColor, m_btnFogColor);
	DDX_Control(pDX, IDC_EDIT_FogDensity, m_editFogDen);
	DDX_Control(pDX, IDC_EDIT_FogStart, m_editFogStart);
	DDX_Control(pDX, IDC_EDIT_FogEnd, m_editFogEnd);
	DDX_Text(pDX, IDC_EDIT_Dist, m_dist);
	DDV_MinMaxInt(pDX, m_dist, 0, 10000);
}

BOOL FileAttribute::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString temp;
	temp.Format(_T("%.2f"), m_vecSunPos.x);
	m_EditSunPosX.SetWindowText(temp);
	temp.Format(_T("%.2f"), m_vecSunPos.y);
	m_EditSunPosY.SetWindowText(temp);
	temp.Format(_T("%.2f"), m_vecSunPos.z);
	m_EditSunPosZ.SetWindowText(temp);

	DWORD sunCol = RGB(int(m_SunCol.R*255), int(m_SunCol.G*255), int(m_SunCol.B*255));
	m_btnSunCol.SetColor(sunCol);
	DWORD ambientCol = RGB(int(m_AmbientCol.R*255), int(m_AmbientCol.G*255), int(m_AmbientCol.B*255));
	m_btnAmbientCol.SetColor(ambientCol);
	DWORD specular = RGB(int(m_Specular.R*255), int(m_Specular.G*255), int(m_Specular.B*255));
	m_btnSpecular.SetColor(specular);
	DWORD skycol = RGB(int(m_SkyCol.R*255), int(m_SkyCol.G*255), int(m_SkyCol.B*255));
	m_btnSkyCol.SetColor(skycol);
	DWORD skyShadeColor = RGB( m_dwSkyShadeCol>>16, m_dwSkyShadeCol>>8, m_dwSkyShadeCol);
	m_btnSkyShadeCol.SetColor(skyShadeColor);
	DWORD DynamicDiffCol = RGB( m_dwDynamicDiffCol>>16, m_dwDynamicDiffCol>>8, m_dwDynamicDiffCol);
	m_btnDynamicDiffCol.SetColor(DynamicDiffCol);
	DWORD DynamicAmbCol = RGB( m_dwDynamicAmbCol>>16, m_dwDynamicAmbCol>>8, m_dwDynamicAmbCol);
	m_btnDynamicAmbCol.SetColor(DynamicAmbCol);
	DWORD DynamicSpecCol = RGB( m_dwDynamicSpecCol>>16, m_dwDynamicSpecCol>>8, m_dwDynamicSpecCol);
	m_btnDynamicSpecCol.SetColor(DynamicSpecCol);

	temp.Format(_T("%0.2f"), m_vecSkyBoxSize.x);
	m_editSkyBoxX.SetWindowText(temp);
	temp.Format(_T("%0.2f"), m_vecSkyBoxSize.y);
	m_editSkyBoxY.SetWindowText(temp);
	temp.Format(_T("%0.2f"), m_vecSkyBoxSize.z);
	m_editSkyBoxZ.SetWindowText(temp);

	temp.Format(_T("%0.2f"), m_vecSkyBoxOffSize.x);
	m_EditSkyBoxOffX.SetWindowText(temp);
	temp.Format(_T("%0.2f"), m_vecSkyBoxOffSize.y);
	m_EditSkyBoxOffY.SetWindowText(temp);
	temp.Format(_T("%0.2f"), m_vecSkyBoxOffSize.z);
	m_EditSkyBoxOffZ.SetWindowText(temp);

	temp.Format(_T("%f"), m_fSunModulus);
	m_editSunModulus.SetWindowText(temp);

	temp.Format(_T("%f"), m_fSkyYaw);
	m_editSkyYaw.SetWindowText(temp);

	//--雾
	temp.Format(_T("%f"), m_fstart);
	m_editFogStart.SetWindowText(temp);
	temp.Format(_T("%f"), m_fend);
	m_editFogEnd.SetWindowText(temp);
	temp.Format(_T("%f"), m_fdensity);
	m_editFogDen.SetWindowText(temp);
	DWORD fogColor = RGB(int(m_FogColor.R*255), int(m_FogColor.G*255), int(m_FogColor.B*255));
	m_btnFogColor.SetColor(fogColor);

	return TRUE;
}

BEGIN_MESSAGE_MAP(FileAttribute, CDialog)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_SUNCOL, OnBnClickedButtonSuncol)
	ON_BN_CLICKED(IDC_BUTTON_ECHOCOL, OnBnClickedButtonEchocol)
	ON_BN_CLICKED(IDC_BUTTON_Specular, OnBnClickedButtonSpecular)
	ON_BN_CLICKED(IDC_BTN_TOP, OnBnClickedBtnTop)
	ON_BN_CLICKED(IDC_BTN_FNTRGT, OnBnClickedBtnFntrgt)
	ON_BN_CLICKED(IDC_BTN_BCKLEFT, OnBnClickedBtnBckleft)
	ON_BN_CLICKED(IDC_BUTTON_FogColor, &FileAttribute::OnBnClickedButtonFogcolor)
	ON_EN_CHANGE(IDC_EDIT_Dist, &FileAttribute::OnEnChangeEditDist)
	ON_BN_CLICKED(IDC_BUTTON4_SkyCol, &FileAttribute::OnBnClickedButtonSkycol)
	ON_BN_CLICKED(IDC_SkyShadeCol, &FileAttribute::OnBnClickedSkyshadecol)
	ON_BN_CLICKED(IDC_BUTTON_DynamicDiff, &FileAttribute::OnBnClickedButtonDynamicdiff)
	ON_BN_CLICKED(IDC_BUTTON_DynamicAmb, &FileAttribute::OnBnClickedButtonDynamicamb)
	ON_BN_CLICKED(IDC_BUTTON_DynamicSpec, &FileAttribute::OnBnClickedButtonDynamicspec)
END_MESSAGE_MAP()


// FileAttribute 消息处理程序

void FileAttribute::OnBnClickedOk()
{
	CString temp;
	m_EditSunPosX.GetWindowText(temp);
	m_vecSunPos.x = (float)_tstof(temp.GetBuffer());
	m_EditSunPosY.GetWindowText(temp);
	m_vecSunPos.y = (float)_tstof(temp.GetBuffer());
	m_EditSunPosZ.GetWindowText(temp);
	m_vecSunPos.z = (float)_tstof(temp.GetBuffer());

	//天空盒
	CString szX, szY, szZ;
	m_editSkyBoxX.GetWindowText(szX);
	m_editSkyBoxY.GetWindowText(szY);
	m_editSkyBoxZ.GetWindowText(szZ);
	float x, y, z;
	x = (float)_tstof(szX.GetBuffer());
	y = (float)_tstof(szY.GetBuffer());
	z = (float)_tstof(szZ.GetBuffer());
	m_vecSkyBoxSize = Vector3(x, y, z);

	m_EditSkyBoxOffX.GetWindowText(szX);
	m_EditSkyBoxOffY.GetWindowText(szY);
	m_EditSkyBoxOffZ.GetWindowText(szZ);

	x = (float)_tstof(szX.GetBuffer());
	y = (float)_tstof(szY.GetBuffer());
	z = (float)_tstof(szZ.GetBuffer());
	m_vecSkyBoxOffSize = Vector3(x, y, z);

	CString szModulus;
	m_editSunModulus.GetWindowText(szModulus);
	m_fSunModulus = (FLOAT)_tstof(szModulus.GetBuffer());
	if(m_fSunModulus>=2.0f)
		m_fSunModulus = 2.0f;
	else if(m_fSunModulus<0.0f)
		m_fSunModulus = 0.0f;

	CString szSkyYaw;
	m_editSkyYaw.GetWindowText(szSkyYaw);
	m_fSkyYaw = (FLOAT)_tstof(szSkyYaw.GetBuffer());

	CString start, end, den;
	m_editFogDen.GetWindowText(den);
	m_fdensity = (FLOAT)_tstof(den.GetBuffer());
	m_editFogStart.GetWindowText(start);
	m_fstart = (FLOAT)_tstof(start.GetBuffer());
	m_editFogEnd.GetWindowText(end);
	m_fend = (FLOAT)_tstof(end.GetBuffer());

	OnOK();
}

void FileAttribute::OnBnClickedButtonSuncol()
{
	DWORD colorOld = RGB(int(m_SunCol.R*255), int(m_SunCol.G*255), int(m_SunCol.B*255));
	CColorDialog ClrDlg(colorOld, 0, this);

	if (IDOK == ClrDlg.DoModal())
	{
		COLORREF selClr = ClrDlg.GetColor();
		m_btnSunCol.SetColor(selClr);
		m_SunCol.R = GetRValue(selClr)/255.0f;
		m_SunCol.G = GetGValue(selClr)/255.0f;
		m_SunCol.B = GetBValue(selClr)/255.0f;
		m_SunCol.A = 1.0f;
	}
	SetFocus();
}

void FileAttribute::OnBnClickedButtonEchocol()
{
	DWORD colorOld = RGB(int(m_AmbientCol.R*255), int(m_AmbientCol.G*255), int(m_AmbientCol.B*255));
	CColorDialog ClrDlg(colorOld, 0, this);

	if (IDOK == ClrDlg.DoModal())
	{
		COLORREF selClr = ClrDlg.GetColor();
		m_btnAmbientCol.SetColor(selClr);
		m_AmbientCol.R = GetRValue(selClr)/255.0f;
		m_AmbientCol.G = GetGValue(selClr)/255.0f;
		m_AmbientCol.B = GetBValue(selClr)/255.0f;
		m_AmbientCol.A = 1.0f;
	}
	SetFocus();
}

void FileAttribute::OnBnClickedButtonSpecular()
{
	DWORD colorOld = RGB(int(m_Specular.R*255), int(m_Specular.G*255), int(m_Specular.B*255));
	CColorDialog ClrDlg(colorOld, 0, this);

	if (IDOK == ClrDlg.DoModal())
	{
		COLORREF selClr = ClrDlg.GetColor();
		m_btnSpecular.SetColor(selClr);
		m_Specular.R = GetRValue(selClr)/255.0f;
		m_Specular.G = GetGValue(selClr)/255.0f;
		m_Specular.B = GetBValue(selClr)/255.0f;
		m_Specular.A = 1.0f;
	}
	SetFocus();
}

void FileAttribute::OnBnClickedButtonFogcolor()
{
	DWORD colorOld = RGB(int(m_FogColor.R*255), int(m_FogColor.G*255), int(m_FogColor.B*255));
	CColorDialog ClrDlg(colorOld, 0, this);

	if (IDOK == ClrDlg.DoModal())
	{
		COLORREF selClr = ClrDlg.GetColor();
		m_btnFogColor.SetColor(selClr);
		m_FogColor.R = GetRValue(selClr)/255.0f;
		m_FogColor.G = GetGValue(selClr)/255.0f;
		m_FogColor.B = GetBValue(selClr)/255.0f;
		m_FogColor.A = 1.0f;
	}
	SetFocus();
}

void FileAttribute::GetSkyBox(CString& szTopTex, CString& szFrontRightTex, CString& szBackLeftTex, 
							  Vector3& drawSize, Vector3& offSize)const
{
	szTopTex = m_szSkyBoxTopTex;
	szFrontRightTex = m_szSkyBoxFrontRightTex;
	szBackLeftTex = m_szSkyBoxBackLeftTex;
	drawSize = m_vecSkyBoxSize;
	offSize = m_vecSkyBoxOffSize;
}

void FileAttribute::SetSkyBox(const CString& szTopTex, const CString& szFrontRightTex, const CString& szBackLeftTex, 
							  const Vector3& drawSize, const Vector3& offSize)
{
	m_szSkyBoxTopTex = szTopTex;
	m_szSkyBoxFrontRightTex = szFrontRightTex;
	m_szSkyBoxBackLeftTex =  szBackLeftTex;
	m_vecSkyBoxSize = drawSize;
	m_vecSkyBoxOffSize = offSize;
}
void FileAttribute::OnBnClickedBtnTop()
{
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		_T("纹理文件|*.tga;*.dds;|所有文件(*.*)|*.*||"));
	if(dlg.DoModal()==IDOK)
	{
		CString tempTop = dlg.GetPathName();
		tempTop.MakeLower();
		tstring path = Kernel::Inst()->GetWorkPath();
		int n = tempTop.Find(path.c_str());
		if(n!=0)
		{
			AfxMessageBox(_T("必须选择同一工作目录下的天空盒纹理!"));
		}
		else
		{
			tstring temp = tempTop.GetBuffer();
			size_t len = _tcslen(path.c_str());
			temp.erase(temp.find(path.c_str()), len);
			m_szSkyBoxTopTex = temp.c_str();
		}
		UpdateData(FALSE);					//在编辑框控件中显示文件名
	}
}

void FileAttribute::OnBnClickedBtnFntrgt()
{
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		_T("纹理文件|*.tga;*.dds;|所有文件(*.*)|*.*||"));
	if(dlg.DoModal()==IDOK)
	{
		CString tempRightTex = dlg.GetPathName();	
		tempRightTex.MakeLower();
		tstring path = Kernel::Inst()->GetWorkPath();
		int n = tempRightTex.Find(path.c_str());
		if(n!=0)
		{
			AfxMessageBox(_T("必须选择同一工作目录下的天空盒纹理!"));
		}
		else
		{
			tstring temp = tempRightTex.GetBuffer();
			size_t len = _tcslen(path.c_str());
			temp.erase(temp.find(path.c_str()), len);
			m_szSkyBoxFrontRightTex = temp.c_str();
		}
		UpdateData(FALSE);					//在编辑框控件中显示文件名
	}
}

void FileAttribute::OnBnClickedBtnBckleft()
{
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		_T("纹理文件|*.tga;*.dds;|所有文件(*.*)|*.*||"));
	if(dlg.DoModal()==IDOK)
	{
		CString tempLeftTex = dlg.GetPathName();	
		tempLeftTex.MakeLower();
		tstring path = Kernel::Inst()->GetWorkPath();
		int n = tempLeftTex.Find(path.c_str());
		if(n!=0)
		{
			AfxMessageBox(_T("必须选择同一工作目录下的天空盒纹理!"));
		}
		else
		{
			tstring temp = tempLeftTex.GetBuffer();
			size_t len = _tcslen(path.c_str());
			temp.erase(temp.find(path.c_str()), len);
			m_szSkyBoxBackLeftTex = temp.c_str();
		}
		UpdateData(FALSE);					//在编辑框控件中显示文件名
	}
}



void FileAttribute::OnEnChangeEditDist()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData();
}

void FileAttribute::OnBnClickedButtonSkycol()
{
	DWORD colorOld = RGB(int(m_SkyCol.R*255), int(m_SkyCol.G*255), int(m_SkyCol.B*255));
	CColorDialog ClrDlg(colorOld, 0, this);

	if (IDOK == ClrDlg.DoModal())
	{
		COLORREF selClr = ClrDlg.GetColor();
		m_btnSkyCol.SetColor(selClr);
		m_SkyCol.R = GetRValue(selClr)/255.0f;
		m_SkyCol.G = GetGValue(selClr)/255.0f;
		m_SkyCol.B = GetBValue(selClr)/255.0f;
		m_SkyCol.A = 1.0f;
	}
	SetFocus();
}

void FileAttribute::OnBnClickedSkyshadecol()
{
	DWORD colorOld = RGB( m_dwSkyShadeCol>>16, m_dwSkyShadeCol>>8, m_dwSkyShadeCol);
	CColorDialog ClrDlg(colorOld, 0, this);

	if (IDOK == ClrDlg.DoModal())
	{
		COLORREF selClr = ClrDlg.GetColor();
		m_btnSkyShadeCol.SetColor(selClr);
		m_dwSkyShadeCol = (0xff << 24) | (GetRValue(selClr) << 16) | (GetGValue(selClr) << 8) | GetBValue(selClr);
	}
	SetFocus();
}

void FileAttribute::OnBnClickedButtonDynamicdiff()
{
	DWORD colorOld = RGB( m_dwDynamicDiffCol>>16, m_dwDynamicDiffCol>>8, m_dwDynamicDiffCol);
	CColorDialog ClrDlg(colorOld, 0, this);

	if (IDOK == ClrDlg.DoModal())
	{
		COLORREF selClr = ClrDlg.GetColor();
		m_btnDynamicDiffCol.SetColor(selClr);
		m_dwDynamicDiffCol = (0xff << 24) | (GetRValue(selClr) << 16) | (GetGValue(selClr) << 8) | GetBValue(selClr);
	}
	SetFocus();
}

void FileAttribute::OnBnClickedButtonDynamicamb()
{
	DWORD colorOld = RGB( m_dwDynamicAmbCol>>16, m_dwDynamicAmbCol>>8, m_dwDynamicAmbCol);
	CColorDialog ClrDlg(colorOld, 0, this);

	if (IDOK == ClrDlg.DoModal())
	{
		COLORREF selClr = ClrDlg.GetColor();
		m_btnDynamicAmbCol.SetColor(selClr);
		m_dwDynamicAmbCol = (0xff << 24) | (GetRValue(selClr) << 16) | (GetGValue(selClr) << 8) | GetBValue(selClr);
	}
	SetFocus();
}

void FileAttribute::OnBnClickedButtonDynamicspec()
{
	DWORD colorOld = RGB( m_dwDynamicSpecCol>>16, m_dwDynamicSpecCol>>8, m_dwDynamicSpecCol);
	CColorDialog ClrDlg(colorOld, 0, this);

	if (IDOK == ClrDlg.DoModal())
	{
		COLORREF selClr = ClrDlg.GetColor();
		m_btnDynamicSpecCol.SetColor(selClr);
		m_dwDynamicSpecCol = (0xff << 24) | (GetRValue(selClr) << 16) | (GetGValue(selClr) << 8) | GetBValue(selClr);
	}
	SetFocus();
}
