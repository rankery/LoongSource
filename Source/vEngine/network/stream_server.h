//-----------------------------------------------------------------------------
//!\file stream_server.h
//!\author Lyp
//!
//!\date 2007-11-27
//!
//!\brief tcp_socket 服务器端
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine {

// 定义线程函数类型
typedef UINT (WINAPI* TCPLOGINCALLBACK)(LPBYTE pByte, DWORD dwSize);
typedef UINT (WINAPI* TCPLOGOUTCALLBACK)(DWORD dwClientID);


//-----------------------------------------------------------------------------
// 多线程服务器端，为每个连接开辟读写线程，适用于大流量少连接通讯
// 设置登陆/登出回调函数,外部需要保证两个回调函数线程安全
// 注意当用户登录时,会将第一个包作为参数传给fnLogIn, fnLogIn必须返回一个ID
// 如果上层返回GT_INVALID,会拒绝此客户端登陆
// 当用户登出时,会将上层给的ID作为参数传给fnLogOut
//-----------------------------------------------------------------------------
class VENGINE_API StreamServer
{
public:
	BOOL Init(TCPLOGINCALLBACK fnLogIn, TCPLOGOUTCALLBACK fnLogOut, INT nPort, 
		BOOL bReUsePort=TRUE, BOOL bPackageMode=TRUE);
	BOOL Destroy();

	//-----------------------------------------------------------------------------
	// 网络基本操作
	//-----------------------------------------------------------------------------
	// 返回发送缓冲目前大小
	INT Send(DWORD dwClientID, LPVOID pMsg, DWORD dwSize);
	// nUnRecved=目前底层收包缓冲的包数量
	LPBYTE Recv(DWORD dwClientID, DWORD& dwSize, INT& nUnRecved);
	// 因为内存是内部分配的，所以收包后应该交还内部来释放
	VOID FreeRecved(DWORD dwClientID, LPBYTE pMsg) ;
	
	BOOL Kick(DWORD dwClientID);	// 返回FALSE:找不到指定玩家


	//-----------------------------------------------------------------------------
	// 信息统计以及工具函数
	//-----------------------------------------------------------------------------
	// 得到服务器端口号
	INT GetPort() { return m_nPort; }
	// 得到连接上的客户端数目
	INT	GetClientNum() { return m_mapClient.Size(); }  
	// 检查指定ID的客户是不是存在
	BOOL IsClientExsit(DWORD dwID) { return m_mapClient.IsExist(dwID); }
	// 得到指定ID客户端的sock name
	BOOL GetClientSockName(sockaddr_in* pAddr, DWORD dwClientID);
	// 总发包数
	DWORD GetMsgSend() {return m_dwMsgSend;}
	// 总收包数
	DWORD GetMsgRecv() {return m_dwMsgRecv;}
	// 发送字节总量
	DWORD GetBytesSend() {return m_dwBytesSend;}
	// 收字节总量
	DWORD GetBytesRecv() {return m_dwBytesRecv;}


	StreamServer();
	~StreamServer();

private:
	struct tagClient	// 每客户端数据
	{
		DWORD			dwID;
		SOCKET			Socket;
		VOLATILE BOOL	bTerminate;
		HANDLE			hThreadSend;
		HANDLE			hThreadRecv;
		MsgQueueTS*		pSendMsgQueue;
		MsgQueueTS*		pRecvMsgQueue;
		DWORD			dwConnectTime;
		INT				nRecvPackageNum;

		tagClient():dwID(GT_INVALID),Socket(0),bTerminate(FALSE),
			hThreadSend(0),hThreadRecv(0),pSendMsgQueue(0),pRecvMsgQueue(0),
			dwConnectTime(0),nRecvPackageNum(0){}
	};

	TSFPTrunk<StreamServer>				m_Trunk;
	SafeMemPool*						m_pMemPool;
	CRITICAL_SECTION					m_DisconnectLock;
	BOOL								m_bPackageMode;			// 包模式
	TCPLOGINCALLBACK					m_fnLogIn;				// 各种回调函数
	TCPLOGOUTCALLBACK					m_fnLogOut;

	VOLATILE BOOL						m_bTerminateAccept;		// 是否需要关闭Accept线程
	VOLATILE BOOL						m_bTerminateDisconnect;	// 是否需要关闭断连线程
	TSafeMap<DWORD, tagClient*>			m_mapClient;			// 记录用户nCDIndex和ID情况
	TSafeList<tagClient*>				m_listDisClient;		// 将要删除的客户端队列
	INT									m_nPort;				// 端口
	sockaddr_in							m_Address;				// server address
	SOCKET								m_Socket;				// 用来Accept的socket
	HANDLE								m_hThreadAccept;		// 线程句柄
	HANDLE								m_hThreadDisconnect;	// 线程句柄

	volatile DWORD						m_dwMsgSend;			// 统计数据
	volatile DWORD						m_dwMsgRecv;
	volatile DWORD						m_dwBytesSend;
	volatile DWORD						m_dwBytesRecv;


	// 内部线程
	UINT ThreadAccept(LPVOID);
	UINT ThreadDisconnect(LPVOID);
	// 每客户端线程
	UINT ThreadSend(tagClient*);
	UINT ThreadRecv(tagClient*);
	UINT ThreadStreamRecv(tagClient*);

	VOID DisconnetClient(tagClient* pClient);

	
};


} // namespace wEngine {