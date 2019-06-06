//-----------------------------------------------------------------------------
//!\file game_server.h
//!\brief 服务器基类，LoginServer，LoongWorld和LoongDB均从该基类继承
//!
//!\date 2009-04-20
//! last 2009-04-20
//!\author zhangrong
//!
//! Copyright (c) 1985-2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "../ServerDefine/login_define.h"
#include "../ServerDefine/msg_rt_c.h"
#include "../ServerDefine/msg_rt_s.h"

#define MAX_WAIT_FOR_TICK_M		1 
//------------------------------------------------------------------------------
// 服务器基类
//------------------------------------------------------------------------------
class GameServer
{
protected:
	//--------------------------------------------------------------------------
	// CONSTRUCT
	//--------------------------------------------------------------------------
	GameServer(DWORD dwID, LPCSTR szIP);
	virtual ~GameServer();

public:
	//--------------------------------------------------------------------------
	// 更新
	//--------------------------------------------------------------------------
	VOID			Update();

	//--------------------------------------------------------------------------
	// 初始化和销毁
	//--------------------------------------------------------------------------
	BOOL			ServerStart();
	BOOL			ServerShutDown();

	//--------------------------------------------------------------------------
	// 各种Get
	//--------------------------------------------------------------------------
	EServerType		GetType() const			{ return m_eType; }
	DWORD			GetID()	const			{ return m_dwID; }
	LPCSTR			GetIP() const			{ return m_szIP.c_str(); }
	BOOL			IsValid() const			{ return m_bValid; }
	EWorldStatus	GetStatus() const		{ return m_eStatus; }
	INT				GetOnline() const		{ return m_nPlayerNum; }
	INT				GetOnlineDisplay() const{ return m_nPlayerNumDisplay; }

	//--------------------------------------------------------------------------
	// 生成动态信息
	//--------------------------------------------------------------------------
	VOID			FillServerInfo(tagServerInfo* pInfo) const;

	//--------------------------------------------------------------------------
	// 更新状态
	//--------------------------------------------------------------------------
	VOID			RefreshStatus(EWorldStatus eStatus, INT nOnlineNum, INT nMaxOnlineNum, INT nOnlineNumDisplay);
	VOID			ChangeStatus(EWorldStatus eStatus);
	VOID			RefreshLastTickTime(){m_dwLastestTick = GetCurrentDWORDTime();}

	//--------------------------------------------------------------------------
	// 开关服务器
	//--------------------------------------------------------------------------
	BOOL			StartServer();
	BOOL			ShutdownServer();

	//--------------------------------------------------------------------------
	// 执行系统命令
	//--------------------------------------------------------------------------
	BOOL			ExecuteCmd(tagNetCmd*);

	BOOL			IsConnected(){return m_pTran->IsConnected();}

protected:
	VOID			UpdateSession();
	VOID			UpdateConnection(); //与各个server的本地运维服务的连接
	
protected:
	DWORD					m_dwID;				// 全局唯一ID
	BOOL					m_bValid;			// 是否连接
	std::string				m_szIP;				// IP地址

	StreamTransport*		m_pTran;			// 服务器本地守护服务的连接，用于启动及硬关闭远程服务器
	BOOL					m_bTryingConnect;	// 尝试连接中
	INT						m_nPort;

	EServerType				m_eType;			// 服务器类型
	EWorldStatus			m_eStatus;			// 当前的状态
	INT						m_nPlayerNum;		// 人数，对于login和world来说有意义，对db无意义
	INT						m_nPlayerNumDisplay;// 用于显示的，对db无意义
	INT						m_nMaxPlayerNum;	// 最高人数，对于login和world来说有意义，对db无意义
	tagDWORDTime			m_dwLastestTick;	// 该服务器最近的一次心跳的时间
};

//--------------------------------------------------------------------------------------------------
// 生成server的状态
//--------------------------------------------------------------------------------------------------
inline VOID GameServer::FillServerInfo(tagServerInfo* pInfo) const
{
	pInfo->bValid			=	m_bValid;
	pInfo->eStatus			=	m_eStatus;
	pInfo->nMaxPlayerNum	=	m_nMaxPlayerNum;
	pInfo->nPlayerOnline	=	m_nPlayerNumDisplay;//m_nPlayerNum;
	pInfo->nRealOnline		=	m_nPlayerNum;
}

//-------------------------------------------------------------------------------------------------
// Login服务器
//-------------------------------------------------------------------------------------------------
class GameServerLogin : public GameServer
{
public:
	//----------------------------------------------------------------------------------------------
	// CONSTRUCT
	//----------------------------------------------------------------------------------------------
	GameServerLogin(DWORD dwID, LPCSTR szIP);
	~GameServerLogin() {}

};

//---------------------------------------------------------------------------------------------------
// World服务器
//---------------------------------------------------------------------------------------------------
class GameServerWorld : public GameServer
{
public:
	//-----------------------------------------------------------------------------
	// CONSTRUCT
	//-----------------------------------------------------------------------------
	GameServerWorld(DWORD dwID, LPCSTR szIP);
	~GameServerWorld() {}

	//-----------------------------------------------------------------------------
	// 设置双倍
	//-----------------------------------------------------------------------------
	DWORD SetDoubleRate(DWORD dwTime, DWORD dwOpenTime, DWORD dwRatio, DWORD dwDType);

};

//---------------------------------------------------------------------------------------------------
// DB服务器
//---------------------------------------------------------------------------------------------------
class GameServerDB : public GameServer
{
public:
	//-----------------------------------------------------------------------------
	// CONSTRUCT
	//-----------------------------------------------------------------------------
	GameServerDB(DWORD dwID, LPCSTR szIP);
	~GameServerDB() {}
};
