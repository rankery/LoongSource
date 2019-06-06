#include "stdafx.h"
#include "process.h"
#include "launcher.h"

#include "launcherDlg.h"
#include "httpdown.h"
#include "cpkvfs.h"
#include "disciomgr.h"
#include "md5.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "algorithm"

Process::Process():m_Trunk(this)
{
	m_pHttp = new HttpSocket;
	m_hThread = 0;
	m_pUtil = new Util;
	m_bConfigLoaded = this->GetConfig(_T("launcher\\launcher.ini"));
}

Process::~Process()
{
	SAFE_DEL(m_pHttp);
	SAFE_DEL(m_pUtil);
}



//-----------------------------------------------------------------------------
// 启动更新
//-----------------------------------------------------------------------------
BOOL Process::Work(HWND hWndMain)
{
	m_bTerminateThread = FALSE;
	m_hWndMain = hWndMain;
	m_pHttp->SetWndMain(hWndMain);

	SendMessage(hWndMain, WM_USER_GAME_VER, (WPARAM)(LPCTSTR)m_strLocalGameVersion, 0);
	SendMessage(hWndMain, WM_USER_LAUNCHER_VER, (WPARAM)(LPCTSTR)m_strLocalLauncherVersion, 0);

	if( !m_bConfigLoaded )
		return FALSE;

	m_hThread = (HANDLE)_beginthreadex(NULL, 0, (THREADPROC)m_Trunk.sfp1
		(&Process::WorkThread), 0, 0, NULL);

	return TRUE;
}



//-----------------------------------------------------------------------------
// 更新线程
//-----------------------------------------------------------------------------
UINT Process::WorkThread(void *pArg)
{
	BYTE digest[16];
	std::string strMD5;

	// 先删除原先的更新信息文件patchinfo.txt和服务器信息文件
	::DeleteFile(m_strDestPatchInfo);
	::DeleteFile(m_strDestServerInfo);

	// 下载更新信息文件patchinfo.txt
	m_pHttp->Download(m_strSrcPatchInfo, FALSE);
	m_pHttp->WaitForEnd(INFINITE);
	if( m_bTerminateThread )
		return GT_INVALID;
	if( !m_pHttp->GetResult() )
	{
		PostMessage(m_hWndMain, WM_USER_CAN_UPDATE, 0, 0);
		return GT_INVALID;
	}

	// 是否需要更新自己
	INT nResult = this->ParseLauncherVersion();
	if( nResult != 0 )
	{
		::DeleteFile(m_strDestNewLauncher);

		// 下载新的launcher
		m_pHttp->Download(m_strSrcNewLauncher, FALSE);
		m_pHttp->WaitForEnd(INFINITE);
		if( m_bTerminateThread )
			return GT_INVALID;
		if( !m_pHttp->GetResult() )
		{
			PostMessage(m_hWndMain, WM_USER_CAN_UPDATE, 0, 0);
			return GT_INVALID;
		}

		// 更新自己
		this->UpdateSelf();
		return 0;
	}

	// 分析本次更新下载文件
	while( this->ParseGameVersion() == 1 )
	{
		// 下载本次更新下载文件
_download:
		m_pHttp->Download(m_strSrcGamePatch);
		m_pHttp->WaitForEnd(INFINITE);
		if( m_bTerminateThread )
			return GT_INVALID;
		if( !m_pHttp->GetResult() )
		{
			PostMessage(m_hWndMain, WM_USER_CAN_UPDATE, 0, 0);
			return GT_INVALID;
		}

		// 检查MD5
		if( !m_strGamePatchMD5.empty() )
		{
			PostMessage(m_hWndMain, WM_USER_INFO, (WPARAM)g_StrTable[_T("CheckingMD5")], 1);
			CMD5::MD5ForFile(m_pHttp->GetDestFileName(), digest);
			CMD5::MD5ToString(strMD5, digest);
			if( strMD5 != m_strGamePatchMD5 )	// MD5不匹配
			{
				::DeleteFile(m_pHttp->GetDestFileName());
				goto _download;
			}
		}
		
		// 解压缩释放,并更新版本号
		CStringA strDestFile = m_pUtil->UnicodeToAnsi(m_pHttp->GetDestFileName());
		PostMessage(m_hWndMain, WM_USER_INFO, (WPARAM)g_StrTable[_T("Decompressing")], 2);
		if( !this->Unpack(strDestFile) )
		{
			PostMessage(m_hWndMain, WM_USER_CAN_UPDATE, 0, 0);
			return GT_INVALID;
		}
	}

	// 下载服务器信息文件
	m_pHttp->Download(m_strSrcServerInfo, FALSE);
	m_pHttp->WaitForEnd(INFINITE);
	if( m_bTerminateThread )
		return GT_INVALID;
	if( !m_pHttp->GetResult() )
	{
		PostMessage(m_hWndMain, WM_USER_CAN_UPDATE, 0, 0);
		return GT_INVALID;
	}

	// 备份服务器信息文件
	::CopyFile(m_pHttp->GetDestFileName(), _T("launcher\\serverinfo_back.xml"), FALSE);

	PostMessage(m_hWndMain, WM_USER_PROGRESS_RANGE, 100, 0);
	PostMessage(m_hWndMain, WM_USER_PROGRESS_POS, 100, 0);
	PostMessage(m_hWndMain, WM_USER_INFO, (WPARAM)g_StrTable[_T("UpdateOK")], 3);
	PostMessage(m_hWndMain, WM_USER_CAN_RUN_GAME, 0, 0);
	return 0;

}


