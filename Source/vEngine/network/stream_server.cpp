//-----------------------------------------------------------------------------
//!\file stream_server.cpp
//!\author Lyp
//!
//!\date 2007-11-27
//!
//!\brief tcp_socket 服务器端
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "stream_server.h"

// 使用其它部件
#include "..\frame\msg_queue_ts.h"
#include "..\memory\safe_mem_pool.h"

namespace vEngine {
//-----------------------------------------------------------------------------
//! construction
//-----------------------------------------------------------------------------
StreamServer::StreamServer():m_Trunk(this)
{
	// 登陆回调函数
	m_fnLogIn = NULL;
	m_fnLogOut = NULL;
	InitializeCriticalSection(&m_DisconnectLock);
	m_pMemPool = new SafeMemPool(4*1024*1024);	// 默认4MB缓冲
}


//-----------------------------------------------------------------------------
//! destruction
//-----------------------------------------------------------------------------
StreamServer::~StreamServer()
{
	SAFE_DEL(m_pMemPool);
	DeleteCriticalSection(&m_DisconnectLock);
}


//-----------------------------------------------------------------------------
// 初始化完成端口网络会话层
//-----------------------------------------------------------------------------
BOOL StreamServer::Init(TCPLOGINCALLBACK fnLogIn, TCPLOGOUTCALLBACK fnLogOut, 
						INT nPort, BOOL bReUsePort, BOOL bPackageMode)
{
	m_fnLogIn = fnLogIn; 
	m_fnLogOut = fnLogOut;
	m_bPackageMode = bPackageMode;	// 是否使用包模式

	m_mapClient.Clear();
	m_listDisClient.Clear();
	InterlockedExchange((LONG*)&m_bTerminateAccept, FALSE);
	InterlockedExchange((LONG*)&m_bTerminateDisconnect, FALSE);
	m_dwMsgSend = 0; // 统计数据
	m_dwMsgRecv = 0;
	m_dwBytesSend = 0;
	m_dwBytesRecv = 0;

	// 设定地址
	m_nPort = nPort;
	m_Address.sin_addr.s_addr = htonl(INADDR_ANY);
	m_Address.sin_family = AF_INET;
	m_Address.sin_port = htons(m_nPort);
	m_Socket = socket(AF_INET, SOCK_STREAM, 0);	// 创建 socket

	DWORD dwCmd = 1;
	INT nResult = ioctlsocket( m_Socket, FIONBIO, &dwCmd );	// 使用非阻塞方式
	if( SOCKET_ERROR == nResult )
	{
		ERR(_T("set nonblocking mode failed in init server"));
		closesocket( m_Socket );
		return FALSE;
	}
	
	linger lin;
	lin.l_onoff = 1;
	lin.l_linger = 0;
	setsockopt(m_Socket, SOL_SOCKET, SO_LINGER, (char*)&lin, sizeof(lin));	// 软关闭

	// reuse造成多次绑定成功，可能不能找到正确可用的端口
	// 如果两台服务器开在同一台机器会使用同一个端口，造成混乱
	// set addr reusabel
	if( bReUsePort )
	{
		DWORD dwReUseAddr = TRUE;	
		setsockopt( m_Socket, SOL_SOCKET, SO_REUSEADDR, 
			(char*)&dwReUseAddr, sizeof(dwReUseAddr) );
	}

	INT nAddresslen = sizeof(m_Address);
	if( bind(m_Socket, (sockaddr*)&m_Address, nAddresslen) )	// 绑定
	{
		ERR(_T("bind error"));
		closesocket( m_Socket );
		return FALSE;
	}
	else
	{
		listen( m_Socket, 5 );	// 开始侦听

		// 启动线程 Accept/Disconnect
		m_hThreadAccept = (HANDLE)_beginthreadex
			(NULL, 0, (THREADPROC)m_Trunk.sfp1(&vEngine::StreamServer::ThreadAccept), NULL, 0, NULL);
		m_hThreadDisconnect = (HANDLE)_beginthreadex
			(NULL, 0, (THREADPROC)m_Trunk.sfp1(&vEngine::StreamServer::ThreadDisconnect), NULL, 0, NULL);
	}

	return TRUE;
}


//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
BOOL StreamServer::Destroy()
{
	InterlockedExchange((LONG*)&m_bTerminateAccept, TRUE);
	WaitForSingleObject(m_hThreadAccept, INFINITE);
	SAFE_CLOSE_HANDLE(m_hThreadAccept);

	std::list<tagClient*> list;
	m_mapClient.ExportAllValue(list);
	std::list<tagClient*>::iterator it;
	for(it=list.begin(); it!=list.end(); ++it)
		this->DisconnetClient((*it));

	InterlockedExchange((LONG*)&m_bTerminateDisconnect, TRUE);
	WaitForSingleObject(m_hThreadDisconnect, INFINITE);
	SAFE_CLOSE_HANDLE(m_hThreadDisconnect);

	closesocket( m_Socket );
	m_fnLogIn = NULL;
	m_fnLogOut = NULL;

	return TRUE;
}


//-----------------------------------------------------------------------------
// 发送,返回发送缓冲目前大小
//-----------------------------------------------------------------------------
INT StreamServer::Send(DWORD dwClientID, LPVOID pMsg, DWORD dwSize)
{
	tagClient* pClient = m_mapClient.Peek(dwClientID);
	if( !P_VALID(pClient) )
		return GT_INVALID;

	pClient->pSendMsgQueue->AddMsg(pMsg, dwSize);
	return pClient->pSendMsgQueue->GetMsgNum();
}


//-----------------------------------------------------------------------------
// 从网络收unit,处理完后应该调用FreeRecved交还
//-----------------------------------------------------------------------------
LPBYTE StreamServer::Recv(DWORD dwClientID, DWORD& dwSize, INT& nUnRecved)
{
	tagClient* pClient = m_mapClient.Peek(dwClientID);
	if( !P_VALID(pClient) )
		return NULL;

	nUnRecved = pClient->pRecvMsgQueue->GetMsgNum();
	return pClient->pRecvMsgQueue->GetMsg(dwSize);
}


//-----------------------------------------------------------------------------
// 因为内存是内部分配的，所以收包后应该交还内部来释放
//-----------------------------------------------------------------------------
VOID StreamServer::FreeRecved(DWORD dwClientID, LPBYTE pMsg) 
{
	tagClient* pClient = m_mapClient.Peek(dwClientID);
	if( !P_VALID(pClient) )
		return;

	pClient->pRecvMsgQueue->FreeMsg(pMsg);
}



//-----------------------------------------------------------------------------
// 踢出指定ID的玩家
//-----------------------------------------------------------------------------
BOOL StreamServer::Kick(DWORD dwClientID)
{
	tagClient* pClient = m_mapClient.Peek(dwClientID);
	if( !P_VALID(pClient) )
		return FALSE;

	this->DisconnetClient(pClient);
	return TRUE;
}


//-----------------------------------------------------------------------------
// 得到指定ID客户端的sock name
//-----------------------------------------------------------------------------
BOOL StreamServer::GetClientSockName(sockaddr_in* pAddr, DWORD dwClientID)
{
	tagClient* pClient = m_mapClient.Peek(dwClientID);
	if( !P_VALID(pClient) )
		return FALSE;

	INT nLen = (INT)sizeof(sockaddr_in);
	if( 0 == getpeername(pClient->Socket, (sockaddr*)pAddr, &nLen) )
		return TRUE;
	else
		return FALSE;
}


//-----------------------------------------------------------------------------
// thread accept and receive
//-----------------------------------------------------------------------------
UINT StreamServer::ThreadAccept(LPVOID)
{
	fd_set fdread;
	timeval block_time;
	block_time.tv_sec = 0;
	block_time.tv_usec = 1000*200;	// 200 ms
	INT nAddlen = sizeof(m_Address);
	
	while( !m_bTerminateAccept )
	{
		FD_ZERO(&fdread);	// "select" will change fdread, so this must put in the loop
		FD_SET(m_Socket, &fdread);
		if( select(0, &fdread, NULL, NULL, &block_time) == 1 )	// 阻塞侦听
		{
			tagClient* pClient = new tagClient;
			if( NULL == pClient )
				continue;

			pClient->Socket	= accept( m_Socket, (sockaddr*)&(m_Address), &nAddlen);
			if( pClient->Socket == INVALID_SOCKET )
			{	
				delete(pClient);
				continue;
			}

			// 申请发送接受缓冲
			pClient->pSendMsgQueue = new MsgQueueTS(TRUE, m_bPackageMode);
			pClient->pRecvMsgQueue = new MsgQueueTS(FALSE, FALSE);
			if( NULL == pClient->pSendMsgQueue || NULL == pClient->pRecvMsgQueue )
			{
				SAFE_DEL(pClient->pSendMsgQueue);
				SAFE_DEL(pClient->pRecvMsgQueue);
				closesocket(pClient->Socket);
				delete(pClient);
				continue;
			}

			pClient->dwConnectTime = timeGetTime();	// 记录登陆时间

			if( m_bPackageMode )
			{
				pClient->hThreadRecv = (HANDLE)_beginthreadex
					(NULL, 0, (THREADPROC)m_Trunk.sfp1(&vEngine::StreamServer::ThreadRecv), pClient, 0, NULL);
			}
			else
			{
				// 通知上层登陆
				DWORD dwClientID = (*m_fnLogIn)(0, 0);
				if( GT_INVALID == dwClientID )
				{
					SAFE_DEL(pClient->pSendMsgQueue);
					SAFE_DEL(pClient->pRecvMsgQueue);
					closesocket(pClient->Socket);
					delete(pClient);
					continue;
				}

				pClient->dwID = dwClientID;
				m_mapClient.Add(dwClientID, pClient);

				pClient->hThreadRecv = (HANDLE)_beginthreadex
					(NULL, 0, (THREADPROC)m_Trunk.sfp1(&vEngine::StreamServer::ThreadStreamRecv), pClient, 0, NULL);
			}

			pClient->hThreadSend = (HANDLE)_beginthreadex
				(NULL, 0, (THREADPROC)m_Trunk.sfp1(&vEngine::StreamServer::ThreadSend), pClient, 0, NULL);
		}
	}
	
	_endthreadex(0);
	return 0;
}


//-----------------------------------------------------------------------------
// thread disconnect
//-----------------------------------------------------------------------------
UINT StreamServer::ThreadDisconnect(LPVOID)
{
	while( !m_bTerminateDisconnect || m_listDisClient.Size() > 0 )
	{
		tagClient* pClient = m_listDisClient.PopFront();
		if( !P_VALID(pClient) )
		{
			Sleep(200);
			continue;
		}

		WaitForSingleObject(pClient->hThreadRecv, INFINITE);
		SAFE_CLOSE_HANDLE(pClient->hThreadRecv);

		WaitForSingleObject(pClient->hThreadSend, INFINITE);
		SAFE_CLOSE_HANDLE(pClient->hThreadSend);

		SAFE_DEL(pClient->pRecvMsgQueue);
		SAFE_DEL(pClient->pSendMsgQueue);

		closesocket(pClient->Socket);
		delete(pClient);
	}

	_endthreadex(0);
	return 0;
}


//-----------------------------------------------------------------------------
// thread send
//-----------------------------------------------------------------------------
UINT StreamServer::ThreadSend(tagClient* pClient)
{
	fd_set fdwrite;
	timeval block_time;	// 设置发送阻塞时间
	block_time.tv_sec = 0;
	block_time.tv_usec = 1000*200;	// 200 ms

	LPBYTE	pMsg = NULL, pTempPtr = NULL;
	DWORD	dwMsgSize = 0;
	INT		nReturn = 0;

	while( FALSE == pClient->bTerminate )
	{
		for(;;)	// 注意内部的break
		{
			pTempPtr = pMsg = pClient->pSendMsgQueue->GetMsg(dwMsgSize);
			if( NULL == pMsg )
				break;	// 发完为止

			// send 可能不能一次将数据发完
			while( dwMsgSize > 0 && !pClient->bTerminate )
			{
				nReturn = send(pClient->Socket, (char*)pTempPtr, dwMsgSize, 0);
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
						FD_SET(pClient->Socket, &fdwrite);
						select(0, NULL, &fdwrite, NULL, &block_time);
						break;

					default:
						pClient->pSendMsgQueue->FreeMsg(pMsg);
						goto __thread_send_end;
						break;
					}
				}
				else
				{
					pTempPtr += nReturn;
					dwMsgSize -= nReturn;
					m_dwBytesSend += nReturn;
				}
			}

			pClient->pSendMsgQueue->FreeMsg(pMsg);
			pTempPtr = pMsg = NULL;
			m_dwMsgSend++;
		}

