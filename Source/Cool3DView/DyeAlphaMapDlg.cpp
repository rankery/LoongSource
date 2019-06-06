// DyeAlphaMapDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Cool3DView.h"
#include "DyeAlphaMapDlg.h"
#include ".\dyealphamapdlg.h"
#include "..\Cool3D\Image\Image.h"
#include "..\Cool3D\Image\ImageTGA.h"

using namespace Cool3D;

// CDyeAlphaMapDlg 对话框

IMPLEMENT_DYNAMIC(CDyeAlphaMapDlg, CDialog)
CDyeAlphaMapDlg::CDyeAlphaMapDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDyeAlphaMapDlg::IDD, pParent)
	, m_szBaseTexPath(_T(""))
	, m_nAlphaTexWidth(128)
	, m_nAlphaTexHeight(128)
	, m_fLumMask(0.9f)
{
}

CDyeAlphaMapDlg::~CDyeAlphaMapDlg()
{
}

void CDyeAlphaMapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_BaseTexPath, m_szBaseTexPath);
	DDX_Text(pDX, IDC_AlphaMapWidth, m_nAlphaTexWidth);
	DDX_Text(pDX, IDC_AlphaMapHeight, m_nAlphaTexHeight);
	DDX_Text(pDX, IDC_LumMask, m_fLumMask);
}


BEGIN_MESSAGE_MAP(CDyeAlphaMapDlg, CDialog)
	ON_BN_CLICKED(IDC_Browse, OnBnClickedBrowse)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDyeAlphaMapDlg 消息处理程序

void CDyeAlphaMapDlg::OnBnClickedBrowse()
{
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		NULL,
		this,0);
	if(IDOK==dlg.DoModal())
	{
		UpdateData();
		m_szBaseTexPath=dlg.GetPathName();
		UpdateData(FALSE);
	}
}

float _Luminance(BYTE r,BYTE g,BYTE b)
{
	float rf=r/255.0f;
	float gf=g/255.0f;
	float bf=b/255.0f;
	return 0.3f*rf + 0.59f*gf + 0.11f*bf;
}

void CDyeAlphaMapDlg::OnBnClickedOk()
{
	if(FALSE==UpdateData())
		return;

	if(m_szBaseTexPath.IsEmpty())
		return;
	if(m_nAlphaTexWidth<=0
		||m_nAlphaTexWidth>1024
		||m_nAlphaTexHeight<=0
		||m_nAlphaTexHeight>1024)
		return;
	if(m_fLumMask<0
		||m_fLumMask>1)
		return;
	
	//load base image
	Image img;
	if(!img.LoadFromFile((LPCTSTR)m_szBaseTexPath))
	{
		MessageBox(_T("打开原始贴图文件失败！"));
		return;
	}
	
	//convert to 32bit
	img.ConvertFmt(EPF_A8R8G8B8);

	//calc alpha map rgba
	ImageTGA::tagPixel* pData=(ImageTGA::tagPixel*)img.GetRawBuffer();
	for(UINT i=0;i<img.Width()*img.Height();i++)
	{
		float lum=_Luminance(pData->R,pData->G,pData->B);
		if(lum>=m_fLumMask)
		{
			float r=1-(lum-m_fLumMask)/(1-m_fLumMask);
			pData->A=BYTE(r*255);
		}
		else
			pData->A=255;

		//pData->R=pData->G=pData->B=255;

		pData++;
	}

	//scale
	img.Scale(m_nAlphaTexWidth,m_nAlphaTexHeight,EIS_BestQuality);

	//save alpha map
	TCHAR szDriver[10];
	TCHAR szDir[512];
	TCHAR szFileName[512];
	_tsplitpath((LPCTSTR)m_szBaseTexPath,szDriver,szDir,szFileName,NULL);
	TCHAR szAlphaMap[512];
	_stprintf(szAlphaMap,_T("%s%s%s_a.tga"),szDriver,szDir,szFileName);
	img.WriteToFile(szAlphaMap);

	MessageBox(_T("生成成功"));
	OnOK();
}
