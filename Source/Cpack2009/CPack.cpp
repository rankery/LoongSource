// CPack.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "CPack.h"

#include "MainFrm.h"
#include "CPackDoc.h"
#include "LeftView.h"
#include "ProgressDialog.h"

#include "cpkcore\cpkcore.h"
#include "memmgr\memmgr.h"
#include "cpkcore\disciomgr.h"
#include "filtermgr\filtermgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CDiscIOMgr	DiscMgr();
CFilterMgr	FilterMgr();

extern HWND  g_hWndFileName;
extern HWND  g_hWndProgress;
extern HWND  g_hWndInfo;



/////////////////////////////////////////////////////////////////////////////
// CCPackApp


BEGIN_MESSAGE_MAP(CCPackApp, CWinApp)
	//{{AFX_MSG_MAP(CCPackApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCPackApp construction

CCPackApp::CCPackApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	CoInitialize(NULL);
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CCPackApp object

CCPackApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CCPackApp initialization

BOOL CCPackApp::InitInstance()
{
	AfxOleInit();
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
/*
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
*/
	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("SoftStar"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)


	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CCPackDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CLeftView));
	AddDocTemplate(pDocTemplate);

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if( strlen(cmdInfo.m_strFileName) != 0 
		&& 0 == _stricmp(cmdInfo.m_strFileName, TASK_FILE))
	{
		CStdioFile TextFile;
		CProgressDialog pd;
		pd.Create(IDD_PROGRESSDIALOG, NULL);
		pd.GetDlgItem(IDC_FILENAME, &g_hWndFileName);
		pd.GetDlgItem(IDC_PROGRESS1, &g_hWndProgress);
		pd.GetDlgItem(IDC_INFO, &g_hWndInfo);

		pd.ShowWindow(SW_SHOW);
		pd.UpdateWindow();


		// 临时加的命令行参数打包目录
		CString csFilename, cs;
		TextFile.Open((LPCTSTR)TASK_FILE, CFile::modeRead);

		while( TextFile.ReadString(csFilename) )
		{
			cs = csFilename; 

			csFilename += ".cpk";
			CPK* pCpk = new CPK;
			if( pCpk == NULL )
				_exit(0);

			pCpk->Create((LPCTSTR)csFilename);


			TCHAR szPath[_MAX_PATH];
			TCHAR szFindPath[_MAX_PATH];

			GetModuleFileName(NULL, szPath, sizeof(szPath));

			if (NULL != _tcsrchr(szPath, '\\'))
			{
				szPath[_tcsrchr(szPath, '\\') - szPath + 1] = 0;
			}

			lstrcat(szPath, (LPCTSTR)cs);

			strcpy(szFindPath, szPath);
			strcat(szFindPath, "\\*.*");

			lstrcat(szPath, "\\");

			WIN32_FIND_DATA finddata;
			HANDLE hHandle = FindFirstFile(szFindPath, &finddata);
			if (hHandle == INVALID_HANDLE_VALUE)
			{
				MessageBox(NULL, _T("没找到任何文件或目录"), NULL, MB_OK);
				_exit(0);
			}

			DWORD dwFatherCrc = pCpk->AddDirCyc(cs.GetBuffer(), 0);
			if( dwFatherCrc == GT_INVALID )
				dwFatherCrc = 0;

			char szTemp[MAX_PATH];
			BOOL bRet = FALSE;
			while(0 != FindNextFile(hHandle, &finddata))
			{
				if (0 != strcmp(finddata.cFileName, ".."))
				{
					strcpy(szTemp, szPath);
					strcat(szTemp, finddata.cFileName);
					bRet = pCpk->AddFile(szTemp, dwFatherCrc);

					if( bRet == FALSE )
					{
						INT nRet = MessageBox(NULL, "文件添加失败,是否继续...", \
							NULL, MB_OKCANCEL);

						if( nRet == IDCANCEL )
						{
							break;
						}
					}
				}
			}

			pCpk->Unload();
			delete pCpk;
			pCpk = NULL;

		}

		TextFile.Close();
		_exit(0);
	}


	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CCPackApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CCPackApp message handlers
