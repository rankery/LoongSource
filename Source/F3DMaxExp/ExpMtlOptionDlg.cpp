#include "stdafx.h"
#include "ExpMtlOptionDlg.h"
#include "resource.h"
#include "F3DExpMtl.h"
using namespace Cool3D;
using namespace std;

const TCHAR* const g_szConfigFile=_T("c:\\Cool3DExp.ini");
ExpMtlOptionDlg::ExpMtlOptionDlg()
{
}

ExpMtlOptionDlg::~ExpMtlOptionDlg()
{

}

bool ExpMtlOptionDlg::AutoSetOutPathByExportPath(HWND hwndDlg)
{
	TCHAR szPath[512];
	memset(szPath,0,sizeof(szPath));
	GetDlgItemText(hwndDlg,IDC_BasePath,szPath,511);
	//最后一个字符如果不是斜线,则加上一个
	int len=_tcslen(szPath);
	if(len>0)
	{
		if(szPath[len-1]!=_T('\\'))
		{
			szPath[len]=_T('\\');
			szPath[len+1]=0;
		}
	}
	
	tstring szSubPath=m_szExportFilePath;
	int p=szSubPath.find(szPath);
	if(p!=tstring::npos)
	{
		szSubPath=szSubPath.substr(p+_tcslen(szPath));
		SetDlgItemText(hwndDlg,IDC_OutPath,szSubPath.c_str());
		return true;
	}
	else
		return false;
}

int CALLBACK ExpMtlOptionDlg::DialogProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_BasePath:
		case IDC_OutPath:
			if(HIWORD(wParam)==EN_CHANGE)
			{
				UpdateFullPath(hwndDlg);
			}
			break;
		case IDC_MdlPath:
			{
				if(HIWORD(wParam)==BN_CLICKED)
				{
					//--if checked
					if(SendMessage(GetDlgItem(hwndDlg,IDC_MdlPath),BM_GETCHECK,0,0))
					{
						if(!AutoSetOutPathByExportPath(hwndDlg))
						{
							MessageBox(NULL,_T("输出路径必须是'基本路径'下的一个子目录"),_T("Error"),MB_OK);
						}
					}
				}
			}
			break;
		case IDOK:
			{
				TCHAR szPath[512];
				memset(szPath,0,sizeof(szPath));
				GetDlgItemText(hwndDlg,IDC_BasePath,szPath,511);
				F3DExpMtl::s_szBasePath=szPath;
				BOOL b=WritePrivateProfileString(_T("Cool3DExp"),_T("BasePath"),szPath,g_szConfigFile);

				GetDlgItemText(hwndDlg,IDC_OutPath,szPath,511);
				F3DExpMtl::s_szSubPath=szPath;
				b=WritePrivateProfileString(_T("Cool3DExp"),_T("OutPath"),szPath,g_szConfigFile);

				GetDlgItemText(hwndDlg,IDC_Origin,szPath,511);
				F3DExpMtl::s_szOriginNode=szPath;

				F3DExpMtl::s_bAutoCopy=IsDlgButtonChecked(hwndDlg,IDC_AutoCopy)!=0;
				F3DExpMtl::s_bExportLMUV=IsDlgButtonChecked(hwndDlg,IDC_ExportLightMapUV)!=0;
				F3DExpMtl::s_bExportBUUV=IsDlgButtonChecked(hwndDlg,IDC_ExportNormalMapUV)!=0;
				F3DExpMtl::s_bMultiPass=IsDlgButtonChecked(hwndDlg,IDC_MultiPass)!=0;
				F3DExpMtl::s_bExportOpcity=IsDlgButtonChecked(hwndDlg,IDC_ExportOpcity)!=0;
				F3DExpMtl::s_bExportBox=IsDlgButtonChecked(hwndDlg,IDC_ExportBox)!=0;
				F3DExpMtl::s_iLightMapSize=GetDlgItemInt(hwndDlg,IDC_LightMapSize,NULL,FALSE);
				F3DExpMtl::s_fCollideBoxSize=GetDlgItemFloat(hwndDlg,IDC_CollideBoxSize);

				EndDialog(hwndDlg, IDOK);
			}
			break;
		case IDC_ExportBox:
			{
				if(HIWORD(wParam)==BN_CLICKED)
				{
					F3DExpMtl::s_bExportBox=IsDlgButtonChecked(hwndDlg,IDC_ExportBox)!=0;
					if( !F3DExpMtl::s_bExportBox )
					{
						CheckDlgButton(hwndDlg,IDC_ExportOpcity,0);
						F3DExpMtl::s_bExportOpcity=false;
					}
				}
			}
			break;
		case IDC_ExportOpcity:
			{
				if(HIWORD(wParam)==BN_CLICKED)
				{
					F3DExpMtl::s_bExportOpcity=IsDlgButtonChecked(hwndDlg,IDC_ExportOpcity)!=0;
					if( F3DExpMtl::s_bExportOpcity )
					{
						CheckDlgButton(hwndDlg,IDC_ExportBox,1);

						F3DExpMtl::s_bExportBox=false;
					}
				}
			}
			break;
		case IDCANCEL:
			{
				EndDialog(hwndDlg, -1);
			}
			return TRUE;
		}
		break;
		case WM_INITDIALOG:
			{
				TCHAR szBasePath[512];
				TCHAR szOutPath[512];
				GetPrivateProfileString(_T("Cool3DExp"),_T("BasePath"),_T("d:\\work\\bin\\"),
					szBasePath,sizeof(szBasePath),g_szConfigFile);
				GetPrivateProfileString(_T("Cool3DExp"),_T("OutPath"),_T("mesh\\"),
					szOutPath,sizeof(szOutPath),g_szConfigFile);
				
				SetDlgItemText(hwndDlg,IDC_BasePath,szBasePath);
				SetDlgItemText(hwndDlg,IDC_OutPath,szOutPath);
				SetDlgItemText(hwndDlg,IDC_ExpPath,m_szExportFilePath.c_str());
				CenterWindow(hwndDlg,GetParent(hwndDlg));

				CheckDlgButton(hwndDlg,IDC_MdlPath,1);
				
				CheckDlgButton(hwndDlg,IDC_AutoCopy,1);
				CheckDlgButton(hwndDlg,IDC_ExportLightMapUV,0);
				CheckDlgButton(hwndDlg,IDC_ExportNormalMapUV,0);
				CheckDlgButton(hwndDlg,IDC_MultiPass,0);

				if( F3DExpMtl::s_bExportOpcity )
					CheckDlgButton(hwndDlg,IDC_ExportOpcity,1);
				else
					CheckDlgButton(hwndDlg,IDC_ExportOpcity,0);

				if( F3DExpMtl::s_bExportBox )
					CheckDlgButton(hwndDlg,IDC_ExportBox,1);
				else
					CheckDlgButton(hwndDlg,IDC_ExportBox,0);

				AutoSetOutPathByExportPath(hwndDlg);

				SetDlgItemInt(hwndDlg, IDC_LightMapSize, 0, FALSE);
				SetDlgItemFloat(hwndDlg, IDC_CollideBoxSize, 50.0f);
			}
		return 1;
	case WM_CLOSE:
		{
			EndDialog(hwndDlg, -1);
		}
		return TRUE;
	}
	return DefWindowProc(hwndDlg,uMsg,wParam,lParam);
}

