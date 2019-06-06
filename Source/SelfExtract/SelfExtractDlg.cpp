// SelfExtractDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SelfExtract.h"
#include "SelfExtractDlg.h"
#include "ExtractHanle.h"
#include "CPKCore/DiscIOMgr.h"
#include "MemMgr/ts_memmgr.h"
#include ".\selfextractdlg.h"
#include "ProcessWrapper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define PROGRESS_WIDTH		370
#define VERSION_LABLE			0x0b0c

struct tagVersion
{
	DWORD	dwLable;		//0x0b0c
	DWORD	dwVersion;		//版本号
};
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


// CSelfExtractDlg 对话框



CSelfExtractDlg::CSelfExtractDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelfExtractDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSelfExtractDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_FILE, m_staticFile);
	DDX_Control(pDX, IDC_PROGRESS1, m_Progress);
	DDX_Control(pDX, IDC_STA_TITLE, Title);
	DDX_Control(pDX, IDC_UPDATE, Update);
	DDX_Control(pDX, IDCANCEL, Cancel);
}

BEGIN_MESSAGE_MAP(CSelfExtractDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_UPDATE, OnBnClickedUpdate)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CSelfExtractDlg 消息处理程序

BOOL CSelfExtractDlg::OnInitDialog()
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

	m_Progress.SetRange( 0, 100 );

	m_bTerminateThread = FALSE;
	// TODO: 在此添加额外的初始化代
	Check();

	GetDlgItem(IDC_STA_TITLE)->SetWindowText(Translate("<龙>手动更新"));
	GetDlgItem(IDC_STATIC_FILE)->SetWindowText(Translate("准备更新"));
	GetDlgItem(IDC_UPDATE)->SetWindowText(Translate("更新"));
	GetDlgItem(IDCANCEL)->SetWindowText(Translate("取消"));
	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

void CSelfExtractDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSelfExtractDlg::OnPaint() 
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

DWORD	CSelfExtractDlg::GetUnPackSize( CPKVfs* pCpK )
{
	ASSERT( pCpK != NULL );

	tagCPKHeader* pHeader = pCpK->GetCPKHeader();
	ASSERT( pHeader != NULL );
	
	tagCPKTable* pTable = pCpK->GetCPKTable();
	ASSERT( pTable != NULL );

	DWORD dwUnPackSize = 0;

	for( DWORD i =0 ; i < pHeader->dwValidTableNum; i++ )
	{
		dwUnPackSize += pTable[ i ].dwOriginSize;
	}
	
	return dwUnPackSize;

}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CSelfExtractDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