//-----------------------------------------------------------------------------
// 读取配置信息
//-----------------------------------------------------------------------------
BOOL Process::GetConfig(LPCTSTR szIniFileName)
{
	const int MAX_STRING_SIZE = 512;
	TCHAR szTemp[MAX_STRING_SIZE];

	// 获取EXE路径
	GetModuleFileName(NULL, szTemp, sizeof(szTemp)/sizeof(TCHAR));
	TCHAR *pFind = ::_tcsrchr(szTemp, _T('\\'));
	if( pFind )
		*(pFind+1) = 0;
	_tcslwr_s(szTemp, sizeof(szTemp)/sizeof(TCHAR));
	m_strAppPath = szTemp;
	
	m_strIniFileFullName = m_strAppPath + szIniFileName;
	if( !CDiscIOMgr::IsExist(m_strIniFileFullName) )
		return FALSE;

	// 获取patchinfo.txt位置
	::GetPrivateProfileString(_T("Launcher"), _T("PatchInfo"), NULL,  szTemp, MAX_STRING_SIZE, m_strIniFileFullName);
	m_strSrcPatchInfo = szTemp;
	this->GetSimpleFileName(szTemp, szTemp);
	m_strDestPatchInfo = m_strAppPath + _T("launcher\\") + szTemp;
	//!!! 检测m_strSrcPatchInfo

	// 获取主窗口显示页面
	::GetPrivateProfileString(_T("Launcher"), _T("URL"), NULL,  szTemp, MAX_STRING_SIZE, m_strIniFileFullName);
	m_strURL = szTemp;

	// 获取官方手动补丁下载页面
	::GetPrivateProfileString(_T("Launcher"), _T("ManualURL"), NULL,  szTemp, MAX_STRING_SIZE, m_strIniFileFullName);
	m_strManualURL = szTemp;

	// 获取Game EXE位置
	::GetPrivateProfileString(_T("Launcher"), _T("GameExe"), NULL,  szTemp, MAX_STRING_SIZE, m_strIniFileFullName);
	m_strGameExe = m_strAppPath + szTemp;

	// 获取游戏配置文件路径
	::GetPrivateProfileString(_T("Launcher"), _T("GameConfig"), NULL,  szTemp, MAX_STRING_SIZE, m_strIniFileFullName);
	m_strCfgFileFullName = m_strAppPath + szTemp;

	// 获取serverinfo.txt服务器位置
	::GetPrivateProfileString(_T("Launcher"), _T("ServerInfo"), NULL,  szTemp, MAX_STRING_SIZE, m_strIniFileFullName);
	m_strSrcServerInfo = szTemp;
	this->GetSimpleFileName(szTemp, szTemp);
	m_strDestServerInfo = m_strAppPath + _T("launcher\\") + szTemp;

	// 获取新版本launcher服务器位置
	::GetPrivateProfileString(_T("Launcher"), _T("NewLauncher"), NULL,  szTemp, MAX_STRING_SIZE, m_strIniFileFullName);
	m_strSrcNewLauncher = szTemp;
	this->GetSimpleFileName(szTemp, szTemp);
	m_strDestNewLauncher = m_strAppPath + _T("launcher\\") + szTemp;	// 新版本launcher本地位置

	// 获取当前launcher版本
	::GetPrivateProfileString(_T("Launcher"), _T("LocalLauncher"), NULL,  szTemp, MAX_STRING_SIZE, m_strIniFileFullName);
	m_strLocalLauncherVersion = szTemp;

	// 获取当前游戏版本
	::GetPrivateProfileString(_T("Launcher"), _T("LocalGame"), NULL,  szTemp, MAX_STRING_SIZE, m_strIniFileFullName);
	m_strLocalGameVersion = szTemp;

	return TRUE;
}