		::WaitForSingleObject(pClient->pSendMsgQueue->GetEvent(), 200);
	}

__thread_send_end:
	this->DisconnetClient(pClient);
	_endthreadex(0);
	return 0;
}



//-----------------------------------------------------------------------------
// receive thread function
//-----------------------------------------------------------------------------
UINT StreamServer::ThreadRecv(tagClient* pClient)
{
	fd_set fdread;
	timeval block_time;	// 设置阻塞时间
	block_time.tv_sec = 0;
	block_time.tv_usec = 1000*200;	// 200 ms

	INT nResult = 0;
	DWORD dwMsgSize = 0;
	DWORD dwTempSize = 0;
	CHAR* pTempPtr = NULL;

	// 循环获得数据
	while( FALSE == pClient->bTerminate )
	{
		// 先接受DWORD型的信息长度信息
		dwTempSize = sizeof(DWORD);
		pTempPtr = (char*)&dwMsgSize;

		while( dwTempSize > 0 )
		{
			// 先取得数据的大小
			FD_ZERO(&fdread);
			FD_SET(pClient->Socket, &fdread);
			if( 1 == select(0, &fdread, NULL, NULL, &block_time) )
			{
				nResult = recv(pClient->Socket, pTempPtr, dwTempSize, 0);
				if( 0 == nResult || SOCKET_ERROR == nResult )
					goto __thread_recv_end;

				dwTempSize -= nResult;
				pTempPtr += nResult;
				m_dwBytesRecv += nResult;
			}

			if( pClient->bTerminate )
				goto __thread_recv_end;

			if( 0 == pClient->nRecvPackageNum && timeGetTime() - pClient->dwConnectTime > 1000 * 60 * 3 )
				goto __thread_recv_end;	// 长时间(3分钟)没有发送第一个验证包
		}

		if( SOCKET_ERROR != nResult )
		{
			char* pTemp = (char*)m_pMemPool->Alloc(dwMsgSize);
			dwTempSize = dwMsgSize;
			pTempPtr = pTemp;
			while( dwTempSize > 0 )
			{
				FD_ZERO(&fdread);
				FD_SET(pClient->Socket, &fdread);
				if( 1 == select(0, &fdread, NULL, NULL, &block_time) )
				{
					nResult = recv(pClient->Socket, pTempPtr, dwTempSize, 0);
					if( 0 == nResult || nResult == SOCKET_ERROR )	
					{
						m_pMemPool->Free(pTemp);
						goto __thread_recv_end;
					}

					dwTempSize -= nResult;
					pTempPtr += nResult;
					m_dwBytesRecv += nResult;
				}

				if( 0 == pClient->nRecvPackageNum && timeGetTime() - pClient->dwConnectTime > 1000 * 60 * 3 )
					goto __thread_recv_end; // 长时间(3分钟)没有发送第一个验证包

				if( pClient->bTerminate )
					goto __thread_recv_end;
			}

			//初始化信息统计
			pClient->pRecvMsgQueue->AddMsg(pTemp, dwMsgSize);
			pClient->nRecvPackageNum++;
			if( 1 == pClient->nRecvPackageNum )
			{
				// 第一个包，通知上层登陆
				DWORD dwClientID = (*m_fnLogIn)((LPBYTE)pTemp, dwMsgSize);
				if( GT_INVALID == dwClientID )
				{
					m_pMemPool->Free(pTemp);
					goto __thread_recv_end;
				}

				pClient->dwID = dwClientID;
				m_mapClient.Add(dwClientID, pClient);
			}

			m_dwMsgRecv++;
			m_pMemPool->Free(pTemp);
		}
	}



__thread_recv_end:
	this->DisconnetClient(pClient);
	_endthreadex(0);
	return 0;
}


