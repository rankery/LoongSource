//-----------------------------------------------------------------------------
//!\file x_client.cpp
//!\author Lyp
//!
//!\date 2003-03-17
//! last 2008-04-17
//!
//!\brief 广域网客户端通讯底层
/*
  in http://tangentsoft.net/wskfaq/newbie.html#howclose 

	The proper sequence for closing a TCP connection is:
	1.Finish sending data. 
	2.Call shutdown() with the how parameter set to 1. 
	3.Loop on recv() until it returns 0. 
	4.Call closesocket(). 

  in MSDN:
	To assure that all data is sent and received on a connected socket 
	before it is closed, an application should use shutdown to close connection 
	before calling closesocket. For example, to initiate a graceful disconnect:
	1.Call WSAAsyncSelect to register for FD_CLOSE notification. 
	2.Call shutdown with how=SD_SEND. 
	3.When FD_CLOSE received, call recv until zero returned, or SOCKET_ERROR. 
	4.Call closesocket. 
*/
//-----------------------------------------------------------------------------
#include "..\stdafx.h"

#include "x_client.h"
#include "..\console\console_gui.h"

// 使用其它部件
#include "..\frame\msg_queue_ts.h"
#include "..\filter\FilterMgr.h"
#include "..\memory\mem_pool.h"
#include "..\util\util.h"

#define ONE_SECOND			1000
#define TEMP_BUFF			512*1024