//-----------------------------------------------------------------------------
// 分析launcher本身版本,0:已经是最新版，1:需要升级
//-----------------------------------------------------------------------------
INT	Process::ParseLauncherVersion()
{
	const int MAX_STRING_SIZE = 512;
	TCHAR szTemp[MAX_STRING_SIZE];

	::GetPrivateProfileString(_T("version"), _T("latest_launch"), NULL,  szTemp, MAX_STRING_SIZE, m_strDestPatchInfo);
	m_strLatestLauncherVersion = szTemp;
	SendMessage(m_hWndMain, WM_USER_LATEST_LAUNCHER_VER, (WPARAM)(LPCTSTR)m_strLatestLauncherVersion, 0);

	if( m_strLatestLauncherVersion == m_strLocalLauncherVersion )
		return 0;	// 不需要升级
	else
		return 1;
}



//--------------------------------------------------------------------------------
// 更新launcher版本号
//--------------------------------------------------------------------------------
BOOL Process::RefreshLauncherVersion()
{
	const int MAX_STRING_SIZE = 512;
	TCHAR szTemp[MAX_STRING_SIZE];

	::GetPrivateProfileString(_T("version"), _T("latest_launch"), NULL,  szTemp, MAX_STRING_SIZE, m_strDestPatchInfo);

	// 更新当前launcher版本
	::WritePrivateProfileString(_T("Launcher"), _T("LocalLauncher"), szTemp, m_strIniFileFullName);
	m_strLocalLauncherVersion = szTemp;

	SendMessage(m_hWndMain, WM_USER_LAUNCHER_VER, (WPARAM)(LPCTSTR)m_strLocalLauncherVersion, 0);
	return TRUE;
}


//-----------------------------------------------------------------------------
// 分析游戏版本,0:已经是最新版，1:找到新的补丁，-1:无法找到合适的补丁
//-----------------------------------------------------------------------------
INT Process::ParseGameVersion()
{
	const int MAX_STRING_SIZE = 512;
	TCHAR szTemp[MAX_STRING_SIZE];

	::GetPrivateProfileString(_T("version"), _T("latest_game"), NULL,  szTemp, MAX_STRING_SIZE, m_strDestPatchInfo);
	m_strLatestGameVersion = szTemp;
	SendMessage(m_hWndMain, WM_USER_LATEST_GAME_VER, (WPARAM)(LPCTSTR)m_strLatestGameVersion, 0);

	if( m_strLatestGameVersion == m_strLocalGameVersion )
	{
		m_strToGameVersion.Empty();
		m_strSrcGamePatch.Empty();
		m_strGamePatchMD5.clear();
		return 0;
	}

	CString strFrom, strTo, strURL, strField;
	for(INT nPatch=0; ;nPatch++)
	{
		// 寻找合适的升级补丁
		strField.Format(_T("patch_%d"), nPatch);
		DWORD dwReturn = ::GetPrivateProfileString(strField, _T("from"), NULL,  szTemp, MAX_STRING_SIZE, m_strDestPatchInfo);
		if( 0 == dwReturn )
			break;

		strFrom = szTemp;
		if( strFrom == m_strLocalGameVersion )
		{
			::GetPrivateProfileString(strField, _T("to"), NULL,  szTemp, MAX_STRING_SIZE, m_strDestPatchInfo);
			m_strToGameVersion = szTemp;
			::GetPrivateProfileString(strField, _T("url"), NULL,  szTemp, MAX_STRING_SIZE, m_strDestPatchInfo);
			m_strSrcGamePatch = szTemp;
			::GetPrivateProfileString(strField, _T("md5"), NULL,  szTemp, MAX_STRING_SIZE, m_strDestPatchInfo);
			tstring strTemp = szTemp;
			m_pUtil->CutSpaceOfStringHeadAndTail(strTemp);	// 去掉首尾空格
			m_strGamePatchMD5 = m_pUtil->UnicodeToUnicode8(strTemp.c_str());

			// 转化为小写
			transform(m_strGamePatchMD5.begin(), m_strGamePatchMD5.end(), m_strGamePatchMD5.begin(), tolower);

			return 1;
		}
	}

	// 找不到对应补丁
	INT nRet = ::MessageBox(NULL, g_StrTable[_T("PatchNotFound")], NULL, MB_RETRYCANCEL);
	if( nRet == IDRETRY )
		return -1;

	_exit(0);	// 强行退出
	return -1;
}


