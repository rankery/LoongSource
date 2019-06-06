#pragma once
#include "Singleton.h"

//-----------------------------------------------------------------------------
// 防沉迷session
//-----------------------------------------------------------------------------
class FatigueSession : public Singleton<FatigueSession>
{
public:
	//-------------------------------------------------------------------------
	// CONSTRUCT
	//-------------------------------------------------------------------------
	FatigueSession() : m_Trunk(this), m_nPort(0), m_bTerminateUpdate(FALSE), m_bTerminateConnect(FALSE), m_bConnected(FALSE) {}
	~FatigueSession() {}

public:
	//-------------------------------------------------------------------------
	// 初始化和销毁
	//-------------------------------------------------------------------------
	BOOL	Init();
	VOID	Destroy();

	VOID	SendQueryStatus(DWORD dwAccount);
	VOID	SendOLGuardAccounts( DWORD *pOLAccounts, DWORD dwNum );
	DWORD	HandleGuardAccountNotify( PVOID pMsg );

	//-------------------------------------------------------------------------
	// 更新消息
	//-------------------------------------------------------------------------
	VOID	Update();

	//-------------------------------------------------------------------------
	// 状态信息
	//-------------------------------------------------------------------------
	BOOL	IsConnected()	{	return m_pTransport->IsConnected();	}

private:
	//-------------------------------------------------------------------------
	// 连接线程
	//-------------------------------------------------------------------------
	VOID	ThreadConnectServer();

	//-------------------------------------------------------------------------
	// 初始化配置
	//-------------------------------------------------------------------------
	BOOL	InitConfig();


private:
	TSFPTrunk<FatigueSession>	m_Trunk;
	TObjRef<Util>				m_pUtil;
	TObjRef<Thread>				m_pThread;
	TObjRef<StreamTransport>	m_pTransport;

	//-------------------------------------------------------------------------
	// 线程结束标志
	//-------------------------------------------------------------------------
	volatile BOOL				m_bTerminateUpdate;
	volatile BOOL				m_bTerminateConnect;

	//-------------------------------------------------------------------------
	// 连接参数
	//-------------------------------------------------------------------------
	CHAR						m_szIP[X_IP_LEN];		// 连接防沉迷服务器的IP
	INT							m_nPort;				// 连接防沉迷服务器的端口
	volatile BOOL				m_bConnected;			// 连接标志

};

#define sFatigueSession FatigueSession::getSingleton()