DWORD CSelfExtractDlg::ThreadUnPack( LPVOID* pParam )
{
	CSelfExtractDlg*	pDlg = (CSelfExtractDlg*)pParam;
	CString				strVersionPath;
	std::string			strLocalGameVersion;
	CTSMemMgr			TSMemMgr;
	CExtractHanle*		pReadCPK = new CExtractHanle(&TSMemMgr);
	CPKVfs*				pWriteCPK = new CPKVfs( &TSMemMgr );
	CONST INT			MAX_STRING_SIZE = 512;
	TCHAR				szTemp[MAX_STRING_SIZE];

	// 读入补丁
	GetModuleFileName( NULL, szTemp, MAX_STRING_SIZE );
	if( FALSE == pReadCPK->LoadExtractFile( szTemp ) )
	{
		//加载补丁文件失败
		pDlg->MessageBox(Translate("没有找到补丁文件,升级失败!"));
		goto __UnPack_End;
	}

	// 先通过读取注册表，查找游戏路径
	if( pDlg->ReadReg("SOFTWARE\\TENGREN\\LOONG", "InstallDir", szTemp) ) // 注册表包含此键
	{
		pDlg->m_strAppPath = szTemp;
		strVersionPath = pDlg->m_strAppPath  + "\\launcher\\launcher.ini";
		if( CDiscIOMgr::IsExist(strVersionPath.GetBuffer()) )	// 文件是否真实存在
		{
			::GetPrivateProfileString(_T("Launcher"), _T("LocalGame"), NULL,  szTemp, MAX_STRING_SIZE, strVersionPath.GetBuffer());
			strLocalGameVersion = szTemp;

			// 比较更新程序和游戏程序的版本号
			if( pReadCPK->IsVersionCorrect( strLocalGameVersion ) )
				goto _version_ok;
		}
	}
		
	// 通过注册表查找游戏以及版本失败， 就手动选择游戏程序路径
	if( !pDlg->SelectPath() )
	{
		pDlg->GetDlgItem( IDC_UPDATE )->EnableWindow( TRUE );
		CloseHandle( pDlg->m_Mutex );
		goto __UnPack_End;
	}

	// 获取当前游戏版本
	strVersionPath = pDlg->m_strAppPath  + "\\launcher\\launcher.ini";
	if( !CDiscIOMgr::IsExist( strVersionPath.GetBuffer() ) )
	{
		pDlg->MessageBox(Translate("获得程序版本信息错误，请确认选择的目录为游戏目录"), "Error");
		pDlg->GetDlgItem( IDC_UPDATE )->EnableWindow( TRUE );
		goto __UnPack_End;
	}
	::GetPrivateProfileString(_T("Launcher"), _T("LocalGame"), NULL,  szTemp, MAX_STRING_SIZE, strVersionPath.GetBuffer());
	strLocalGameVersion = szTemp;

	//比较更新程序和游戏程序的版本号
	if( !pReadCPK->IsVersionCorrect( strLocalGameVersion ) )
	{
		pDlg->MessageBox(Translate("此更新程序和游戏的版本不匹配"), "Error");
		pDlg->GetDlgItem( IDC_UPDATE )->EnableWindow( TRUE );
		goto __UnPack_End;
	}


_version_ok:

	INT nPos = strVersionPath.Find( _T(":\\") );
	strVersionPath.Delete( nPos + 2, strVersionPath.GetLength() - nPos - 2);
	//是否有足够的空间
	if( FALSE == CDiscIOMgr::ClueFreeSpace(strVersionPath.GetBuffer(), pDlg->GetUnPackSize( pReadCPK ) ) )
	{
		pDlg->MessageBox("磁盘空间不足", "Error");
		goto __UnPack_End;		
	}

	//读取翻译信息
	{
		ReloadTranslate(pDlg->m_strAppPath.GetBuffer());
		pDlg->GetDlgItem(IDC_STA_TITLE)->SetWindowText(Translate("<龙>手动更新"));
		pDlg->GetDlgItem(IDC_STATIC_FILE)->SetWindowText(Translate("准备更新"));
		pDlg->GetDlgItem(IDC_UPDATE)->SetWindowText(Translate("更新"));
		pDlg->GetDlgItem(IDCANCEL)->SetWindowText(Translate("取消"));
	}


	//解压文件
	tagCPKHeader *pCPKHeader = pReadCPK->GetCPKHeader();
	tagCPKTable  *pTable = pReadCPK->GetCPKTable();

	pDlg->m_dwTotalFileNum = pCPKHeader->dwFileNum;
	DWORD dwUnPackNum = 0;
	BOOL bRet = pDlg->UnPackCpk(0, dwUnPackNum, pReadCPK, pWriteCPK);
	 

	if( TRUE == bRet )
	{
		//下载完成
		pDlg->SetProgressPos(100);

		//删除升级文件
		//::DeleteFile(m_szDiscoFileName);
		//更新版本到文件
		pDlg->RefreshVersion(pReadCPK->GetVersionDest());

		//
		pDlg->m_staticFile.SetWindowText( Translate("更新完成") );

		pDlg->GetDlgItem( IDCANCEL )->SetWindowText( Translate("退出") );
		//给出提示信息
		//pDlg->MessageBox("升级成功 !");
		//ShowWindow(GetDlgItem(m_hWnd, ID_BUTN_UPDATE), SW_NORMAL);
		//ShowWindow(GetDlgItem(m_hWnd, ID_BUTN_CANCEL), SW_HIDE);

	}
	else
	{
		pDlg->MessageBox(Translate("解压升级被中断, 升级失败 !"), "Error");	
	}

__UnPack_End:

	// 关闭文件
	pReadCPK->Unload();
	FREE(pReadCPK);
	pWriteCPK->Unload();
	FREE(pWriteCPK);
	CloseHandle( pDlg->m_Mutex );
	return TRUE;	
}

