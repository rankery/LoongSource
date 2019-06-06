// NavMapEditor.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "NavMapEditor.h"
#include "PlugNavMapEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//


// CNavMapEditorApp

BEGIN_MESSAGE_MAP(CNavMapEditorApp, CWinApp)
END_MESSAGE_MAP()


// CNavMapEditorApp construction

CNavMapEditorApp::CNavMapEditorApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CNavMapEditorApp object

CNavMapEditorApp theApp;


// CNavMapEditorApp initialization

BOOL CNavMapEditorApp::InitInstance()
{
	CWinApp::InitInstance();
	::SetCurrentDirectory(Kernel::Inst()->GetWorkPath());

	return TRUE;
}

extern "C"
{
	__declspec(dllexport) bool _cdecl Plug_Init()
	{
		return true;
	}
	__declspec(dllexport) void _cdecl Plug_Destroy()
	{}

	__declspec(dllexport) int _cdecl Plug_GetNumClass()
	{
		return 1;
	}

	__declspec(dllexport) const TCHAR* _cdecl Plug_GetClassName(int i)
	{
		return _T("NavMapEditor");
	}
	__declspec(dllexport) void* _cdecl Plug_NewObject(const TCHAR* szClassName,DWORD param1,const TCHAR* param2)
	{
		g_navEditor=new PlugNavMapEditor;
		return g_navEditor;
	}
	__declspec(dllexport) void _cdecl Plug_DelObject(const TCHAR* szClassName,void *pObj,const TCHAR* param2)
	{
		PlugNavMapEditor* pPlug=(PlugNavMapEditor*)pObj;
		delete pPlug;
		ASSERT(pPlug==g_navEditor);
		g_navEditor=NULL;
	}
}