void ExpMtlOptionDlg::UpdateFullPath(HWND hwndDlg)
{
	tstring szFullPath;
	TCHAR szPath[512];
	memset(szPath,0,sizeof(szPath));
	GetDlgItemText(hwndDlg,IDC_BasePath,szPath,511);
	int len=_tcslen(szPath);
	if(len>0)
	{
		if(szPath[len-1]!=_T('\\'))
		{
			szPath[len]=_T('\\');
			szPath[len+1]=0;
		}
	}
	
	szFullPath=szPath;
	GetDlgItemText(hwndDlg,IDC_OutPath,szPath,511);
	szFullPath+=szPath;
				
	SetDlgItemText(hwndDlg,IDC_FullPath,szFullPath.c_str());
}

extern HANDLE g_hInst;
tstring	ExpMtlOptionDlg::m_szExportFilePath;
int ExpMtlOptionDlg::Run(const TCHAR* szExportFileName)
{
	TCHAR szDriver[16];
	TCHAR szPath[512];
	TCHAR szName[512];
	TCHAR szExt[16];
	_tsplitpath(szExportFileName,szDriver,szPath,szName,szExt);
	_tcslwr(szDriver);
	_tcslwr(szPath);
	m_szExportFilePath=szDriver;
	m_szExportFilePath=m_szExportFilePath+szPath;//+_T("texture\\");

	return DialogBox((HINSTANCE)g_hInst,MAKEINTRESOURCE(IDD_MtlDlg),GetActiveWindow(),DialogProc);

}