//-----------------------------------------------------------------------------
// 更新自己,运行新下载的副本并关闭自己就行
//-----------------------------------------------------------------------------
BOOL Process::UpdateSelf()
{
	const int MAX_STRING_SIZE = 512;
	TCHAR szTemp[MAX_STRING_SIZE];

	// 获取EXE路径
	GetModuleFileName(NULL, szTemp, sizeof(szTemp)/sizeof(TCHAR));

	CString strParam;
	HANDLE hProcessorig = ::OpenProcess(SYNCHRONIZE, TRUE, ::GetCurrentProcessId());
	strParam.Format(_T("%s %s %d"), m_strDestNewLauncher, szTemp, hProcessorig);

	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi;
	::CreateProcess(m_strDestNewLauncher, (LPWSTR)(LPCTSTR)strParam, 0, 0, TRUE, 0, 0, m_strAppPath, &si, &pi);
	::CloseHandle(hProcessorig);


	RefreshLauncherVersion();

	_exit(0);
	return TRUE;
}



//-----------------------------------------------------------------------------
// 解压缩
//-----------------------------------------------------------------------------
BOOL Process::Unpack(CStringA& strCPKFile)
{
	BOOL bRet = FALSE;
	PostMessage(m_hWndMain, WM_USER_PROGRESS_RANGE, 100, 0);
	
	// 游戏客户端正在运行,无法解压补丁更新,需要先关闭游戏客户端
	// 不允许重复启动
	HANDLE hMutex = ::CreateMutex(NULL, FALSE, _T("Loong_client"));
	while( GetLastError() == ERROR_ALREADY_EXISTS )
	{
		//该程序已经存在或者与此相同互斥的程序已经启动
		INT nRet = ::MessageBox(NULL, g_StrTable[_T("ClientRunning")], NULL, MB_RETRYCANCEL);
		if( nRet == IDRETRY )
		{
			hMutex = ::CreateMutex(NULL, FALSE, _T("Loong_client"));
			continue;
		}

		if( nRet == IDCANCEL )
			return FALSE;
	}

	CloseHandle(hMutex);

	// 开始解压文件
	CPKVfs* pReadCPK = new CPKVfs;
	CPKVfs* pCPK = new CPKVfs;

	if( !pReadCPK->Load(strCPKFile) )
	{
		//加载补丁文件失败
		SAFE_DEL(pReadCPK);
		SAFE_DEL(pCPK);
		return FALSE;
	}

	tagCPKHeader *pCPKHeader = pReadCPK->GetCPKHeader();
	tagCPKTable  *pTable = pReadCPK->GetCPKTable();
	m_dwTotalFileNum = pCPKHeader->dwFileNum;

	// 更新
	DWORD dwUnPackNum = 0;
	bRet = UnpackCpk(0, dwUnPackNum, pReadCPK, pCPK);

	//关闭该文件
	pReadCPK->Unload();
	pCPK->Unload();
	SAFE_DEL(pReadCPK);
	SAFE_DEL(pCPK);

	if( FALSE == bRet )
		return FALSE;

	// 所有的都解压成功
	// 更新版本到文件
	RefreshGameVersion(m_strToGameVersion);

	//删除相关文件
	::DeleteFileA(strCPKFile);
	return TRUE;
}