BOOL CSelfExtractDlg::UnPackCpk(DWORD dwFatherCRC, DWORD& dwUnPackNum, CPKVfs *pReadCpk, CPKVfs *pWriteCpk)
{
	Sleep( 1 );		//休息缓冲

	CString strPath;
	CString strWndText;

	if( dwFatherCRC != 0 )
	{
		//先查找该项
		INT nTableIndex = pReadCpk->GetTableIndexFromeCrc(dwFatherCRC);
		if( nTableIndex == GT_INVALID )
		{
			ASSERT( FALSE && Translate("cpk文件中未找到该项") );
			return TRUE;
		}

		//打开文件映像句柄
		//DWORD dwFileCPKHandle = pReadCpk->GetFileInfo(nTableIndex);	//打开
		//if( 0 == dwFileCPKHandle )
		//{
		//	ASSERT( FALSE && "cpk打开文件失败" );
		//	return TRUE;
		//}

		char lpFileName[MAX_PATH];
		BOOL	bRet = pReadCpk->GetFileOrDirName( nTableIndex, lpFileName );
		if( bRet == FALSE )
		{
			ASSERT( FALSE && Translate("cpk打开文件失败") );
			return TRUE;
		}

//		CPKFile *pCPKFileHandle = (CPKFile*)dwFileCPKHandle;
		
		CString strPath;
		CString strWndText = m_strAppPath + "\\" + lpFileName;//pCPKFileHandle->szFullName ;
		strPath.Format( "%s %s ...",Translate("解压"),strWndText.GetBuffer() );
		m_staticFile.SetWindowText( strWndText.GetBuffer() );

		//虽然是路径，但如果是ｃｐｋ格式，那说明在磁盘上是cpk文件
		TCHAR szFullName[MAX_PATH];
		LPTSTR lpExtName = NULL;

		lstrcpy(szFullName, lpFileName );//pCPKFileHandle->szFullName);
		lpExtName = _tcsrchr(szFullName, _T('.'));

		//判断是不是cpk文件
		if( lpExtName && _tcsicmp(lpExtName, _T(".cpk")) == 0 )
		{
			dwUnPackNum++;

			pWriteCpk->Unload();

			if( FALSE == CDiscIOMgr::IsExist(strWndText.GetBuffer()) )
			{
				//先创建好相应的路径
				char szTemp[MAX_PATH];
				for(int i=0; strWndText.GetAt(i) != '\0'; i++)
				{
					if( strWndText.GetAt(i) == '\\' && i > 0 )
					{
						ZeroMemory(szTemp, sizeof(szTemp));
						strncpy(szTemp, strWndText.GetBuffer(), i);

						::CreateDirectory(szTemp, NULL);
					}
				}

				//创建cpk文件
				if( FALSE == pWriteCpk->Create( lpFileName ) )//pCPKFileHandle->szFullName) )
				{
					//创建文件失败
					ASSERT( FALSE && Translate("创建CPK文件失败"));

					//pReadCpk->Close(dwFileCPKHandle);
					return TRUE;
				}
			}
			else
			{
				//加载它
				if( FALSE == pWriteCpk->Load(strWndText.GetBuffer()) )
				{
					//INT nRet = MessageBox(NULL, "打开文件%s失败", fp->szFullName, NULL, MB_ABORTRETRYIGNORE);

					//pReadCpk->Close(dwFileCPKHandle);
					return TRUE;
				}
			}

			//关闭该句柄
			//pReadCpk->Close(dwFileCPKHandle);

			//这是一个cpk文件,我们把它所有的子目录放到该CPK文件里面去
			return UnPackCPKFromCPK(dwFatherCRC, dwUnPackNum, pReadCpk, pWriteCpk);
		}
		else
		{
			//不是cpk文件,那么直接在磁盘上创建目录
			CreateDirToDisc(strWndText.GetBuffer());
			dwUnPackNum++;
		}

		//牢记用完之后关闭句柄
		//pReadCpk->Close(dwFileCPKHandle);
	}

	//普通的目录,我们只需要得到下一级别的文件和文件夹
	DWORD* pdwChildArray = NULL;
	DWORD dwChildDirNum = 0;

	//得到一级子目录
	pReadCpk->GetFirstLevelChildDir(dwFatherCRC, dwChildDirNum);

	DWORD dwChildNum = 0;
	//得到一级子文件
	pReadCpk->GetFirstLevelChild(dwFatherCRC, dwChildNum);

	//申请内存取较大者
	if( dwChildDirNum > dwChildNum )
	{
		pdwChildArray = new DWORD[dwChildDirNum];
	}
	else
	{
		pdwChildArray = new DWORD[dwChildNum];
	}

	if( dwChildDirNum > 0 )
	{
		if( TRUE == m_bTerminateThread )
		{
			FREEARR(pdwChildArray);
			return FALSE;
		}

		pReadCpk->GetFirstLevelChildDir(dwFatherCRC, dwChildDirNum, pdwChildArray);
		for( DWORD i=0; i<dwChildDirNum; i++ )
		{
			//对所有子目录, 嵌套调用解压
			if( FALSE == UnPackCpk(pdwChildArray[i], dwUnPackNum, pReadCpk, pWriteCpk) )
			{
				FREEARR(pdwChildArray);
				return FALSE;
			}
		}
	}

	if( dwChildNum > 0 )
	{
		pReadCpk->GetFirstLevelChild(dwFatherCRC, dwChildNum, pdwChildArray);
		for(DWORD i=0; i<dwChildNum; i++)
		{
			if( TRUE == m_bTerminateThread )
			
			{
				FREEARR(pdwChildArray);
				return FALSE;
			}
			//对所有一级子文件, 直接调用文件解压函数
			if( FALSE == UnPackSimpleFile(pdwChildArray[i], dwUnPackNum, pReadCpk) )
			{	FREEARR(pdwChildArray);
				return FALSE;
			}
		}
	}

	FREEARR(pdwChildArray);
	return TRUE;
}


