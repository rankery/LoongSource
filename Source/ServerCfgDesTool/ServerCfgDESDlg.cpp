// ServerCfgDESDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ServerCfgDES.h"
#include "ServerCfgDESDlg.h"
#include ".\ServerCfgDESdlg.h"

#include "../vEngine/vEngine.h"

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
END_MESSAGE_MAP()


// CServerCfgDESDlg 对话框



CServerCfgDESDlg::CServerCfgDESDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CServerCfgDESDlg::IDD, pParent)
	, m_strFileName(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	vEngine::InitEngine();
	m_pUtil = "Util";
}

CServerCfgDESDlg::~CServerCfgDESDlg()
{
	vEngine::DestroyEngine();
}

void CServerCfgDESDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FILE_NAME, m_strFileName);
	DDV_MaxChars(pDX, m_strFileName, MAX_PATH - 1);
}

BEGIN_MESSAGE_MAP(CServerCfgDESDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnBnClickedButtonBrowse)
END_MESSAGE_MAP()


// CServerCfgDESDlg 消息处理程序

BOOL CServerCfgDESDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将\“关于...\”菜单项添加到系统菜单中。

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
	
	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

void CServerCfgDESDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CServerCfgDESDlg::OnPaint() 
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
HCURSOR CServerCfgDESDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CServerCfgDESDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	
	//获取被加密文件名
	UpdateData();
	if("" == m_strFileName)
	{
		//没有输入被加密文件，直接返回
		return;
	}

	//加密
	if(EncryptAndSave())
	{
		MessageBox(_T("文件加密成功！"), _T("加密工具"), MB_OK);
	}
	else
	{
		MessageBox(_T("文件加密失败！"), _T("加密工具"), MB_OK);
	}
#ifdef _DEBUG
	DecryptAndSave();	//调试用
#endif

	return;

	//加密完成后不退出
//	OnOK();
}

//根据被加密文件名生成加密文件名(将后缀改为.dat)
CString CServerCfgDESDlg::GetSaveFileName() const
{
	ASSERT(m_strFileName != "");

	CString strFileName(m_strFileName);
	strFileName.Replace(_T(".ini"), _T(".inc"));

	return strFileName;
}

//随机生成加密号
INT CServerCfgDESDlg::GenDesCode()
{
	return m_pUtil->Rand() % 20;
}

//读取被加密文件，并加密后保存
BOOL CServerCfgDESDlg::EncryptAndSave()
{
	//获得加密后文件名(根据被加密文件名生成)
	CString strSaveFileName = GetSaveFileName();
	//随机生成加密号
	INT nDesCode = GenDesCode();

	TObjRef<DiscIOMgr>	pIO = "DiscIOMgr";
	DWORD dwHFileIn, dwHFileEnc;		//输入输出文件句柄
	DWORD dwWritten = 0, dwInFileSize;	//一次写入文件的大小(字符)

	//打开被加密文件
	dwHFileIn = pIO->Create(m_strFileName,
			GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, 
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, 
			NULL);
	if(dwHFileIn == INVALID_FILE_HANDLE)
	{
		return FALSE;
	}

	//打开加密后生成文件
		dwHFileEnc = pIO->Create(strSaveFileName,
			GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, CREATE_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, 
			NULL);
	if(dwHFileEnc == INVALID_FILE_HANDLE)
	{
		pIO->Close(dwHFileIn);
		return FALSE;
	}

	//将密钥对应的号码保存到文件中(大小占sizeof(nDesCode)字符)//??
	pIO->Write(dwHFileEnc, &nDesCode, sizeof(nDesCode), &dwWritten, NULL);

	//得到被加密文件大小
	dwInFileSize = pIO->GetSize(dwHFileIn);
	//调整大小为8的整数倍(加密字段要求)
	dwInFileSize = (((dwInFileSize - 1) >> 3) << 3) + 8;
	
	//申请读取内存
	char *pReadBuf = new char[dwInFileSize];
	if(NULL == pReadBuf)
	{
		pIO->Close(dwHFileIn);
		pIO->Close(dwHFileEnc);
		return FALSE;
	}
	memset(pReadBuf, ' ', dwInFileSize);
	
	//读取文件，加密后保存
	pIO->Read(dwHFileIn, pReadBuf, dwInFileSize);
	m_pUtil->Encrypt(pReadBuf, pReadBuf, dwInFileSize, nDesCode, 1);
	pIO->Write(dwHFileEnc, pReadBuf, dwInFileSize, &dwWritten, NULL);

	//回收内存，并关闭文件
	delete []pReadBuf;
	pIO->Close(dwHFileIn);
	pIO->Close(dwHFileEnc);
	
	return TRUE;
}

//读取加密文件，并解密后保存(测试用)
BOOL CServerCfgDESDlg::DecryptAndSave()
{
	//获得加密后文件名(根据被加密文件名生成)
	CString strSaveFileName = GetSaveFileName();

	TObjRef<DiscIOMgr>	pIO = "DiscIOMgr";
	DWORD dwHFileIn, dwHFileDec;		//输入输出文件句柄
	DWORD dwWritten = 0, dwInFileSize;	//一次写入文件的大小(字符)

	//打开被加密文件
		dwHFileIn = pIO->Create(strSaveFileName,
			GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, 
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, 
			NULL);

	if(dwHFileIn == INVALID_FILE_HANDLE)
	{
		return FALSE;
	}

	//打开加密后生成文件
	dwHFileDec = pIO->Create(_T("out_dec.ini"),
		GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, 
		NULL);

	if(dwHFileDec == INVALID_FILE_HANDLE)
	{
		pIO->Close(dwHFileIn);
		return FALSE;
	}

	//得到被加密文件大小
	dwInFileSize = pIO->GetSize(dwHFileIn);
	
	//dwInFileSize = n * 8 + 4
	ASSERT(((dwInFileSize >> 3) << 3) + 4 == dwInFileSize);

	//申请读取内存
	char *pReadBuf = new char[dwInFileSize];
	if(NULL == pReadBuf)
	{
 		pIO->Close(dwHFileIn);
 		pIO->Close(dwHFileDec);
		return FALSE;
	}

	//读取文件
	pIO->Read(dwHFileIn, pReadBuf, dwInFileSize);
	//解密后保存
	m_pUtil->Decrypt(pReadBuf + 4, pReadBuf + 4, dwInFileSize - 4, *(INT*)pReadBuf, 1);
	pIO->Write(dwHFileDec, pReadBuf + 4, dwInFileSize - 4, &dwWritten, NULL);

	//回收内存，并关闭文件
	delete []pReadBuf;
	pIO->Close(dwHFileIn);
	pIO->Close(dwHFileDec);

	return TRUE;
}

//设置被加密文件
void CServerCfgDESDlg::OnBnClickedButtonBrowse()
{
	// TODO: 在此添加控件通知处理程序代码
	CString		szFilter=_T("INI文件(*.ini)|*.ini||");   
	CFileDialog	dlgFile(TRUE, NULL, NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, AfxGetMainWnd());   

	INT_PTR nResponse = dlgFile.DoModal();
	if (nResponse == IDOK)
	{
		m_strFileName = dlgFile.GetPathName();
		UpdateData(FALSE);
	}
}
