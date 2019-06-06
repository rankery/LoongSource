#pragma once



class Process  
{
public:
	BOOL Work(HWND hWndMain);
	
	HttpSocket*	GetHttp() { return m_pHttp; }
	LPCTSTR	GetURL() { return m_strURL; }
	LPCTSTR	GetManualURL() { return m_strManualURL; }
	

	VOID	Terminate();
	DWORD	WaitForEnd(DWORD dwTime) { return ::WaitForSingleObject(m_hThread, dwTime); }

	LPCTSTR GetGameFullName() { return (LPCTSTR)m_strGameExe; }
	LPCTSTR GetAppPath() { return (LPCTSTR)m_strAppPath; }
	LPCTSTR GetCfgFileFullName() { return (LPCTSTR)m_strCfgFileFullName; }

	Process();
	virtual ~Process();

protected:	
	
	// 读取launcher.ini配置
	BOOL	GetConfig(LPCTSTR szIniFileName);

	// 分析launcher本身版本,0:已经是最新版，1:需要升级
	INT		ParseLauncherVersion();
	// 更新launcher版本
	BOOL	RefreshLauncherVersion();

	// 分析游戏版本,0:已经是最新版，1:找到新的补丁，-1:无法找到合适的补丁
	INT		ParseGameVersion();

	// 工作线程
	UINT	WorkThread(void *pArg);

	// 更新自己
	BOOL	UpdateSelf();

	// 解压缩
	BOOL	Unpack(CStringA& strCPKFile);

	//将一个cpk包解压出指定的目录
	BOOL	UnpackCpk(DWORD dwFatherCRC, DWORD& dwUnPackNum, CPKVfs *pReadCpk, CPKVfs *pWriteCpk);
	//从cpk文件解压出单个的文件(磁盘上);
	BOOL	UnpackSimpleFile(DWORD dwCRC, DWORD& dwUnPackNum, CPKVfs *pReadCPK);
	//从一个cpk包解压出里面的子cpk包的目录出来
	BOOL	UnpackCPKFromCPK(DWORD dwDirCRC, DWORD& dwUnPackNum, CPKVfs *pReadCPK, CPKVfs *pWriteCPK);
	//更新版本号
	BOOL	RefreshGameVersion(LPCTSTR szToVersion);
	// 往磁盘上创建目录
	BOOL	CreateDirToDisc(LPCSTR szPath);
	// 往磁盘上创建文件
	DWORD	CreateFileToDisc(LPCSTR szFileName);
	// 由任意文件名得到简单文件名(不带路径)
	BOOL	GetSimpleFileName(LPTSTR pSimpleName, LPCTSTR pFullName);
	
	TSFPTrunk<Process>		m_Trunk;
	HttpSocket*				m_pHttp;
	Util*					m_pUtil;
	HWND					m_hWndMain;
	BOOL					m_bConfigLoaded;

	CString					m_strAppPath;
	CString					m_strGameExe;				// 游戏EXE位置
	CString					m_strIniFileFullName;		// INI 全名
	CString					m_strCfgFileFullName;		// 配置文件全名(一般为gamecfg.xml)
	CString					m_strSrcPatchInfo;			// patchinfo.txt服务器位置
	CString					m_strDestPatchInfo;			// patchinfo.txt本地位置
	CString					m_strSrcServerInfo;			// serverinfo.txt服务器位置
	CString					m_strDestServerInfo;		// serverinfo.txt本地位置
	CString					m_strSrcNewLauncher;		// 新版本launcher服务器位置
	CString					m_strDestNewLauncher;		// 新版本launcher本地位置
	CString					m_strLocalLauncherVersion;	// 当前launcher版本
	CString					m_strLocalGameVersion;		// 当前游戏版本
	CString					m_strLatestLauncherVersion;	// 最新launcher版本
	CString					m_strLatestGameVersion;		// 最新游戏版本

	CString					m_strURL;					// 主窗口显示页面
	CString					m_strManualURL;				// 官方手动补丁下载页面

	CString					m_strSrcGamePatch;			// 补丁服务器位置
	CString					m_strToGameVersion;			// 新版本号
	std::string				m_strGamePatchMD5;			// 补丁MD5


	DWORD					m_dwTotalFileNum;			// 总共需解压的文件
	volatile	BOOL		m_bTerminateThread;
	HANDLE					m_hThread;
};