BOOL	CSelfExtractDlg::CreateDirToDisc(LPCTSTR szPath)
{

	ASSERT(szPath);
	//首先一步一步剥离路径,比如c:\bin\tidy, c:\, c:\bin\tidy

	char szTemp[MAX_PATH];
	for(int i=0; szPath[i] != '\0'; i++)
	{
		if( szPath[i] == '\\' && i > 0 )
		{
			ZeroMemory(szTemp, sizeof(szTemp));
			strncpy(szTemp, szPath, i);

			::CreateDirectory(szTemp, NULL);
		}
	}

	return ::CreateDirectory(szPath, NULL);

}

VOID	CSelfExtractDlg::SetProgressPos(INT nPos)
{
	//DWORD dwProgress = nPos * PROGRESS_WIDTH / 100;
	//if( m_dwProgress == dwProgress )
	//	return;

	//InterlockedExchange( (LONG*)&m_dwProgress , dwProgress );

	m_Progress.SetPos( nPos );

}

//-------------------------------------------------------------------------------
//从一个ｃｐｋ包解压出里面的子cpk包的目录出来
//-------------------------------------------------------------------------------
BOOL	CSelfExtractDlg::UnPackCPKFromCPK(DWORD dwDirCRC, DWORD& dwUnPackNum, CPKVfs *pReadCPK, CPKVfs *pWriteCPK)
{
	DWORD* pdwChildArray = NULL;

	//获取一级子目录
	DWORD dwChildDirNum = 0;
	pReadCPK->GetFirstLevelChildDir(dwDirCRC, dwChildDirNum);

	//获取一级子文件
	DWORD dwChildNum = 0;
	pReadCPK->GetFirstLevelChild(dwDirCRC, dwChildNum);

	//申请内存取较大者
	if( dwChildDirNum > dwChildNum )
	{
		pdwChildArray = new DWORD[dwChildDirNum];
	}
	else
	{
		pdwChildArray = new DWORD[dwChildNum];
	}

	if( dwChildNum > 0 )
	{
		//对所有一级子文件, 我们直接把它放入该cpk文件里面去

		pReadCPK->GetFirstLevelChild(dwDirCRC, dwChildNum, pdwChildArray);
		for(DWORD i=0; i<dwChildNum; i++)
		{
			if( TRUE == m_bTerminateThread )
			{
				FREEARR(pdwChildArray);
				return FALSE;
			}

			Sleep( 0 );		//休息缓冲
			dwUnPackNum++;

			INT nTableIndex = pReadCPK->GetTableIndexFromeCrc(pdwChildArray[i]);
			if( nTableIndex == GT_INVALID )
			{
				ASSERT( FALSE && Translate("cpk文件中未找到该项") );
				continue;
			}

			//打开文件映像句柄
			DWORD dwCPKFileHandle = pReadCPK->Open(nTableIndex, 0, true );	//打开
			if( 0 == dwCPKFileHandle )
			{
				ASSERT( FALSE && Translate("打开文件失败") );
				continue;
			}

			CPKFile *pCPKFileHandle = (CPKFile*)dwCPKFileHandle;

			CString strWndText;
			strWndText.Format( "%s %s ...",Translate("解压"), pCPKFileHandle->szFullName );
			m_staticFile.SetWindowText( strWndText.GetBuffer() );

			//确实是文件,我们可以先创建好路径,然后在加入文件
			//首先一步一步剥离路径,比如c:\bin\tidy, c:\, c:\bin, c:\bin\tidy

			char szFileName[MAX_PATH], szCPKName[MAX_PATH];
			ZeroMemory(szFileName, sizeof(szFileName));
			ZeroMemory(szCPKName, sizeof(szCPKName));

			BOOL bRet = pReadCPK->IsFileForCPK(pCPKFileHandle->szFullName, szCPKName, szFileName);
			ASSERT( bRet == TRUE && Translate("本来是cpk包的子文件, 检测却不是"));

			char szPathFile[MAX_PATH];
			DWORD dwCPKFatherCRC = 0;

			INT j=0;
			for(int k=0, j=0; szFileName[k] != '\0'; k++)
			{
				if( szFileName[k] == '\\' && k > 0 )
				{
					ZeroMemory(szPathFile, sizeof(szPathFile));
					strncpy(szPathFile, szFileName+j, k-j);
					dwCPKFatherCRC = pWriteCPK->CreateDir(szPathFile, dwCPKFatherCRC);

					j = k+1;
				}
			}

			//首先计算需要映射的大小
			tagCPKTable *pTable = pReadCPK->GetCPKTable();
			DWORD dwMapSize = pTable[nTableIndex].dwPackedSize + pTable[nTableIndex].dwExtraInfoSize;


			INT nWriteTableIndex = pWriteCPK->InitFileCPKToCPK(szFileName, dwCPKFatherCRC, pCPKFileHandle->pTableItem);
			ASSERT( nWriteTableIndex != GT_INVALID );


			DWORD dwHaveMapSize = 0 ;
			for( ; ; )
			{
				//写入数据
				if( FALSE == pWriteCPK->WriteFileCPKToCPK(nWriteTableIndex, dwHaveMapSize, 
					pCPKFileHandle->lpStartAddress, pCPKFileHandle->dwMapSize) )
				{
					ASSERT(FALSE && Translate("写入文件失败"));
					return FALSE;
				}

				dwHaveMapSize += pCPKFileHandle->dwMapSize;
				//关闭句柄
				pReadCPK->Close(dwCPKFileHandle);

				if( dwHaveMapSize >= dwMapSize )
					break;

				Sleep( 1 );

				dwCPKFileHandle = pReadCPK->Open(nTableIndex, dwHaveMapSize);	//打开
				ASSERT( dwCPKFileHandle != GT_INVALID );
				pCPKFileHandle = (CPKFile*)dwCPKFileHandle;
			}
			//结束添加文件
			pWriteCPK->FinishFileCPKToCPK(nWriteTableIndex);

			SetProgressPos( (INT)((100L * dwUnPackNum) / m_dwTotalFileNum ) );
		}
	}



	if( dwChildDirNum > 0 )
	{
		//所有子文件夹, 我们嵌套调用此函数, 继续解压展开

		pReadCPK->GetFirstLevelChildDir(dwDirCRC, dwChildDirNum, pdwChildArray);

		for(DWORD i=0; i<dwChildDirNum; i++)
		{
			if( TRUE == m_bTerminateThread )
			{
				FREEARR(pdwChildArray);
				return FALSE;
			}

			dwUnPackNum++;

			INT nTableIndex = pReadCPK->GetTableIndexFromeCrc(pdwChildArray[i]);
			if( nTableIndex == GT_INVALID )
			{
				ASSERT( FALSE && Translate("cpk文件中未找到该项") );
				continue;
			}

			//打开文件映像句柄
			//DWORD dwCPKFileHandle = pReadCPK->Open(nTableIndex);	//打开
			//if( 0 == dwCPKFileHandle )
			//{
			//	ASSERT( FALSE && "打开文件失败" );
			//	continue;
			//}

			//CPKFile *pCPKFileHandle = (CPKFile*)dwCPKFileHandle;

			char lpFileName[MAX_PATH];
			BOOL	bRet = pReadCPK->GetFileOrDirName( nTableIndex, lpFileName );
			if( bRet == FALSE )
			{
				ASSERT( FALSE && Translate("cpk打开文件失败") );
				continue;
			}

			CString strWndText;
			strWndText.Format( "%s %s ...",Translate("解压"),lpFileName );//pCPKFileHandle->szFullName );
			m_staticFile.SetWindowText( strWndText.GetBuffer() );


			//先创建
			char szFileName[MAX_PATH], szCPKName[MAX_PATH];
			ZeroMemory(szFileName, sizeof(szFileName));
			ZeroMemory(szCPKName, sizeof(szCPKName));

			bRet = pReadCPK->IsFileForCPK(lpFileName, szCPKName, szFileName);
			ASSERT( bRet == TRUE && Translate("本来是cpk包的子文件夹, 检测之后发现不是"));

			if( bRet == TRUE )
			{
				//只是路径
				pWriteCPK->AddDirCyc(szFileName, 0); 
			}

			//关闭文件映像句柄
			//pReadCPK->Close(dwCPKFileHandle);

			if( FALSE == UnPackCPKFromCPK(pdwChildArray[i], dwUnPackNum, pReadCPK, pWriteCPK) )
			{
				FREEARR(pdwChildArray);
				return FALSE;
			}
		}
	}

	FREEARR(pdwChildArray);
	return TRUE;
}

