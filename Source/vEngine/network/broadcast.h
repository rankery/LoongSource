//-----------------------------------------------------------------------------
//!\file broadcast.h
//!\author Lyp
//!
//!\date 2004-08-04
//! last 2004-08-04
//!
//!\brief 局域网广播工具
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine {


//-----------------------------------------------------------------------------
//!\brief 局域网广播工具
//!
//!
//-----------------------------------------------------------------------------
class Broadcast
{
public:
	//! 初始化广播对象
	//! \param szLocalIP 绑定到本地什么IP，可以为空
	BOOL Init(INT nPort, LPCSTR szLocalIP=NULL);
	VOID Destroy();

	//! 开始广播线程
	BOOL StartBroadcast();
	//! 开始收听广播线程
	BOOL StartRecvBroadcast();

	//结束接收广播线程
	BOOL TerminateRecvBroadcast(DWORD dwTimeOut=0);
	//结束广播线程
	BOOL TerminateBroadcast(DWORD dwTimeOut=0);

	//! 广播
	VOID Broad(LPBYTE pMsg, DWORD dwMsgSize);
	//! 得到接收到的广播信息，没有就返回NULL
	LPBYTE RecvBroadcast(DWORD& dwMsgSize);
	//! 因为内存是内部分配的，所以应该交还内部来释放
	VOID FreeRecved(LPBYTE pMsg);
	
	Broadcast();
	~Broadcast();


private:
	TSFPTrunk<Broadcast>	m_Trunk;
	
	MsgQueueTS*		m_pSendMsgQueue;
	MsgQueueTS*		m_pRecvMsgQueue;

	HANDLE			m_hThreadBroadcast;
	HANDLE			m_hThreadRecvBroadcast;
	volatile BOOL	m_bTerminateBroadcast;
	volatile BOOL	m_bTerminateRecvBroadcast;

	volatile INT	m_nPort;
	volatile TCHAR	m_szLocalIP[X_LONG_STRING];

	UINT ThreadBroadcast(LPVOID);		// 广播线程
	UINT ThreadRecvBroadcast(LPVOID);	// 听广播线程
};






} // namespace vEngine {
