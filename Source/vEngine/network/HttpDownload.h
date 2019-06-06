//-----------------------------------------------------------------------------
//!\file HttpDownload.h
//!\author Lyp
//!
//!\date 2003-03-17
//! last 2009-04-27
//!
//!\brief Http下载
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine {

class VENGINE_API HttpDownload
{
public:

	BOOL		Download(LPCTSTR szSrcFileName, BOOL bCache=TRUE);
	LPCTSTR		GetDestFileName() { return m_strDestFileName.c_str(); }
	DWORD		WaitForEnd(DWORD dwTime) { return ::WaitForSingleObject(m_hThread, dwTime); }

	VOID		Terminate() { m_bTerminateThread = TRUE; }
	BOOL		GetResult() { return m_bResult; }

	INT64		GetFileSize() { return m_n64FileSize; }
	INT64		GetCompletedSize() { return m_n64CompletedSize; }

	HttpDownload();
	~HttpDownload();


protected:	

	TSFPTrunk<HttpDownload>	m_Trunk;
	TObjRef<DiscIOMgr>		m_pDiscIOMgr;
	CHAR					m_szRequestHeader[4096];		// 请求头
	CHAR					m_szResponseHeader[4096];		// 回应头

	Util*					m_pUtil;
	StreamTransport*		m_pStream;

	tstring					m_strSrcFileName;
	tstring					m_strDestFileName;

	DWORD					m_dwFileHandle;
	INT64					m_n64CompletedSize;
	INT64					m_n64FileSize;

	HANDLE					m_hThread;
	BOOL					m_bTerminateThread;
	BOOL					m_bResult;
	BOOL					m_bCache;


	INT		GetServerState();	// 返回服务器状态码 -1表示不成功
	INT64	GetHttpFileSize();		// 返回文件长度 -1表示不成功
	BOOL	GetNewLocation(std::string &strLocation);

	BOOL	GetResponseHeader(); // 获取返回头
	INT		FormatRequestHeader(LPCSTR pServer, LPCSTR pObject, INT64 nFrom=0);	//格式化请求头

	BOOL	SendRequest(LPCSTR pRequestHeader=NULL, long Length=0);
	BOOL	Connect(LPCSTR szHostName, INT nPort=80);

	UINT	DownloadThread(LPVOID pArg);

	BOOL	ParseDestFileName(LPCTSTR szObjName);	// 分析目标文件名

	BOOL	AfxParseURL(LPCTSTR pstrURL, DWORD& dwServiceType, tstring& strServer, 
		tstring& strObject, INTERNET_PORT& nPort);
	BOOL	_AfxParseURLWorker(LPCTSTR pstrURL, LPURL_COMPONENTS lpComponents, 
		DWORD& dwServiceType, INTERNET_PORT& nPort, DWORD dwFlags);
};

}