//-------------------------------------------------------------------------------
//从cpk文件解压出单个的文件(磁盘上);
//-------------------------------------------------------------------------------
BOOL CSelfExtractDlg::UnPackSimpleFile(DWORD dwCRC, DWORD& dwUnPackNum, CPKVfs *pReadCPK)
{
	dwUnPackNum++;

	//首先查找tableindex
	INT nTableIndex = pReadCPK->GetTableIndexFromeCrc(dwCRC);
	if( nTableIndex == GT_INVALID )
	{
		ASSERT( FALSE && Translate("CPK文件中未找到此项"));
		return TRUE;
	}

	//打开文件映像句柄
	DWORD dwCPKFileHandle = pReadCPK->Open(nTableIndex);	//打开
	if( 0 == dwCPKFileHandle )
	{
		ASSERT( FALSE && Translate("CPK打开文件失败"));
		return TRUE;
	}
	CPKFile *pCPKFileHandle = (CPKFile*)dwCPKFileHandle;

	char szMess[1024];
	sprintf( szMess, "CPKFile: name: %s; addr: %ld; size: %ld\n", pCPKFileHandle->szFullName, (DWORD)pCPKFileHandle->lpMem,
		pCPKFileHandle->dwFileSize );
	OutputDebugString( szMess );

	CString strWndText;
	strWndText.Format( "%s %s ...",Translate("解压"), pCPKFileHandle->szFullName );
	m_staticFile.SetWindowText( strWndText.GetBuffer() );

__loop_retry:		//重新尝试的标签

	CString strFilePath = m_strAppPath + "\\" + pCPKFileHandle->szFullName;

	DWORD dwFileHandle = CreateFileToDisc(strFilePath.GetBuffer());
	if( INVALID_FILE_HANDLE == dwFileHandle )
	{
		INT nRet = MessageBox("fail to create file", \
			NULL, MB_ABORTRETRYIGNORE);

		if( nRet == IDABORT )
		{
			//中止
			pReadCPK->Close(dwCPKFileHandle);
			return FALSE;
		}
		else if( nRet == IDRETRY )
		{
			//重新尝试
			goto __loop_retry;
		}
		else if( nRet == IDIGNORE )
		{
			//忽略，以便继续
			pReadCPK->Close(dwCPKFileHandle);
			return TRUE;
		}

		pReadCPK->Close(dwCPKFileHandle);
		return TRUE;
	}
	//对大文件进行部分映射，循环写
	if( pCPKFileHandle->bCompressed == false && pCPKFileHandle->dwMapSize < pCPKFileHandle->dwFileSize  )
	{     
		DWORD dwTemp = 0;
		DWORD dwIncrease = 0;

		while(dwTemp != pCPKFileHandle->dwFileSize)
		{
			if(pCPKFileHandle->dwFileSize -  dwTemp >= GT_CPK_RAW_BUFFER_SIZE)
			{//计算每次要写的大小
				dwIncrease = GT_CPK_RAW_BUFFER_SIZE;
			}
			else
			{//如果剩下的不够GT_CPK_RAW_BUFFER_SIZE，则只写剩下的
				dwIncrease = pCPKFileHandle->dwFileSize - dwTemp;
			}
			if( FALSE == CDiscIOMgr::Write(dwFileHandle, pCPKFileHandle->lpMem, dwIncrease, 0) )
			{
				//关闭相关句柄
				CDiscIOMgr::Close(dwFileHandle);
				pReadCPK->Close(dwCPKFileHandle);
				return FALSE;
			}
			dwTemp += dwIncrease;
			pReadCPK->Close(dwCPKFileHandle);
			dwCPKFileHandle = pReadCPK->Open(nTableIndex, dwTemp);           //映射文件的下一部分
			pCPKFileHandle = (CPKFile*)dwCPKFileHandle;
		}
	}
	else
	{
		if( FALSE == CDiscIOMgr::Write(dwFileHandle, pCPKFileHandle->lpMem, pCPKFileHandle->dwFileSize, 0) )
		{
			//关闭相关句柄
			CDiscIOMgr::Close(dwFileHandle);
			pReadCPK->Close(dwCPKFileHandle);
			return FALSE;
		}

	}

	SetProgressPos( (INT)((100L * dwUnPackNum) / m_dwTotalFileNum ) ); 
	//关闭相关句柄
	CDiscIOMgr::Close(dwFileHandle);
	pReadCPK->Close(dwCPKFileHandle);

	return TRUE;
}

