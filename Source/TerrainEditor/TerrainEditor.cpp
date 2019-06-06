// TerrainEditor.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "TerrainEditor.h"
#include "PlugTerrainEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//	注意！
//
//		如果此 DLL 动态链接到 MFC
//		DLL，从此 DLL 导出并
//		调入 MFC 的任何函数在函数的最前面
//		都必须添加 AFX_MANAGE_STATE 宏。
//
//		例如:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CTerrainEditorApp

BEGIN_MESSAGE_MAP(CTerrainEditorApp, CWinApp)
END_MESSAGE_MAP()


// CTerrainEditorApp 构造

CTerrainEditorApp::CTerrainEditorApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CTerrainEditorApp 对象

CTerrainEditorApp theApp;


// CTerrainEditorApp 初始化

BOOL CTerrainEditorApp::InitInstance()
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
		return _T("TerrainEditor");
	}
	__declspec(dllexport) void* _cdecl Plug_NewObject(const TCHAR* szClassName,DWORD param1,const TCHAR* param2)
	{
		g_editor=new PlugTerrainEditor;
		return g_editor;
	}
	__declspec(dllexport) void _cdecl Plug_DelObject(const TCHAR* szClassName,void *pObj,const TCHAR* param2)
	{
		PlugTerrainEditor* pPlug=(PlugTerrainEditor*)pObj;
		delete pPlug;
		ASSERT(pPlug==g_editor);
		g_editor=NULL;
	}
}