//-----------------------------------------------------------------------------
//!\file transport.h
//!\author Lyp
//!
//!\date 2004-04-03
//! last 2008-04-17
//!
//!\brief 服务器端传输底层
//-----------------------------------------------------------------------------
#pragma once

// 因为某些函数inline, 所以这里不得不包括一些头文件
#include "safe_unit_queue.h"	
#include "safe_mutex_list.h"
#include "unit_pool.h"

namespace vEngine {
//-----------------------------------------------------------------------------
//!\brief 服务器端传输底层
//!
//!
//-----------------------------------------------------------------------------
class VENGINE_API CompletionTransport
{
public:
	BOOL Init(tagServerInitParam* pInitParam);
	BOOL Destroy();

	// 设置注册注销回调函数
	VOID SetLogCallback(LOGINCALLBACK fnLogIn, LOGOUTCALLBACK fnLogOut);
	// 设置加密解密unit回调函数
	VOID SetSecureCallBack(CRYPTCALLBACK fnEncrypt, CRYPTCALLBACK fnDecrypt);
	// 设置非法用户举报回调函数
	VOID SetHackReportCallBack(HACKREPORTCALLBACK fnHackReport);


	// 得到尚未发送完成的unit数目
	__forceinline INT GetUnsendUnitNum() { return m_pSendQueue->GetNum(); } 
	// 发送unit, 返回此用户还有多少包未发出去
	__forceinline INT SendUnit(tagUnitData* pUnit);
	// 得到发出wsasend但是没有收到完成通知的包数目
	__forceinline INT	GetSendCast(INT nCDIndex) 
	{return m_ClientData[nCDIndex].nSendCast;}

	// 接收unit,并可以得到当前收到的unit数目总和
	__forceinline tagUnitData* RecvUnit(DWORD& dwRecvedNum, INT nCDIndex);
	
	// 返还unit给transport
	__forceinline VOID ReturnUnit(tagUnitData* pUnit);
	// 从transport取得unit
	__forceinline tagUnitData* GetFreeUnit();

	// 得到序列ID
	DWORD GetSerialID(INT nCDIndex) { return m_ClientData[nCDIndex].dwSerialID; }
	// 得到客户端sockname
	BOOL GetClientSockName(sockaddr_in& addr_in, DWORD dwSerialID);

	// 踢出客户端
	VOID Kick(DWORD dwSerialID);

	// 得到端口号
	INT GetPort() { return m_nPort; }
	TCHAR* GetInfo() { return m_szInfo; }

	CompletionTransport();
	~CompletionTransport();

private:
	TSFPTrunk<CompletionTransport>	m_Trunk;

	// callback functions
	LOGINCALLBACK		m_fnLogIn;		// 登陆回调
	LOGOUTCALLBACK		m_fnLogOut;		// 登出回调
	CRYPTCALLBACK		m_fnEncrypt;	// 加密回调
	CRYPTCALLBACK		m_fnDecrypt;	// 解密回调
	HACKREPORTCALLBACK	m_fnHackReport;	// 攻击回调

	// unit data
	UnitPool*				m_pAcceptUnitPool;
	UnitPool*				m_pUnitPool[MAX_UNIT_POOL_NUM];
	SafeQueue<tagUnitData*>*	m_pSendQueue;
	SectCrypt*				m_pCrypt;
	
	TSafeList<tagClientData*>	m_listFreeClientData;

	TSafeMutexList<INT>	m_listCloseClient;	// 需要关闭的client data队列
	TSafeMutexList<INT> m_listUnauthClient;

	// client data
	VOLATILE DWORD		m_dwClientSerialID;
	tagClientData*		m_ClientData;
	

	VOLATILE BOOL		m_bPreShutdown;		// 是否需要准备关闭

	// thread info
	VOLATILE BOOL		m_bTerminateWorkThread;	// 是否结束所有线程
	VOLATILE LONG		m_lThreadCounter;	// 服务器线程计数
	std::vector<HANDLE>	m_vecWorkThreadHandle;	// 用来保存线程的vector

	// pool balance
	VOLATILE INT		m_nSmallPool;
	VOLATILE INT		m_nBigPool;
	VOLATILE BOOL		m_bOverBalance;	// 非常不平衡
	
	// base config
	VOLATILE INT		m_nMaxLoad;		// 服务器最大人数
    VOLATILE SOCKET		m_sockListener;	// 监听socket
	VOLATILE HANDLE		m_hCompletionPort; // 服务器完成端口
	VOLATILE INT		m_nPort;		// 服务器端口
	VOLATILE INT		m_nAcceptExs;	// 放出的AcceptEx数量

