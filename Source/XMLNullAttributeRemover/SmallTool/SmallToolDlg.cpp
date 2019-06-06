// SmallToolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SmallTool.h"
#include "SmallToolDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CSmallToolDlg 对话框




CSmallToolDlg::CSmallToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSmallToolDlg::IDD, pParent)
	, m_szInfo(_T(""))
	, m_bBackUp(FALSE)
	, m_bNotifyThreadEnd(false)
	, m_hThread(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSmallToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_szInfo);
	DDX_Control(pDX, IDC_PROGRESS1, m_ProgressBar);
}

BEGIN_MESSAGE_MAP(CSmallToolDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_OPENSINGLEFILE, &CSmallToolDlg::OnFileOpensinglefile)
	ON_COMMAND(ID_FILE_FOLDER, &CSmallToolDlg::OnFileFolder)
	ON_BN_CLICKED(IDC_CHECK_BACKUP, &CSmallToolDlg::OnBnClickedCheckBackup)
	ON_COMMAND(IDM_ABOUT, &CSmallToolDlg::OnAbout)
	ON_MESSAGE(WM_UPDATEDATA, &CSmallToolDlg::OnUpdateDataMessage)
	ON_MESSAGE(WM_THREADEND, &CSmallToolDlg::OnThreadEnd)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CSmallToolDlg 消息处理程序

BOOL CSmallToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_menu.LoadMenu(IDR_MENU1);
	SetMenu(&m_menu);
	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSmallToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSmallToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CSmallToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//处理单个文件
void CSmallToolDlg::OnFileOpensinglefile()
{
	static TCHAR szFilter[] = _T("XML Files (*.xml)|*.xml||");
	CFileDialog dlg(TRUE, _T("xml"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this, 0);
	if (dlg.DoModal() == IDOK)
	{
		m_szInfo = "";
		CString szFilePath = dlg.GetPathName();
		CString szFileName = dlg.GetFileTitle();
		HandleOneXmlFile(szFilePath.GetBuffer());
		m_ProgressBar.SetRange(0, 1);
		PostMessage(WM_UPDATEDATA, 1,0);
	}

}

//处理单个XML文件
void CSmallToolDlg::HandleOneXmlFile(const char *filename)
{
	ULONGLONG oldSize;
	CFileStatus fStatus;   
	if   (CFile::GetStatus(filename, fStatus))   
	{   
		oldSize = fStatus.m_size;   
	}
	TiXmlDocument doc;
	doc.LoadFile(filename);
	TiXmlElement *RootElement = doc.RootElement();
	if (!RootElement)
		return;
	TiXmlElement *Child = RootElement->FirstChildElement();
	RemoveNullAttribute(Child);
	if (m_bBackUp)
	{
		CString szNewFileName = filename;
		szNewFileName += ".bak";
		MoveFile(filename, szNewFileName.GetBuffer());
		doc.SaveFile(filename);
	}
	else
	{
		doc.SaveFile(filename);
	}
	ULONGLONG newSize;
	if (CFile::GetStatus(filename, fStatus))   
	{   
		newSize = fStatus.m_size;   
	}
	SetInfo(filename, oldSize, newSize);

}

//去掉空属性
void CSmallToolDlg::RemoveNullAttribute(TiXmlElement *child)
{
	for (; child != NULL; child = child->NextSiblingElement())
	{
		TiXmlElement * childchild = child->FirstChildElement();
		if (childchild)
			RemoveNullAttribute(childchild);
		TiXmlAttribute *attr = child->FirstAttribute();
		while (attr != NULL)
		{
			if (strcmp(attr->Value(), "") == 0)
			{
				TiXmlAttribute *attrTemp = attr;
				attr = attr->Next();
				child->RemoveAttribute(attrTemp->Name());
			}
			else
			{
				attr = attr->Next();
			}
		}
	}
}

//EDIT CONTROL 信息显示
void CSmallToolDlg::SetInfo(const char *filename, ULONGLONG oldSize, ULONGLONG newSize)
{
	CString szInfo;
	//szInfo += filename;
	szInfo.Format("处理文件: %s, ", filename);
	CString szSrcfileSizeInfo;
	szSrcfileSizeInfo.Format("原文件大小: %dbyte, ", oldSize);
	CString szNewFileSizeInfo;
	szNewFileSizeInfo.Format("新文件大小: %dbyte", newSize);

	m_szInfo += szInfo;
	m_szInfo += szSrcfileSizeInfo;
	m_szInfo += szNewFileSizeInfo;
	m_szInfo += "\r\n";
	
}

//处理整个文件夹
void CSmallToolDlg::OnFileFolder()
{
	CString   strFilePath;  
	char   pszBuffer[MAX_PATH];  
	BROWSEINFO bi;     
	LPITEMIDLIST pidl;  
	bi.hwndOwner = NULL;  
	bi.pidlRoot = NULL;  
	bi.pszDisplayName = pszBuffer;  
	bi.lpszTitle = _T("请选择所处理的文件夹");  
	bi.ulFlags = BIF_RETURNFSANCESTORS   |   BIF_RETURNONLYFSDIRS;  
	bi.lpfn = NULL;  
	bi.lParam = 0;  
	if((pidl = SHBrowseForFolder(&bi)) != NULL)
	{
		if(SHGetPathFromIDList(pidl, pszBuffer))  
		{
			CString strTemp(pszBuffer);
			strFilePath = strTemp;  
			if (strFilePath.GetLength() <= 1)  
			{
			}  
			else if (strFilePath.Right(1) != _T("\\"))  
			{
				strFilePath += _T("\\");
			}
		}
		GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_FILE_FOLDER, MF_DISABLED | MF_GRAYED);
		GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_FILE_OPENSINGLEFILE, MF_DISABLED | MF_GRAYED);
		SearchAndAddToFileLists(strFilePath);
		m_szInfo = "";
		int nSize = static_cast<int>(m_listFilePaths.size());
		m_ProgressBar.SetRange32(0, nSize);
		m_hThread = CreateThread(NULL, 0, ThreadFunc, this, 0, NULL);
	}
	
}

