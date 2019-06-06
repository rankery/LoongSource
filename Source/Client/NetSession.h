//-----------------------------------------------------------------------------
//!\file NetSession.h
//!\author Lyp
//!
//!\date 2008-06-10
//! last 2008-06-12
//!
//!\brief net session
//!
//!	Copyright (c) 1985-2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once


//-----------------------------------------------------------------------------
//!\brief net session
//! 控制发包频率在每次心跳一个，同时提供监控，可以查看有哪些消息累积
//! 可以方便的替换队列中的消息
//-----------------------------------------------------------------------------
class NetSession
{
public:
	VOID Init();
	VOID Connect(LPCTSTR szIP, INT nPort);
	VOID Disconnect();
	BOOL IsConnect();
	VOID ClearMsgQueue();

	VOID Update();

	VOID Send(tagNetCmd* pNetCmd);
	INT	GetSendQueueSize();
	INT	GetSendQueueActionSize();
	INT	GetSendQueueIdleSize();


	// 得到远程IP
	LPCTSTR GetRemoteIPString() { return m_strRemoteIP.c_str(); }

	
	NetSession():m_Trunk(this){}
	~NetSession();

protected:
	TSFPTrunk<NetSession>		m_Trunk;
	TObjRef<Util>				m_pUtil;
	TObjRef<Console>			m_pConsole;
	TObjRef<XClient>			m_pStream;
	TObjRef<NetCmdMgr>			m_pCmdMgr;

	tstring						m_strRemoteIP;
	DWORD						m_dwLastSendTime;

	MsgQueue					m_SendQueueAction;	// 行为队列
	MsgQueue					m_SendQueue;		// 普通队列
	MsgQueue					m_SendQueueIdle;	// 次要消息队列

	DWORD SendQueue(LPCTSTR szCommand);	// 显示sendQueue的信息
};