//-------------------------------------------------------------------------------
//将一个cpk包解压出指定的目录
//-------------------------------------------------------------------------------
BOOL Process::UnpackCpk(DWORD dwFatherCRC, DWORD& dwUnPackNum, CPKVfs *pReadCpk, CPKVfs *pWriteCpk)
{
	if( dwFatherCRC != 0 )
	{
		//先查找该项
		INT nTableIndex = pReadCpk->GetTableIndexFromeCrc(dwFatherCRC);
		if( nTableIndex == GT_INVALID )
		{
			ErrMsg(g_StrTable[_T("ItemNotExistInCPK")]);
			return TRUE;
		}

		//打开文件映像句柄
		DWORD dwFileCPKHandle = pReadCpk->Open(nTableIndex);	//打开
		if( 0 == dwFileCPKHandle )
		{
			ErrMsg(g_StrTable[_T("FailInOpenCPKItem")]);
			return TRUE;
		}

		CPKFile *pCPKFileHandle = (CPKFile*)dwFileCPKHandle;

		//虽然是路径，但如果是cpk格式，那说明在磁盘上是cpk文件
		CHAR szFullName[MAX_PATH];
		LPSTR lpExtName = NULL;

		strcpy(szFullName, pCPKFileHandle->szFullName);
		lpExtName = strrchr(szFullName, '.');

		//判断是不是cpk文件
		if( lpExtName && _stricmp(lpExtName, ".cpk") == 0 )
		{
			dwUnPackNum++;
			pWriteCpk->Unload();
			if( FALSE == CDiscIOMgr::IsExist(pCPKFileHandle->szFullName) )
			{
				//先创建好相应的路径
				char szTemp[MAX_PATH];
				for(int i=0; pCPKFileHandle->szFullName[i] != '\0'; i++)
				{
					if( pCPKFileHandle->szFullName[i] == '\\' && i > 0 )
					{
						ZeroMemory(szTemp, sizeof(szTemp));
						strncpy(szTemp, pCPKFileHandle->szFullName, i);
						::CreateDirectoryA(szTemp, NULL);
					}
				}

				// 创建cpk文件
				if( FALSE == pWriteCpk->Create(pCPKFileHandle->szFullName) )
				{
					// 创建文件失败
					ErrMsg(g_StrTable[_T("CreateCPKFailed")]);
					pReadCpk->Close(dwFileCPKHandle);
					return TRUE;
				}
			}
			else
			{
				// 加载它
				if( FALSE == pWriteCpk->Load(pCPKFileHandle->szFullName) )
				{
					pReadCpk->Close(dwFileCPKHandle);
					return TRUE;
				}
			}

			// 关闭该句柄
			pReadCpk->Close(dwFileCPKHandle);

			// 这是一个cpk文件,我们把它所有的子目录放到该CPK文件里面去
			return UnpackCPKFromCPK(dwFatherCRC, dwUnPackNum, pReadCpk, pWriteCpk);
		}
		else
		{
			// 不是cpk文件,那么直接在磁盘上创建目录
			CreateDirToDisc(pCPKFileHandle->szFullName);
			dwUnPackNum++;
		}

		//牢记用完之后关闭句柄
		pReadCpk->Close(dwFileCPKHandle);
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
			SAFE_DEL_ARRAY(pdwChildArray);
			return FALSE;
		}

		pReadCpk->GetFirstLevelChildDir(dwFatherCRC, dwChildDirNum, pdwChildArray);
		for(DWORD i=0; i<dwChildDirNum; i++)
		{
			//对所有子目录, 嵌套调用解压
			if( FALSE == UnpackCpk(pdwChildArray[i], dwUnPackNum, pReadCpk, pWriteCpk) )
			{
				SAFE_DEL_ARRAY(pdwChildArray);
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
				SAFE_DEL_ARRAY(pdwChildArray);
				return FALSE;
			}
			//对所有一级子文件, 直接调用文件解压函数
			if( FALSE == UnpackSimpleFile(pdwChildArray[i], dwUnPackNum, pReadCpk) )
			{
				SAFE_DEL_ARRAY(pdwChildArray);
				return FALSE;
			}
		}
	}

	SAFE_DEL_ARRAY(pdwChildArray);
	return TRUE;
}
//-------------------------------------------------------------------------------
//从cpk文件解压出单个的文件(磁盘上);
//-------------------------------------------------------------------------------
BOOL Process::UnpackSimpleFile(DWORD dwCRC, DWORD& dwUnPackNum, CPKVfs *pReadCPK)
{
	dwUnPackNum++;

	// 首先查找tableindex
	INT nTableIndex = pReadCPK->GetTableIndexFromeCrc(dwCRC);
	if( nTableIndex == GT_INVALID )
	{
		ErrMsg(g_StrTable[_T("ItemNotExistInCPK")]);
		return TRUE;
	}

	// 打开文件映像句柄
	DWORD dwCPKFileHandle = pReadCPK->Open(nTableIndex);	//打开
	if( 0 == dwCPKFileHandle )
	{
		ErrMsg(g_StrTable[_T("FailInOpenCPKItem")]);
		return TRUE;
	}
	CPKFile *pCPKFileHandle = (CPKFile*)dwCPKFileHandle;

__loop_retry:		//重新尝试的标签
	DWORD dwFileHandle = CreateFileToDisc(pCPKFileHandle->szFullName);
	if( INVALID_FILE_HANDLE == dwFileHandle )
	{
		INT nRet = MessageBox(NULL, m_pUtil->AnsiToUnicode(pCPKFileHandle->szFullName), 
			g_StrTable[_T("CreateFileFailed")], MB_ABORTRETRYIGNORE);
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

	// 对大文件进行部分映射，循环写
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

	PostMessage(m_hWndMain, WM_USER_PROGRESS_POS, (INT)((100 * dwUnPackNum) / m_dwTotalFileNum ), 0);

	//关闭相关句柄
	CDiscIOMgr::Close(dwFileHandle);
	pReadCPK->Close(dwCPKFileHandle);

	return TRUE;
}


//-------------------------------------------------------------------------------
//从一个cpk包解压出里面的子cpk包的目录出来
//-------------------------------------------------------------------------------
BOOL Process::UnpackCPKFromCPK(DWORD dwDirCRC, DWORD& dwUnPackNum, CPKVfs *pReadCPK, CPKVfs *pWriteCPK)
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
				SAFE_DEL_ARRAY(pdwChildArray);
				return FALSE;
			}

			Sleep( 0 );		//休息缓冲
			dwUnPackNum++;

			INT nTableIndex = pReadCPK->GetTableIndexFromeCrc(pdwChildArray[i]);
			if( nTableIndex == GT_INVALID )
			{
				ErrMsg(g_StrTable[_T("ItemNotExistInCPK")]);
				continue;
			}

			//打开文件映像句柄
			DWORD dwCPKFileHandle = pReadCPK->Open(nTableIndex, 0, true );	//打开
			if( 0 == dwCPKFileHandle )
			{
				ErrMsg(g_StrTable[_T("FailInOpenCPKItem")]);
				continue;
			}

			CPKFile *pCPKFileHandle = (CPKFile*)dwCPKFileHandle;

			// 确实是文件,我们可以先创建好路径,然后再加入文件
			// 首先一步一步剥离路径,比如c:\bin\tidy, c:\, c:\bin, c:\bin\tidy
			CHAR szFileName[MAX_PATH], szCPKName[MAX_PATH];
			ZeroMemory(szFileName, sizeof(szFileName));
			ZeroMemory(szCPKName, sizeof(szCPKName));

			BOOL bRet = pReadCPK->IsFileForCPK(pCPKFileHandle->szFullName, szCPKName, szFileName);
			if( !bRet )
			{
				ErrMsg(g_StrTable[_T("FailInCheckCPKFile")]);
			}
			

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
					ErrMsg(g_StrTable[_T("FailInWriteToCPK")]);
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

			PostMessage(m_hWndMain, WM_USER_PROGRESS_POS, (INT)((100L * dwUnPackNum) / m_dwTotalFileNum), 0);
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
				SAFE_DEL_ARRAY(pdwChildArray);
				return FALSE;
			}

			dwUnPackNum++;

			INT nTableIndex = pReadCPK->GetTableIndexFromeCrc(pdwChildArray[i]);
			if( nTableIndex == GT_INVALID )
			{
				ErrMsg(g_StrTable[_T("ItemNotExistInCPK")]);
				continue;
			}

			//打开文件映像句柄
			DWORD dwCPKFileHandle = pReadCPK->Open(nTableIndex);	//打开
			if( 0 == dwCPKFileHandle )
			{
				ErrMsg(g_StrTable[_T("FailInOpenCPKItem")]);
				continue;
			}

			CPKFile *pCPKFileHandle = (CPKFile*)dwCPKFileHandle;

			//先创建
			char szFileName[MAX_PATH], szCPKName[MAX_PATH];
			ZeroMemory(szFileName, sizeof(szFileName));
			ZeroMemory(szCPKName, sizeof(szCPKName));

			BOOL bRet = pReadCPK->IsFileForCPK(pCPKFileHandle->szFullName, szCPKName, szFileName);
			if( !bRet )
			{
				ErrMsg(g_StrTable[_T("FailInCheckCPKFile")]);
			}

			if( bRet == TRUE )
			{
				//只是路径
				pWriteCPK->AddDirCyc(szFileName, 0); 
			}

			//关闭文件映像句柄
			pReadCPK->Close(dwCPKFileHandle);

			if( FALSE == UnpackCPKFromCPK(pdwChildArray[i], dwUnPackNum, pReadCPK, pWriteCPK) )
			{
				SAFE_DEL_ARRAY(pdwChildArray);
				return FALSE;
			}
		}
	}

	SAFE_DEL_ARRAY(pdwChildArray);
	return TRUE;
}



