// launcher.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "launcher.h"
#include "launcherDlg.h"

#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ClauncherApp
BEGIN_MESSAGE_MAP(ClauncherApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
// construction / destruction
//-----------------------------------------------------------------------------
ClauncherApp::ClauncherApp()
{
	g_StrTable.Init(NULL, _T("launcher\\launcher_table.xml"));
}

ClauncherApp::~ClauncherApp()
{
	g_StrTable.Destroy();
}


// 唯一的一个 ClauncherApp 对象
ClauncherApp theApp;

// ClauncherApp 初始化
BOOL ClauncherApp::InitInstance()
{
	Util util;
	std::vector<tstring> tokens;
	util.StringToToken(tokens, AfxGetApp()->m_lpCmdLine);

	// 如果是复制进程，需要等待原launcher.exe退出，然后覆盖并运行之，最后退出
    //原来是tokens.size()>1 修改==2  begin modify by alex  @ 2009.12.16
    //tw的beanfun启动lancher的参数为 tw uid pwd
	//if( tokens.size() > 1 )
    if( tokens.size()==2 )  
	{
		// 等待原launcher.exe退出
		HANDLE hProcessorig = (HANDLE)_ttoi(tokens[1].c_str());
		::WaitForSingleObject(hProcessorig, INFINITE);
		::CloseHandle(hProcessorig);

		// 获取自身EXE路径
		const int MAX_STRING_SIZE = 512;
		TCHAR szTemp[MAX_STRING_SIZE];
		GetModuleFileName(NULL, szTemp, sizeof(szTemp)/sizeof(TCHAR));

		// 复制覆盖
		::CopyFile(szTemp, tokens[0].c_str(), FALSE);
		::Sleep(3000);	// 等待一下

		// 运行之
		ShellExecute( NULL, _T("open"), tokens[0].c_str(), NULL, NULL, SW_SHOWNORMAL );
		return FALSE;	// 退出
	}

	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("Loong Launcher"));


	// 检查语言表 是否正确读入
	if( !g_StrTable.IsLoaded() )
	{
		//该程序已经存在或者与此相同互斥的程序已经启动
		MessageBox(NULL, _T("File: launcher\\launcher_table.xml could not found!"), _T("Error"), MB_OK);
		return FALSE;
	}


	// 不允许重复启动
	HANDLE hMutex = ::CreateMutex(NULL, FALSE, _T("Loong_launcher"));
	if( GetLastError() == ERROR_ALREADY_EXISTS )
	{
		//该程序已经存在或者与此相同互斥的程序已经启动
		MessageBox(NULL, g_StrTable[_T("AlreadyRunning")], g_StrTable[_T("Warning")], MB_OK);
		return FALSE;
	}

	CLauncherDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();

	return FALSE;
}

