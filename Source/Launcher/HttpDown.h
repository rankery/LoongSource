#pragma once


class Util;
class StreamTransport;
class DownloadInfo;

class HttpSocket  
{
public:

	BOOL		Download(LPCTSTR szSrcFileName, BOOL bCache=TRUE);
	LPCTSTR		GetDestFileName() { return m_strDestFileName; }
	DWORD		WaitForEnd(DWORD dwTime) { return ::WaitForSingleObject(m_hThread, dwTime); }

	VOID		Terminate() { m_bTerminateThread = TRUE; }
	BOOL		GetResult() { return m_bResult; }

	INT64		GetFileSize() { return m_n64FileSize; }
	INT64		GetCompletedSize() { return m_n64CompletedSize; }

	VOID		SetWndMain(HWND hWnd) { m_hWndMain = hWnd; }


	HttpSocket();
	virtual ~HttpSocket();


protected:	

	int		GetServerState();	// 返回服务器状态码 -1表示不成功
	INT64	GetHttpFileSize();		// 返回文件长度 -1表示不成功
	BOOL	GetNewLocation(CStringA& strLocation);
	
	BOOL	GetResponseHeader(); // 获取返回头
	INT		FormatRequestHeader(const char *pServer,const char *pObject, INT64 nFrom=0);	//格式化请求头

	BOOL	SendRequest(const char* pRequestHeader = NULL,long Length = 0);
	BOOL	Connect(const char* szHostName,int nPort=80);

	UINT	DownloadThread(void *pArg);

	BOOL	ParseDestFileName(LPCTSTR szObjName);	// 分析目标文件名

	VOID	ShowLog(LPCTSTR	szText);
	VOID	ShowInfo(LPCTSTR szText);


	TSFPTrunk<HttpSocket>	m_Trunk;
	char					m_szRequestHeader[2048];		// 请求头
	char					m_szResponseHeader[2048];		// 回应头

	Util*					m_pUtil;
	StreamTransport*		m_pStream;
	
	HWND					m_hWndMain;
	CString					m_strSrcFileName;
	CString					m_strDestFileName;

	CFile					m_DownloadFile;
	INT64					m_n64CompletedSize;
	INT64					m_n64FileSize;

	TCHAR					m_szShowInfo[1024*32];
	TCHAR					m_szShowLog[1024*32];

	HANDLE					m_hThread;
	volatile	BOOL		m_bTerminateThread;
	BOOL					m_bResult;
	BOOL					m_bCache;
};