//--------------------------------------------------------------------------------
// 更新版本号
//--------------------------------------------------------------------------------
BOOL Process::RefreshGameVersion(LPCTSTR szToVersion)
{
	::WritePrivateProfileString(_T("Launcher"), _T("LocalGame"), szToVersion, m_strIniFileFullName);
	m_strLocalGameVersion = szToVersion;

	SendMessage(m_hWndMain, WM_USER_GAME_VER, (WPARAM)(LPCTSTR)m_strLocalGameVersion, 0);
	return TRUE;
}


//------------------------------------------------------------------------------
// 往磁盘上创建目录
//------------------------------------------------------------------------------
BOOL Process::CreateDirToDisc(LPCSTR szPath)
{
	ASSERT(szPath);

	//首先一步一步剥离路径,比如c:\bin\tidy, c:\, c:\bin\tidy
	CHAR szTemp[MAX_PATH];
	for(int i=0; szPath[i] != '\0'; i++)
	{
		if( szPath[i] == '\\' && i > 0 )
		{
			ZeroMemory(szTemp, sizeof(szTemp));
			strncpy(szTemp, szPath, i);

			::CreateDirectoryA(szTemp, NULL);
		}
	}

	return ::CreateDirectoryA(szPath, NULL);
}


//------------------------------------------------------------------------------
// 往磁盘上创建文件
//------------------------------------------------------------------------------
DWORD Process::CreateFileToDisc(LPCSTR szFileName)
{
	ASSERT( szFileName );
	if( TRUE == CDiscIOMgr::IsExist(szFileName) )
	{
		DWORD fileAttributes = GetFileAttributesA(szFileName);
		fileAttributes &= ~FILE_ATTRIBUTE_READONLY;
		SetFileAttributesA(szFileName, fileAttributes);
		if( FALSE == ::DeleteFileA(szFileName) )
		{
			ErrMsg(g_StrTable[_T("DeleteFileFailed")], m_pUtil->AnsiToUnicode(szFileName));
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
// 由任意文件名得到简单文件名(不带路径)
//-----------------------------------------------------------------------------
BOOL Process::GetSimpleFileName(LPTSTR pSimpleName, LPCTSTR pFullName)
{
	ASSERT(pSimpleName);
	ASSERT(pFullName);

	LPCTSTR lpSimpleName = _tcsrchr(pFullName, _T('\\'));
	if (NULL == lpSimpleName)
	{
		lpSimpleName = _tcsrchr(pFullName, _T('/'));
		if ( NULL == lpSimpleName )
			lstrcpy(pSimpleName, pFullName);
		else
			lstrcpy(pSimpleName, lpSimpleName+1);
	}
	else
	{
		lstrcpy(pSimpleName, lpSimpleName+1);
	}
	return TRUE;
}


//-----------------------------------------------------------------------------
// 强制结束
//-----------------------------------------------------------------------------
VOID Process::Terminate()
{
	m_bTerminateThread = TRUE; 
	m_pHttp->Terminate();
}