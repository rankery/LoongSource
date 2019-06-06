//-----------------------------------------------------------------------------
//!\file HttpDownload.cpp
//!\author Lyp
//!
//!\date 2003-03-17
//! last 2009-04-27
//!
//!\brief Http下载
//-----------------------------------------------------------------------------
#include "..\StdAfx.h"
#include "HttpDownload.h"

//#include "util.h"
#include "stream_transport.h"
#include "..\system\disciomgr.h"


#define RECV_TIME_OUT (1000*6)

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "wininet.lib")

namespace vEngine {
//-----------------------------------------------------------------------------
// Construction/Destruction
//-----------------------------------------------------------------------------
HttpDownload::HttpDownload():m_Trunk(this)
{
	ZeroMemory(m_szRequestHeader, sizeof(m_szRequestHeader));
	ZeroMemory(m_szResponseHeader, sizeof(m_szResponseHeader));

	m_pStream = new StreamTransport;
	m_pUtil	= new Util;
	m_dwFileHandle = INVALID_FILE_HANDLE;
	m_hThread = 0;
	m_bTerminateThread = FALSE;
	m_bResult = FALSE;
}

HttpDownload::~HttpDownload()
{
	m_bTerminateThread = TRUE;
	if( m_hThread )
	{
		WaitForSingleObject(m_hThread, INFINITE);
		SAFE_CLOSE_HANDLE(m_hThread);
	}
	SAFE_DEL(m_pUtil);
	SAFE_DEL(m_pStream);
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BOOL HttpDownload::Connect(const char *szHostName, int nPort)
{
	if(szHostName==NULL)
		return FALSE;

	m_pStream->Init(TRUE, ESTM_Text);

	///根据主机名获得IP地址
	hostent* pHostent = gethostbyname(szHostName);
	if( pHostent == NULL )
		return FALSE;

	///连接
	struct in_addr ip_addr;
	memcpy(&ip_addr, pHostent->h_addr_list[0], 4); // h_addr_list[0]里4个字节,每个字节8位

	struct sockaddr_in destaddr;
	memset((void *)&destaddr,0,sizeof(destaddr)); 
	destaddr.sin_family=AF_INET;
	destaddr.sin_port=htons(nPort);
	destaddr.sin_addr=ip_addr;


	///保存主机的IP地址字符串
	char szIP[256];
	sprintf(szIP,"%d.%d.%d.%d",
		destaddr.sin_addr.S_un.S_un_b.s_b1,
		destaddr.sin_addr.S_un.S_un_b.s_b2,
		destaddr.sin_addr.S_un.S_un_b.s_b3,
		destaddr.sin_addr.S_un.S_un_b.s_b4);

	m_pStream->TryToConnect(szIP, nPort);
	return TRUE;
}

//-----------------------------------------------------------------------------
// 根据请求的相对URL输出HTTP请求头
//-----------------------------------------------------------------------------
INT HttpDownload::FormatRequestHeader(const char *pServer,const char *pObject, INT64 nFrom)
{
	ZeroMemory(m_szRequestHeader, sizeof(m_szRequestHeader));

	// 方法,请求的路径,版本
	strcat(m_szRequestHeader,"GET ");
	strcat(m_szRequestHeader,pObject);
	strcat(m_szRequestHeader," HTTP/1.1");
	strcat(m_szRequestHeader,"\r\n");

	// 主机
	strcat(m_szRequestHeader,"Host:");
	strcat(m_szRequestHeader,pServer);
	strcat(m_szRequestHeader,"\r\n");

	// 接收的数据类型
	strcat(m_szRequestHeader,"Accept:*/*");
	strcat(m_szRequestHeader,"\r\n");

	// 浏览器类型
	strcat(m_szRequestHeader,"User-Agent:Mozilla/4.0 (compatible; MSIE 5.00; Windows 98)");
	strcat(m_szRequestHeader,"\r\n");

	// 连接设置,保持
	strcat(m_szRequestHeader,"Connection:Keep-Alive");
	strcat(m_szRequestHeader,"\r\n");

	if( !m_bCache )
	{
		strcat(m_szRequestHeader,"Cache-Control: no-cache");
		strcat(m_szRequestHeader,"\r\n");
	}

	// 请求的数据起始字节位置(断点续传的关键)
	if( nFrom > 0 )
	{
		char szTemp[20];
		strcat(m_szRequestHeader,"Range: bytes=");
		_i64toa(nFrom,szTemp,10);
		strcat(m_szRequestHeader,szTemp);
		strcat(m_szRequestHeader,"-");
		strcat(m_szRequestHeader,"\r\n");
	}

	// 最后空行
	strcat(m_szRequestHeader,"\r\n");

	///返回结果
	return strlen(m_szRequestHeader);
}


//-----------------------------------------------------------------------------
///发送请求头
//-----------------------------------------------------------------------------
BOOL HttpDownload::SendRequest(const char *pRequestHeader, long Length)
{
	if( !m_pStream->IsConnected() )
		return FALSE;

	m_pStream->Send((LPVOID)pRequestHeader, Length);
	return TRUE;
}


//-----------------------------------------------------------------------------
//获取HTTP请求的返回头
//-----------------------------------------------------------------------------
BOOL HttpDownload::GetResponseHeader()
{
	INT nIndex = 0;
	ZeroMemory(m_szResponseHeader, sizeof(m_szResponseHeader));
	DWORD dwLastRecvTime = timeGetTime();
	while( nIndex < (sizeof(m_szResponseHeader)/sizeof(CHAR)) )
	{
		DWORD dwMsgSize = 0;
		LPBYTE pRecv = m_pStream->Recv(dwMsgSize);
		if( !pRecv )
		{
			if( !m_pStream->IsConnected() || timeGetTime() - dwLastRecvTime > RECV_TIME_OUT*2)
				return FALSE;

			::Sleep(100);
			if( m_bTerminateThread )
				return FALSE;

			continue;
		}

		dwLastRecvTime = timeGetTime();
		if( nIndex + dwMsgSize >= (sizeof(m_szResponseHeader)/sizeof(CHAR)) )
			return FALSE;
		memcpy(&m_szResponseHeader[nIndex], pRecv, dwMsgSize);
		m_pStream->FreeRecved(pRecv);

		nIndex += dwMsgSize;
		if(nIndex >= 4)
		{
			if( m_szResponseHeader[nIndex - 4] == '\r' 
				&& m_szResponseHeader[nIndex - 3] == '\n'
				&& m_szResponseHeader[nIndex - 2] == '\r'
				&& m_szResponseHeader[nIndex - 1] == '\n')
				break;
		}

	}

	return TRUE;
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
INT64 HttpDownload::GetHttpFileSize()
{
	LPCSTR szContent1 = "content-range: bytes";
	LPCSTR szContent2 = "content-length";
	std::string strValue = "-1";


	//取得某个域值
	std::string strRespons = m_szResponseHeader;
	transform(strRespons.begin(), strRespons.end(), strRespons.begin(), tolower);
	
	int nPos = -1;
	nPos = strRespons.find(szContent1, 0);
	if( nPos != strRespons.npos )	// 找到了
	{
		int nPos1 = strRespons.find("/", nPos);
		int nPos2 = strRespons.find("\r\n", nPos);
		strValue = strRespons.substr(nPos1+1, nPos2-nPos1-1);
	}
	else
	{
		nPos = strRespons.find(szContent2, 0);
		if( nPos != strRespons.npos )
		{
			int nPos1 = strRespons.find(":", nPos);
			int nPos2 = strRespons.find("\r\n", nPos);
			strValue = strRespons.substr(nPos1+1, nPos2-nPos1-1);
		}
	}

	return _atoi64(strValue.c_str());
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BOOL HttpDownload::GetNewLocation(std::string &strLocation)
{
	LPCSTR szContent = "location:";

	//取得域
	std::string strRespons = m_szResponseHeader;
	transform(strRespons.begin(), strRespons.end(), strRespons.begin(), tolower);

	int nPos = -1;
	nPos = strRespons.find(szContent, 0);
	if( nPos == strRespons.npos )	// 没找到
		return FALSE;

	int nPos1 = strRespons.find(":",nPos);
	int nPos2 = strRespons.find("\r\n",nPos);
	strLocation = strRespons.substr(nPos1+2, nPos2-nPos1-2);
	return TRUE;
}


//-----------------------------------------------------------------------------
// 分析目标文件名
//-----------------------------------------------------------------------------
BOOL HttpDownload::ParseDestFileName(LPCTSTR szObjName)
{
	tstring strTemp = szObjName;
	INT nPos = strTemp.rfind(_T('/'));
	if( nPos == strTemp.npos )
		nPos = 0;
	else
		nPos += 1;

	m_strDestFileName = _T("temp\\") + strTemp.substr(nPos, strTemp.npos);
	return TRUE;
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int HttpDownload::GetServerState()
{
	char szState[3];
	szState[0] = m_szResponseHeader[9];
	szState[1] = m_szResponseHeader[10];
	szState[2] = m_szResponseHeader[11];

	return atoi(szState);
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
#define AFX_INET_SERVICE_FTP        INTERNET_SERVICE_FTP
#define AFX_INET_SERVICE_HTTP       INTERNET_SERVICE_HTTP
#define AFX_INET_SERVICE_GOPHER     INTERNET_SERVICE_GOPHER

#define AFX_INET_SERVICE_UNK        0x1000
#define AFX_INET_SERVICE_FILE       (AFX_INET_SERVICE_UNK+1)
#define AFX_INET_SERVICE_MAILTO     (AFX_INET_SERVICE_UNK+2)
#define AFX_INET_SERVICE_MID        (AFX_INET_SERVICE_UNK+3)
#define AFX_INET_SERVICE_CID        (AFX_INET_SERVICE_UNK+4)
#define AFX_INET_SERVICE_NEWS       (AFX_INET_SERVICE_UNK+5)
#define AFX_INET_SERVICE_NNTP       (AFX_INET_SERVICE_UNK+6)
#define AFX_INET_SERVICE_PROSPERO   (AFX_INET_SERVICE_UNK+7)
#define AFX_INET_SERVICE_TELNET     (AFX_INET_SERVICE_UNK+8)
#define AFX_INET_SERVICE_WAIS       (AFX_INET_SERVICE_UNK+9)
#define AFX_INET_SERVICE_AFS        (AFX_INET_SERVICE_UNK+10)
#define AFX_INET_SERVICE_HTTPS      (AFX_INET_SERVICE_UNK+11)


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BOOL HttpDownload::AfxParseURL(LPCTSTR pstrURL, DWORD& dwServiceType,
							   tstring& strServer, tstring& strObject, INTERNET_PORT& nPort)
{
	dwServiceType = AFX_INET_SERVICE_UNK;

	TCHAR szHostName[INTERNET_MAX_URL_LENGTH+1];
	TCHAR szUrlPath[INTERNET_MAX_URL_LENGTH+1];

	URL_COMPONENTS urlComponents;
	memset(&urlComponents, 0, sizeof(URL_COMPONENTS));
	urlComponents.dwStructSize = sizeof(URL_COMPONENTS);

	urlComponents.dwHostNameLength = INTERNET_MAX_URL_LENGTH;
	urlComponents.lpszHostName = szHostName;
	urlComponents.dwUrlPathLength = INTERNET_MAX_URL_LENGTH;
	urlComponents.lpszUrlPath = szUrlPath;

	BOOL bRetVal = _AfxParseURLWorker(pstrURL, &urlComponents,
		dwServiceType, nPort, ICU_BROWSER_MODE);

	strServer = szHostName;
	strObject = szUrlPath;
	return bRetVal;
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BOOL HttpDownload::_AfxParseURLWorker(LPCTSTR pstrURL,
										  LPURL_COMPONENTS lpComponents, DWORD& dwServiceType,
										  INTERNET_PORT& nPort, DWORD dwFlags)
{
	LPTSTR pstrCanonicalizedURL;
	TCHAR szCanonicalizedURL[INTERNET_MAX_URL_LENGTH];
	DWORD dwNeededLength = INTERNET_MAX_URL_LENGTH;
	BOOL bRetVal;
	BOOL bMustFree = FALSE;

	// Decoding is done in InternetCrackUrl/UrlUnescape 
	// so we don't need the ICU_DECODE flag here.

	DWORD dwCanonicalizeFlags = dwFlags &
		(ICU_NO_ENCODE | ICU_NO_META |
		ICU_ENCODE_SPACES_ONLY | ICU_BROWSER_MODE);

	DWORD dwCrackFlags = 0;

	BOOL bUnescape = FALSE;

	if((dwFlags & (ICU_ESCAPE | ICU_DECODE)) && (lpComponents->dwUrlPathLength != 0) )
	{

		// We use only the ICU_ESCAPE flag for decoding even if
		// ICU_DECODE is passed.

		// Also, if ICU_BROWSER_MODE is passed we do the unescaping
		// manually because InternetCrackUrl doesn't do
		// Browser mode unescaping

		if (dwFlags & ICU_BROWSER_MODE)
			bUnescape = TRUE;
		else
			dwCrackFlags |= ICU_ESCAPE;
	}

	bRetVal = InternetCanonicalizeUrl(pstrURL, szCanonicalizedURL,
		&dwNeededLength, dwCanonicalizeFlags);

	if (!bRetVal)
	{
		if (::GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			return FALSE;

		pstrCanonicalizedURL = new TCHAR[dwNeededLength];
		if (pstrCanonicalizedURL == NULL)
			return FALSE;

		bMustFree = TRUE;
		bRetVal = InternetCanonicalizeUrl(pstrURL, pstrCanonicalizedURL,
			&dwNeededLength, dwCanonicalizeFlags);
		if (!bRetVal)
		{
			delete [] pstrCanonicalizedURL;
			return FALSE;
		}
	}
	else
		pstrCanonicalizedURL = szCanonicalizedURL;

	// now that it's safely canonicalized, crack it

	bRetVal = InternetCrackUrl(pstrCanonicalizedURL, 0,
		dwCrackFlags, lpComponents);

	if(bUnescape)
	{
		if(FAILED(UrlUnescape(lpComponents->lpszUrlPath,NULL,NULL,URL_UNESCAPE_INPLACE | URL_DONT_UNESCAPE_EXTRA_INFO)))
		{
			if (bMustFree)
				delete [] pstrCanonicalizedURL;

			return FALSE;
		}

		lpComponents->dwUrlPathLength = lstrlen(lpComponents->lpszUrlPath);
	}

	if (bMustFree)
		delete [] pstrCanonicalizedURL;

	// convert to MFC-style service ID

	if (!bRetVal)
		dwServiceType = AFX_INET_SERVICE_UNK;
	else
	{
		nPort = lpComponents->nPort;
		switch (lpComponents->nScheme)
		{
		case INTERNET_SCHEME_FTP:
			dwServiceType = AFX_INET_SERVICE_FTP;
			break;

		case INTERNET_SCHEME_GOPHER:
			dwServiceType = AFX_INET_SERVICE_GOPHER;
			break;

		case INTERNET_SCHEME_HTTP:
			dwServiceType = AFX_INET_SERVICE_HTTP;
			break;

		case INTERNET_SCHEME_HTTPS:
			dwServiceType = AFX_INET_SERVICE_HTTPS;
			break;

		case INTERNET_SCHEME_FILE:
			dwServiceType = AFX_INET_SERVICE_FILE;
			break;

		case INTERNET_SCHEME_NEWS:
			dwServiceType = AFX_INET_SERVICE_NNTP;
			break;

		case INTERNET_SCHEME_MAILTO:
			dwServiceType = AFX_INET_SERVICE_MAILTO;
			break;

		default:
			dwServiceType = AFX_INET_SERVICE_UNK;
		}
	}

	return bRetVal;
}



//-----------------------------------------------------------------------------
// 启动下载
//-----------------------------------------------------------------------------
BOOL HttpDownload::Download(LPCTSTR szSrcFileName, BOOL bCache)
{
	m_bResult = FALSE;
	m_strSrcFileName = szSrcFileName;
	m_bCache = bCache;
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, (THREADPROC)m_Trunk.sfp1
		(&HttpDownload::DownloadThread), 0, 0, NULL);

	return TRUE;
}



//-----------------------------------------------------------------------------
// 下载线程
//-----------------------------------------------------------------------------
UINT HttpDownload::DownloadThread(void *pArg)
{
	DWORD dwServiceType;
	INTERNET_PORT nPort;
	tstring strServer,strObject;
	std::vector<tstring> vecFileNames;
	m_pUtil->StringToToken(vecFileNames, m_strSrcFileName.c_str(), _T(','));
	if( vecFileNames.empty() )
	{
		_endthreadex(0);
		return 0;
	}

	INT nVecIndex = 0;
	m_strSrcFileName = vecFileNames[nVecIndex].c_str();

_download_thread_start:

	if( INVALID_FILE_HANDLE != m_dwFileHandle )
	{
		m_pDiscIOMgr->Close(m_dwFileHandle);
		m_dwFileHandle = INVALID_FILE_HANDLE;
	}

	TCHAR mess[256];
	_tcscpy(mess, m_strSrcFileName.c_str());
	AfxParseURL(mess, dwServiceType, strServer, strObject, nPort);
	ParseDestFileName(strObject.c_str());
	std::string strServerA = m_pUtil->UnicodeToAnsi(strServer.c_str());
	std::string strObjectA = m_pUtil->UnicodeToAnsi(strObject.c_str());

	m_n64CompletedSize = 0;
	m_dwFileHandle = m_pDiscIOMgr->Create(m_strDestFileName.c_str(), GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);

	if( m_dwFileHandle != INVALID_FILE_HANDLE )
		m_pDiscIOMgr->GetSizeEx(m_dwFileHandle, &m_n64CompletedSize);

	long nLength = this->FormatRequestHeader(strServerA.c_str(),strObjectA.c_str(), m_n64CompletedSize);	

	// try connect
	if( FALSE == this->Connect(strServerA.c_str(), nPort) )
		goto _download_thread_exit;
	
	while(!m_pStream->IsConnected())
	{
		if( !m_pStream->IsTryingToConnect() )
			goto _download_thread_retry;

		Sleep(10);
		if( m_bTerminateThread )
			goto _download_thread_exit;
	}

	this->SendRequest(m_szRequestHeader, nLength);	// 发送下载请求

	if( FALSE == this->GetResponseHeader() )
	{
		if( m_bTerminateThread )
			goto _download_thread_exit;

		m_pStream->Disconnect();
		goto _download_thread_retry;
	}


	INT nServerState = this->GetServerState();
	if( nServerState > 299 && nServerState < 399 )	// 需要重定向
	{
		std::string strNewLocation ;
		if( this->GetNewLocation(strNewLocation) )
		{
			m_strSrcFileName = m_pUtil->AnsiToUnicode(strNewLocation.c_str());
			m_pStream->Disconnect();
			goto _download_thread_start;
		}
	}

	m_n64FileSize = this->GetHttpFileSize();
	if( m_n64FileSize == m_n64CompletedSize )
		goto _download_thread_ok;	// 已经下载完成

	if( m_n64FileSize == -1 || nServerState > 399 )	// 找不到文件长度域或者服务器返回错误状态
	{
		if( nServerState == 416 ) // 超出申请范围
		{
			m_bResult = TRUE;
			goto _download_thread_ok;	// 已经下载完成
		}

		m_pStream->Disconnect();
		goto _download_thread_retry;
	}

	if( m_dwFileHandle == INVALID_FILE_HANDLE )
	{
		goto _download_thread_exit;
	}
	else
	{
		// 检查磁盘空间
		INT64 n64Need = m_n64FileSize - m_n64CompletedSize + 4096;
		if( n64Need > m_pDiscIOMgr->GetDriveFreeSpace(NULL) )
				goto _download_thread_exit;
	}

	m_pDiscIOMgr->SeekEx(m_dwFileHandle, m_n64CompletedSize, FILE_BEGIN);
	m_pStream->SwitchTransportMode(ESTM_Stream);

	DWORD dwLastRecvTime = timeGetTime();
	DWORD dwLastShowTime = timeGetTime();
	while(m_n64CompletedSize < m_n64FileSize)
	{
		DWORD dwRecvSize = 0;
		LPBYTE pRecv = m_pStream->Recv(dwRecvSize);
		if(!pRecv)
		{
			if( !m_pStream->IsConnected() || timeGetTime() - dwLastRecvTime > RECV_TIME_OUT )	// 是否还处于连接状态
			{
				m_pStream->Disconnect();
				m_pDiscIOMgr->Close(m_dwFileHandle);
				m_dwFileHandle = INVALID_FILE_HANDLE;
				goto _download_thread_retry;
			}

			if( m_bTerminateThread )
				goto _download_thread_exit;

			Sleep(100);
			continue;
		}

		m_pDiscIOMgr->Write(m_dwFileHandle, pRecv, dwRecvSize);
		m_n64CompletedSize += dwRecvSize;
		dwLastRecvTime = timeGetTime();

		m_pStream->FreeRecved(pRecv);
	}

_download_thread_ok:
	m_bResult = TRUE;
	m_pDiscIOMgr->Close(m_dwFileHandle);
	m_dwFileHandle = INVALID_FILE_HANDLE;
	m_pStream->Disconnect();
	_endthreadex(0);
	return 0;


_download_thread_retry:
	if( INVALID_FILE_HANDLE != m_dwFileHandle )
	{
		m_pDiscIOMgr->Close(m_dwFileHandle);
		m_dwFileHandle = INVALID_FILE_HANDLE;
	}
	m_pStream->Disconnect();

	// 是否 还有其他的地址可以尝试
	if( (INT)vecFileNames.size() > nVecIndex + 1)
	{
		nVecIndex++;
		m_strSrcFileName = vecFileNames[nVecIndex].c_str();
	}
	else
	{
		// 从第一个地址重新开始轮流尝试
		nVecIndex = 0;
		m_strSrcFileName = vecFileNames[nVecIndex].c_str();
	}
	goto _download_thread_start;


_download_thread_exit:
	if( INVALID_FILE_HANDLE != m_dwFileHandle )
	{
		m_pDiscIOMgr->Close(m_dwFileHandle);
		m_dwFileHandle = INVALID_FILE_HANDLE;
	}
	m_pStream->Disconnect();
	_endthreadex(0);
	return 0;

}





}