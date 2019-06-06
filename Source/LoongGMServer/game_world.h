//-----------------------------------------------------------------------------
//!\file game_world.h
//!\brief 游戏世界
//!
//!\date 2009-04-20
//! last 2009-04-20
//!\author zhangrong
//!
//! Copyright (c) 1985-2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "../WorldDefine/base_define.h"

class GameServer;
class GameServerWorld;
class GameServerDB;
class ServerDB;
//-----------------------------------------------------------------------------
// 游戏世界
//-----------------------------------------------------------------------------
class GameWorld
{
public:
	//-------------------------------------------------------------------------
	// CONSTRUCT
	//-------------------------------------------------------------------------
	GameWorld();
	~GameWorld();

	//-------------------------------------------------------------------------
	// 初始化及更新
	//-------------------------------------------------------------------------
	BOOL Init(INT nIndex);
	VOID Update();

	//-------------------------------------------------------------------------
	// 系统信息相关
	//-------------------------------------------------------------------------
	VOID GenerateWorldStaticInfo(LPBYTE &pBuff, DWORD& dwSize);
	VOID GenerateWorldDynamicInfo(LPBYTE &pBuff, DWORD& dwSize);

	//-------------------------------------------------------------------------
	// 各种Get
	//-------------------------------------------------------------------------
	DWORD GetID() const						{ return m_dwWorldID;	}

	GameServerWorld*	GetWorld()	const	{ return m_pWorldServer;}
	GameServerDB*		GetDB() const		{ return m_pDBServer;	}
	ServerDB*			GetServerDB() const		{ return m_pDB;			}

	LPCTSTR				GetWorldName() const	{ return m_szWorldName; }	

	VOID				SendWorldMsg(tagNetCmd* pCmd);

private:
	BOOL InitDB( LPCTSTR szWorld );

private:
	DWORD					m_dwWorldID;				// 游戏世界ID
	TCHAR					m_szWorldName[X_SHORT_NAME];// 游戏世界名称
	GameServerWorld*		m_pWorldServer;				// LoongWorld服务器
	GameServerDB*			m_pDBServer;				// LoongDB服务器
	ServerDB*				m_pDB;						// todo: 该游戏世界的数据库连接
};