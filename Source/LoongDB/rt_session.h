//-----------------------------------------------------------------------------
//!\file to_rt_session.h
//!\author Lyp
//!
//!\date 2005-04-05
//! last 2005-04-05
//!
//!\brief 服务器与监控服务的对话层
//!
//!	Copyright (c) 2005 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

static const INT	SEND_INFO_INTERVAL	=	TICK_PER_SECOND;		// 每隔多长时间发送一次服务器信息

class CLoong;
//-----------------------------------------------------------------------------
// 服务器与监控服务的对话层
//-----------------------------------------------------------------------------
class RTSession
{
public:
	//-------------------------------------------------------------------------
	// Constructor
	//-------------------------------------------------------------------------
	RTSession();
	~RTSession();

	BOOL	Init(CLoong *pLoong);
	VOID	Destroy();
	VOID	Update();

	BOOL	IsConnected()	{ return m_pTran->IsConnected(); }

private:
	VOID	SendServerInfo();
	VOID	RegisterAllRTCommand();
	VOID	UpdateSession();

	DWORD	ThreadConnectRT();

private:
	DWORD	HandleServerLogin(tagNetCmd* pMsg, DWORD);
	DWORD	HandleCloseServer(tagNetCmd* pMsg, DWORD);

private:
	TSFPTrunk<RTSession>		m_Trunk;
	TObjRef<StreamTransport>	m_pTran;
	TObjRef<NetCmdMgr>			m_pMsgCmdMgr;
	TObjRef<Thread>				m_pThread;
	TObjRef<Util>				m_pUtil;

	volatile BOOL				m_bTermConnect;
	volatile BOOL				m_bInitOK;
	//INT							m_nSendInfoTickCountDown;
	CLoong*						m_pLoong;

	tstring						m_strIP;
	DWORD						m_dwPort;
	DWORD						m_dwSectionID;
	DWORD						m_dwWorldID;
};


extern RTSession g_rtSession;