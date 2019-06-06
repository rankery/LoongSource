/**********************************************************************
*<
FILE: CSBatchExport.cpp

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
#include "MaxExp\FSkelFile.h"

#define CSBATCHEXPORT_CLASS_ID	Class_ID(0x3a1704f, 0x6ff37c71)

class CSBatchExport : public SceneExport
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
	CSBatchExport();
	~CSBatchExport();		

};

class CSBatchExportClassDesc:public ClassDesc2 
{
public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new CSBatchExport(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID		ClassID() { return CSBATCHEXPORT_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("CSBatchExport"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle

};

static CSBatchExportClassDesc CSBatchExportDesc;
ClassDesc2* GetCSBatchExportDesc() { return &CSBatchExportDesc; }

BOOL CALLBACK CSBatchExportOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	static CSBatchExport *imp = NULL;

	switch(message) 
	{
	case WM_INITDIALOG:
		imp = (CSBatchExport *)lParam;
		CenterWindow(hWnd,GetParent(hWnd));
		return TRUE;

	case WM_CLOSE:
		EndDialog(hWnd, 0);
		return TRUE;
	}
	return FALSE;
}

//--- CSBatchExport -----------------------------------------------------------
CSBatchExport::CSBatchExport()
{

}

CSBatchExport::~CSBatchExport() 
{

}

int CSBatchExport::ExtCount()
{
	//TODO: Returns the number of file name extensions supported by the plug-in.
	return 1;
}

const TCHAR *CSBatchExport::Ext(int n)
{		
	//TODO: Return the 'i-th' file name extension (i.e. "3DS").
	return _T("FSkelBatch");
}

const TCHAR *CSBatchExport::LongDesc()
{
	//TODO: Return long ASCII description (i.e. "Targa 2.0 Image File")
	return _T("Cool3D Skel Batch");
}

const TCHAR *CSBatchExport::ShortDesc() 
{			
	//TODO: Return short ASCII description (i.e. "Targa")
	return _T("FSkel Batch");
}

const TCHAR *CSBatchExport::AuthorName()
{			
	//TODO: Return ASCII Author name
	return _T("ll");
}

const TCHAR *CSBatchExport::CopyrightMessage() 
{	
	// Return ASCII Copyright message
	return _T("");
}

const TCHAR *CSBatchExport::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("");
}

const TCHAR *CSBatchExport::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int CSBatchExport::Version()
{				
	//TODO: Return Version number * 100 (i.e. v3.01 = 301)
	return 100;
}

void CSBatchExport::ShowAbout(HWND hWnd)
{			
	// Optional
}

BOOL CSBatchExport::SupportsOptions(int ext, DWORD options)
{
	// TODO Decide which options to support.  Simply return
	// true for each option supported by each Extension 
	// the exporter supports.

	return TRUE;
}

int	CSBatchExport::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)
{
	//TODO: Implement the actual file Export here and 
	//		return TRUE If the file is exported properly

	FSkelFile skelFile;
	BatchExpDlg dlg;
	Interface8* i8 = (Interface8*)i;
	unsigned long flags = Interface8::kSuppressPrompts | Interface8::kUseFileUnits;
	char szFile[260];
	if( dlg.Run(szFile) )
	{
		ExportData export;
		export.LoadSkelBatchFile(szFile);

		// 读取配置文件数据，循环读取max并且导出
		INT maxNum = export.GetMaxNum();
		for( INT maxi=0; maxi<maxNum; maxi++ )
		{
			LPCTSTR szMaxFile = export.GetMaxFile(maxi);
			if( i8->IsMaxFile( szMaxFile ) )
			{
				i8->LoadFromFile( szMaxFile, flags );

				// 一个max只能包含一个动作
				const tagMaxNode *ptagNode = export.GetNode(maxi,0);

				skelFile.LoadFromFile( ptagNode->strExportPath.c_str() );

				g_msgBox.Create( hInstance, IDD_EXPINFO, IDC_OUT );

				//--build ExpSkeleton
				g_msgBox.ShowMsg( _T("正在输出骨骼信息...\r\n") );
				ExpSkeleton expSkel;
				int numBones = expSkel.BuildFromMaxExportInterface( ei );
				g_msgBox.ShowMsg( _T("--共有骨骼:%d,根节点个数:%d\r\n"), numBones, expSkel.GetNumRoot() );

				//--add frames to file
				TimeValue start,end;
				Max_GetAniRange( i, start, end );

				bool remove = skelFile.RemoveTrack( ptagNode->strNodeName.c_str() );
				if( remove )
					g_msgBox.ShowMsg( _T("旧的Track已经替代.\r\n") );
				int numFrm = skelFile.AddTrack( ptagNode->strNodeName.c_str(), &expSkel, start, end, ptagNode->sampleRate );
				g_msgBox.ShowMsg( _T("新Track已经加入,采样%d帧.\r\n"), numFrm );

				//--write file
				if( skelFile.WriteToFile( ptagNode->strExportPath.c_str() ) )
				{
					g_msgBox.ShowMsg( _T("文件写入成功.\r\n已有Track:\r\n") );
					for( set<tstring>::iterator iter=skelFile.m_tracks.begin();
						iter!=skelFile.m_tracks.end(); iter++ )
					{
						const tstring& str = (*iter);
						g_msgBox.ShowMsg( _T("%s\r\n"), str.c_str() );
					}
				}
				else
				{
					g_msgBox.ShowMsg( _T("文件写入失败!\r\n") );
				}
			}
		}
	}

	return TRUE;
}