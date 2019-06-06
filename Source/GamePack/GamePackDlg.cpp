// GamePackDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GamePack.h"
#include "GamePackDlg.h"
#include ".\gamepackdlg.h"

#include "cpkcore\cpkcore.h"
#include "memmgr\memmgr.h"
#include "cpkcore\disciomgr.h"
#include "filtermgr\filtermgr.h"
#include "cpkcore\util.h"
#include "SelfExtract.h"

#include "md5\md5.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CMemMgr		MemMgr(TRUE);
CDiscIOMgr	DiscMgr();
CFilterMgr	FilterMgr();


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


// CGamePackDlg 对话框



CGamePackDlg::CGamePackDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGamePackDlg::IDD, pParent), m_Trunk(this)
	, m_bAddexe(FALSE)
{
	m_strVersionSrc.Empty();
	m_strVersionDest.Empty();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


void CGamePackDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_VERSION_MIN, m_EditVersionSrc);
	DDX_Control(pDX, IDC_EDIT_VERSION_MAX, m_EditVersionDest);
	DDX_Text(pDX, IDC_EDIT_VERSION_MIN, m_strVersionSrc);
	DDX_Text(pDX, IDC_EDIT_VERSION_MAX, m_strVersionDest);
}

BEGIN_MESSAGE_MAP(CGamePackDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BT_NEWDIR, OnBnClickedBtNewdir)
	ON_BN_CLICKED(IDC_BT_OLDDIR, OnBnClickedBtOlddir)
	ON_BN_CLICKED(IDC_BT_SAVEDIR, OnBnClickedBtSavedir)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CHECK_UPDATE, OnBnClickedCheckUpdate)
	ON_STN_CLICKED(IDC_ST_FILE, OnStnClickedStFile)
	ON_EN_CHANGE(IDC_EDIT_VERSION_MIN, OnEnChangeEditVersionMin)
	ON_EN_CHANGE(IDC_EDIT_VERSION_MAX, OnEnChangeEditVersionMax)
	ON_EN_CHANGE(IDC_ET_SAVEDIR, OnEnChangeEtSavedir)
END_MESSAGE_MAP()


// CGamePackDlg 消息处理程序

BOOL CGamePackDlg::OnInitDialog()
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

	m_Progress.Create(WS_CHILD|WS_VISIBLE|PBS_SMOOTH, CRect(40, 200, 397, 226), this, 1);
	m_Progress.SetRange(0, 100);
	m_Progress.SetPos(0);

	m_pETOld = (CEdit*)GetDlgItem(IDC_ET_OLDDIR);
	m_pETNew = (CEdit*)GetDlgItem(IDC_ET_NEWDIR);
	m_pETSave = (CEdit*)GetDlgItem(IDC_ET_SAVEDIR);
	m_pStatic = (CStatic*)GetDlgItem(IDC_ST_FILE);
	m_pSTimer = (CStatic*)GetDlgItem(IDC_ST_TIME);
//从注册表读取
	TCHAR szOldPath[MAX_PATH], szNewPath[MAX_PATH], szSavePath[MAX_PATH];
	ReadReg("SOFTWARE\\TENGREN\\LOONG\\UPDATE", "default_new_path", 
		szNewPath);
	ReadReg("SOFTWARE\\TENGREN\\LOONG\\UPDATE", "default_old_path", 
		szOldPath);
	
	ReadReg("SOFTWARE\\TENGREN\\LOONG\\UPDATE", "default_save_path", 
		szSavePath);

	CString strTemp;
	CString strPath = szSavePath;
	INT nPos = strPath.ReverseFind( _T('.'));
	if( nPos != -1 )
	{
		strTemp = strPath.Left( nPos + 1 );
		strTemp += "cpk";
		
	}

	m_pETOld->SetWindowText(szOldPath);
	m_pETNew->SetWindowText(szNewPath);

	m_pETSave->SetWindowText( strTemp.GetBuffer() );

	ASSERT( m_pETSave && m_pETNew && m_pETOld );
	//初始化
	m_pCpk = new CSelfExtract;
	ASSERT( m_pCpk );
	m_pMD5 = new CMD5;
	ASSERT( m_pMD5 );
	m_pNewCpk = NULL;
	m_pOldCpk = NULL;
	m_bTerminateGamePack = FALSE;
	m_bPacking = FALSE;
	m_hGamePackThread = INVALID_HANDLE_VALUE;


	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

void CGamePackDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CGamePackDlg::OnPaint() 
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
HCURSOR CGamePackDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
//------------------------------------------------
//选择版本比对的新版本目录
//------------------------------------------------
void CGamePackDlg::OnBnClickedBtNewdir()
{
	CString sFolderPath;
	BROWSEINFO bi;
	TCHAR szPath[MAX_PATH];
	ZeroMemory(szPath, sizeof(szPath));
	m_pETNew->GetWindowText(szPath, MAX_PATH);
	//初始化入口参数bi开始
	bi.hwndOwner = NULL;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szPath;			//此参数如为NULL则不能显示对话框
	bi.lpszTitle = "请选择新版本路径";  //副标题
	bi.ulFlags = 0;
	bi.lpfn = NULL;
	bi.iImage	=	IDR_MAINFRAME;
	//初始化入口参数bi结束

	LPITEMIDLIST pIDList = SHBrowseForFolder(&bi);//调用显示选择对话框
	if(pIDList)
	{
		//取得文件夹路径到Buffer里
		SHGetPathFromIDList(pIDList, szPath);

		m_pETNew->SetWindowText(szPath);
	}
	
	LPMALLOC lpMalloc;
	if(FAILED(SHGetMalloc(&lpMalloc))) 
		return;
	//释放内存
	lpMalloc->Free(pIDList);
	lpMalloc->Release();
	
}
//------------------------------------------------
//选择版本比对的老版本目录
//------------------------------------------------
void CGamePackDlg::OnBnClickedBtOlddir()
{
	CString sFolderPath;
	BROWSEINFO bi;
	TCHAR szPath[MAX_PATH];
	ZeroMemory(szPath, sizeof(szPath));

	//初始化入口参数bi开始
	bi.hwndOwner = NULL;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szPath;//此参数如为NULL则不能显示对话框
	bi.lpszTitle = "请选择老版本路径";		//
	bi.ulFlags = 0;
	bi.lpfn = NULL;
	bi.iImage	=	IDR_MAINFRAME;
	//初始化入口参数bi结束

	LPITEMIDLIST pIDList = SHBrowseForFolder(&bi);//调用显示选择对话框
	if(pIDList)
	{
		//取得文件夹路径到Buffer里
		SHGetPathFromIDList(pIDList, szPath);
		m_pETOld->SetWindowText(szPath);
	}

	LPMALLOC lpMalloc;
	if(FAILED(SHGetMalloc(&lpMalloc)))
		return;
	//释放内存
	lpMalloc->Free(pIDList);
	lpMalloc->Release();	
}
//------------------------------------------------
//选择升级包的存储目录
//------------------------------------------------
void CGamePackDlg::OnBnClickedBtSavedir()
{
	CString sFolderPath;
	BROWSEINFO bi;
	TCHAR szPath[MAX_PATH];
	ZeroMemory(szPath, sizeof(szPath));

	//初始化入口参数bi开始
	bi.hwndOwner = NULL;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szPath;//此参数如为NULL则不能显示对话框
	bi.lpszTitle = "请选择升级包存储路径";
	bi.ulFlags = 0;
	bi.lpfn = NULL;
	bi.iImage	=	IDR_MAINFRAME;
	//初始化入口参数bi结束

	LPITEMIDLIST pIDList = SHBrowseForFolder(&bi);//调用显示选择对话框
	if(pIDList)
	{
		//取得文件夹路径到Buffer里
		SHGetPathFromIDList(pIDList, szPath);
		INT nLen = _tcslen(szPath);
		ASSERT( nLen > 0 );
		if( szPath[nLen-1] != '\\' )
		{
			strcat(szPath, "\\");
		}

		strcat(szPath, "update.cpk");

		m_pETSave->SetWindowText(szPath);
	}

	LPMALLOC lpMalloc;
	if(FAILED(SHGetMalloc(&lpMalloc)))
		return;
	//释放内存
	lpMalloc->Free(pIDList);
	lpMalloc->Release();	
}
//------------------------------------------------
//开始打包
//------------------------------------------------
void CGamePackDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	m_Progress.SetPos(0);

	m_pStatic->SetWindowText("");
	m_pSTimer->SetWindowText("0 ms");

	if( m_bAddexe )
	{
	/*	CString	strPath, strTemp, strVersionMin, strVersionMax;
		m_pETSave->GetWindowText( strPath );;
		m_EditVersionMin.GetWindowText( strVersionMin );
		m_EditVersionMax.GetWindowText( strVersionMax );
		INT nPos = strPath.ReverseFind( _T('.') ) ;
		strTemp = strPath.Left( nPos );
		strTemp += " ";
		strTemp += strVersionMin;
		strTemp += "-";
		strTemp += strVersionMax;s
		strTemp += strPath.Right( strPath.GetLength() - nPos );

		m_pETSave->SetWindowText( strTemp.GetBuffer() );*/
		if(  m_strVersionSrc.IsEmpty() || m_strVersionDest.IsEmpty() )
		{
			MessageBox( "版本号不能为空" );
			return ;
		}
	}

	GetDlgItem(IDOK)->EnableWindow(FALSE);	
	GetDlgItem(IDCANCEL)->EnableWindow(TRUE);
	
	//启动打包线程
	::InterlockedExchange((LONG*)&m_bTerminateGamePack, FALSE);
	m_hGamePackThread = (HANDLE)_beginthreadex
		(NULL, 0, (THREADPROC)m_Trunk.sfp1(&CGamePackDlg::GamePackThread), 
		NULL, 0, NULL);
	ASSERT( m_hGamePackThread != (HANDLE)(-1L) );
}
//------------------------------------------------
//取消打包
//------------------------------------------------
void CGamePackDlg::OnBnClickedCancel()
{
	::InterlockedExchange((LONG*)&m_bTerminateGamePack, TRUE);
}
//------------------------------------------------
//关闭程序
//------------------------------------------------
void CGamePackDlg::OnClose()
{
	if( TRUE == m_bPacking )
	{
		::InterlockedExchange((LONG*)&m_bTerminateGamePack, TRUE);
		Sleep(50);
	}
	FREE(m_pCpk);
	FREE(m_pMD5);
	FREE(m_pNewCpk);
	FREE(m_pOldCpk);
	m_Trunk.sfp_clear();

	CString csOldPath, csNewPath, csSavePath;
	m_pETOld->GetWindowText(csOldPath);
	m_pETNew->GetWindowText(csNewPath);
	m_pETSave->GetWindowText(csSavePath);

	WriteReg("SOFTWARE\\TENGREN\\LOONG\\UPDATE", "default_new_path", 
		csNewPath.GetBuffer());
	WriteReg("SOFTWARE\\TENGREN\\LOONG\\UPDATE", "default_old_path", 
		csOldPath.GetBuffer());
	
	WriteReg("SOFTWARE\\TENGREN\\LOONG\\UPDATE", "default_save_path", 
		csSavePath.GetBuffer());
	CDialog::DestroyWindow();
}
//------------------------------------------------
//关键代码:版本打包线程
//------------------------------------------------
static INT s_nProcessPos = 0;
static INT s_dwTick		 = 0;
//------------------------------------------------
UINT CGamePackDlg::GamePackThread(LPVOID)
{
	m_bPacking = TRUE;


	//获取两个版本的目录和升级包存储路径
	TCHAR szOldPath[MAX_PATH];
	TCHAR szNewPath[MAX_PATH];
	TCHAR szSavePath[MAX_PATH];
	TCHAR szCurrentPath[MAX_PATH];
	TCHAR szSaveFullPath[MAX_PATH];

	ZeroMemory(szOldPath, sizeof(szOldPath));
	ZeroMemory(szNewPath, sizeof(szNewPath));
	ZeroMemory(szSavePath, sizeof(szSavePath));
	ZeroMemory(szSaveFullPath, sizeof(szSaveFullPath));
	ZeroMemory(szCurrentPath, sizeof(szCurrentPath));

	//需要去除路径末尾的'\\'
	INT nStrSize = m_pETOld->GetWindowText(szOldPath, MAX_PATH);
	if( nStrSize && szOldPath[nStrSize-1] == '\\' )
		szOldPath[nStrSize-1] = 0;
	nStrSize = m_pETNew->GetWindowText(szNewPath, MAX_PATH);
	if( nStrSize && szNewPath[nStrSize-1] == '\\' )
		szNewPath[nStrSize-1] = 0;

	nStrSize = m_pETSave->GetWindowText(szSaveFullPath, MAX_PATH);
	if( 0 == nStrSize )
	{
		//其中有目录不存在
		AfxMessageBox(_T("升级包文件名设置不能为空"));

		GetDlgItem(IDOK)->EnableWindow(TRUE);
		m_bPacking = FALSE;
		return 0;
	}

	INT i=0;
	for(i=nStrSize-1; i>0; i--)
	{
		if( szSaveFullPath[i] == '\\' )
			break;
	}
	memcpy(szSavePath, szSaveFullPath, i);
	if( i && szSavePath[i-1] == '\\' )
		szSavePath[i-1] = 0;
	

	if( FALSE == CDiscIOMgr::IsDir(szOldPath) )
	{
		//其中有目录不存在
		AfxMessageBox(_T("指定的旧目录不存在"));

		GetDlgItem(IDOK)->EnableWindow(TRUE);
		m_bPacking = FALSE;
		return 0;
	}

	if( FALSE == CDiscIOMgr::IsDir(szNewPath) )
	{
		//其中有目录不存在
		AfxMessageBox(_T("指定的新目录不存在"));

		GetDlgItem(IDOK)->EnableWindow(TRUE);
		m_bPacking = FALSE;
		return 0;
	}

	if( FALSE == CDiscIOMgr::IsDir(szSavePath) )
	{
		//其中有目录不存在
		AfxMessageBox(_T("指定的升级包文件所在目录不存在"));

		GetDlgItem(IDOK)->EnableWindow(TRUE);
		m_bPacking = FALSE;
		return 0;
	}


	std::string strOldPath(szOldPath);
	std::string strNewPath(szNewPath);
	
	if( strOldPath.find(strNewPath, 0) != strOldPath.npos ||
		strNewPath.find(strOldPath, 0) != strOldPath.npos )
	{
		//如果新旧版本目录有包含关系，似乎打包无意义
		strOldPath.resize(0);
		strNewPath.resize(0);
		AfxMessageBox(_T("新旧版本所在目录有依存关系, 无法比对"));

		GetDlgItem(IDOK)->EnableWindow(TRUE);
		m_bPacking = FALSE;

		return FALSE;
	} 

	//可以开始比对了

	if( TRUE == CDiscIOMgr::IsExist(szSaveFullPath) )
	{ 
		INT nRet = MessageBox(_T("该补丁文件已经存在, 是否覆盖重新生成?"), _T("GamePack"), MB_OKCANCEL);
		if( nRet == IDCANCEL )
		{
			GetDlgItem(IDOK)->EnableWindow(TRUE);
			m_bPacking = FALSE;
			return FALSE;			
		}

		//删除旧的升级包
		BOOL bRet = CDiscIOMgr::DelFileOrDir(szSaveFullPath);
		ASSERT( bRet );
		if( bRet == FALSE )
		{
			MessageBox(_T("补丁文件已存在,并且正被使用，无法删除"), _T("GamePack"), MB_OK);
			GetDlgItem(IDOK)->EnableWindow(TRUE);
			m_bPacking = FALSE;
			return FALSE;
		}
	}

	if( FALSE == m_pCpk->CreateSaveFile(szSaveFullPath, m_bAddexe) )
	{
		MessageBox(_T("无法创建补丁文件"), _T("GamePack"), MB_OK);
		GetDlgItem(IDOK)->EnableWindow(TRUE);
		m_bPacking = FALSE;
		return FALSE;
	}


	DWORD dwNewFileNum = CDiscIOMgr::GetFileNumInDir(szNewPath);
	m_fIntePos = ((FLOAT)100) / dwNewFileNum;  //预估进度条
	m_nFileNum = 0;

	::GetCurrentDirectory(MAX_PATH, szCurrentPath);
	::SetCurrentDirectory(szNewPath);  //设置工作路径为当前新版本路径

	//***----------------------****************------------------------***/
	s_nProcessPos = 0;
	s_dwTick = GetTickCount();
	m_dwTick = GetTickCount();

	BOOL ret = GamePack("", (TCHAR*)szOldPath);	//打包入口
	m_pCpk->WriteTail( m_strVersionSrc, m_strVersionDest );						//写入文件读取地址
	m_pCpk->Unload();
	//***----------------------****************------------------------***/

	if( FALSE == m_bTerminateGamePack )		//确保正常完成,避免浮点数计算误差
		m_Progress.SetPos(100);
	GetDlgItem(IDOK)->EnableWindow(TRUE);
	::SetCurrentDirectory(szCurrentPath);	//恢复工作路径

	m_bPacking = FALSE;

	CString strTxtPath = szSaveFullPath;
	INT nPos = strTxtPath.ReverseFind( '\\' );
	if( nPos == -1 )
	{
		return ret;
	}
	INT nExtPos = strTxtPath.ReverseFind( '.' );
	if( nExtPos == -1 )
	{
		return ret;
	}

	strTxtPath.Delete( nExtPos - 1, strTxtPath.GetLength() - nExtPos + 1 );
	strTxtPath += "_md5.txt";

	//计算补丁包的MD5值，并写入文件
	DWORD dwHandle = CDiscIOMgr::Create( strTxtPath.GetBuffer(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );
	if( dwHandle == GT_INVALID )
	{
		MessageBox( "创建md5值文件错误，请手工计算" );
		return ret;
	}

	//开始计算MD5值 szSaveFullPath;

	BYTE  byMd5[16] = {0};
	BOOL bRet =  CMD5::MD5ForFile( szSaveFullPath, byMd5 );
	if( !bRet )
	{
		MessageBox( "计算MD5值错误" );
		return ret;
	}

	char szMess[512] = {0};
	for(int i=0; i<16; i++)
		sprintf(szMess, "%s%02x", szMess, byMd5[i]);

	//将MD5写入文件
	bRet = CDiscIOMgr::Write( dwHandle, szMess, strlen(szMess) );
	if( !bRet )
	{
		MessageBox( "将md5 写入文件错误" );
		return ret;
	}

	CDiscIOMgr::Close( dwHandle );
	return ret;
}
//------------------------------------------------
//关键代码:版本比对
//------------------------------------------------

static TCHAR szTime[MAX_PATH];

BOOL	CGamePackDlg::GamePack(TCHAR *szNewPath, TCHAR *szOldPath)
{
	BOOL bResult = FALSE;
	TCHAR szTemp[MAX_PATH];
	TCHAR szOldTemp[MAX_PATH];
	WIN32_FIND_DATA FindData;
	HANDLE hHandle = INVALID_HANDLE_VALUE;
	BYTE byDigest_new[16], byDigest_old[16];
	DWORD dwTick = 0;
	INT nPos = 0;

	//相关预处理
	lstrcpy(szTemp, szNewPath);
	if( szTemp[0] == '\0' )
		lstrcat(szTemp, _T("*.*"));
	else 
	{
		if( szTemp[_tcslen(szTemp)-1] != '\\' )
			lstrcat(szTemp, _T("\\"));
		lstrcat(szTemp, _T("*.*"));
	}
	hHandle = ::FindFirstFile(szTemp, &FindData);
	if( hHandle == INVALID_HANDLE_VALUE )
		return FALSE;

	while( FALSE == m_bTerminateGamePack && TRUE == ::FindNextFile(hHandle, &FindData) )
	{
		if( FindData.cFileName[0] == _T('.') )
			continue;

		lstrcpy(szTemp, szNewPath);
		if( szTemp[0] != '\0' )
		{
			lstrcat(szTemp, _T("\\"));
		}
		lstrcat(szTemp, FindData.cFileName);
		lstrcpy(szOldTemp, szOldPath);
		lstrcat(szOldTemp, _T("\\"));
		lstrcat(szOldTemp, FindData.cFileName);

		if( FILE_ATTRIBUTE_DIRECTORY & FindData.dwFileAttributes )
		{//是目录
			if( FALSE ==  CDiscIOMgr::IsDir(szOldTemp) )
			{
				m_pCpk->AddDir(szTemp, 0);
			}
			GamePack(szTemp, szOldTemp);
			goto __Continue;
		}
		m_nFileNum++;
		m_pStatic->SetWindowText(szTemp);

		if( FALSE == CDiscIOMgr::IsExist(szOldTemp) )
		{
			//那个文件老版本不存在,直接加入升级包
			if( AddFileAndDir(szTemp) == FALSE )
				break;
		}
		else
		{
			//如果文件小于５m，我们就先比较ｍｄ５
			DWORD dwNewFileSize = CDiscIOMgr::GetSize(szTemp);
			DWORD dwOldFileSize = CDiscIOMgr::GetSize(szOldTemp);
							
			if( dwNewFileSize != dwOldFileSize )
			{
				////文件大小不等
				//if( dwNewFileSize <= 5242880 ) // 5M
				//{

				//	//文件大小都不一样，文件自然就不一样
				//	if( AddFileAndDir(szTemp) == FALSE )
				//		break;
				//}
				//else
				{
					//文件都存在,我们首先查看是不是cpk文件
					BOOL bRetOld = IsCPKFile(szOldTemp) && m_pCpk->IsValidCPK(szOldTemp);
					BOOL bRetNew = IsCPKFile(szTemp) && m_pCpk->IsValidCPK(szTemp);

					if( TRUE == bRetOld && TRUE == bRetNew )
					{
						//都是cpk文件并且不等
						//只好ｃｐｋ文件比对了
						CompareCPK(szTemp, szOldTemp);
					}
					else
					{
						//文件大小都不一样，文件自然就不一样
						if( AddFileAndDir(szTemp) == FALSE )
							break;
					}
				}
			}
			else
			{
				//文件大小一致, 需要进一步分析
				//if( dwNewFileSize <= 5242880 ) // 5M
				//{
				//	bResult = m_pMD5->MD5ForFile(szTemp, byDigest_new);
				//	ASSERT( bResult );
				//	bResult = m_pMD5->MD5ForFile(szOldTemp, byDigest_old);
				//	ASSERT( bResult );
				//	if( FALSE == IsEqualForMD5(byDigest_new, byDigest_old) )
				//	{ 
				//		// md5不一致，文件不同
				//		if( AddFileAndDir(szTemp) == FALSE )
				//			break;
				//	}
				//}
				//else
				{
					// > 5M,我们首先查看是不是cpk文件
					BOOL bRetOld = IsCPKFile(szOldTemp) && m_pCpk->IsValidCPK(szOldTemp);
					BOOL bRetNew = IsCPKFile(szTemp) && m_pCpk->IsValidCPK(szTemp);

					if( bRetOld && bRetNew)
					{
						//比对cpk文件

						bResult = m_pMD5->MD5ForFile(szTemp, byDigest_new);
						ASSERT( bResult );
						bResult = m_pMD5->MD5ForFile(szOldTemp, byDigest_old);
						ASSERT( bResult );
						if( FALSE == IsEqualForMD5(byDigest_new, byDigest_old) )
						{ 
							// md5不一致，文件内容不一定相同
							//只好ｃｐｋ文件比对了
							CompareCPK(szTemp, szOldTemp);
						}
					}
					else if( bRetOld || bRetNew )
					{
						//一个是cpk文件,一个不是,显然这两个文件不同
						if( AddFileAndDir(szTemp) == FALSE )
							break;
					}
					else
					{
						//都不是cpk文件，那好办，直接计算ｍｄ５，比对就行

						bResult = m_pMD5->MD5ForFile(szTemp, byDigest_new);
						ASSERT( bResult );
						bResult = m_pMD5->MD5ForFile(szOldTemp, byDigest_old);
						ASSERT( bResult );
						if( FALSE == IsEqualForMD5(byDigest_new, byDigest_old) )
						{ 
							// md5不一致，文件不同，需要打包
							if( AddFileAndDir(szTemp, 0) == FALSE )
								break;
						}
					} // if (是否是cpk文件)
				}  //if (是否<5M)

			}	//if (新旧文件是否相等)
		}

__Continue:
		dwTick = GetTickCount() - m_dwTick;
		if( dwTick - s_dwTick > 300 )
		{
			//显示消耗时间
			sprintf(szTime, "%ld,%ld ms", dwTick / 1000, dwTick % 1000);
			m_pSTimer->SetWindowText(szTime);
			s_dwTick = dwTick;

			nPos = (INT)(m_fIntePos*m_nFileNum);
			if( nPos != s_nProcessPos )
			{	
				m_Progress.SetPos(nPos);
				s_nProcessPos = nPos;
			}
		}
	
		Sleep( 10 );
	}

	::FindClose(hHandle);
	
	return TRUE;
}
//------------------------------------------------
//是否cpk文件
//------------------------------------------------
BOOL	CGamePackDlg::IsCPKFile(TCHAR *szFileName)
{
	WORD wNameLen = _tcslen(szFileName);
	if( wNameLen <= 4 )
		return FALSE;
	if( (szFileName[wNameLen-1] == 'k' || szFileName[wNameLen-1] == 'K') &&
		(szFileName[wNameLen-2] == 'p' || szFileName[wNameLen-2] == 'P') &&
		(szFileName[wNameLen-3] == 'c' || szFileName[wNameLen-1] == 'C') &&
		(szFileName[wNameLen-4] == '.') )
		return TRUE;

	return FALSE;
}
//-----------------------------------------------------------------------------
//比较两个MD5是否相等
//----------------------------------------------------------------------------
BOOL	CGamePackDlg::IsEqualForMD5(BYTE digest1[16], BYTE digest2[16])
{
	return ( memcmp(digest1, digest2, 16) == 0 );
}
//----------------------------------------------------------------------------
//比对新旧两个版本的cpk文件
//----------------------------------------------------------------------------
BOOL	CGamePackDlg::CompareCPK(TCHAR *szNewCPK, TCHAR *szOldCPK)
{
	//准备工作
	if( NULL == m_pNewCpk )
		m_pNewCpk = new CPK;
	if( NULL == m_pOldCpk )
		m_pOldCpk = new CPK;

	m_pNewCpk->Unload();
	m_pOldCpk->Unload();

	if( FALSE == m_pNewCpk->Load(szNewCPK) || FALSE == m_pOldCpk->Load(szOldCPK) )
		return FALSE;
	
	DWORD dwFatherCrc = m_pCpk->AddDir(szNewCPK, 0);
	if( GT_INVALID == dwFatherCrc )
		return FALSE;


	//得到相应的表项地址
	tagCPKHeader *pNewCPKHeader = m_pNewCpk->GetCPKHeader();
	tagCPKTable	 *pNewCPKTable = m_pNewCpk->GetCPKTable();
	tagCPKHeader *pOldCPKHeader = m_pOldCpk->GetCPKHeader();
	tagCPKTable	 *pOldCPKTable = m_pOldCpk->GetCPKTable();
	
	DWORD dwFileMap = m_pNewCpk->CreateMap();
	ASSERT( dwFileMap != 0 );

	//开始逐个文件比对
	LPVOID p = NULL;
	LPVOID pMapAddress = NULL;
	BYTE digest_new[16], digest_old[16];
	TCHAR szTmp[MAX_PATH];
	TCHAR szTempDir[MAX_PATH];

	for(DWORD i=0; i<pNewCPKHeader->dwMaxTableNum; i++)
	{
		if( FALSE == m_pNewCpk->GetMD5FromTableIndex(digest_new, i) )
			continue;

		if( FALSE == m_pOldCpk->GetMD5FromCrc(digest_old, pNewCPKTable[i].dwCRC) || \
			FALSE == IsEqualForMD5(digest_new, digest_old) )
		{	
			//旧版本并没有该文件
			if( FALSE == m_pNewCpk->GetFullNameFromCRC(szTmp, pNewCPKTable[i].dwCRC) )
				continue;

			if( FALSE != IS_DIR(pNewCPKTable[i].dwFlag) )
				DWORD dwCurrentCrc = m_pCpk->AddDir(szTmp, dwFatherCrc);
			else
			{
				//我们先创建好路径,得到crc，之后在此基础上添加文件
				DWORD dwCurrentCrc = dwFatherCrc;
				INT nLen = _tcslen(szTmp);
				ASSERT( nLen >= 1 );
				for(INT k=nLen-1; k>=0; k--)
				{
					if( szTmp[k] == '\\' && k > 0 )
					{
						ZeroMemory(szTempDir, sizeof(szTempDir));
						memcpy(szTempDir, szTmp, k);
						dwCurrentCrc = m_pCpk->AddDir(szTempDir, dwCurrentCrc);
						break;
					}
				}

				
				p = m_pNewCpk->MapFile(dwFileMap, i, pMapAddress);
				if( NULL == p )
				{
					MessageBox(_T("文件映射失败!!"));
					goto __End;
				}
				m_pCpk->AddFile(szTmp, dwCurrentCrc, pNewCPKTable+i, p); //...
				m_pNewCpk->UnMapFile(pMapAddress);
				p = pMapAddress = NULL;
			}
			continue;
		}
	}

__End:
	//卸载
	m_pNewCpk->CloseMap(dwFileMap);
	m_pNewCpk->Unload();
	m_pOldCpk->Unload();

	return TRUE;
}
//----------------------------------------------------------------------------
// -->> 将指定键值写入注册表中指定路径,如果指定路径没有,则创建并写入
//----------------------------------------------------------------------------
BOOL CGamePackDlg::WriteReg(TCHAR* KEY_ROOT/*注册表中的路径*/, TCHAR* KEY_NAME, LPCSTR sz)
{
	if ( KEY_ROOT == NULL ) return FALSE;
	if ( KEY_NAME == NULL ) return FALSE;

	HKEY hKey;
	DWORD dwDisposition = REG_CREATED_NEW_KEY;


	TCHAR buf[256]={0};

	long ret0 = RegOpenKeyEx(HKEY_LOCAL_MACHINE, KEY_ROOT, 0, KEY_READ, &hKey);
	RegCloseKey(hKey);
	if ( ret0 != ERROR_SUCCESS )
	{
		ret0 = RegCreateKeyEx(HKEY_LOCAL_MACHINE, KEY_ROOT, 0, NULL,
			REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
		if ( ret0 != ERROR_SUCCESS )
			return FALSE;

		RegCloseKey(hKey);
	}
	// -- 写入
	RegOpenKeyEx( HKEY_LOCAL_MACHINE, KEY_ROOT, 0, KEY_WRITE, &hKey );
	ret0 = RegSetValueEx(hKey, KEY_NAME, NULL, REG_SZ, (const BYTE*)sz, _tcslen(sz)+sizeof(sz[0]));
	if ( ret0 != ERROR_SUCCESS )
		return FALSE;

	RegCloseKey(hKey);
	return true;
}
//----------------------------------------------------------------------------
// -->> 读取注册表中指定路径的键值
//----------------------------------------------------------------------------
BOOL CGamePackDlg::ReadReg(TCHAR *KEY_ROOT, TCHAR* KEY_NAME, DWORD& key_value)
{
	HKEY hKey;
	long ret0 = RegOpenKeyEx( HKEY_LOCAL_MACHINE, KEY_ROOT, 0, KEY_QUERY_VALUE, &hKey );
	if( ret0 != ERROR_SUCCESS )
		return FALSE;

	DWORD dwBufLen=32;
	RegQueryValueEx( hKey, KEY_NAME, NULL, NULL, (LPBYTE)&key_value, &dwBufLen);
	RegCloseKey(hKey);

	return TRUE;
}
//----------------------------------------------------------------------------
BOOL CGamePackDlg::ReadReg(TCHAR* KEY_ROOT, TCHAR* KEY_NAME, TCHAR outstr[])
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
//----------------------------------------------------------------------------

void CGamePackDlg::OnBnClickedBtSavedir2()
{
	// TODO: 在此添加控件通知处理程序代码
	CString sFolderPath;
	CFileDialog  OpenDlg( TRUE, "*.exe", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"可执行程序(*.exe)|*.exe");
	INT nRet = OpenDlg.DoModal();
	if( nRet == IDOK )
	{
		m_pExePath.SetWindowText( OpenDlg.GetPathName() );
	}
	
}


void CGamePackDlg::OnBnClickedCheckUpdate()
{
	// TODO: 在此添加控件通知处理程序代码

	m_bAddexe = !m_bAddexe;
	CString  strPath, strTemp;
	m_pETSave->GetWindowText(strPath);
	
	INT nPos = strPath.ReverseFind( _T('.') ) ;
	strTemp = strPath.Left( nPos );

	if( m_bAddexe )
	{
		strTemp += ".exe";
	}
	else
	{
	    strTemp += ".cpk";
	}

	m_pETSave->SetWindowText( strTemp.GetBuffer() );
	
	m_EditVersionSrc.EnableWindow( m_bAddexe );
	m_EditVersionDest.EnableWindow( m_bAddexe );

}

void CGamePackDlg::OnStnClickedStFile()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CGamePackDlg::OnEnChangeEditVersionMin()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	//CString strVersionMin, strVersionMax, strSavePath, strTemp, strFileExt;
	//m_EditVersionMin.GetWindowText( strVersionMin );
	//m_EditVersionMax.GetWindowText( strVersionMax);
	//m_pETSave->GetWindowText( strSavePath );


	//INT nPos = strSavePath.ReverseFind( _T('\\') );

	//strTemp = strSavePath.Left( nPos + 1);
	//
	//m_strVersion.Empty();
	//m_strVersion += strVersionMin;
	//m_strVersion += _T( "-" );
	//m_strVersion += strVersionMax;
	//
	//strTemp += m_strFileName;
	//strTemp += m_strVersion;
	//strTemp += m_strExt;

	//m_pETSave->SetWindowText( strTemp.GetBuffer() );


}

void CGamePackDlg::OnEnChangeEditVersionMax()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码

	//CString strVersionMin, strVersionMax, strSavePath, strTemp, strFileExt;
	//m_EditVersionMin.GetWindowText( strVersionMin );
	//m_EditVersionMax.GetWindowText( strVersionMax);
	//m_pETSave->GetWindowText( strSavePath );


	//INT nPos = strSavePath.ReverseFind( _T('\\') );

	//strTemp = strSavePath.Left( nPos + 1);

	//m_strVersion.Empty();
	//m_strVersion += strVersionMin;
	//m_strVersion += _T( "-" );
	//m_strVersion += strVersionMax;

	//strTemp += m_strFileName;
	//strTemp += m_strVersion;
	//strTemp += m_strExt;

	//m_pETSave->SetWindowText( strTemp.GetBuffer() );


}