//递归处理所有子文件夹及子文件
void CSmallToolDlg::SearchAndAddToFileLists(CString szFile)
{
	CFileFind finder;
	szFile += "*.*";
	BOOL bWorking = finder.FindFile(szFile);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if(finder.IsDirectory() && !finder.IsDots())
		{
			CString strPath = finder.GetFilePath(); 
			CString strTitle = finder.GetFileName();
			strPath += "\\*.*";
			SearchAndAddToFileLists(strPath);
		}
		else if (!finder.IsDirectory() && !finder.IsDots())
		{
			CString szPathExt = finder.GetFilePath().Right(3);
			if (szPathExt == "xml")
				m_listFilePaths.push_back(finder.GetFilePath());
		}
	}

}

void CSmallToolDlg::OnBnClickedCheckBackup()
{
	m_bBackUp = !m_bBackUp;
}

void CSmallToolDlg::OnAbout()
{
	CAboutDlg dlg;
	dlg.DoModal();
}

//处理文件夹中子文件的工作线程
DWORD WINAPI CSmallToolDlg::ThreadFunc(PVOID param)
{
	CSmallToolDlg *p = static_cast<CSmallToolDlg*>(param);
	::InterlockedExchange((LONG volatile*)&p->m_bNotifyThreadEnd, FALSE);
	std::list<CString>::iterator ed = p->m_listFilePaths.end();
	int i = 0;
	int j = 0;
	for (std::list<CString>::iterator bg = p->m_listFilePaths.begin(); 
		 bg != ed; ++bg, ++i, j++)
	{
		p->HandleOneXmlFile(bg->GetBuffer());
		if (j > 50)
		{
			p->PostMessage(WM_UPDATEDATA, i, 0);
			j = 0;
		}
		if (p->m_bNotifyThreadEnd == TRUE)
			break;
	}
	p->PostMessage(WM_UPDATEDATA, i,0);
	p->PostMessage(WM_THREADEND);
	return 0;
}

//通知UI线程刷新
LRESULT CSmallToolDlg::OnUpdateDataMessage(WPARAM wparam, LPARAM lparam)
{
	m_ProgressBar.SetPos(static_cast<int>(wparam));
	UpdateData(FALSE);
	::SendMessage(GetDlgItem(IDC_EDIT1)->GetSafeHwnd(), EM_LINESCROLL, 0, ::SendMessage(GetDlgItem(IDC_EDIT1)->GetSafeHwnd(), EM_GETLINECOUNT, 0, 0));
	return 0;
}

//通知线程结束
LRESULT CSmallToolDlg::OnThreadEnd(WPARAM wparam, LPARAM lparam)
{
	GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_FILE_FOLDER, MF_ENABLED);
	GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_FILE_OPENSINGLEFILE, MF_ENABLED);
	m_hThread = NULL;
	return 0;
}

//安全退出
void CSmallToolDlg::OnClose()
{
	if (m_hThread == NULL)
		CDialog::OnClose();
	::InterlockedExchange((LONG volatile*)&m_bNotifyThreadEnd, TRUE);
	if (WAIT_OBJECT_0 == WaitForSingleObject(m_hThread, INFINITE))
		CDialog::OnClose();
}
