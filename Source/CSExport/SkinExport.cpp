/**********************************************************************
 *<
	FILE: CSExport.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#include "stdafx.h"
#include "CSExport.h"
#include "MaxExp\ExpSkeleton.h"
#include "MaxExp\ExpSkin.h"
#include "MaxExp\FSkinFile.h"
#include "..\F3DMaxExp\ExpMtlOptionDlg.h"
#include "..\F3DMaxExp\F3DExpMtl.h"

#define SKINEXPORT_CLASS_ID	Class_ID(0x621b3cdd, 0x284037cb)

class SkinExport : public SceneExport {
	public:


		static HWND hParams;


		int				ExtCount();					// Number of extensions supported
		const TCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
		const TCHAR *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
		const TCHAR *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
		const TCHAR *	AuthorName();				// ASCII Author name
		const TCHAR *	CopyrightMessage();			// ASCII Copyright message
		const TCHAR *	OtherMessage1();			// Other message #1
		const TCHAR *	OtherMessage2();			// Other message #2
		unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
		void			ShowAbout(HWND hWnd);		// Show DLL's "About..." box

		BOOL SupportsOptions(int ext, DWORD options);
		int	 DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);


		
		//Constructor/Destructor

		SkinExport();
		~SkinExport();		

};


class SkinExportClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new SkinExport(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID		ClassID() { return SKINEXPORT_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("SkinExport"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle

};



static SkinExportClassDesc SkinExportDesc;
ClassDesc2* GetSkinExportDesc() { return &SkinExportDesc; }


BOOL CALLBACK SkinExportOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	static SkinExport *imp = NULL;

	switch(message) {
		case WM_INITDIALOG:
			imp = (SkinExport *)lParam;
			CenterWindow(hWnd,GetParent(hWnd));
			return TRUE;

		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return TRUE;
	}
	return FALSE;
}


//--- SkinExport -------------------------------------------------------
SkinExport::SkinExport()
{

}

SkinExport::~SkinExport() 
{

}

int SkinExport::ExtCount()
{
	//TODO: Returns the number of file name extensions supported by the plug-in.
	return 1;
}

const TCHAR *SkinExport::Ext(int n)
{		
	//TODO: Return the 'i-th' file name extension (i.e. "3DS").
	return _T("FSkin");
}

const TCHAR *SkinExport::LongDesc()
{
	//TODO: Return long ASCII description (i.e. "Targa 2.0 Image File")
	return _T("F3D Skin File");
}
	
const TCHAR *SkinExport::ShortDesc() 
{			
	//TODO: Return short ASCII description (i.e. "Targa")
	return _T("F3D Skin File");
}

const TCHAR *SkinExport::AuthorName()
{			
	//TODO: Return ASCII Author name
	return _T("yanliang");
}

const TCHAR *SkinExport::CopyrightMessage() 
{	
	// Return ASCII Copyright message
	return _T("");
}

const TCHAR *SkinExport::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("");
}

const TCHAR *SkinExport::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int SkinExport::Version()
{				
	//TODO: Return Version number * 100 (i.e. v3.01 = 301)
	return 100;
}

void SkinExport::ShowAbout(HWND hWnd)
{			
	// Optional
}

BOOL SkinExport::SupportsOptions(int ext, DWORD options)
{
	// TODO Decide which options to support.  Simply return
	// true for each option supported by each Extension 
	// the exporter supports.

	return TRUE;
}


int	SkinExport::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)
{
	//TODO: Implement the actual file Export here and 
	//		return TRUE If the file is exported properly
/*
	if(!suppressPrompts)
		DialogBoxParam(hInstance, 
				MAKEINTRESOURCE(IDD_SKIN_PANEL), 
				GetActiveWindow(), 
				SkinExportOptionsDlgProc, (LPARAM)this);
				*/

	g_msgBox.Create(hInstance,IDD_EXPINFO,IDC_OUT);
	
	//--mtl option
	F3DExpMtl::s_pMsgBox=&g_msgBox;
	ExpMtlOptionDlg dlg;
	if(dlg.Run(name) < 0)
		return TRUE;

	//if(F3DExpMtl::s_bMultiPass)
	//{
	//	g_msgBox.ShowMsg(_T("游戏中蒙皮的运算量很大，请使用镂空贴图，不要使用半透明贴图进行多遍渲染!\r\n"));
	//	return TRUE;
	//}
	
	//--
	g_msgBox.ShowMsg(_T("正在计算骨骼信息...\r\n"));
	ExpSkeleton expSkel;
	expSkel.SetShowInfo(false);
	int numBones=expSkel.BuildFromMaxExportInterface(ei);
	g_msgBox.ShowMsg(_T("--共有骨骼:%d,根节点个数:%d\r\n"),numBones,expSkel.GetNumRoot());

	//--
	g_msgBox.ShowMsg(_T("正在输出蒙皮信息...\r\n"));
	ExpSkin expSkin;
	expSkin.SetVertInfo(F3DExpMtl::s_bExportBUUV);
	expSkin.BuildFromMaxExportInterface(ei,&expSkel);
	g_msgBox.ShowMsg(_T("--得到%d个mesh节点...\r\n"),expSkin.GetNumMeshNodes());

	g_msgBox.ShowMsg(_T("正在写入文件...\r\n"));
	TCHAR szDriver[16]={0};
	TCHAR szDir[512]={0};
	TCHAR szName[256]={0};
	_tsplitpath(name,szDriver,szDir,szName,NULL);
	for(UINT idx=0;idx<expSkin.GetNumMeshNodes();idx++)
	{
		TCHAR szSkinFileName[512];
		_stprintf(szSkinFileName,_T("%s%s%s_%s.fskin"),
			szDriver,szDir,szName,expSkin.GetMeshName(idx));
		
		FSkinFile skinFile;
		if(skinFile.WriteToFile(szSkinFileName,expSkin.GetExpSkinMesh(idx)))
			g_msgBox.ShowMsg(_T("写入[%s]ok...\r\n"),szSkinFileName);
		else
			g_msgBox.ShowMsg(_T("写入[%s]失败，请检查材质中不能有半透明贴图!\r\n"),szSkinFileName);
	}

	g_msgBox.ShowMsg(_T("写入完成!\r\n"));
	return TRUE;
}


