//-----------------------------------------------------------------------------
//!\file stream_transport.h
//!\author Lyp
//!
//!\date 2003-03-17
//! last 2008-04-17
//!
//!\brief 广域网客户端通讯底层
//!
//!	Copyright (c) 1985-2007 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

class MsgQueueTS;
#include "common.h"
//-----------------------------------------------------------------------------
//! Provides sequenced, reliable, two-way, connection-based virtual package 
//! streams transmission mechanism. Uses TCP for the Internet address family.
//-----------------------------------------------------------------------------
class StreamTransport
{
public:
	BOOL Init(BOOL bNagle=TRUE, BOOL bPackageMode=TRUE);	// 是否打开Nagle算法
	VOID Destroy();

	//-----------------------------------------------------------------------------
	// 网络基本操作
	//-----------------------------------------------------------------------------
	VOID TryToConnect(LPCSTR szIP, INT nPort=0); // 尝试连接指定地址
	BOOL IsTryingToConnect();	// 是否正在尝试连接
	VOID StopTryingConnect();	// 放弃尝试连接
	VOID Disconnect(); // 断开连接
	BOOL IsConnected() { return m_bConnected; }

	// 如果返回FALSE,代表没有连接或没有内存空间存放所给的msg
	BOOL Send(LPVOID pMsg, DWORD dwMsgSize);
	// 如果返回NULL表示没有连接或未收到数据
	LPBYTE Recv(DWORD& dwMsgSize);
	// 对应于Recv(),因为内存是内部分配的，所以应该交还内部来释放
	__declspec(noinline) VOID FreeRecved(LPBYTE pMsg) {delete[](pMsg);}

	
	//-----------------------------------------------------------------------------
	// 信息统计以及工具函数
	//-----------------------------------------------------------------------------
	INT	GetUnsendPackageNum();	// 得到未发送的包数量，可以以此来判断网络负荷
	INT	GetReceivedPackageNum(); // 得到已接受但未被上层处理的包数量
	DWORD GetTotalSendPackNum() { return m_dwSendPackNum; }	// 获取总发包数(含未发出的或发送失败的)
	DWORD GetTotalRecvPackNum() { return m_dwRecvPackNum; }	// 包含总收包数
	DWORD GetBytesFlowOut(){ return m_dwByteOut; } // 得到每秒流出量
	DWORD GetBytesFlowIn(){ return m_dwByteIn; } // 得到每秒流入量

	CHAR* GetRemoteIPString() { return m_szIP; }// 得到对方IP地址字符串
	CHAR* GetHostName();	// 得到本地主机名称
	BOOL GetIPByHostName(DWORD& dwIP, LPCSTR lpHostName);	// 得到指定主机的IP
	BOOL GetLocalLANIP(DWORD& dwIP);	// 得到本机局域网地址
	BOOL GetLocalWANIP(DWORD& dwIP);	// 得到本机广域网地址
	CHAR* IP2String(DWORD dwIP);	// 将真正IP地址转化为IP地址字符串
	DWORD StringIP2IP(CHAR* szIP);	// 将IP地址字符串转化为真正IP地址


	StreamTransport();
	~StreamTransport();

private:
	BOOL					m_bPackageMode;			// 包模式

	// 发送接收安全缓冲
	MsgQueueTS*				m_pSendMsgQueue;
	MsgQueueTS*				m_pRecvMsgQueue;
	
	// 加密解密源块
	PBYTE					m_pSect;
	volatile BOOL			m_bEncrypt;			// 是否开始加密

	// 网络配置
	CHAR					m_szIP[X_LONG_STRING];
	CHAR					m_szHostName[X_LONG_STRING*2];
	INT						m_nPort;
	SOCKET					m_Sock;
	BOOL					m_bNagle;

	// 内部线程句柄
	HANDLE					m_hConnect;
	HANDLE					m_hSend;
	HANDLE					m_hReceive;

	// 内部线程控制
	volatile BOOL			m_bTerminateConnect;
	volatile BOOL			m_bTerminateSend;
	volatile BOOL			m_bTerminateReceive;
	volatile BOOL			m_bConnected;		// 是否连接上

	//! 每秒流量统计
	volatile DWORD			m_dwLastStatInTime;	// 上次In统计时间
	volatile DWORD			m_dwLastStatOutTime;// 上次Out统计时间
	volatile DWORD			m_dwByteOut;		// 当前输出
	volatile DWORD			m_dwByteIn;			// 当前输入
	volatile DWORD			m_dwByteOutCounter;
	volatile DWORD			m_dwByteInCounter;


	//收发包基本信息(连接开始--连接结束)
	volatile DWORD			m_dwSendPackNum;	// 总发包数(包含发送失败和还未发出去的)
	volatile DWORD			m_dwRecvPackNum;    // 总收包数(包含还未处理的)

	// 内部方法
	VOID Clear();
	BOOL GracefulCloseSocket(SOCKET& socket, INT nRetryTimes);
	BOOL GracefulCloseThread(HANDLE& thread, INT nRetryTime);

	// 内部线程
	static UINT ThreadConnect(LPVOID);	// connect thread function
	static UINT ThreadSend(LPVOID);	// send thread function
	static UINT ThreadReceive(LPVOID);	// receive thread function
	static UINT ThreadStreamReceive(LPVOID);

	// 加密解密相关
	VOID CreateSect();	// 创造加密源
	VOID Encrypt(LPVOID pMsg, DWORD dwRealSize, DWORD dwEncryptType);
	VOID Decrypt(LPVOID pMsg, DWORD dwRealSize, DWORD dwEncryptType);
};

