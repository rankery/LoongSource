//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: login_session.h
// author: Aslan
// actor:
// data: 2008-7-10
// last:
// brief: 游戏服务器与登陆服务器的对话层
//-----------------------------------------------------------------------------
#pragma once

#include "StdAfx.h"
#include "../WorldDefine/base_define.h"
#include "../WorldDefine/base_define.h"
#include "../WorldDefine/time.h"
#include "mutex.h"

//------------------------------------------------------------------------------
// 即将登陆区域服务器的帐号结构
//------------------------------------------------------------------------------
struct tagWillLoginPlayer
{
	DWORD			dwVerifyCode;	// 验证码
	DWORD			dwTick;			// 通知时的Tick
	BYTE			byPrivilege;	// 帐号权限
	BOOL			bGuard;			// 防沉迷
	DWORD			dwAccOLTime;	// 累计登录时间
	char			szAccount[X_SHORT_NAME];//账号

	tagDWORDTime	dwPreLoginTime;				//上次登录时间
	DWORD			dwPreLoginIP;				//上次登录ip

	tagWillLoginPlayer(DWORD)
	{
		dwVerifyCode = GT_INVALID;
		dwTick = GT_INVALID;
		byPrivilege = 0;
		bGuard = TRUE;
		dwAccOLTime = 0;

		dwPreLoginTime = 0;
		dwPreLoginIP = 0;
	}

	tagWillLoginPlayer(DWORD verifyCode, DWORD tick, BYTE privilege, BOOL guard, 
						DWORD AccOLTime, LPCSTR Account,tagDWORDTime PreLoginTime,
						DWORD PreLoginIP)
	{
		dwVerifyCode = verifyCode;
		dwTick = tick;
		byPrivilege = privilege;
		bGuard = guard;
		dwAccOLTime = AccOLTime;
		strncpy_s(szAccount, Account, X_SHORT_NAME);

		dwPreLoginTime = PreLoginTime;
		dwPreLoginIP = PreLoginIP;
	}
};

//--------------------------------------------------------------------------------
// Login Session
//--------------------------------------------------------------------------------
class LoginSession
{
public:
	LoginSession();
	~LoginSession();

	BOOL Init();
	VOID Destroy();
	VOID Update();

	BOOL IsWell() { return (m_bInitOK && m_pTran->IsConnected()); }

	VOID Send(LPVOID pMsg, DWORD dwMsgSize)	{ if(FALSE == IsWell() ) return;		m_pTran->Send(pMsg, dwMsgSize); }

	INT	GetUnsendPackageNum() { return m_pTran->GetUnsendPackageNum(); }
	INT	GetReceivedPackageNum() { return m_pTran->GetReceivedPackageNum(); }

public:
	VOID RemoveWillLoginPlayer(DWORD dwAccountID)
	{
		m_WillLoginPlayerMutex.Acquire();
		m_mapWillLoginPlayer.Erase(dwAccountID);
		m_WillLoginPlayerMutex.Release();
	}

public:
	BOOL IsAccountValid(DWORD dwAccountID, DWORD dwVerifyCode, BYTE& byPrivilege, BOOL& bGuard, 
						DWORD& dwAccOLSec, LPSTR szAccount,tagDWORDTime &dwPreLoginTime,DWORD &dwPreLoginIP);
	VOID SendPlayerLogin(DWORD dwAccountID, DWORD dwIP, DWORD dwErrorCode);
	VOID SendPlayerLogout(DWORD dwAccountID);
	VOID SendWorldStatus();
private:
	VOID UpdateSession();
	VOID UpdateWillLoginPlayer();

private:
	// 读取文件, 初始化成员
	BOOL InitConfig();
	// 注册所有的网络命令
	VOID RegisterAllLoginCommand();

private:
	// 连接数据库服务器
	DWORD ThreadConnectLogin();

private:
	DWORD HandleCertification(tagNetCmd* pMsg, DWORD);
	DWORD HandleHeartBeat(tagNetCmd* pMsg, DWORD);
	DWORD HandlePlayerWillLogin(tagNetCmd* pMsg, DWORD);
	DWORD HandleKickPlayer(tagNetCmd* pMsg, DWORD);
	DWORD HandleWorldColsed(tagNetCmd* pMsg, DWORD);
	DWORD HandleFatigueNotice(tagNetCmd* pMsg, DWORD);
	
private:
	TSFPTrunk<LoginSession>				m_Trunk;
	TObjRef<Util>						m_pUtil;
	TObjRef<StreamTransport>			m_pTran;
	TObjRef<Thread>						m_pThread;
	TObjRef<NetCmdMgr>					m_pMsgCmdMgr;

	// 连接参数
	CHAR								m_szLoginIP[X_IP_LEN];		// 登陆服务器ip
	DWORD								m_dwLoginPort;				// 登陆服务器port
	
	DWORD								m_dwLocalWanIP;				// 本机广域网IP

	DWORD								m_dwGoldenCode;				// 登陆服务器金色代码

	volatile BOOL						m_bTermConnect;				// 记录当前连接状态
	volatile BOOL						m_bInitOK;					// 是否初始化完成

	TMap<DWORD, tagWillLoginPlayer>		m_mapWillLoginPlayer;		// 将要登陆到区域服务器的玩家列表
	Mutex								m_WillLoginPlayerMutex;		// 即将登陆到区域服务器玩家列表锁

	HANDLE								m_hWorldClosed;
};

extern LoginSession g_loginSession;		// Login Session全局对象