//-----------------------------------------------------------------------------
//!\file to bill_session.h
//!\author Sunnee
//!
//!\date 2009-07-04
//! last 2009-07-04
//!
//!\brief 服务器与划拨服务器的对话层
//!
//!	Copyright (c) 2005 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// 服务器与划拨服务器的对话层
//-----------------------------------------------------------------------------
class BillSession
{
public:
	//-------------------------------------------------------------------------
	// Constructor
	//-------------------------------------------------------------------------
	BillSession();
	~BillSession();

	BOOL	Init();
	VOID	Destroy();
	VOID	Update();

	BOOL	IsConnected()	{ return m_pTran->IsConnected(); }

private:
	VOID	RegisterAllBillCommand();
	VOID	UpdateSession();

	DWORD	ThreadConnectBill();

private:
	DWORD	HandleServerLogin(tagNetCmd* pMsg, DWORD);
	DWORD	HandlePickBaiBao(tagNetCmd* pMsg, DWORD);
	DWORD	NoticeTheBaiBaoYuanbaoChange(tagNetCmd* pMsg);
private:
	TSFPTrunk<BillSession>		m_Trunk;
	TObjRef<StreamTransport>	m_pTran;
	TObjRef<NetCmdMgr>			m_pMsgCmdMgr;
	TObjRef<Thread>				m_pThread;
	TObjRef<Util>				m_pUtil;

	volatile BOOL				m_bTermConnect;
	volatile BOOL				m_bInitOK;

	tstring						m_strIP;
	DWORD						m_dwPort;
	DWORD						m_dwSectionID;
	DWORD						m_dwWorldID;
};


extern BillSession g_billSession;