void CGamePackDlg::OnEnChangeEtSavedir()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码

	/*CString strPath;
	m_pETSave->GetWindowText(strPath);

	INT nPos = strPath.ReverseFind( _T('\\') );
	if( nPos != -1)
	{
		m_strSavePath = strPath.Left( nPos + 1 );
	}*/

}
BOOL CGamePackDlg::AddFileAndDir( LPCTSTR lpFullName, DWORD dwFatherCrc )
{
	ASSERT( m_pCpk != NULL );

__retry:
	BOOL bResult = m_pCpk->AddFileAndDir( lpFullName, dwFatherCrc );
	if( bResult == FALSE ) 
	{
		TCHAR szBuf[4096];	// FIXME: make the array safe
		LPVOID lpMsgBuf = NULL;

		DWORD dwErr = ::GetLastError();

		sprintf( szBuf, "添加文件%s失败", lpFullName );

		// 检查是否可以从系统得到错误信息
		if( dwErr != 0 )
		{
			// Read Error message from system
			FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM 
				| FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwErr,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR) &lpMsgBuf, 0, NULL);

			wsprintf(szBuf, "%s\nCode: %lu\nNews: %s", szBuf,
				dwErr, (const char*)lpMsgBuf);

			LocalFree( lpMsgBuf );
		}

		INT nRet = MessageBox( szBuf, " Error ", MB_ABORTRETRYIGNORE ) ;
		if( nRet == IDRETRY )
			goto __retry;
		else if( nRet == IDABORT )
			return FALSE;

	}

	return TRUE;
}