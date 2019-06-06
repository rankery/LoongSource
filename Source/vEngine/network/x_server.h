//-----------------------------------------------------------------------------
//!\file x_server.h
//!\author Lyp
//!
//!\date 2004-04-03
//! last 2009-03-19
//!
//!\brief 服务器端传输底层
//-----------------------------------------------------------------------------
#pragma once

// 因为某些函数inline, 所以这里不得不包括一些头文件
#include "x_define.h"

namespace vEngine {
class SafeMemPool;
//-----------------------------------------------------------------------------
// 服务器端传输底层
//-----------------------------------------------------------------------------
class VENGINE_API XServer
{
public:
	BOOL Init(tagXServerConfig& param);
	BOOL Destroy();

	//-----------------------------------------------------------------------------
	// 网络基本操作
	//-----------------------------------------------------------------------------
	// 发送
	__forceinline VOID Send(DWORD dwHandle, LPVOID pMsg, DWORD dwSize);
	// 激活发送线程
	__forceinline VOID ActiveSend() { ::SetEvent(m_hEventSend);	}
	// 接收,并可以得到当前底层收到包数（nRecvNum）
	__forceinline LPBYTE Recv(DWORD dwHandle, DWORD& dwSize, INT& nRecvNum);
	// 返还recv给内部释放
	__forceinline VOID FreeRecved(LPBYTE pMsg);

	// 踢出客户端
	VOID Kick(DWORD dwHandle);

	//-----------------------------------------------------------------------------
	// 信息反馈
	//-----------------------------------------------------------------------------
	tagXServerConfig* GetConfig() { return &m_Config; }	// 内含实际端口号等信息
	// 得到发出wsasend但是没有收到完成通知的包数目
	LONG GetSendCast(DWORD dwHandle) { return ((tagXClient*)dwHandle)->lSendCast; }
	DWORD GetSendCastSize(DWORD dwHandle) { return ((tagXClient*)dwHandle)->dwSendSize; }
	INT GetPoolNum() { return m_nPoolNum; }	// 内存池状态
	SafeMemPool* GetPool(INT n) { return &m_Pool[n]; }	// 内存池状态
	INT64 GetSendSize() { return m_n64SendSize; }			// 上层发送大小
	INT64 GetRealSendSize() { return m_n64RealSendSize; }	// 压缩后实际发送大小
	INT GetFreeClientNum() { return m_listFreeClient.Size(); }

	XServer();
	~XServer();

private:
	TSFPTrunk<XServer>	m_Trunk;
	TObjRef<FilterMgr>	m_pFilter;
	TObjRef<Util>		m_pUtil;

	LPFN_ACCEPTEX				m_lpfnAcceptEx;
	LPFN_GETACCEPTEXSOCKADDRS	m_lpfnGetAcceptExSockAddr;

	XList<tagXClient*>	m_listFreeClient;
	XList<tagXClient*>	m_listUnauthClient;
	XList<tagXClient*>	m_listDestroyClient;
	
	std::vector<HANDLE>	m_vecThreadHandle;		// 用来保存线程的vector
	BOOL				m_bTerminateWorkThread;	// 是否结束所有线程
	HANDLE				m_hEventAccepEx;
	HANDLE				m_hEventSend;
	HANDLE				m_hEventDestroy;

	tagXServerConfig	m_Config;				// 启动参数
    SOCKET				m_sockListen;			// 监听socket
	HANDLE				m_hCompletionPort;		// 服务器完成端口

	tagXClient*			m_Client;				// client data
	SafeMemPool*		m_Pool;					// 内存池
	INT					m_nPoolNum;				// 内存池个数

	INT	VOLATILE		m_nAcceptExs;			// 放出的AcceptEx数量
	INT64				m_n64SendSize;			// 统计数据
	INT64				m_n64RealSendSize;		// 统计数据

	// 接受连接请求的线程
	BOOL	CreateSetupAccepExThread();
	UINT	ThreadSetupAcceptEx(LPVOID);
	// 完成通知处理线程
	BOOL	CreateWorkThread(INT nThreadNum);
	UINT	ThreadWorker(INT nThreadIndex);
	// 踢出未认证客户端线程
	BOOL	CreateUnauthHeartbeatThread();
	UINT	ThreadUnauthHeartbeat(DWORD dwWaitForAuthTime);
	// 网络包发送线程	
	BOOL	CreateSendThread();
	UINT	ThreadSend(LPVOID);
	// 资源回收线程
	BOOL	CreateDestroyThread();
	UINT	ThreadDestroy(LPVOID);

	// 回收资源
	VOID	PreDestroyClient(tagXClient* pClient, tagUnit* pUnit);
	VOID	DestroyClient(tagXClient* pClient);
};


//-----------------------------------------------------------------------------
// Send
//-----------------------------------------------------------------------------
VOID XServer::Send(DWORD dwHandle, LPVOID pMsg, DWORD dwSize)
{
	tagXClient* pClient = (tagXClient*)dwHandle;
	if( !pClient->bShutDown )
	{
		// 寻找可以进入锁定的内存池
		INT n = (dwHandle%(m_nPoolNum-1))+1;
		tagRawUnit* pSendUnit=0;
		for(;!pSendUnit;)
		{
			if( ++n >= m_nPoolNum )
				n = 1;
			pSendUnit = (tagRawUnit*)m_Pool[n].TryAlloc(RAWUNIT_SIZE(dwSize));
		}

		pSendUnit->dwSize = dwSize;
		pSendUnit->pPool = &m_Pool[n];
		memcpy(pSendUnit->pBuffer, pMsg, dwSize);
		InterlockedIncrement(&pClient->lSendCast);
		pClient->dwSendSize += dwSize;
		pClient->pSendQueue->Add(pSendUnit);
	}
}


//-----------------------------------------------------------------------------
// 接收unit,并可以得到当前收到的unit数目总和
//-----------------------------------------------------------------------------
LPBYTE XServer::Recv(DWORD dwHandle, DWORD& dwSize, INT& nRecvNum)
{
	tagXClient* pClient = (tagXClient*)dwHandle;
	if( !pClient->bShutDown )
	{
		nRecvNum = pClient->pRecvQueue->GetNum();
		tagUnit* pUnit = pClient->pRecvQueue->Get();
		if( !pUnit )
			return NULL;

		dwSize = pUnit->dwSize;
		return (LPBYTE)pUnit->pBuffer;
	}
	return NULL;
}


//-----------------------------------------------------------------------------
// 接收unit,并可以得到当前收到的unit数目总和
//-----------------------------------------------------------------------------
VOID XServer::FreeRecved(LPBYTE pMsg)
{
	tagUnit* pUnit = CONTAINING_RECORD(pMsg, tagUnit, pBuffer);
	pUnit->pPool->Free(pUnit);
}


} // namespace vEngine {
