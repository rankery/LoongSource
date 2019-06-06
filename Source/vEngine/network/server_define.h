//-----------------------------------------------------------------------------
//!\file server_define.h
//!\author Lyp
//!
//!\date 2004-07-02
//! last 2008-04-17
//!
//!\brief 服务器端基本定义
//-----------------------------------------------------------------------------
#pragma once


namespace vEngine {
//-----------------------------------------------------------------------------
// 服务器端基本定义
//-----------------------------------------------------------------------------
#ifndef VOLATILE
#define VOLATILE volatile
#endif
#define GT_MAX_PACKAGE_LEN		512	// max package length
#define MAX_UNIT_POOL_NUM		512
#define DEFAULT_SPIN_COUNT		4000

// #define _DEBUG_UNIT	// 是否检查 Unit



template<typename ITEM_POINTER>
class SafeQueue;
class UnitPool;
class CompletionTransport;
class CompletionSession;
class SectCrypt;


//-----------------------------------------------------------------------------
// 服务器启动参数
//-----------------------------------------------------------------------------
struct tagServerInitParam
{
	INT	nPort;						// 端口
	INT nMaxServerLoad;				// 服务器承载量人数极限
	INT nPerCpuWorkerThread;		// 每CPU工作线程
	INT nAdditionalWorkerThread;	// 附加工作线程数 
	INT nAuthWaitTime;				// 认证最长等待时间
	INT nUnitPerWorkerThread;		// 每个工作线程的unit数量
	INT nAcceptExNum;				// 建立的acceptex数量
	INT nCloseSocketWaitTime;		// close queue 中的每个pUnit等待时间
	bool bReusePort;				// 是否重用端口

	tagServerInitParam()
	{ 
		nPort = 6008; 
		nMaxServerLoad = 256; 
		nPerCpuWorkerThread = 2; 
		nAdditionalWorkerThread = 2; 
		nAuthWaitTime = 300000; 
		nUnitPerWorkerThread = 1000;
		nAcceptExNum = 32; 
		nCloseSocketWaitTime = 5000;
		bReusePort = false;
	}

	VOID SetLarge()
	{
		nMaxServerLoad = 20000;
		nUnitPerWorkerThread = 50000;
		nAcceptExNum = 128; 
	}

	VOID SetNormal()
	{
		nMaxServerLoad = 10000;
		nUnitPerWorkerThread = 10000;
		nAcceptExNum = 128; 
	}
};


//-----------------------------------------------------------------------------
// 登陆参数，可以作为客户端登陆时 上层和下层的之间信息交换的手段
//-----------------------------------------------------------------------------
struct tagLoginParam
{
	DWORD	dwAddress;
	WORD	wPort;
};


//-----------------------------------------------------------------------------
// server exchage unit 状态
//-----------------------------------------------------------------------------
enum ECSUnitState
{
	EUS_Null = 0,
	EUS_Accept,
	EUS_FirstRecv,
	EUS_RecvLength,
	EUS_RecvLengthComplete,
	EUS_Recv,
	EUS_RecvComplete,
	EUS_Send,
	EUS_SendComplete,
	EUS_Read,
	EUS_Write,
	EUS_End 
};



//-----------------------------------------------------------------------------
//!	名称定义：针对每个unit, 开放了1个缓冲区，为windows定义的wbuf
//-----------------------------------------------------------------------------
struct tagUnitData
{
	WSAOVERLAPPED	ov;						// 重叠IO结构
	WSABUF			wbuf;					// 基本缓冲

	ECSUnitState	eState;					// 基本状态
	INT				nCDIndex;				// point to client data array
	DWORD			dwBytesReady;			// 已经有多少字节传输完毕

	tagUnitData*	pNext;					// 指向下一个unit,用于safe_unit_queue

	bool			bFirstRecvUnit;			// 是否收到的第一个包
	bool			bFirstUnitOfPackage;	// 是否是package中的第一个unit
	BYTE			byEncryptNum;			// 加密号,对于同一个package必须相同

	
};



//-----------------------------------------------------------------------------
//!	针对于单个客户端的定义
//-----------------------------------------------------------------------------
struct tagClientData
{
	VOLATILE BOOL		bValid;
	VOLATILE BOOL		bShutDown;			// 进入等待关闭状态
	VOLATILE BOOL		bClosed;			// 是否closesocket过
	VOLATILE BOOL		bReadyForRecv;		// 是否准备好接受消息
	
	VOLATILE INT		nSendCast;			// 已经发出的wsasend,尚未收到通知的
	VOLATILE INT		nRecvCast;			// 已经发出的wsarecv,尚未收到通知的
	
	VOLATILE DWORD		dwConnectTime;		// 用来确定踢出长时间未验证客户端
	VOLATILE DWORD		dwLogoutTime;		// 用来确定closesocket时间

	INT					nIndex;
	DWORD				dwClientID;			// 客户端ID,上层确认第一个包以后才有效
	DWORD				dwSerialID;			// 序列号,用于client id invalid的情况下

	SafeQueue<tagUnitData*>*  pRecvQueue;			// 收包队列

	SOCKET				sock;
	DWORD				dwAddress;
	WORD				wPort;
};





// 定义线程函数类型
typedef UINT (WINAPI* LOGINCALLBACK)(tagUnitData*, tagLoginParam*);
typedef UINT (WINAPI* LOGOUTCALLBACK)(DWORD);
typedef UINT (WINAPI* CRYPTCALLBACK)(tagUnitData*);
typedef UINT (WINAPI* HACKREPORTCALLBACK)(DWORD);


} // namespace vEngine {