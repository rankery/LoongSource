/**********************************************************************
*<
FILE: SkinBatchExport.cpp

DESCRIPTION:	Appwizard generated plugin

CREATED BY:		leili

HISTORY: 

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/

#include "StdAfx.h"
#include "CSExport.h"
#include "..\F3DMaxExp\BatchExpDlg.h"
#include "..\F3DMaxExp\ExportData.h"
#include "..\F3DMaxExp\F3DExpMtl.h"
#include "MaxExp\ExpSkeleton.h"
#include "MaxExp\ExpSkin.h"
#include "MaxExp\FSkinFile.h"

#define SKINBATCHEXPORT_CLASS_ID	Class_ID(0x792b200d, 0x221b0a83)

class SkinBatchExport : public SceneExport
{
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
	SkinBatchExport();
	~SkinBatchExport();		

};

class SkinBatchExportClassDesc:public ClassDesc2 
{
public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new SkinBatchExport(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID		ClassID() { return SKINBATCHEXPORT_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("SkinBatchExport"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle

};

static SkinBatchExportClassDesc SkinBatchExportDesc;
ClassDesc2* GetSkinBatchExportDesc() { return &SkinBatchExportDesc; }

BOOL CALLBACK SkinBatchExportOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	static SkinBatchExport *imp = NULL;

	switch(message) 
	{
	case WM_INITDIALOG:
		imp = (SkinBatchExport *)lParam;
		CenterWindow(hWnd,GetParent(hWnd));
		return TRUE;

	case WM_CLOSE:
		EndDialog(hWnd, 0);
		return TRUE;
	}
	return FALSE;
}

//--- SkinBatchExport ---------------------------------------------------------
SkinBatchExport::SkinBatchExport()
{

}

SkinBatchExport::~SkinBatchExport() 
{

}

int SkinBatchExport::ExtCount()
{
	//TODO: Returns the number of file name extensions supported by the plug-in.
	return 1;
}

const TCHAR *SkinBatchExport::Ext(int n)
{		
	//TODO: Return the 'i-th' file name extension (i.e. "3DS").
	return _T("FSkinBatch");
}

const TCHAR *SkinBatchExport::LongDesc()
{
	//TODO: Return long ASCII description (i.e. "Targa 2.0 Image File")
	return _T("Cool3D Skin Batch");
}

const TCHAR *SkinBatchExport::ShortDesc() 
{			
	//TODO: Return short ASCII description (i.e. "Targa")
	return _T("FSkin Batch");
}

const TCHAR *SkinBatchExport::AuthorName()
{			
	//TODO: Return ASCII Author name
	return _T("ll");
}

const TCHAR *SkinBatchExport::CopyrightMessage() 
{	
	// Return ASCII Copyright message
	return _T("");
}

const TCHAR *SkinBatchExport::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("");
}

const TCHAR *SkinBatchExport::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int SkinBatchExport::Version()
{				
	//TODO: Return Version number * 100 (i.e. v3.01 = 301)
	return 100;
}

void SkinBatchExport::ShowAbout(HWND hWnd)
{			
	// Optional
}

BOOL SkinBatchExport::SupportsOptions(int ext, DWORD options)
{
	// TODO Decide which options to support.  Simply return
	// true for each option supported by each Extension 
	// the exporter supports.

	return TRUE;
}


int	SkinBatchExport::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)
{
	//TODO: Implement the actual file Export here and 
	//		return TRUE If the file is exported properly

	BatchExpDlg dlg;
	Interface8* i8 = (Interface8*)i;
	unsigned long flags = Interface8::kSuppressPrompts | Interface8::kUseFileUnits;
	char szFile[260];
	if( dlg.Run(szFile) )
	{
		ExportData export;
		export.LoadSkinBatchFile(szFile);

		// 读取配置文件数据，循环读取max并且导出
		INT maxNum = export.GetMaxNum();
		for( INT maxi=0; maxi<maxNum; maxi++ )
		{
			LPCTSTR szMaxFile = export.GetMaxFile(maxi);
			if( i8->IsMaxFile( szMaxFile ) )
			{
				i8->LoadFromFile( szMaxFile, flags );

				INT nodeNum = export.GetNodeNum( maxi );
				for( INT nodei=0; nodei<nodeNum; nodei++ )
				{
					const tagMaxNode *ptagNode = export.GetNode(maxi,nodei);
					if( ptagNode != NULL )
					{
						INode *pNode = i8->GetINodeByName( ptagNode->strNodeName.c_str() );

						F3DExpMtl::s_bExportBUUV	= ptagNode->bExpNormalmap;
						F3DExpMtl::s_bMultiPass		= ptagNode->bMultiPass;
						F3DExpMtl::s_szBasePath		= export.GetMaxBasePath(maxi);
						F3DExpMtl::s_szSubPath		= export.GetMaxMtlPath(maxi);
						F3DExpMtl::s_bAutoCopy		= true;

						g_msgBox.Create( hInstance, IDD_EXPINFO, IDC_OUT );

						g_msgBox.ShowMsg( _T("正在计算骨骼信息...\r\n"));
						ExpSkeleton expSkel;
						expSkel.SetShowInfo( false );
						int numBones = expSkel.BuildFromMaxExportInterface( ei );
						g_msgBox.ShowMsg( _T("--共有骨骼:%d,根节点个数:%d\r\n"), numBones, expSkel.GetNumRoot() );

						//--
						g_msgBox.ShowMsg( _T("正在输出蒙皮信息...\r\n") );
						ExpSkin expSkin;
						expSkin.SetVertInfo( F3DExpMtl::s_bExportBUUV );
						expSkin.SetSkeletion( &expSkel );
						expSkin.AddMeshNode( pNode );
						g_msgBox.ShowMsg( _T("--得到%d个mesh节点...\r\n"), expSkin.GetNumMeshNodes() );

						g_msgBox.ShowMsg( _T("正在写入文件...\r\n") );
						for( UINT idx=0; idx<expSkin.GetNumMeshNodes(); idx++ )
						{
							TCHAR szSkinFileName[512];
							_stprintf( szSkinFileName, _T("%s_%s.fskin"),
								ptagNode->strExportPath.c_str(), expSkin.GetMeshName(idx) );

							FSkinFile skinFile;
							if( skinFile.WriteToFile( szSkinFileName,expSkin.GetExpSkinMesh(idx) ) )
								g_msgBox.ShowMsg( _T("写入[%s]ok...\r\n"), szSkinFileName );
							else
								g_msgBox.ShowMsg( _T("写入[%s]失败，请检查材质中不能有半透明贴图!\r\n"), szSkinFileName );
						}

						g_msgBox.ShowMsg( _T("写入完成!\r\n") );
					}
				}
			}
		}
	}

	return TRUE;
}