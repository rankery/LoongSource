//-----------------------------------------------------------------------------
//!\file transport.cpp
//!\author Lyp
//!
//!\date 2004-04-03
//! last 2008-04-17
//!
//!\brief 服务器端传输底层
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "transport.h"
#include "crypt.h"

#include "..\system\sysinfo.h"
#include "..\debug\mini_dump.h"
namespace vEngine {
//-----------------------------------------------------------------------------
//! construction
//-----------------------------------------------------------------------------
CompletionTransport::CompletionTransport():m_Trunk(this)
{
	m_pAcceptUnitPool = NULL;
	ZeroMemory(m_pUnitPool, sizeof(m_pUnitPool));
	m_pSendQueue = NULL;

	m_pCrypt = new SectCrypt();
	ZeroMemory(m_szInfo, sizeof(m_szInfo));


	// 登陆注销回调函数
	m_fnLogIn = NULL;
	m_fnLogOut = NULL;
	// 加密解密unit回调函数
	m_fnEncrypt = (CRYPTCALLBACK)m_Trunk.sfp1(&vEngine::CompletionTransport::DefaultEncryptFunction);
	m_fnDecrypt = (CRYPTCALLBACK)m_Trunk.sfp1(&vEngine::CompletionTransport::DefaultDecryptFunction);
	// 非法用户举报回调函数
	m_fnHackReport = NULL;

	m_hCompletionPort = 0;

	m_nPort = 0;
	m_bTerminateWorkThread = FALSE;
	m_bPreShutdown = FALSE;	// 关闭socket线程是否结束
	m_lThreadCounter = 0;

	m_nSmallPool = 1;
	m_nBigPool = 1;
	m_bOverBalance = FALSE;

	m_hEventSetupAccepEx
		= ::CreateEvent(NULL, FALSE, TRUE, NULL);	// 自动激活;
}

//-----------------------------------------------------------------------------
//! destruction
//-----------------------------------------------------------------------------
CompletionTransport::~CompletionTransport()
{
	this->Destroy();
	CloseHandle(m_hEventSetupAccepEx);
	delete(m_pCrypt);
}


//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL CompletionTransport::Init(tagServerInitParam* pInitParam)
{
	if( P_VALID(pInitParam) )
		memcpy(&m_InitParam, pInitParam, sizeof(m_InitParam));

	m_nPort = m_InitParam.nPort;

	// 服务器客户端载荷上限
	m_nMaxLoad = m_InitParam.nMaxServerLoad;

	// 建立client data
	m_dwClientSerialID = 0;
	m_ClientData = new tagClientData[m_nMaxLoad];
	if( NULL == m_ClientData )
		return FALSE;
	ZeroMemory(m_ClientData, sizeof(tagClientData)*m_nMaxLoad);

	m_listFreeClientData.Clear();

	
	for(INT n=0; n<m_nMaxLoad; n++)	// 设置 index
		m_ClientData[n].nIndex = n;
	for(INT n=0; n<m_nMaxLoad; n++)	// 全部空闲
		m_listFreeClientData.PushBack(&m_ClientData[n]);

	m_listCloseClient.Clear();
	m_listUnauthClient.Clear();
	m_vecWorkThreadHandle.clear();
	m_mapClientSerialIndex.Clear();	// Client SerialID -> index

    // Open a TCP socket connection to the server.
    m_sockListener = socket(AF_INET, SOCK_STREAM, 0);
    if( m_sockListener < 0 )
    {
		ERR(_T("socket() failed"));
        return FALSE;
    }
	

	// reuse造成多次绑定成功，可能不能找到正确可用的端口
	// 如果两台服务器开在同一台机器会使用同一个端口，造成混乱
	// set addr reusabel
	if( pInitParam->bReusePort )
	{
		DWORD dwReUseAddr = TRUE;	
		setsockopt( m_sockListener, SOL_SOCKET, SO_REUSEADDR, 
			(char*)&dwReUseAddr, sizeof(dwReUseAddr) );
	}

	// disables the Nagle algorithm for send coalescing
	BOOL bNoDelay = TRUE;
	setsockopt(m_sockListener, IPPROTO_TCP, TCP_NODELAY, (char*)&bNoDelay, sizeof(bNoDelay));

    // Bind our local address
	sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port        = htons(m_nPort);

	INT nErr = bind( m_sockListener, (sockaddr*)&address, sizeof(address) );
    while( SOCKET_ERROR == nErr )
	{
		INT nResult = WSAGetLastError();
		if( WSAEADDRINUSE == nResult )
		{
			++m_nPort;
			address.sin_port = htons(m_nPort);
			nErr = bind( m_sockListener, (sockaddr*)&address, sizeof(address) );
		}
		else
		{
			closesocket(m_sockListener);
			ERR(_T("bind() failed"));
			return FALSE;
		}
	}

	// create the completion port
    m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

    if( m_hCompletionPort == NULL )
	{
		closesocket(m_sockListener);
		ERR(_T("CreateIoCompletionPort() failed"));
		return FALSE;
	}

	// 设置此socket为监听socket
    listen( m_sockListener, SOMAXCONN );

	// Associate the socket with the completion port
	// Use default # of threads
	::CreateIoCompletionPort((HANDLE)m_sockListener, m_hCompletionPort, (DWORD)0, 0);
	
	// 检查配置文件允许的最多工作线程数目
	m_vecWorkThreadHandle.clear();

	// 按配置文件指定的数目建立工作线程
	INT nPerCPU = m_InitParam.nPerCpuWorkerThread;
	INT nAddition = m_InitParam.nAdditionalWorkerThread;
	INT nWorkerThreadNum = TObjRef<SysInfo>()->GetProcessorNum() * nPerCPU + nAddition;

	if( nWorkerThreadNum > MAX_UNIT_POOL_NUM - 1 )
		nWorkerThreadNum = MAX_UNIT_POOL_NUM -1;

	m_lThreadCounter = 0;
	CreateWorkThread(nWorkerThreadNum);

	// 建立accept unit pool
	m_pAcceptUnitPool = new UnitPool;
	if( NULL == m_pAcceptUnitPool )
	{
		ERR(_T("create accept unit pool failed"));
		return FALSE;
	}

	INT nAcceptExNum = m_InitParam.nAcceptExNum;
	if( FALSE == m_pAcceptUnitPool->Init(nAcceptExNum*2) )
	{
		ERR(_T("init accept unit pool failed"));
		return FALSE;
	}

	// 创建SetupAccpetEx线程
	if( FALSE == CreateSetupAccepExThread(nAcceptExNum) )
		return FALSE;

	// 创建send线程
	if( FALSE == CreateSendThread() )
		return FALSE;
	
	// 创建close socket线程
	if( FALSE == CreateCloseSocketThread() )
		return FALSE;

	// 创建unauth heartbeat线程
	if( FALSE == CreateUnauthHeartbeatThread() )
		return FALSE;

	// 创建pool balance线程
	if( FALSE == CreatePoolBalanceThread() )
		return FALSE;

#ifdef _DEBUG_UNIT
	mapUnitAddress.Clear(); // FOR DEBUG 
#endif

	return TRUE;
}


//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
BOOL CompletionTransport::Destroy()
{
#ifdef _DEBUG_UNIT
	this->CheckUnit();
#endif

	SAFE_CLOSE_HANDLE(m_hCompletionPort);

	// 等待工作线程结束
	InterlockedExchange((LONG*)&m_bTerminateWorkThread, TRUE);
	for(INT n=(INT)m_vecWorkThreadHandle.size()-1; n>=0; n--)
	{
		if( m_vecWorkThreadHandle[n] )
		{
			WaitForSingleObject(m_vecWorkThreadHandle[n], INFINITE);
			SAFE_CLOSE_HANDLE(m_vecWorkThreadHandle[n]);
		}
	}

	SAFE_DEL(m_pAcceptUnitPool);

    for(INT n=0; n<MAX_UNIT_POOL_NUM; n++)
		SAFE_DEL(m_pUnitPool[n]);

	SAFE_DEL(m_pSendQueue);

	if( m_ClientData )
	{
		for(INT n=0; n<m_nMaxLoad; n++)	// 设置 index
		{
			if( m_ClientData[n].bValid )
			{
				closesocket(m_ClientData[n].sock);

				// 如果在这里出assert说明还有上层未处理的网络消息残留
				// 在recv queue和resend queue
				SAFE_DEL(m_ClientData[n].pRecvQueue);
			}
		}
	}

	SAFE_DEL_ARRAY(m_ClientData);
	return TRUE;
}



//-----------------------------------------------------------------------------
// 注册登陆注销回调函数
//-----------------------------------------------------------------------------
VOID CompletionTransport::SetLogCallback(LOGINCALLBACK fnLogIn, 
										  LOGOUTCALLBACK fnLogOut)
{
	m_fnLogIn = fnLogIn;
	m_fnLogOut = fnLogOut;
}

//-----------------------------------------------------------------------------
// 设置加密解密unit回调函数
//-----------------------------------------------------------------------------
VOID CompletionTransport::SetSecureCallBack(CRYPTCALLBACK fnEncrypt, 
												CRYPTCALLBACK fnDecrypt)
{
	m_fnEncrypt = fnEncrypt;
	m_fnDecrypt = fnDecrypt;
}

//-----------------------------------------------------------------------------
// 设置非法用户举报回调函数
//-----------------------------------------------------------------------------
VOID CompletionTransport::SetHackReportCallBack(HACKREPORTCALLBACK fnHackReport)
{
	m_fnHackReport = fnHackReport;
}


//-----------------------------------------------------------------------------
// setup acceptEx thread
//-----------------------------------------------------------------------------
BOOL CompletionTransport::CreateSetupAccepExThread(INT nAcceptNum)
{
	// 放出指定数量的AcceptEx
	m_nAcceptExs = 0;

	// 创建setup acceptEx线程
	INT nReturn = _beginthreadex(NULL, 0, 
		(THREADPROC)m_Trunk.sfp1(&vEngine::CompletionTransport::ThreadSetupAcceptEx), 
		(LPVOID)nAcceptNum, 0, NULL);
	if( 0 == nReturn || -1 == nReturn )
	{
		ERR(_T("[s]create setup acceptex thread failed"));
		return FALSE;
	}

	m_vecWorkThreadHandle.push_back((HANDLE)nReturn);
	// 激活event
	::SetEvent(m_hEventSetupAccepEx);	

	return TRUE;
}


//-----------------------------------------------------------------------------
// thread setup acceptEx
//-----------------------------------------------------------------------------
UINT CompletionTransport::ThreadSetupAcceptEx(LPVOID p)
{
	EXCEPTION_PROTECT_START;;

	INT nRequiredAcceptNum = (INT)p;
	SOCKET sockIncome;

	while( FALSE == m_bTerminateWorkThread )
	{
		while( m_nAcceptExs < nRequiredAcceptNum )
		{
__get_free_client_data:
			// first 申请一个新的 client data 数据块
			tagClientData* pClient = m_listFreeClientData.PopFront();
			if( FALSE == GT_VALID(pClient) )
			{
				// no free client data, wait and try again
				Sleep(120);
				if( TRUE == m_bTerminateWorkThread ) // 防止死循环
					break;

				goto __get_free_client_data;
			}

			sockIncome = socket(AF_INET, SOCK_STREAM, 0);
			tagUnitData* pUnit = m_pAcceptUnitPool->AllocUnit();

			pClient->bValid = TRUE;
			pClient->bShutDown = FALSE;
			pClient->bClosed = FALSE;
			pClient->bReadyForRecv = FALSE;

			pClient->sock = sockIncome;
			pClient->dwClientID = GT_INVALID;
			pClient->dwSerialID = m_dwClientSerialID;
			pClient->nSendCast = 0;
			pClient->nRecvCast = 1;

			m_mapClientSerialIndex.Add(m_dwClientSerialID, pClient->nIndex);
			m_dwClientSerialID++;

			pUnit->eState = EUS_Accept;
			pUnit->nCDIndex = pClient->nIndex;
			pUnit->wbuf.len = GT_MAX_PACKAGE_LEN;
			pUnit->ov.hEvent = NULL;

			DWORD dwBytes = 0;	// 临时使用
			BOOL bResult = AcceptEx(m_sockListener, sockIncome, 
				pUnit->wbuf.buf, 0, sizeof(sockaddr_in)+16, 
				sizeof(sockaddr_in)+16, &dwBytes, (OVERLAPPED*)pUnit);

			if( FALSE == bResult && ERROR_IO_PENDING != WSAGetLastError() )
			{
				ClearAndReturnClientData(pClient);
				m_pAcceptUnitPool->FreeUnit(pUnit);
				continue;
			}

			// 将要或者立即成功
			InterlockedIncrement((LONG*)&m_nAcceptExs);
		}

		WaitForSingleObject(m_hEventSetupAccepEx, 120);
	}

	shutdown(m_sockListener, SD_BOTH);
	closesocket(m_sockListener);

	EXCEPTION_PROTECT_END
	return 0;
}




//-----------------------------------------------------------------------------
// create worker thread
//-----------------------------------------------------------------------------
BOOL CompletionTransport::CreateWorkThread(INT nThreadNum)
{
	LONG lUnit = m_InitParam.nUnitPerWorkerThread;

    for(INT n=0; n<nThreadNum; n++)
    {
		// 首先建立worker thread对应的unit pool
		m_pUnitPool[n+1] = new UnitPool;
		if( NULL == m_pUnitPool[n+1] )
		{
			ERR(_T("[s]create unit mem pool failed"));
			return FALSE;
		}
		if( FALSE == m_pUnitPool[n+1]->Init(lUnit) )
		{
			ERR(_T("[s]init unit mem pool failed"));
			return FALSE;
		}

		// 创建工作线程
        INT nReturn = _beginthreadex
			(NULL, 0, (THREADPROC)m_Trunk.sfp1(&vEngine::CompletionTransport::ThreadWorker), NULL, 0, NULL);
		if( 0 == nReturn || -1 == nReturn )
		{
			ERR(_T("[s]create worker thread failed"));
			return FALSE;
		}

		m_vecWorkThreadHandle.push_back((HANDLE)nReturn);
    }

	return TRUE;
}


//-----------------------------------------------------------------------------
// work thread
//-----------------------------------------------------------------------------
UINT CompletionTransport::ThreadWorker(LPVOID pDataEntry)
{

	EXCEPTION_PROTECT_START;

	// 记录自己的编号
	INT	nThreadIndex = InterlockedIncrement((LONG*)&m_lThreadCounter);

	// 临时变量
	DWORD dwFlags = 0;
	DWORD dwBytes = 0;
	DWORD* pKey=0;

	sockaddr_in *pRemoteAddr, *pLocalAddr;
	DWORD dwRemoteAddrSize = 0, dwLocalAddrSize = 0;
	WSABUF wbuf;
	DWORD dwBytesNeed = 0;
	DWORD dwBytesComp = 0;
	BOOL bResult= FALSE;
	WSAOVERLAPPED*	pOverlapped = NULL;
	tagUnitData* pUnitGot = NULL;
        
	while( FALSE == m_bTerminateWorkThread )
	{
		// 得到完成状态
		bResult = ::GetQueuedCompletionStatus(m_hCompletionPort, &dwBytesComp, 
			(DWORD*)&pKey, &pOverlapped, 500);

		if( 0 == bResult )
		{
			if( NULL != pOverlapped )
			{
				switch(GetLastError())
				{
				case WAIT_TIMEOUT:ERR(_T("work thread wait time out"));break;
				case ERROR_NETNAME_DELETED:break;
				case ERROR_NO_SYSTEM_RESOURCES:break;
				case ERROR_SEM_TIMEOUT:break;
				case ERROR_OPERATION_ABORTED:break;
				case ERROR_HOST_UNREACHABLE:break;
				case ERROR_CONNECTION_ABORTED:break;
				default:ERR(_T("work thread error"));break;
				}

				pUnitGot = CONTAINING_RECORD(pOverlapped, tagUnitData, ov);
				tagClientData* pClient = &m_ClientData[pUnitGot->nCDIndex];
			
				switch( pUnitGot->eState )
				{
				case EUS_Accept: // accept 过程失败
					// 清扫client data,返还unit
					this->ClearAndReturnClientData(pClient);
					m_pAcceptUnitPool->FreeUnit(pUnitGot);

					// 激活setup acceptex thread event
					InterlockedDecrement((LONG*)&m_nAcceptExs);
					::SetEvent(m_hEventSetupAccepEx);	
					break;

				default:	// 其他过程失败,连接中断,交还资源
					this->PerformClientLogout(pUnitGot);
					break;
				}
			}
			continue;
		}

		// 得到实际指针
		pUnitGot = CONTAINING_RECORD(pOverlapped, tagUnitData, ov);
		tagClientData* pClient = &m_ClientData[pUnitGot->nCDIndex];

		// unit状态分析
		switch( pUnitGot->eState )
		{
		case EUS_Accept:
			{
				// 设置一些socket属性
				// 继承属性
				INT nResult = 0;
				nResult = setsockopt(pClient->sock, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, 
					(char*)&m_sockListener, sizeof(m_sockListener));

				// hard shut down
				linger lin;
				lin.l_linger = 0;
				lin.l_onoff = 1;
				nResult = setsockopt(pClient->sock, SOL_SOCKET, SO_LINGER, 
					(char*)&lin, sizeof(lin));

				// 处理客户端IP表
				::GetAcceptExSockaddrs(pUnitGot->wbuf.buf, 0, 
					sizeof(sockaddr_in)+16,	sizeof(sockaddr_in)+16, 
					(sockaddr**)&pLocalAddr, (LPINT)&dwLocalAddrSize, 
					(sockaddr**)&pRemoteAddr, (LPINT)&dwRemoteAddrSize);

				// 记录 ip/port
				pClient->dwAddress = pRemoteAddr->sin_addr.S_un.S_addr;
				pClient->wPort = pRemoteAddr->sin_port;

				// 激活setup acceptex thread event
				InterlockedDecrement((LONG*)&m_nAcceptExs);
				::SetEvent(m_hEventSetupAccepEx);

				// Associate the socket with the completion port
				if( NULL == ::CreateIoCompletionPort((HANDLE)pClient->sock, m_hCompletionPort, 0, 0) )
				{
					this->ClearAndReturnClientData(pClient);
					m_pAcceptUnitPool->FreeUnit(pUnitGot);
					break;
				}

				// 未验证客户
				pClient->dwConnectTime = timeGetTime();
				m_listUnauthClient.PushBack(pClient->nIndex);

				// 引发一个recv length重叠IO请求
				tagUnitData* pUnit = m_pUnitPool[nThreadIndex]->AllocUnit();
				pUnit->ov.hEvent = NULL;
				pUnit->eState = EUS_RecvLength;
				pUnit->dwBytesReady = 0;
				pUnit->wbuf.len = sizeof(DWORD);
				pUnit->nCDIndex = pUnitGot->nCDIndex;
				pUnit->bFirstRecvUnit = true;	// 第一个接收包

				dwFlags = 0;
				if( 0 != WSARecv(pClient->sock, &pUnit->wbuf, 1, &dwBytes, &dwFlags,
					&pUnit->ov, NULL) )
				{
					// 发生异常错误，自动返还unit
					if( WSA_IO_PENDING != WSAGetLastError() )
					{
						// 连接中断,交还资源
						this->PerformClientLogout(pUnit);
					}
				}	

				// 返还accept unit
				m_pAcceptUnitPool->FreeUnit(pUnitGot);
			}
			break;

		case EUS_RecvLength:	// 先读入包长度
			{
				pUnitGot->eState = EUS_RecvLengthComplete;
				if( dwBytesComp == 0 )
				{
					// 连接中断,交还资源
					this->PerformClientLogout(pUnitGot);
					break;
				}

				dwBytesNeed = pUnitGot->wbuf.len - pUnitGot->dwBytesReady;
				pUnitGot->dwBytesReady += dwBytesComp;
				if( dwBytesComp == dwBytesNeed )
				{
					pUnitGot->dwBytesReady = 0;
					pUnitGot->wbuf.len = (*(DWORD*)&pUnitGot->wbuf.buf[0]) & 0xffffff;	// 取低三位
					pUnitGot->byEncryptNum = *(BYTE*)&pUnitGot->wbuf.buf[3];	// DWORD最高位
					// 客户端可能非法发出超长的包
					if( pUnitGot->wbuf.len > GT_MAX_PACKAGE_LEN )
					{
						// 连接中断,交还资源
						// 纠正,以免致命错误
						pUnitGot->wbuf.len = GT_MAX_PACKAGE_LEN;
						this->PerformClientLogout(pUnitGot);
						break;

						// 非法用户举报回调函数
						if( m_fnHackReport )
							(m_fnHackReport)(pClient->dwSerialID);
					}

					pUnitGot->eState = EUS_Recv;
					pUnitGot->bFirstUnitOfPackage = true;
					pUnitGot->ov.hEvent = NULL;
					dwFlags = 0;
					if( 0 != WSARecv(pClient->sock, &pUnitGot->wbuf, 1, &dwBytes, 
						&dwFlags, &pUnitGot->ov, NULL) )
					{
						// 发生异常错误，自动返还unit
						if( WSA_IO_PENDING != WSAGetLastError() )
						{
							// 连接中断,交还资源
							this->PerformClientLogout(pUnitGot);
						}
					}	
				}
				else
				{
					// The array of WSABUF structures pointed to by the lpBuffers 
					// parameter is transient. If this operation completes in an 
					// overlapped manner, it is the service provider's responsibility 
					// to capture these WSABUF structures before returning from this 
					// call. This enables applications to build stack-based WSABUF arrays
					wbuf.buf = pUnitGot->wbuf.buf + pUnitGot->dwBytesReady;
					wbuf.len = pUnitGot->wbuf.len - pUnitGot->dwBytesReady;
					pUnitGot->ov.hEvent = NULL;
					pUnitGot->eState = EUS_RecvLength;
					dwFlags = 0;
					if( 0 != WSARecv(pClient->sock, &wbuf, 1, &dwBytes, &dwFlags,
						&pUnitGot->ov, NULL) )
					{
						// 发生异常错误，自动返还unit
						if( WSA_IO_PENDING != WSAGetLastError() )
						{
							// 连接中断,交还资源
							this->PerformClientLogout(pUnitGot);
						}
					}		
				}
			}
			break;

		case EUS_Recv:	// 实际接收完成
			{
				pUnitGot->eState = EUS_RecvComplete;
				if( dwBytesComp == 0 )
				{
					// 连接中断,交还资源
					this->PerformClientLogout(pUnitGot);
					break;
				}

				dwBytesNeed = pUnitGot->wbuf.len - pUnitGot->dwBytesReady;
				pUnitGot->dwBytesReady += dwBytesComp;

				if( dwBytesComp == dwBytesNeed )
				{
					// 调用回调函数,对数据包进行解密
					if( 0 != pUnitGot->byEncryptNum )
						(m_fnDecrypt)(pUnitGot);

					// 通过第一个包对用户身份进行校验
					if( true == pUnitGot->bFirstRecvUnit )
					{
						if( TRUE == m_listUnauthClient.Erase(pUnitGot->nCDIndex) )
						{
							// 收到第一个包,用户数目增加,通知外部,调用回调函数
							// 并且第一个包也会加入到接收缓冲
							tagLoginParam login_param;
							login_param.dwAddress = pClient->dwAddress;	// 远程地址
							login_param.wPort = pClient->wPort;	// 远程端口

							InterlockedExchange((LONG*)&pClient->bReadyForRecv, FALSE);
							pClient->dwClientID = (m_fnLogIn)(pUnitGot, &login_param);

							if( GT_INVALID == pClient->dwClientID )
							{
								// 未通过身份验证,踢出
								this->PerformClientLogout(pUnitGot);
								break;
							}

							while( NULL == pClient->pRecvQueue )	// 不停尝试申请
							{
								pClient->pRecvQueue = new SafeQueue<tagUnitData*>;
							}

							// 准备好接受消息
							InterlockedExchange((LONG*)&pClient->bReadyForRecv, TRUE);
						}
						else	// 用户已经超过验证时间
						{
							this->PerformClientLogout(pUnitGot);
							break;
						}
					}
					
					// 申请新的unit
					tagUnitData* pUnit = m_pUnitPool[nThreadIndex]->AllocUnit();
					pUnit->ov.hEvent = NULL;
					pUnit->eState = EUS_RecvLength;
					pUnit->wbuf.len = sizeof(DWORD);
					pUnit->dwBytesReady = 0;
					pUnit->nCDIndex = pUnitGot->nCDIndex;
					pUnit->bFirstRecvUnit = false;

					// 将收到的加入接收缓冲
					if( FALSE == pClient->pRecvQueue->Add(pUnitGot) )
					{
						// 接受缓冲充满，踢掉客户端
						m_pUnitPool[nThreadIndex]->FreeUnit(pUnit);
						this->PerformClientLogout(pUnitGot);
						break;
					}

					// 引发一个recv length重叠IO请求
					dwFlags = 0;
					if( 0 != WSARecv(pClient->sock, &pUnit->wbuf, 1, &dwBytes, &dwFlags,
						&pUnit->ov, NULL) )
					{
						// 发生异常错误
						if( WSA_IO_PENDING != WSAGetLastError() )
						{
							// 连接中断,交还资源
							this->PerformClientLogout(pUnit);
						}
					}				
				}
				else
				{
					pUnitGot->eState = EUS_Recv;
					wbuf.buf = pUnitGot->wbuf.buf + pUnitGot->dwBytesReady;
					wbuf.len = pUnitGot->wbuf.len - pUnitGot->dwBytesReady;
					pUnitGot->ov.hEvent = NULL;
					dwFlags = 0;
					if( 0 != WSARecv(pClient->sock, &wbuf, 1, &dwBytes, &dwFlags,
						&pUnitGot->ov, NULL) )
					{
						// 发生异常错误
						if( WSA_IO_PENDING != WSAGetLastError() )
						{
							// 连接中断,交还资源
							this->PerformClientLogout(pUnitGot);
						}
					}
				}
			}
			break;

		case EUS_Send:	// 发送完成
			{
				InterlockedDecrement((LONG*)&pClient->nSendCast);

				// 发送正常完成,交还unit到内存池
				if( FALSE == m_pUnitPool[m_nSmallPool]->TryFreeUnit(pUnitGot) )
				{
					BOOL bFreed = FALSE;
					for( INT n=0; n<m_lThreadCounter; n++ )
					{
						if( m_pUnitPool[n+1]->TryFreeUnit(pUnitGot) )
						{
							bFreed = TRUE;
							break;
						}
					}
					if( !bFreed )
						m_pUnitPool[nThreadIndex]->FreeUnit(pUnitGot);
				}

			}	// US_Send
			break;

		default:	// 从来没有发生过
			ASSERT(0);
			TObjRef<Log>()->Write(_T("err unit state %d\r\n"), pUnitGot->eState);
			
			if( FALSE == m_pUnitPool[m_nSmallPool]->TryFreeUnit(pUnitGot) )
				m_pUnitPool[nThreadIndex]->FreeUnit(pUnitGot);
			break;
		}
	}
	
	EXCEPTION_PROTECT_END;
	_endthreadex(0);
	return 0;
}





//-----------------------------------------------------------------------------
// create send thread
//-----------------------------------------------------------------------------
BOOL CompletionTransport::CreateSendThread()
{
	// 首先建立send thread对应的send queue
	m_pSendQueue = new SafeQueue<tagUnitData*>(TRUE);
	if( NULL == m_pSendQueue )
	{
		ERR(_T("create send unit queue failed"));
		return FALSE;
	}

    INT nReturn = _beginthreadex
		(NULL, 0, (THREADPROC)m_Trunk.sfp1(&vEngine::CompletionTransport::ThreadSend), NULL, 0, NULL);
	if( 0 == nReturn || -1 == nReturn )
	{
		ERR(_T("create send thread failed"));
		return FALSE;
    }

	SetThreadPriority((HANDLE)nReturn, HIGH_PRIORITY_CLASS);

	m_vecWorkThreadHandle.push_back((HANDLE)nReturn);
	return TRUE;
}


//-----------------------------------------------------------------------------
// send thread
//-----------------------------------------------------------------------------
UINT CompletionTransport::ThreadSend(LPVOID)
{
	EXCEPTION_PROTECT_START;

	tagUnitData*	pUnit = NULL;
	INT				nReturn = 0;
	DWORD			dwBytes = 0;

	while( FALSE == m_bTerminateWorkThread )
	{
		for(;;)	// 注意内部的break
		{
			pUnit = m_pSendQueue->Get();
			if( NULL == pUnit )
				break;	// 发完为止

			tagClientData* pClient = &m_ClientData[pUnit->nCDIndex];
			if( TRUE == pClient->bShutDown )
			{
				// 此人可能已经登出
				InterlockedDecrement((LONG*)&pClient->nSendCast);
				this->ReturnUnit(pUnit);
				continue;
			}

			// 加密unit回调函数
			if( pUnit->byEncryptNum )
				(m_fnEncrypt)(pUnit);

			pUnit->ov.hEvent = NULL;
			pUnit->dwBytesReady = 0;

			nReturn = WSASend(pClient->sock, &pUnit->wbuf, 1, &dwBytes, 0, &pUnit->ov, NULL);
			if( nReturn )
			{
				if( WSA_IO_PENDING != WSAGetLastError() )
				{
					// 发生错误,连接中断,交还资源
					this->PerformClientLogout(pUnit);
				}
			}
		}

		WaitForSingleObject(m_pSendQueue->GetEvent(), 120);
	}
	
	EXCEPTION_PROTECT_END;
	_endthreadex(0);
	return 0;
}




//-----------------------------------------------------------------------------
// 创建平衡 unit pool 线程
//-----------------------------------------------------------------------------
BOOL CompletionTransport::CreatePoolBalanceThread()
{
    INT nReturn = _beginthreadex
		(NULL, 0, (THREADPROC)m_Trunk.sfp1(&vEngine::CompletionTransport::ThreadPoolBalance), NULL, 0, NULL);
	if( 0 == nReturn || -1 == nReturn )
	{
		ERR(_T("[s]create pool balance thread failed"));
		return FALSE;
    }

	m_vecWorkThreadHandle.push_back((HANDLE)nReturn);
	return TRUE;
}



//-----------------------------------------------------------------------------
// 平衡 unit pool 线程
//-----------------------------------------------------------------------------
UINT CompletionTransport::ThreadPoolBalance(LPVOID)
{
	EXCEPTION_PROTECT_START;

	INT nSmallPool = 1;
	INT nBigPool = 1;
	INT nSmallPoolSize = 0x7fffffff;
	INT nBigPoolSize = 0;
	BOOL bChanged = FALSE;
	INT nTotalSize = 0;

	while( FALSE == m_bTerminateWorkThread )
	{
		nSmallPool = 1;
		nBigPool = 1;
		nSmallPoolSize = 0x7fffffff;
		nBigPoolSize = 0;
		bChanged = FALSE;
		nTotalSize = 0;

		m_szInfo[0] = 0;
		// 统计出最小和最大unit池
		INT nAllSize = 0;
		for(INT n=0; n<m_lThreadCounter; n++)
		{
			INT nSize = m_pUnitPool[n+1]->GetSize();
			nTotalSize += m_pUnitPool[n+1]->GetTotalNum();	// 一共真正从内存alloc多少unit

			if( nSize <= nSmallPoolSize )
			{
				nSmallPool = n+1;
				nSmallPoolSize = nSize;
				bChanged = TRUE;
			}

			if( nSize > nBigPoolSize )
			{
				nBigPool = n+1;
				nBigPoolSize = nSize;
				bChanged = TRUE;
			}

			nAllSize += nSize;	// 统计总大小
		}

		// 显示出unit数量数据以及他们的和
		/*_stprintf( m_szInfo, _T("%d<%d ac%d cd%d cl%d uc%d"), 
			nAllSize, nTotalSize, 
			m_nAcceptExs, m_nMaxLoad-m_listFreeClientData.Size(), 
			m_listCloseClient.Size(), m_listUnauthClient.Size());
		*/
		if( (nSmallPoolSize * 16) < nBigPoolSize )
			InterlockedExchange((LONG*)&m_bOverBalance, TRUE);
		else
			InterlockedExchange((LONG*)&m_bOverBalance, FALSE);

		if( bChanged )
		{
			InterlockedExchange((LONG*)&m_nSmallPool, nSmallPool);
			InterlockedExchange((LONG*)&m_nBigPool, nBigPool);
		}

		Sleep(200);	// run per 200 ms
	}

	EXCEPTION_PROTECT_END;
	_endthreadex(0);
	return 0;
}





//-----------------------------------------------------------------------------
// 得到客户端sockname
//-----------------------------------------------------------------------------
BOOL CompletionTransport::GetClientSockName(sockaddr_in& addr_in, DWORD dwSerialID)
{
	INT nIndex = m_mapClientSerialIndex.Peek(dwSerialID);
	if( GT_INVALID == nIndex )
		return FALSE;

	tagClientData* pClient = &m_ClientData[nIndex];
	if( FALSE == pClient->bValid  )
		return FALSE;

	// 返回一个可以供任何socket函数使用的合法地址
	ZeroMemory(addr_in.sin_zero, sizeof(addr_in.sin_zero));
	addr_in.sin_family = AF_INET;   // internetwork: UDP, TCP, etc.
	addr_in.sin_addr.S_un.S_addr = pClient->dwAddress; 
	addr_in.sin_port = pClient->wPort; 
	return TRUE;
}



//-----------------------------------------------------------------------------
// 踢出客户端
//-----------------------------------------------------------------------------
VOID CompletionTransport::Kick(DWORD dwSerialID)
{
	INT nCDIndex = m_mapClientSerialIndex.Peek(dwSerialID);
	if( nCDIndex != GT_INVALID )
	{
		if( FALSE == m_ClientData[nCDIndex].bClosed )
		{
			shutdown(m_ClientData[nCDIndex].sock, SD_BOTH);
		}
	}
}


//-----------------------------------------------------------------------------
// 当连接中断,交还资源时调用
//-----------------------------------------------------------------------------
VOID CompletionTransport::PerformClientLogout(tagUnitData* pUnit)
{
	tagClientData* pClient = &m_ClientData[pUnit->nCDIndex];

	if( FALSE == pClient->bClosed )
	{
		if( FALSE == pClient->bShutDown )
		{
			InterlockedExchange((LONG*)&pClient->bShutDown, TRUE);
			shutdown(pClient->sock, SD_BOTH);

			// 用户数目减少 通知外部,调用回调函数
			(m_fnLogOut)(pClient->dwClientID);

			m_listCloseClient.PushBack(pUnit->nCDIndex);
		}
	}


	// 记录登出时间
	pClient->dwLogoutTime = timeGetTime();

	switch(pUnit->eState)
	{
	case EUS_Send: 
	case EUS_SendComplete: 
		InterlockedDecrement((LONG*)&pClient->nSendCast); 
		break;
	case EUS_RecvLength: 
	case EUS_RecvLengthComplete:
	case EUS_Recv:
	case EUS_RecvComplete:
		InterlockedDecrement((LONG*)&pClient->nRecvCast); 
		break;
	default:
		ASSERT(0);
		break;
	}

	this->ReturnUnit(pUnit);
}


//-----------------------------------------------------------------------------
// create close socket thread
//-----------------------------------------------------------------------------
BOOL CompletionTransport::CreateCloseSocketThread()
{
	// 读配置文件
	INT nCloseWaitTime = m_InitParam.nCloseSocketWaitTime;

	// 创建队列对象
	m_listCloseClient.Clear();

	// 创建close socket thread
	INT nReturn = _beginthreadex(NULL, 0, 
		(THREADPROC)m_Trunk.sfp1(&vEngine::CompletionTransport::ThreadCloseSocket), 
		(LPVOID)(nCloseWaitTime), 0, NULL);
	if( 0 == nReturn || -1 == nReturn )
	{
		ERR(_T("[s]create close socket thread failed"));
		return FALSE;
	}

	m_vecWorkThreadHandle.push_back((HANDLE)nReturn);
	return TRUE;
}


//-----------------------------------------------------------------------------
// thread close socket
//-----------------------------------------------------------------------------
UINT CompletionTransport::ThreadCloseSocket(LPVOID p)
{
	EXCEPTION_PROTECT_START;

	DWORD			dwWaitForCloseTime = (DWORD)p;
	INT				nIndex = 0;
	tagClientData*	pClient = NULL;

	while( FALSE == m_bTerminateWorkThread )
	{
		// 程序关闭时，thread_worker可能已经结束，所以unit可能不能完全回收，
		for(;!m_bTerminateWorkThread;)	// 注意内部的break
		{
			nIndex = m_listCloseClient.PopFront();
			if( GT_INVALID == nIndex )
				break;	// 处理完为止

			pClient = &m_ClientData[nIndex];
			
			// 等待时间成熟
			while( timeGetTime() - pClient->dwLogoutTime <= dwWaitForCloseTime )
				WaitForSingleObject(m_listCloseClient.GetEvent(), 1000);

			if( 0 == pClient->nSendCast && 0 == pClient->nRecvCast )
			{
				ClearAndReturnClientData(pClient);	// 清除并且回收clientdata
			}
			else
			{
				if( FALSE == pClient->bClosed )
				{
					InterlockedExchange((LONG*)&pClient->bClosed, TRUE);
					closesocket(pClient->sock);
				}

				m_listCloseClient.PushBack(nIndex);
			}
		} // for(;;)

		WaitForSingleObject(m_listCloseClient.GetEvent(), 120);
	}

	EXCEPTION_PROTECT_END;
	_endthreadex(0);
	return 0;
}



//-----------------------------------------------------------------------------
// create thread heart beat to unauth client
//-----------------------------------------------------------------------------
BOOL CompletionTransport::CreateUnauthHeartbeatThread()
{
	// 读配置文件
	INT nAuthWaitTime = m_InitParam.nAuthWaitTime;

	// 创建队列对象
	m_listUnauthClient.Clear();

	// 创建 heart beat to unauth client thread
	INT nReturn = _beginthreadex
		(NULL, 0, (THREADPROC)m_Trunk.sfp1(&vEngine::CompletionTransport::ThreadUnauthHeartbeat), 
		(LPVOID)(nAuthWaitTime), 0, NULL);
	if( 0 == nReturn || -1 == nReturn )
	{
		ERR(_T("[s]create heart beat to unauth client thread failed"));
		return FALSE;
	}

	m_vecWorkThreadHandle.push_back((HANDLE)nReturn);
	return TRUE;
}



//-----------------------------------------------------------------------------
// thread heart beat to unauth client
//-----------------------------------------------------------------------------
UINT CompletionTransport::ThreadUnauthHeartbeat(LPVOID p)
{
	EXCEPTION_PROTECT_START;

	DWORD	dwWaitForAuthTime = (DWORD)p;	// 等待认证的时间
	INT		nIndex = 0;

	while( FALSE == m_bTerminateWorkThread )
	{
		m_listUnauthClient.Lock();
		m_listUnauthClient.ResetIterator();

		while( m_listUnauthClient.PeekNext(nIndex) )
		{
			tagClientData* pClient = &m_ClientData[nIndex];
			if( timeGetTime() - pClient->dwConnectTime > dwWaitForAuthTime )
			{
				if( FALSE == pClient->bClosed )
				{
					InterlockedExchange((LONG*)&pClient->bShutDown, TRUE);
					shutdown(pClient->sock, SD_BOTH);
				}

				// 记录登出时间
				pClient->dwLogoutTime = timeGetTime();
				m_listCloseClient.PushBack(nIndex);
				m_listUnauthClient.Erase(nIndex);
				break;
			}
		}

		m_listUnauthClient.Unlock();
		Sleep(120);
	}

	EXCEPTION_PROTECT_END;
	_endthreadex(0);
	return 0;
}





//-----------------------------------------------------------------------------
// 清除并且回收client data
//-----------------------------------------------------------------------------
VOID CompletionTransport::ClearAndReturnClientData(tagClientData* pClient)
{
	if( m_mapClientSerialIndex.Erase(pClient->dwSerialID) )
	{
		if( FALSE == pClient->bClosed )
		{
			InterlockedExchange((LONG*)&pClient->bClosed, TRUE);
			closesocket(pClient->sock);
		}

		// 将我们收到的所有包都丢弃
		tagUnitData* pUnit = NULL;
		if( pClient->pRecvQueue )
		{
			while( pUnit = pClient->pRecvQueue->Get() )
				ReturnUnit(pUnit);
		}

		pClient->bValid = FALSE;
		pClient->bShutDown = TRUE;
		pClient->bReadyForRecv = FALSE;

		// 收回client data
		m_listFreeClientData.PushBack(pClient);	
	}
}




//-----------------------------------------------------------------------------
// 默认加密函数
//-----------------------------------------------------------------------------
UINT CompletionTransport::DefaultEncryptFunction(tagUnitData* pUnit)
{
	m_pCrypt->Encrypt(pUnit);
	return 0;
}

//-----------------------------------------------------------------------------
// 默认解密函数
//-----------------------------------------------------------------------------
UINT CompletionTransport::DefaultDecryptFunction(tagUnitData* pUnit)
{
	m_pCrypt->Decrypt(pUnit);
	return 0;
}


//-----------------------------------------------------------------------------
// for DEBUG
// 检测unit泄漏情况
// 检查unit是否完全归返,如若没有,则找出它们的地址
//-----------------------------------------------------------------------------
VOID CompletionTransport::CheckUnit()
{
#ifdef _DEBUG_UNIT

	std::list<tagUnitData*>	listUnit;
	std::list<tagUnitData*>::iterator it;
	INT nNum = 0;

	listUnit.clear();
	INT nSize = mapUnitAddress.Size();
	mapUnitAddress.ExportAllKey(listUnit);

	tagUnitData* pUnit = NULL;

	for(it=listUnit.begin(); it!=listUnit.end(); ++it)
	{
		pUnit = *it;
		if( pUnit && pUnit->eState != EUS_Accept )
		{
			// ASSERT(0);
			TObjRef<Log>()->Write("unit%d kind%d\r\n", nNum, pUnit->eState);
			nNum++;
		}
	}

	TObjRef<Log>()->Write("total leaked unit=%d\r\n", nNum);
	TObjRef<Log>()->Write("close client list:%d\r\n", m_listCloseClient.Size());
	
	while( m_listCloseClient.Size() > 0 )
	{
		INT nIndex = m_listCloseClient.PopFront();
		if( GT_INVALID == nIndex )
				break;	// 处理完为止

		tagClientData* pClient = &m_ClientData[nIndex];
			
		// 对异常情况进行文件输出
		TObjRef<Log>()->Write("nIndex%d SendCast=%d\r\n", nIndex, pClient->nSendCast);
		TObjRef<Log>()->Write("nIndex%d RecvCast=%d\r\n", nIndex, pClient->nRecvCast);
	}

	TObjRef<Log>()->Write("unit in accept queue:%d\r\n", 
		m_pAcceptUnitPool->GetSize());


#endif // #ifdef _DEBUG_UNIT
}



} // namespace vEngine {
