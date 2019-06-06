//-----------------------------------------------------------------------------
//!\file x_define.h
//!\author Lyp
//!
//!\date 2004-07-02
//! last 2009-04-03
//!
//!\brief 服务器端基本定义
//-----------------------------------------------------------------------------
#pragma once

#include "memory\safe_mem_pool.h"
#include "x_queue.h"
#include "x_list.h"


namespace vEngine {
//-----------------------------------------------------------------------------
// 服务器端基本定义
//-----------------------------------------------------------------------------
#define X_SLEEP_TIME			500


//-----------------------------------------------------------------------------
// server unit 状态
//-----------------------------------------------------------------------------
enum EXState
{
	EXS_Null = 0,
	EXS_Accept,
	EXS_RecvLength,
	EXS_Recv,
	EXS_Send,
	EXS_End 
};


//-----------------------------------------------------------------------------
//!	unit定义
//-----------------------------------------------------------------------------
struct tagUnit
{
	WSAOVERLAPPED	ov;						// 重叠IO结构
	EXState			eState;					// 基本状态
	SafeMemPool*	pPool;					// 记录从哪个mem_pool分配
	tagUnit*		pNext;					// 指向下一个unit,用于safe_unit_queue
	DWORD			dwBytesReady;			// 已经有多少字节传输完毕
	DWORD			dwSize;					// 有效内容大小,并非实际内存空间大小
	CHAR			pBuffer[sizeof(DWORD)];
};


struct tagRawUnit
{
	SafeMemPool*	pPool;					// 记录从哪个mem_pool分配
	DWORD			dwSize;					// 有效内容大小,并非实际内存空间大小
	tagRawUnit*		pNext;					// 指向下一个unit,用于safe_unit_queue
	CHAR			pBuffer[sizeof(DWORD)];
};


// 计算实际申请unit的大小
#define UNIT_SIZE(size) (sizeof(tagUnit)+(size))

// LZO可能使数据压缩后变大
#define LZO_ADD(size) (((size)/64) + 16 + 3)

// 计算实际申请unit的大小
#define RAWUNIT_SIZE(size) (sizeof(tagRawUnit)+(size))


//-----------------------------------------------------------------------------
//!	针对于单个客户端的定义
//-----------------------------------------------------------------------------
struct tagXClient
{
	BOOL VOLATILE			bShutDown;		// 是否已经被shutdown
	LONG VOLATILE			lSendCast;		// 已经发出的send,尚未收到通知的
	LONG VOLATILE			lRecvCast;		// 已经发出的recv,尚未收到通知的

	SOCKET					sock;			// 客户端对应的socket
	DWORD					dwClientID;		// 客户端ID,上层确认第一个包以后才有效
	DWORD					dwConnectTime;	// 用来确定踢出长时间未验证客户端
	DWORD					dwSendSize;		// 等待发送大小

	XQueue<tagRawUnit*>* 	pSendQueue;		// 发包队列,待拼合压缩
	XQueue<tagUnit*>* 		pRecvQueue;		// 收包队列,待上层收取
};



//-----------------------------------------------------------------------------
// 登陆参数，可以作为客户端登陆时 上层和下层的之间信息交换的手段
//-----------------------------------------------------------------------------
struct tagXLoginParam
{
	DWORD	dwHandle;	// 每客户端handle
	DWORD	dwAddress;	// 远端地址
	DWORD	dwPort;		// 远端端口
};

#define PARAM_SIZE (((sizeof(sockaddr_in)+16)*2)+sizeof(tagXLoginParam))
#define PARAM_INDEX ((sizeof(sockaddr_in)+16)*2)

//-----------------------------------------------------------------------------
// 服务器启动参数
//-----------------------------------------------------------------------------
// 定义线程函数类型
typedef UINT (WINAPI* XLOGINCALLBACK)(tagUnit*, tagXLoginParam*);
typedef UINT (WINAPI* XLOGOUTCALLBACK)(DWORD);

struct tagXServerConfig
{
	XLOGINCALLBACK	fnLogIn;		// 登陆回调
	XLOGOUTCALLBACK	fnLogOut;		// 登出回调

	INT	nPort;						// 端口
	INT nMaxServerLoad;				// 服务器承载量人数极限
	INT nPerCpuWorkerThread;		// 每CPU工作线程
	INT nAdditionWorkerThread;		// 附加工作线程数 
	INT nAcceptExNum;				// 建立的acceptex数量
	DWORD dwPoolSize;				// 每个内存池的大小，内存池数量由线程数决定
	DWORD dwAuthWaitTime;			// 认证最长等待时间(毫秒)
	DWORD dwPerTickSendSize;		// 每客户端每心跳发送最大字节数
	DWORD dwMaxRecvSize;			// 最大接受包长度
	bool bReusePort;				// 是否重用端口
	bool bNagle;					// 是否使用Nagle算法

	tagXServerConfig()
	{ 
		fnLogIn = NULL;
		fnLogOut = NULL;
		nPort = 6008; 
		nMaxServerLoad = 4000; 
		nPerCpuWorkerThread = 2;
		nAdditionWorkerThread = 2;
		nAcceptExNum = 32; 
		
		dwPoolSize = 4*1024*1024;
		dwAuthWaitTime = 20*1000;
		dwPerTickSendSize = 4*1024*1024;
		dwMaxRecvSize = 512;
		
		bReusePort = false;
		bNagle = false;
	}
};


} // namespace vEngine {