	TSafeMap<DWORD, INT>	m_mapClientSerialIndex;	// Client SerialID -> index

	// thread stuff
	HANDLE	m_hEventSetupAccepEx;

	tagServerInitParam	m_InitParam;	// 启动参数
	TCHAR				m_szInfo[1024];	// 运行信息，可供外部显示

	// 专门放出接受连接请求的线程
	BOOL	CreateSetupAccepExThread(INT nAcceptNum);
	UINT	ThreadSetupAcceptEx(LPVOID);
	// 专门用来关闭socket的线程
	BOOL	CreateCloseSocketThread();
	UINT	ThreadCloseSocket(LPVOID);
	// 对于未认证客户端的延时踢出线程
	BOOL	CreateUnauthHeartbeatThread();
	UINT	ThreadUnauthHeartbeat(LPVOID);
	// 完成端口通知处理线程
	BOOL	CreateWorkThread(INT nThreadNum);
	UINT	ThreadWorker(LPVOID);
	// 网络包发送线程	
	BOOL	CreateSendThread();
	UINT	ThreadSend(LPVOID);
	// 内存池平衡线程 	
	BOOL	CreatePoolBalanceThread();
	UINT	ThreadPoolBalance(LPVOID);

	// 当用户离线时调用
	VOID	PerformClientLogout(tagUnitData* pUnit);
	// 清除并且回收client data
	VOID	ClearAndReturnClientData(tagClientData* pClient);


	// 默认加密解密函数
	UINT	DefaultEncryptFunction(tagUnitData*);
	UINT	DefaultDecryptFunction(tagUnitData*);

	// 检查unit是否完全归返,如若没有,则找出它们的地址
	VOID	CheckUnit();

};



//-----------------------------------------------------------------------------
// 发送unit，返回此用户还有多少包未发出去
//-----------------------------------------------------------------------------
INT CompletionTransport::SendUnit(tagUnitData* pUnit)
{
	pUnit->eState = EUS_Send;

	// 必须在此处理nSendCast,而不能放在threadsend
	// 因为肯定是游戏主循环调用此函数，而logout回调函数还未调用
	// 这样就可以保证逻辑严密性
	tagClientData* pClient = &m_ClientData[pUnit->nCDIndex];
	InterlockedIncrement((LONG*)&pClient->nSendCast);

	m_pSendQueue->Add(pUnit);
	return pClient->nSendCast;
}


//-----------------------------------------------------------------------------
// 接收unit,并可以得到当前收到的unit数目总和
//-----------------------------------------------------------------------------
tagUnitData* CompletionTransport::RecvUnit(DWORD& dwRecvedNum, INT nCDIndex)
{
	ASSERT(nCDIndex >= -1 && nCDIndex < m_nMaxLoad);

	tagClientData* pClient = &m_ClientData[nCDIndex];
	if( FALSE == pClient->bReadyForRecv )
		return NULL;

	dwRecvedNum = pClient->pRecvQueue->GetNum();
	return pClient->pRecvQueue->Get();
}



//-----------------------------------------------------------------------------
// return unit
//-----------------------------------------------------------------------------
VOID CompletionTransport::ReturnUnit(tagUnitData* pUnit)
{
	ASSERT(pUnit);
	// 还比较平衡,可以先选择能瞬间进入锁定区的pool来return
	if( FALSE == m_bOverBalance )	
	{
		if( m_pUnitPool[m_nSmallPool]->TryFreeUnit(pUnit) )
			return;

		for( INT n=0; n<m_lThreadCounter; n++ )
		{
			if( m_pUnitPool[n+1]->TryFreeUnit(pUnit) )
				return;
		}
	}

	// 最后等待进入critical section
	m_pUnitPool[m_nSmallPool]->FreeUnit(pUnit);
}



//-----------------------------------------------------------------------------
// get free unit
//-----------------------------------------------------------------------------
tagUnitData* CompletionTransport::GetFreeUnit()
{
	tagUnitData* pUnit = NULL;
	// 还比较平衡,可以先选择能瞬间进入锁定区的pool来get
	if( FALSE == m_bOverBalance )	
	{
		if( pUnit = m_pUnitPool[m_nBigPool]->TryAllocUnit() )
			return pUnit;

		for( INT n=0; n<m_lThreadCounter; n++ )
		{
			if( pUnit = m_pUnitPool[n+1]->TryAllocUnit() )
				return pUnit;
		}
	}
	
	// 最后等待进入critical section
	return m_pUnitPool[m_nBigPool]->AllocUnit();
}


} // namespace vEngine {
