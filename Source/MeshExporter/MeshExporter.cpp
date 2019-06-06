/**********************************************************************
 *<
	FILE: MeshExporter.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/
#include "MeshExporter.h"

#include "SceneContext.h"
using namespace Cool3D;

#include "MsgBox.h"
#include "..\F3DMaxExp\ExpMtlOptionDlg.h"
#include "..\F3DMaxExp\F3DExpMtl.h"

#define MESHEXPORTER_CLASS_ID	Class_ID(0x376ca1a9, 0x69a48f14)

class MeshExporter : public SceneExport {
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
		int				DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);


		
		//Constructor/Destructor

		MeshExporter();
		~MeshExporter();		

};


class MeshExporterClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new MeshExporter(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID		ClassID() { return MESHEXPORTER_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("MeshExporter"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle

};



static MeshExporterClassDesc MeshExporterDesc;
ClassDesc2* GetMeshExporterDesc() { return &MeshExporterDesc; }


BOOL CALLBACK MeshExporterOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	static MeshExporter *imp = NULL;

	switch(message) {
		case WM_INITDIALOG:
			imp = (MeshExporter *)lParam;
			CenterWindow(hWnd,GetParent(hWnd));
			return TRUE;

		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return TRUE;
	}
	return FALSE;
}


//--- MeshExporter -------------------------------------------------------
MeshExporter::MeshExporter()
{

}

MeshExporter::~MeshExporter() 
{

}

int MeshExporter::ExtCount()
{
	//TODO: Returns the number of file name extensions supported by the plug-in.
	return 1;
}

const TCHAR *MeshExporter::Ext(int n)
{		
	//TODO: Return the 'i-th' file name extension (i.e. "3DS").
	switch(n)
	{
	case 0:	
		return _T("FSM");//static mesh model
	}
	return NULL;
}

const TCHAR *MeshExporter::LongDesc()
{
	//TODO: Return long ASCII description (i.e. "Targa 2.0 Image File")
	return _T("F3D Static mesh modle");
}
	
const TCHAR *MeshExporter::ShortDesc() 
{			
	//TODO: Return short ASCII description (i.e. "Targa")
	return _T("F3D Static Mesh");
}

const TCHAR *MeshExporter::AuthorName()
{			
	//TODO: Return ASCII Author name
	return _T("Yanliang Fang");
}

const TCHAR *MeshExporter::CopyrightMessage() 
{	
	// Return ASCII Copyright message
	return _T("");
}

const TCHAR *MeshExporter::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("");
}

const TCHAR *MeshExporter::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int MeshExporter::Version()
{				
	//TODO: Return Version number * 100 (i.e. v3.01 = 301)
	return 100;
}

void MeshExporter::ShowAbout(HWND hWnd)
{			
	// Optional
}

BOOL MeshExporter::SupportsOptions(int ext, DWORD options)
{
	// TODO Decide which options to support.  Simply return
	// true for each option supported by each Extension 
	// the exporter supports.

	return TRUE;
}


// Dummy function for progress bar
DWORD WINAPI DummyFun(LPVOID arg)
{
	return(0);
}


int _MyExport(const TCHAR *name,ExpInterface *ei)
{
	g_msgBox.Create(hInstance,IDD_EXPORTINFO,IDC_OUT);
	
	SceneContext::Inst()->ClearAll();
	
	g_msgBox.ShowMsg(_T("building nodes list...\r\n"));
	SceneContext::Inst()->BuildNodeList(ei->theScene);
	
	g_msgBox.ShowMsg(_T("exporting data...\r\n"));
	SceneContext::Inst()->ExportData();
	
	g_msgBox.ShowMsg(_T("writing file ...\r\n"));
	if(SceneContext::Inst()->WriteToFile(name, F3DExpMtl::s_bExportLMUV, F3DExpMtl::s_bExportBUUV,F3DExpMtl::s_szOriginNode.c_str()))
		g_msgBox.ShowMsg(_T("³É¹¦\r\n"));
	else
		g_msgBox.ShowMsg(_T("Ê§°Ü!\r\n"));
	
	//--free memory
	SceneContext::Inst()->ClearAll();

	return TRUE;
}

int	MeshExporter::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)
{
	//TODO: Implement the actual file Export here and 
	//		return TRUE If the file is exported properly

	/*if(!suppressPrompts)
		DialogBoxParam(hInstance, 
				MAKEINTRESOURCE(IDD_PANEL), 
				GetActiveWindow(), 
				MeshExporterOptionsDlgProc, (LPARAM)this);
	*/

	F3DExpMtl::s_pMsgBox=&g_msgBox;
	ExpMtlOptionDlg dlg;
	if ( dlg.Run(name) >= 0 )
		return _MyExport(name,ei);

	return 0;	
}