//------------------------------------------------------------------------------
//往磁盘上创建文件
//------------------------------------------------------------------------------
DWORD	CSelfExtractDlg::CreateFileToDisc(LPCTSTR szFileName)
{
	ASSERT( szFileName );

	if( TRUE == CDiscIOMgr::IsExist(szFileName) )
	{
		DWORD fileAttributes = GetFileAttributes(szFileName);
		fileAttributes &= ~FILE_ATTRIBUTE_READONLY;
		SetFileAttributes(szFileName, fileAttributes);
		if( FALSE == ::DeleteFile(szFileName) )
		{
			char mess[500];
			sprintf(mess, "fail to delete file:%s", szFileName);
			MessageBox( mess, "error", MB_OK);

			return INVALID_FILE_HANDLE;
		}

	}

	INT nLen = (INT)strlen(szFileName);
	if( nLen == 0 ) return FALSE;

	char szTemp[MAX_PATH];
	ZeroMemory(szTemp, sizeof(szTemp));
	for(INT i=nLen-1; i>=0; i--)
	{
		if( szFileName[i] == '\\' && i>0 )
		{
			memcpy(szTemp, szFileName, i);
			CreateDirToDisc(szTemp);
			/*
			char mess[500];
			sprintf(mess, "fail to create dir:%s", szTemp);
			MessageBox(m_hWnd, mess, "error", MB_OK);

			return INVALID_FILE_HANDLE;
			/**/
			break;
		}
	}

	return CDiscIOMgr::Create(szFileName,
		GENERIC_READ | GENERIC_WRITE,
		0, // 
		NULL, 
		OPEN_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, 
		NULL);
}

