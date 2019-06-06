//-----------------------------------------------------------------------------
//!\file proof_policy_own.h
//!\author Aslan
//!
//!\date 2008-11-30
//! last 2008-11-30
//!
//!\brief 自己公司的验证策略——中央激活
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "proof_policy.h"

//-----------------------------------------------------------------------------
// 该验证策略的数据库
//-----------------------------------------------------------------------------
class ProofPolicyOwnDB
{
public:
	//-------------------------------------------------------------------------
	// CONSTRUCT
	//-------------------------------------------------------------------------
	ProofPolicyOwnDB() {}
	~ProofPolicyOwnDB() {}

	//-------------------------------------------------------------------------
	// 初始化
	//-------------------------------------------------------------------------
	BOOL	Init();

	//-------------------------------------------------------------------------
	// 查询
	//-------------------------------------------------------------------------
	BOOL	QueryAccount(LPCSTR szAccountName, string& strPsd, tagProofResult* pResult);
	BOOL	InsertAccout(LPCSTR szAccountName, LPCSTR szPsd, LPCSTR szMiBao, BOOL bGuard);
	BOOL	UpdateAccout(LPCSTR szAccountName, LPCSTR szPsd, LPCSTR szMiBao, BOOL bGuard);
	BOOL	UpdatePsd(LPCSTR szAccountName, LPCSTR szNewPsd);
	BOOL	UpdateMibao(LPCSTR szAccountName, LPCSTR szMibao);

private:
	DataBase		m_DB;
};

//-----------------------------------------------------------------------------
// 验证策略
//-----------------------------------------------------------------------------
class ProofPolicyOwn : public ProofPolicy
{
public:
	//-------------------------------------------------------------------------
	// CONSTRUCT
	//-------------------------------------------------------------------------
	ProofPolicyOwn() : m_Trunk(this), m_nPort(0), m_bTerminateUpdate(FALSE), m_bTerminateConnect(FALSE), m_bConnected(FALSE) {}
	~ProofPolicyOwn() {}

public:
	//-------------------------------------------------------------------------
	// 初始化和销毁
	//-------------------------------------------------------------------------
	BOOL	Init(PROOFCALLBACK fn);
	VOID	Destroy();

	//-------------------------------------------------------------------------
	// 验证接口
	//-------------------------------------------------------------------------
	VOID	Proof(DWORD dwClientID, LPCSTR szAccoutName, LPCSTR szPsd, LPCSTR szGUID, DWORD dwIP);

	//-------------------------------------------------------------------------
	// 验证服务器连接状态
	//-------------------------------------------------------------------------
	INT		GetProofServerStatus();

	//-------------------------------------------------------------------------
	// 玩家登出事件
	//-------------------------------------------------------------------------
	VOID	SigPlayerLogoutEvent(DWORD dwAccountID){};

private:
	//-------------------------------------------------------------------------
	// 更新
	//-------------------------------------------------------------------------
	VOID	ThreadUpdate();

	//-------------------------------------------------------------------------
	// 连接线程
	//-------------------------------------------------------------------------
	VOID	ThreadConnectServer();

	//-------------------------------------------------------------------------
	// 初始化配置
	//-------------------------------------------------------------------------
	BOOL	InitConfig();

	//-------------------------------------------------------------------------
	// 更新验证列表
	//-------------------------------------------------------------------------
	VOID	UpdateProofList();

	//-------------------------------------------------------------------------
	// 更新消息
	//-------------------------------------------------------------------------
	VOID	UpdateSession();

	//-------------------------------------------------------------------------
	// 消息注册
	//-------------------------------------------------------------------------
	VOID	RegisterProofMsg();
	VOID	UnregisterProofMsg();

	//-------------------------------------------------------------------------
	// 消息处理函数
	//-------------------------------------------------------------------------
	DWORD	HandleLogin(tagNetCmd* pCmd, DWORD);
	DWORD	HandleUserLogin(tagNetCmd* pCmd, DWORD);
	DWORD	HandleUserUpdatePwd(tagNetCmd* pCmd, DWORD);
	DWORD	HandleUserBindMibao(tagNetCmd* pCmd, DWORD);
	DWORD	HandleUserUnbindMibao(tagNetCmd* pCmd, DWORD);
	DWORD	HandleBlockAccount(tagNetCmd* pCmd, DWORD);
	DWORD	HandleUnBlockAccount(tagNetCmd* pCmd, DWORD);
	DWORD	HandleSetChenMi(tagNetCmd* pCmd, DWORD);

private:
	TSFPTrunk<ProofPolicyOwn>	m_Trunk;
	TObjRef<Util>				m_pUtil;
	TObjRef<Thread>				m_pThread;
	TObjRef<NetCmdMgr>			m_pCmdMgr;
	TObjRef<StreamTransport>	m_pTransport;

	//-------------------------------------------------------------------------
	// 线程结束标志
	//-------------------------------------------------------------------------
	volatile BOOL				m_bTerminateUpdate;
	volatile BOOL				m_bTerminateConnect;


	//-------------------------------------------------------------------------
	// 连接参数
	//-------------------------------------------------------------------------
	CHAR						m_szIP[X_IP_LEN];		// 连接中央认证服务器的IP
	INT							m_nPort;				// 连接中央认证服务器的端口
	volatile BOOL				m_bConnected;			// 连接标志

	//-------------------------------------------------------------------------
	// 验证数据
	//-------------------------------------------------------------------------
	struct tagPlayerProofData
	{
		DWORD		dwClientID;
		string		strAccoutName;
		string		strPsd;
		string		strGUID;

		tagPlayerProofData(DWORD _dwClientID, LPCSTR szAccoutName, LPCSTR szPsd, LPCSTR szGUID)
		{
			dwClientID		=	_dwClientID;
			strAccoutName	=	szAccoutName;
			strPsd			=	szPsd;
			strGUID			=	szGUID;
		}
	};

	TSafeMutexList<tagPlayerProofData*>	m_listProofData;		// 验证用户数据

	//--------------------------------------------------------------------------
	// 数据库
	//--------------------------------------------------------------------------
	ProofPolicyOwnDB					m_ProofDB;				// 数据库接口
};