//-----------------------------------------------------------------------------
// stream receive thread function
//-----------------------------------------------------------------------------
UINT StreamServer::ThreadStreamRecv(tagClient* pClient)
{
	fd_set fdread;
	timeval block_time;	// 设置阻塞时间
	block_time.tv_sec = 0;
	block_time.tv_usec = 1000*200;	// 200 ms

	CONST INT RECV_BUFF_SIZE = 2048;	// 2KB recv buff
	INT nResult = 0;
	LPBYTE pBuffer = (LPBYTE)m_pMemPool->Alloc(RECV_BUFF_SIZE);

	// 循环获得数据
	while( FALSE == pClient->bTerminate )
	{
		FD_ZERO(&fdread);
		FD_SET(pClient->Socket, &fdread);
		if( 1 == select(0, &fdread, NULL, NULL, &block_time) )
		{
			nResult = recv(pClient->Socket, (CHAR*)pBuffer, RECV_BUFF_SIZE, 0);
			if( 0 == nResult || nResult == SOCKET_ERROR )	
			{
				goto __thread_stream_recv_end;
			}

			m_dwBytesRecv += nResult;
			pClient->pRecvMsgQueue->AddMsg(pBuffer, nResult);
			++pClient->nRecvPackageNum;
			m_dwMsgRecv++;
		}

		if( pClient->bTerminate )
		{
			goto __thread_stream_recv_end;
		}
	}

__thread_stream_recv_end:
	m_pMemPool->Free(pBuffer);
	this->DisconnetClient(pClient);
	_endthreadex(0);
	return 0;
}


//-----------------------------------------------------------------------------
// disconnet
//-----------------------------------------------------------------------------
VOID StreamServer::DisconnetClient(tagClient* pClient)
{
	// 此处锁定是为了防止外部同时调用此函数导致多次调用登出相关操作
	EnterCriticalSection(&m_DisconnectLock);
	if( pClient->bTerminate )
	{
		LeaveCriticalSection(&m_DisconnectLock);
		return;
	}

	if( m_fnLogOut && GT_VALID(pClient->dwID) )	// 上层未登陆成功，就不用调用上层登出
		(*m_fnLogOut)(pClient->dwID);	// 通知上层登出

	shutdown(pClient->Socket, SD_SEND);
	InterlockedExchange((LONG*)&pClient->bTerminate, TRUE);
	m_mapClient.Erase(pClient->dwID);

	LeaveCriticalSection(&m_DisconnectLock);

	// 交给线程释放
	m_listDisClient.PushBack(pClient);
}

} // namespace wEngine {