//-----------------------------------------------------------------------------
//更新版本号
//-----------------------------------------------------------------------------
BOOL CSelfExtractDlg::RefreshVersion(const std::string& strVersion)
{
	// 版本号文件
	std::string strVersionPath = m_strAppPath  + "\\launcher\\launcher.ini";
	BOOL bResult = ::WritePrivateProfileString(_T("Launcher"), _T("LocalGame"), strVersion.c_str(), strVersionPath.c_str());

	return bResult;	
}


void CSelfExtractDlg::OnBnClickedUpdate()
{
	// TODO: 在此添加控件通知处理程序代码
	Check();

	//CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)ThreadUnPack, this, NULL, NULL );
}


void CSelfExtractDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	::InterlockedExchange((LONG*)&m_bTerminateThread, TRUE);
	Sleep( 20 );
	OnCancel();
}

BOOL CSelfExtractDlg::SelectPath()
{

	BROWSEINFO bi;
	TCHAR szPath[MAX_PATH];
	ZeroMemory(szPath, sizeof(szPath));

	//初始化入口参数bi开始
	bi.hwndOwner = m_hWnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szPath;			//此参数如为NULL则不能显示对话框
	bi.lpszTitle = Translate("找不到游戏路径，请手动选择游戏目录");  //副标题
	bi.ulFlags = 0;
	bi.lpfn = NULL;
	bi.iImage	=	IDR_MAINFRAME;
	//初始化入口参数bi结束

	LPITEMIDLIST pIDList = SHBrowseForFolder(&bi);//调用显示选择对话框
	if(pIDList)
	{
		//取得文件夹路径到Buffer里
		SHGetPathFromIDList(pIDList, szPath);
		m_strAppPath = szPath;
	}
	else
	{
		LPMALLOC lpMalloc;
		if(FAILED(SHGetMalloc(&lpMalloc))) 
			return FALSE;
		//释放内存
		lpMalloc->Free(pIDList);
		lpMalloc->Release();

		return FALSE;
	}

	LPMALLOC lpMalloc;
	if(FAILED(SHGetMalloc(&lpMalloc))) 
		return TRUE;
	//释放内存
	lpMalloc->Free(pIDList);
	lpMalloc->Release();

	return TRUE;
}

