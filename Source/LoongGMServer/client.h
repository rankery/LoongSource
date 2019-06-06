//-----------------------------------------------------------------------------
//!\file	client.h
//!\brief	运营客户端
//!
//!\date	2009-04-20
//! last	2009-04-20
//!\author	zhangrong
//!
//! Copyright (c) 1985-2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "../ServerDefine/rt_define.h"

#include "client_mgr.h"

//-----------------------------------------------------------------------------
// 运营客户端
//-----------------------------------------------------------------------------
class Client
{
public:
	//-------------------------------------------------------------------------
	// CONSTRUCT
	//-------------------------------------------------------------------------
	Client(DWORD dwID) : m_dwID(dwID), m_dwAccountID(GT_INVALID), m_ePrivilege(EP_Null) {}
	~Client() {}

	//-------------------------------------------------------------------------
	// 初始化
	//-------------------------------------------------------------------------
	VOID	Init(DWORD dwAccoutID, INT nPrivilege, LPCTSTR szName, DWORD dwIP);

	//-------------------------------------------------------------------------
	// 各种Get
	//-------------------------------------------------------------------------
	DWORD		GetID() const				{ return m_dwID; }
	DWORD		GetAccountID() const		{ return m_dwAccountID; }
	EPrivilege	GetPrivilege() const		{ return m_ePrivilege; }
	LPCTSTR		GetName() const				{ return m_szName; }
	LPCTSTR		GetIP() const				{ return m_szIP; }

	//-------------------------------------------------------------------------
	// 消息相关
	//-------------------------------------------------------------------------
	VOID	SendMsg(LPVOID pMsg, DWORD dwSize)		{ g_ClientMgr.SendMsg(m_dwID, pMsg, dwSize); }

private:
	DWORD					m_dwID;					// 全局分配的ID
	DWORD					m_dwAccountID;			// 帐号ID
	EPrivilege				m_ePrivilege;			// 权限
	TCHAR					m_szName[X_SHORT_NAME];	// 账号名称
	TCHAR					m_szIP[X_SHORT_NAME];	// 登录IP
};

inline VOID Client::Init(DWORD dwAccoutID, INT nPrivilege, LPCTSTR szName, DWORD dwIP)
{
	m_dwAccountID = dwAccoutID;
	m_ePrivilege = (EPrivilege)nPrivilege;
	_tcsncpy(m_szName, szName, X_SHORT_NAME);

	TObjRef<StreamTransport> pTrans = "StreamTransport";
	TObjRef<Util> pUtil = "Util";
	pUtil->Unicode8ToUnicode(pTrans->IP2String(dwIP), m_szIP);
}