namespace vEngine {
//-----------------------------------------------------------------------------
// construction / destruction
//-----------------------------------------------------------------------------
XClient::XClient():m_Trunk(this)
{
	Clear();
	m_pMemPool = new MemPool(TEMP_BUFF);
	m_pFilter->InitFilter(FMF_MiniLzo);
	m_pUtil = "Util";

}

XClient::~XClient()
{
	this->Destroy();
	SAFE_DEL(m_pMemPool);
}


//-----------------------------------------------------------------------------
// clear
//-----------------------------------------------------------------------------
VOID XClient::Clear()
{
	m_pSendMsgQueue = NULL;
	m_pRecvMsgQueue = NULL;

	m_strIP.clear();
	m_nPort = 0;
	m_Sock = 0;
	m_bNagle = TRUE;

	m_hConnect = 0;
	m_hSend = 0;
	m_hReceive = 0;

	::InterlockedExchange((LONG*)&m_bTerminateConnect, FALSE);
	::InterlockedExchange((LONG*)&m_bTerminateSend, FALSE);
	::InterlockedExchange((LONG*)&m_bTerminateReceive, FALSE);
	::InterlockedExchange((LONG*)&m_bConnected, FALSE);

	ZeroMemory(m_szHostName, sizeof(m_szHostName));
	m_Trunk.sfp_clear();
}



//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL XClient::Init(BOOL bNagle)
{
	this->Clear();
	m_bNagle = bNagle;
	
	// 申请发送接受缓冲
	m_pSendMsgQueue = new MsgQueueTS(TRUE, TRUE);
	m_pRecvMsgQueue = new MsgQueueTS(FALSE, FALSE);
	if( NULL == m_pSendMsgQueue || NULL == m_pRecvMsgQueue )
	{
		SAFE_DEL(m_pSendMsgQueue);
		SAFE_DEL(m_pRecvMsgQueue);
		return FALSE;
	}

	//创建socket
	m_Sock = socket(AF_INET, SOCK_STREAM, 0);
	if( INVALID_SOCKET == m_Sock )
	{
		IMSG(_T("could not create socket\r\n"));
		SAFE_DEL(m_pSendMsgQueue);
		SAFE_DEL(m_pRecvMsgQueue);
		return FALSE;
	}

	// 使用非阻塞方式,linger行为
	DWORD dwCmd = 1;
	INT nResult = ioctlsocket( m_Sock, FIONBIO, &dwCmd );
	if( SOCKET_ERROR == nResult )
	{
		IMSG(_T("set nonblocking mode failed in init client\r\n"));
		closesocket( m_Sock );
		m_Sock = 0;
		SAFE_DEL(m_pSendMsgQueue);
		SAFE_DEL(m_pRecvMsgQueue);
		return FALSE;
	}

	// disables the Nagle algorithm for send coalescing
	if( !m_bNagle )
	{
		BOOL bNoDelay = TRUE;
		setsockopt(m_Sock, IPPROTO_TCP, TCP_NODELAY, (char*)&bNoDelay, sizeof(bNoDelay));
	}

	// set addr reusabel
	DWORD dwReUseAddr = TRUE;
	setsockopt( m_Sock, SOL_SOCKET, SO_REUSEADDR, 
		(char*)&dwReUseAddr, sizeof(dwReUseAddr) );

	// hard shut down
	linger lin;
	lin.l_linger = 0;
	lin.l_onoff = 1;
	setsockopt( m_Sock, SOL_SOCKET, SO_LINGER, (char*)&lin, sizeof(lin) );

	// 每秒流量统计
	m_dwLastStatOutTime = timeGetTime();
	m_dwLastStatInTime = timeGetTime();
	m_dwByteOut = 0;
	m_dwByteIn = 0;
	m_dwByteOutCounter = 0;
	m_dwByteInCounter = 0;

	m_dwSendPackNum = 0;
	m_dwRecvPackNum = 0;

	return TRUE;
}


//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
void XClient::Destroy()
{
	// 设置结束线程标志
	::InterlockedExchange((LONG*)&m_bTerminateConnect, TRUE);
	::InterlockedExchange((LONG*)&m_bTerminateSend, TRUE);
	::InterlockedExchange((LONG*)&m_bTerminateReceive, TRUE);

	// 等待线程结束,一直等下去
	GracefulCloseThread(m_hConnect, INFINITE);
	GracefulCloseThread(m_hSend, INFINITE);
	GracefulCloseThread(m_hReceive, INFINITE);

	// 完美关闭socket
	if( TRUE == m_bConnected )
	{
		GracefulCloseSocket(m_Sock, 20);	// 重试 20 次
	}
	else
	{
		// m_bConnected尚未为真之前极短时间内可能已经连上
		// 所以先shutdown一下，并无副作用
		shutdown(m_Sock, SD_SEND);
		closesocket(m_Sock);
		m_Sock = 0;
	}

	// 关闭缓冲
	::InterlockedExchange((LONG*)&m_bConnected, FALSE);
	SAFE_DEL( m_pSendMsgQueue );
	SAFE_DEL( m_pRecvMsgQueue );
}



//-----------------------------------------------------------------------------
// 完美关闭socket
//-----------------------------------------------------------------------------
BOOL XClient::GracefulCloseSocket(SOCKET& socket, INT nRetryTimes)
{
	if( 0 == socket )
		return TRUE;

	shutdown(socket, SD_SEND);
	
	INT nRetry = 0;
	fd_set fdread;
	timeval block_time;	// 设置阻塞时间
	block_time.tv_sec = 0;
	block_time.tv_usec = GT_NET_BLOCK_TIME;	// 150 ms 
	
	while( nRetry++ < nRetryTimes )
	{
		FD_ZERO(&fdread);
		FD_SET(socket, &fdread);
		if( 1 == select(0, &fdread, NULL, NULL, &block_time) )
		{
			CHAR chTemp;
			INT nResult = recv(socket, &chTemp, 1, 0);
			if( 0 == nResult )	// receive FIN, ok
			{
				closesocket( socket );
				socket = 0;
				return TRUE;
			}
			
			if( SOCKET_ERROR == nResult )	// network down
				break;
		}
	}
	

	closesocket( socket );
	// a hard shutdown
	socket = 0;
	return FALSE;
}


//-----------------------------------------------------------------------------
// 完美结束thread
//-----------------------------------------------------------------------------
BOOL XClient::GracefulCloseThread(HANDLE& thread, INT nRetryTime)
{
	if( 0 == thread )
		return TRUE;

	DWORD dwResult = WaitForSingleObject(thread, nRetryTime);
	SAFE_CLOSE_HANDLE(thread);
	thread = 0;

	if( WAIT_TIMEOUT == dwResult )
		return FALSE;
	else
		return TRUE;
}




//-----------------------------------------------------------------------------
// 发送网络信息
//-----------------------------------------------------------------------------
BOOL XClient::Send(LPVOID pMsg, DWORD dwMsgSize)
{
	ASSERT(NULL != pMsg);
	ASSERT(dwMsgSize > 0 && dwMsgSize < 0xFFFFFF);	// 数据过分长
	// 将发送信息添加到发送信息缓冲
	if( FALSE == m_bConnected )
		return FALSE;

	//初始化信息统计
	++m_dwSendPackNum;
	return m_pSendMsgQueue->AddMsg((LPBYTE)pMsg, dwMsgSize);
}



//-----------------------------------------------------------------------------
//! 接受网络信息,返回FALSE说明没有消息
//! 外部需要负责释放返回的内存指针
//-----------------------------------------------------------------------------
LPBYTE XClient::Recv(DWORD& dwMsgSize)
{
	if( !P_VALID(m_pRecvMsgQueue) )
		return NULL;

	// 从接受缓冲接收消息
	return m_pRecvMsgQueue->GetMsg(dwMsgSize);
}


//-----------------------------------------------------------------------------
// 对应于Recv(),因为内存是内部分配的，所以应该交还内部来释放
//-----------------------------------------------------------------------------
VOID XClient::FreeRecved(LPBYTE pMsg) 
{
	if( m_pRecvMsgQueue )
		m_pRecvMsgQueue->FreeMsg(pMsg);
}


//-----------------------------------------------------------------------------
// 尝试连接指定地址
//-----------------------------------------------------------------------------
VOID XClient::TryToConnect(LPCSTR szIP, INT nPort)
{
	this->Disconnect();
	
	m_strIP = szIP;
	m_nPort = nPort;

	// 启动线程 connect
	m_hConnect = (HANDLE)_beginthreadex(NULL, 0, (THREADPROC)m_Trunk.sfp1
		(&vEngine::XClient::ThreadConnect), NULL, 0, NULL);
}


//-----------------------------------------------------------------------------
//! 返回是否正在尝试连接
//! 假如所有设置正确，但是远程主机并没有启动服务器，那么就会一直尝试连接
//! 但是如果网络发生错误，就有可能停止尝试连接
//-----------------------------------------------------------------------------
BOOL XClient::IsTryingToConnect()
{
	if( !m_hConnect )
		return FALSE;

	DWORD dwConnectExitCode = STILL_ACTIVE;
	GetExitCodeThread(m_hConnect, &dwConnectExitCode);
	if( STILL_ACTIVE == dwConnectExitCode )
		return TRUE;
	else
		return FALSE;
}


//-----------------------------------------------------------------------------
// 放弃尝试连接
//-----------------------------------------------------------------------------
VOID XClient::StopTryingConnect()
{
	// 如果我们已经连接上了，那么也会断开
	this->Destroy();
	this->Init(m_bNagle);
}

//-----------------------------------------------------------------------------
// 正常断开连接
//-----------------------------------------------------------------------------
VOID XClient::Disconnect()
{
	this->StopTryingConnect();
}


//-----------------------------------------------------------------------------
// 得到未发送包的数量，可以以此来判断网络负荷
//-----------------------------------------------------------------------------
INT	XClient::GetUnsendPackageNum()
{
	if( !m_bConnected )
		return 0;

	return m_pSendMsgQueue->GetMsgNum();
}


//-----------------------------------------------------------------------------
// 得到已接受包的数量，可以以此来判断网络负荷
//-----------------------------------------------------------------------------
INT	XClient::GetReceivedPackageNum()
{
	if( !m_bConnected )
		return 0;

	return m_pRecvMsgQueue->GetMsgNum();
}


//-----------------------------------------------------------------------------
// thread connect
//-----------------------------------------------------------------------------
UINT XClient::ThreadConnect(LPVOID)
{
	INT nResult = 0;
	sockaddr_in address;
	ZeroMemory(&address, sizeof(address));
	address.sin_addr.s_addr = inet_addr(m_strIP.c_str());
	address.sin_family = AF_INET;
	address.sin_port = htons(m_nPort);

	// 检查IP是否合法
	if( INADDR_NONE == address.sin_addr.s_addr )
	{
		IMSG(_T("[c] invalid IP.\r\n"));
		goto __connected_terminated;
	}
	
	fd_set fdwrite;
	timeval block_time;	// 设置阻塞时间
	block_time.tv_sec = 0;
	block_time.tv_usec = GT_NET_BLOCK_TIME;
	
	// 连接到服务器
	// For connection-oriented, nonblocking sockets, it is often not possible 
	// to complete the connection immediately. In such a case, "connect"
	// returns the error WSAEWOULDBLOCK. However, the operation proceeds.
	nResult = connect(m_Sock, (sockaddr*)&(address), sizeof(address));
	if( SOCKET_ERROR == nResult )
	{	
		if( WSAEWOULDBLOCK == WSAGetLastError() )
		{
			for(INT n=0; n<64; ++n) // 不停重试,外部可以通过调用StopTryingConnect停止重试
			{
				FD_ZERO(&fdwrite);
				FD_SET(m_Sock, &fdwrite);

				if( 1 == select(0, NULL, &fdwrite, NULL, &block_time) )
					goto __connected;

				if( m_bTerminateConnect )
					goto __connected_terminated;
			}
		}

		// 连接尝试失败
		goto __connected_terminated;
	}
	
__connected:

	//初始化信息统计
	m_dwSendPackNum = 0;
	m_dwRecvPackNum = 0; 

	// 启动线程 send
	m_hSend = (HANDLE)_beginthreadex(NULL, 0, (THREADPROC)m_Trunk.sfp1
		(&vEngine::XClient::ThreadSend), NULL, 0, NULL);

	// 启动线程 receive
	m_hReceive = (HANDLE)_beginthreadex(NULL, 0, (THREADPROC)m_Trunk.sfp1
		(&vEngine::XClient::ThreadReceive), NULL, 0, NULL);

	// 连接成功
	::InterlockedExchange((LONG*)&m_bConnected, TRUE);
	_endthreadex(0);
	return 0;

__connected_terminated:
		
	// 连接失败
	::InterlockedExchange((LONG*)&m_bConnected, FALSE);
	_endthreadex(0);
	return 0;
}



//-----------------------------------------------------------------------------
// thread send
//-----------------------------------------------------------------------------
UINT XClient::ThreadSend(LPVOID)
{
	fd_set fdwrite;
	timeval block_time;	// 设置发送阻塞时间
	block_time.tv_sec = 0;
	block_time.tv_usec = GT_NET_BLOCK_TIME;	// 150ms

	LPBYTE	pMsg = NULL, pTempPtr = NULL;
	DWORD	dwMsgSize = 0;
	INT		nReturn = 0;

	while( FALSE == m_bTerminateSend )
	{
		for(;;)	// 注意内部的break
		{
			// 流量统计
			DWORD dwTime = timeGetTime();
			if( dwTime - m_dwLastStatOutTime >= ONE_SECOND ) 
			{
				m_dwLastStatOutTime = dwTime;
				m_dwByteOut = m_dwByteOutCounter;
				m_dwByteOutCounter = 0;
			}

			pTempPtr = pMsg = m_pSendMsgQueue->GetMsg(dwMsgSize);
			if( NULL == pMsg )
				break;	// 发完为止

			//	进行协议扰码处理
			if(m_pSendMsgQueue->IsAddSizeAhead())
			{
				DWORD fix_size = (*(DWORD*)pTempPtr);
				XCS_SET_SIZE(fix_size);
				(*(DWORD*)pTempPtr) = fix_size;
			}

			// send 可能不能一次将数据发完
			while( dwMsgSize > 0 && FALSE == m_bTerminateSend )
			{
				nReturn = send(m_Sock, (char*)pTempPtr, dwMsgSize, 0);
				if( SOCKET_ERROR == nReturn )
				{
					switch(WSAGetLastError())
					{
					case WSAEWOULDBLOCK:
					case WSAENOBUFS:
						// no buffer space is available within the transport 
						// system to hold the data to be transmitted
						// 等待数据可以发送的时机
						FD_ZERO(&fdwrite);
						FD_SET(m_Sock, &fdwrite);
						select(0, NULL, &fdwrite, NULL, &block_time);
						break;

					default:	// 故障
						shutdown(m_Sock, SD_SEND);
						m_pSendMsgQueue->FreeMsg(pMsg);
						goto __thread_send_end;
					}
				}
				else
				{
					pTempPtr += nReturn;
					dwMsgSize -= nReturn;
					m_dwByteOutCounter += nReturn;	// 流量统计
				}
			}

			m_pSendMsgQueue->FreeMsg(pMsg);
			pTempPtr = pMsg = NULL;
		}

		::WaitForSingleObject(m_pSendMsgQueue->GetEvent(), 120);
	}


__thread_send_end:
	::InterlockedExchange((LONG*)&m_bConnected, FALSE);
	_endthreadex(0);
	return 0;
}



//-----------------------------------------------------------------------------
// receive thread function
//-----------------------------------------------------------------------------
UINT XClient::ThreadReceive(LPVOID)
{
	fd_set fdread;
	timeval block_time;	// 设置阻塞时间
	block_time.tv_sec = 0;
	block_time.tv_usec = GT_NET_BLOCK_TIME;

	INT nResult = 0;
	DWORD dwMsgSize = 0;
	DWORD dwTempSize = 0;
	CHAR* pTempPtr = NULL;

	// 循环获得数据
	while( FALSE == m_bTerminateReceive )
	{
		// 先接受DWORD型的信息长度信息
		dwTempSize = sizeof(DWORD);
		pTempPtr = (char*)&dwMsgSize;

		// 流量统计
		DWORD dwTime = timeGetTime();
		if( dwTime - m_dwLastStatInTime >= ONE_SECOND )
		{
			m_dwLastStatInTime = dwTime;
			m_dwByteIn = m_dwByteInCounter;
			m_dwByteInCounter = 0;
		}

		while( dwTempSize > 0 )
		{
			// 先取得数据的大小
			FD_ZERO(&fdread);
			FD_SET(m_Sock, &fdread);
			if( 1 == select(0, &fdread, NULL, NULL, &block_time) )
			{
				nResult = recv(m_Sock, pTempPtr, dwTempSize, 0);
				if( 0 == nResult )	// FIN
					goto __thread_receive_end;

				if( SOCKET_ERROR == nResult )
				{
					// 网络故障
					shutdown(m_Sock, SD_SEND);
					goto __thread_receive_end;
				}
				else
				{
					dwTempSize -= nResult;
					pTempPtr += nResult;

					// 流量统计
					m_dwByteInCounter += nResult;
				}
			}
			
			if( m_bTerminateReceive )
				goto __thread_receive_end;
		}

		// ASSERT(dwMsgSize < 40960);	// 异常的数据
		if( SOCKET_ERROR != nResult )
		{
			CHAR* pBufferIn = (CHAR*)m_pMemPool->Alloc(dwMsgSize+dwMsgSize/64+64);
			dwTempSize = sizeof(DWORD) + dwMsgSize;
			pTempPtr = pBufferIn;
			while( dwTempSize > 0 )
			{
				FD_ZERO(&fdread);
				FD_SET(m_Sock, &fdread);
				if( 1 == select(0, &fdread, NULL, NULL, &block_time) )
				{
					nResult = recv(m_Sock, pTempPtr, dwTempSize, 0);
					if( 0 == nResult )	// FIN
					{
						m_pMemPool->Free(pBufferIn);
						goto __thread_receive_end;
					}

					if( nResult == SOCKET_ERROR )
					{
						// 网络故障
						shutdown(m_Sock, SD_SEND);
						m_pMemPool->Free(pBufferIn);
						goto __thread_receive_end;
					}
					else
					{
						dwTempSize -= nResult;
						pTempPtr += nResult;

						// 流量统计
						m_dwByteInCounter += nResult;
					}
					
					if( m_bTerminateReceive )
					{
						m_pMemPool->Free(pBufferIn);
						goto __thread_receive_end;
					}
				}
			}
			
			//初始化信息统计
			++m_dwRecvPackNum;

			CHAR* pBufferOut = (CHAR*)m_pMemPool->Alloc(*(DWORD*)pBufferIn);

			// 反过滤（解压）文件
			tagFilterParam param;
			param.bCompress = FALSE;
			param.bSuccess = FALSE;
			param.dwInBufferSize = dwMsgSize+dwMsgSize/64+64;
			param.dwInDataSize = dwMsgSize;
			param.dwOutBufferSize = *(DWORD*)pBufferIn;
			param.dwOutDataSize = 0;
			param.eFilter = FMF_MiniLzo;
			param.lpMemIn = pBufferIn + sizeof(DWORD);
			param.lpMemOut = pBufferOut;

			// 进行过滤文件信息
			m_pFilter->Filter(&param);

			m_pRecvMsgQueue->AddMsg(pBufferOut, param.dwOutDataSize);
			m_pMemPool->Free(pBufferIn);
			m_pMemPool->Free(pBufferOut);
		}
	}

__thread_receive_end:
	::InterlockedExchange((LONG*)&m_bConnected, FALSE);
	_endthreadex(0);
	return 0;
}



//-----------------------------------------------------------------------------
// 得到本地主机名
//-----------------------------------------------------------------------------
CHAR* XClient::GetHostName()
{
	gethostname(m_szHostName, sizeof(m_szHostName)-10);
	return m_szHostName;
}


//-----------------------------------------------------------------------------
// 得到指定主机的IP
//-----------------------------------------------------------------------------
BOOL XClient::GetIPByHostName(DWORD& dwIP, LPCSTR lpHostName)
{
	ASSERT(lpHostName);
	hostent* p = gethostbyname(lpHostName);
	if( p )
	{
		INT n=0;
		while((DWORD*)p->h_addr_list[n])
		{
			dwIP = *((DWORD*)p->h_addr_list[n]);
			++n;
		}
		return TRUE;
	}
	else
		return FALSE;
}


//-----------------------------------------------------------------------------
// 得到本机局域网地址
//-----------------------------------------------------------------------------
BOOL XClient::GetLocalLANIP(DWORD& dwIP)
{
    SOCKET sd = WSASocket(AF_INET, SOCK_DGRAM, 0, 0, 0, 0);
    if( sd == SOCKET_ERROR )
	{
        IMSG(_T("failed to get a socket.\r\n"));
		return FALSE;
    }

    INTERFACE_INFO InterfaceList[128];
    DWORD dwBytesReturned;
    if( SOCKET_ERROR == WSAIoctl(sd, SIO_GET_INTERFACE_LIST, 0, 0, 
		&InterfaceList,	sizeof(InterfaceList), &dwBytesReturned, 0, 0) ) 
	{
        IMSG(_T("failed calling WSAIoctl\r\n"));
		closesocket(sd);
		return FALSE;
    }

	closesocket(sd);

	INT nNumInterfaces = dwBytesReturned / sizeof(INTERFACE_INFO);
    for( INT i = 0; i < nNumInterfaces; ++i) 
	{
		u_long nFlags = InterfaceList[i].iiFlags;
		if( !(nFlags & IFF_UP) )	// 必须可用
			continue;

		if( nFlags & IFF_LOOPBACK ) // 不能是 loopback
			continue;

        sockaddr_in *pAddress;
        pAddress = (sockaddr_in *) & (InterfaceList[i].iiAddress);
        dwIP = pAddress->sin_addr.S_un.S_addr;

		if( pAddress->sin_addr.S_un.S_un_b.s_b1 == 10 
			|| (pAddress->sin_addr.S_un.S_un_b.s_b1 == 172 && pAddress->sin_addr.S_un.S_un_b.s_b2 >= 16 && pAddress->sin_addr.S_un.S_un_b.s_b2 <= 31)
			|| (pAddress->sin_addr.S_un.S_un_b.s_b1 == 192 && pAddress->sin_addr.S_un.S_un_b.s_b2 == 168) )
			return TRUE;
    }

	return FALSE;
}


//-----------------------------------------------------------------------------
// 得到本机广域网地址
//-----------------------------------------------------------------------------
BOOL XClient::GetLocalWANIP(DWORD& dwIP)
{
    SOCKET sd = WSASocket(AF_INET, SOCK_DGRAM, 0, 0, 0, 0);
    if( sd == SOCKET_ERROR )
	{
        IMSG(_T("failed to get a socket.\r\n"));
		return FALSE;
    }

    INTERFACE_INFO InterfaceList[128];
    DWORD dwBytesReturned;
    if( SOCKET_ERROR == WSAIoctl(sd, SIO_GET_INTERFACE_LIST, 0, 0, 
		&InterfaceList,	sizeof(InterfaceList), &dwBytesReturned, 0, 0) ) 
	{
        IMSG(_T("failed calling WSAIoctl\r\n"));
		closesocket(sd);
		return FALSE;
    }

	closesocket(sd);

	INT nNumInterfaces = dwBytesReturned / sizeof(INTERFACE_INFO);
    for( INT i = 0; i < nNumInterfaces; ++i) 
	{
		u_long nFlags = InterfaceList[i].iiFlags;
		if( !(nFlags & IFF_UP) )	// 必须可用
			continue;

		if( nFlags & IFF_LOOPBACK ) // 不能是 loopback
			continue;

        sockaddr_in *pAddress;
        pAddress = (sockaddr_in *) & (InterfaceList[i].iiAddress);
        dwIP = pAddress->sin_addr.S_un.S_addr;

   		if( !( pAddress->sin_addr.S_un.S_un_b.s_b1 == 10 
			|| (pAddress->sin_addr.S_un.S_un_b.s_b1 == 172 && pAddress->sin_addr.S_un.S_un_b.s_b2 >= 16 && pAddress->sin_addr.S_un.S_un_b.s_b2 <= 31)
			|| (pAddress->sin_addr.S_un.S_un_b.s_b1 == 192 && pAddress->sin_addr.S_un.S_un_b.s_b2 == 168) ))
			return TRUE;
    }

	return FALSE;
}



//-----------------------------------------------------------------------------
// 将真正IP地址转化为IP地址字符串
//-----------------------------------------------------------------------------
CHAR* XClient::IP2String(DWORD dwIP)
{
	in_addr addr;
	addr.S_un.S_addr = dwIP;
	return inet_ntoa(addr);
}

//-----------------------------------------------------------------------------
// 将IP地址字符串转化为真正IP地址
//-----------------------------------------------------------------------------
DWORD XClient::StringIP2IP(CHAR* szIP)
{
	return inet_addr(szIP);
}


} // namespace vEngine {