BOOL	CSelfExtractDlg::ReadReg(TCHAR* KEY_ROOT, TCHAR* KEY_NAME, TCHAR outstr[])
{
	ASSERT( outstr != NULL );
	HKEY hKey;
	long ret0 = RegOpenKeyEx(HKEY_LOCAL_MACHINE, KEY_ROOT, 0, KEY_READ, &hKey);
	if ( ret0 != ERROR_SUCCESS )
	{
		outstr[0] = 0;
		return FALSE;
	}
	DWORD dwLen = MAX_PATH, dwType = REG_SZ;
	// if TCHAR* (LPBYTE)TCHAR*
	// if TCHAR [] (LPBYTE)&TCHAR
	RegQueryValueEx(hKey, KEY_NAME, NULL, &dwType, (LPBYTE)outstr, &dwLen);

	RegCloseKey(hKey);
	return TRUE;
}

VOID	CSelfExtractDlg:: Check()
{

	//检测自动更新程序是否在运行
	m_Mutex = ::CreateMutex( NULL, FALSE, "Loong_launcher" );
	if( GetLastError() == ERROR_ALREADY_EXISTS )
	{
		CloseHandle( m_Mutex );
		MessageBox( Translate("自动更新程序正在运行，请先关闭自动更新程序") );
		GetDlgItem( IDC_UPDATE )->EnableWindow( TRUE );
	}
	else
	{
		//检测游戏是否在运行
		CProcessWrapper  Process;
		DWORD dwThreadID;
		HANDLE hKungfu = Process.FindProcess( dwThreadID,  "Client.exe" );
		if( hKungfu != INVALID_HANDLE_VALUE )
		{
			MessageBox( Translate("游戏正在运行，请先关闭游戏在进行更新") );
			GetDlgItem( IDC_UPDATE )->EnableWindow( TRUE );
			CloseHandle( m_Mutex );
		}
		else
		{
			CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)ThreadUnPack, this, NULL, NULL );
			CWnd* pWnd = GetDlgItem( IDC_UPDATE );
			pWnd->EnableWindow( FALSE );
		}
	}
}

void CSelfExtractDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	SendMessage( WM_NCLBUTTONDOWN, HTCAPTION, (LPARAM)&point );

	CDialog::OnMouseMove(nFlags, point);
}
