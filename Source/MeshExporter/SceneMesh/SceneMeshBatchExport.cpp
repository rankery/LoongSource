/**********************************************************************
*<
FILE: SceneMeshBatchExport.cpp

DESCRIPTION:	Appwizard generated plugin

CREATED BY:		leili

HISTORY: 

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/

#include "..\SceneContext.h"
#include "..\MsgBox.h"
#include "..\MeshExporter.h"
#include "..\..\F3DMaxExp\BatchExpDlg.h"
#include "..\..\F3DMaxExp\ExportData.h"
#include "..\..\F3DMaxExp\F3DExpMtl.h"

#define SCENEMESHBATCHEXPORT_CLASS_ID	Class_ID(0x42f44ad4, 0x4afa4f68)

class SceneMeshBatchExport : public SceneExport
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
	SceneMeshBatchExport();
	~SceneMeshBatchExport();		

};

class SceneMeshBatchExportClassDesc:public ClassDesc2 
{
public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new SceneMeshBatchExport(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID		ClassID() { return SCENEMESHBATCHEXPORT_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("SceneMeshBatchExport"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle

};

static SceneMeshBatchExportClassDesc SceneMeshBatchExportDesc;
ClassDesc2* GetSceneMeshBatchExportDesc() { return &SceneMeshBatchExportDesc; }

BOOL CALLBACK SceneMeshBatchExportOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	static SceneMeshBatchExport *imp = NULL;

	switch(message) 
	{
		case WM_INITDIALOG:
			imp = (SceneMeshBatchExport *)lParam;
			CenterWindow(hWnd,GetParent(hWnd));
			return TRUE;

		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return TRUE;
	}
	return FALSE;
}

//--- SceneMeshBatchExport ----------------------------------------------------
SceneMeshBatchExport::SceneMeshBatchExport()
{

}

SceneMeshBatchExport::~SceneMeshBatchExport() 
{

}

int SceneMeshBatchExport::ExtCount()
{
	//TODO: Returns the number of file name extensions supported by the plug-in.
	return 1;
}

const TCHAR *SceneMeshBatchExport::Ext(int n)
{		
	//TODO: Return the 'i-th' file name extension (i.e. "3DS").
	return _T("FSCNBatch");
}

const TCHAR *SceneMeshBatchExport::LongDesc()
{
	//TODO: Return long ASCII description (i.e. "Targa 2.0 Image File")
	return _T("Cool3D SceneMesh Batch");
}

const TCHAR *SceneMeshBatchExport::ShortDesc() 
{			
	//TODO: Return short ASCII description (i.e. "Targa")
	return _T("FSCN Batch");
}

const TCHAR *SceneMeshBatchExport::AuthorName()
{			
	//TODO: Return ASCII Author name
	return _T("ll");
}

const TCHAR *SceneMeshBatchExport::CopyrightMessage() 
{	
	// Return ASCII Copyright message
	return _T("");
}

const TCHAR *SceneMeshBatchExport::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("");
}

const TCHAR *SceneMeshBatchExport::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int SceneMeshBatchExport::Version()
{				
	//TODO: Return Version number * 100 (i.e. v3.01 = 301)
	return 100;
}

void SceneMeshBatchExport::ShowAbout(HWND hWnd)
{			
	// Optional
}

BOOL SceneMeshBatchExport::SupportsOptions(int ext, DWORD options)
{
	// TODO Decide which options to support.  Simply return
	// true for each option supported by each Extension 
	// the exporter supports.

	return TRUE;
}


int	SceneMeshBatchExport::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)
{
	//TODO: Implement the actual file Export here and 
	//		return TRUE If the file is exported properly

	BatchExpDlg dlg;
	Interface8* i8 = (Interface8*)i;
	unsigned long flags = Interface8::kSuppressPrompts | Interface8::kUseFileUnits;
	char szFile[260];
	if (dlg.Run(szFile))
	{
		ExportData export;
		export.LoadFSCNBatchFile(szFile);

		// 读取配置文件数据，循环读取max并且导出
		INT maxNum = export.GetMaxNum();
		for( INT maxi=0; maxi<maxNum; maxi++ )
		{
			LPCTSTR szMaxFile = export.GetMaxFile(maxi);
			if( i8->IsMaxFile( szMaxFile ) )
			{
				i8->LoadFromFile( szMaxFile, flags );

				INT nodeNum = export.GetNodeNum(maxi);
				for( INT nodei=0; nodei<nodeNum; nodei++ )
				{
					const tagMaxNode *ptagNode = export.GetNode(maxi,nodei);
					if( ptagNode != NULL )
					{
						INode *pNode = i8->GetINodeByName( ptagNode->strNodeName.c_str() );

						F3DExpMtl::s_bExportLMUV	= ptagNode->bExpLightmap;
						F3DExpMtl::s_iLightMapSize	= ptagNode->lightmapSize;
						F3DExpMtl::s_bExportBUUV	= ptagNode->bExpNormalmap;
						F3DExpMtl::s_bExportBox		= ptagNode->bExpCollideBox;
						F3DExpMtl::s_bExportOpcity	= ptagNode->bExpOpcity;
						F3DExpMtl::s_fCollideBoxSize = ptagNode->CollideBoxSize;
						F3DExpMtl::s_szBasePath		= export.GetMaxBasePath(maxi);
						F3DExpMtl::s_szSubPath		= export.GetMaxMtlPath(maxi);
						F3DExpMtl::s_bAutoCopy		= true;

						g_msgBox.Create(hInstance,IDD_EXPORTINFO,IDC_OUT);
						SceneContext::Inst()->ClearAll();

						g_msgBox.ShowMsg(_T("building nodes list...\r\n"));
						SceneContext::Inst()->AddMeshNode(pNode);

						g_msgBox.ShowMsg(_T("exporting data...\r\n"));
						SceneContext::Inst()->ExportData();

						g_msgBox.ShowMsg(_T("writing file ...\r\n"));
						if(SceneContext::Inst()->WriteToFile(ptagNode->strExportPath.c_str(), F3DExpMtl::s_bExportLMUV, F3DExpMtl::s_bExportBUUV))
							g_msgBox.ShowMsg(_T("成功\r\n"));
						else
							g_msgBox.ShowMsg(_T("失败!\r\n"));

						//--free memory
						SceneContext::Inst()->ClearAll();
					}
				}
			}
		}
	}

	return TRUE;
}