// WorldEditor.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "MainFrm.h"
#include ".\WorldEditor.h"
#include "Cool3DView.h"
#include "PluginMgr.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


bool CWorldEditorApp::m_bMyActive=true;
// CWorldEditorApp

BEGIN_MESSAGE_MAP(CWorldEditorApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
END_MESSAGE_MAP()


// CWorldEditorApp 构造

CWorldEditorApp::CWorldEditorApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
	
	// 禁止屏幕保护及电源管理
	::SystemParametersInfo(SPI_SETLOWPOWERTIMEOUT, 0, NULL, 0);
	::SystemParametersInfo(SPI_SETPOWEROFFTIMEOUT, 0, NULL, 0);
	::SystemParametersInfo(SPI_SETSCREENSAVETIMEOUT, 0, NULL, 0);
}


// 唯一的一个 CWorldEditorApp 对象

CWorldEditorApp theApp;

// CWorldEditorApp 初始化

BOOL CWorldEditorApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControls()。否则，将无法创建窗口。
	InitCommonControls();

	CWinApp::InitInstance();

	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	// 若要创建主窗口，此代码将创建新的框架窗口
	// 对象，然后将其设置为应用程序的主窗口对象
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// 创建并加载带有其资源的框架
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);
	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	// 仅当存在后缀时才调用 DragAcceptFiles，
	//  在 SDI 应用程序中，这应在 ProcessShellCommand  之后发生

	_set_se_translator(Cool3D::Cool_SEH_Translation);
	return TRUE;
}

int CWorldEditorApp::Run()
{
#ifdef _DEBUG
	return CWinApp::Run();
#else
	try
	{
		return CWinApp::Run();
	}
	catch(Cool3D::Exception e)
	{
		AfxMessageBox(_T("编辑器发生异常，请将log文件发给程序人员!"));
		exit(0);
	}
	return 0;
#endif
}


// CWorldEditorApp 消息处理程序



// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CWorldEditorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CWorldEditorApp 消息处理程序


extern bool g_bNoSleep;
BOOL CWorldEditorApp::OnIdle(LONG lCount)
{
	// TODO: 在此添加专用代码和/或调用基类
	//if(AfxGetMainWnd()->IsWindowVisible())
	//ASSERT(m_pMainWnd->IS_STATIC_CLASS(CMainFrame));	
	CMainFrame* pFrame=(CMainFrame*)m_pMainWnd;
	pFrame->UpdateUI();

	Cool3DView::Inst()->Render();
	PluginMgr::Inst()->RenderPreview();

	//--限制帧数不超过30FPS
	if(!g_bNoSleep)
	{
		int sleepMS=(1000/30)-Cool3D::Kernel::Inst()->GetDeltaTimeDW();
		if(sleepMS>0)
			Sleep(sleepMS);

		if(!m_bMyActive)
			Sleep(200);
	}
	//return CWinApp::OnIdle(lCount);
	return true;
}
