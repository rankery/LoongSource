//-----------------------------------------------------------------------------
//!\file game_world.cpp
//!\brief 游戏世界
//!
//!\date 2009-04-20
//! last 2009-04-20
//!\author zhangrong
//!
//! Copyright (c) 1985-2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "game_world.h"
#include "game_server.h"
#include "server.h"
#include "section_mgr.h"
#include "server_db.h"


//-----------------------------------------------------------------------------
// CONSTRUCT
//-----------------------------------------------------------------------------
GameWorld::GameWorld() : m_dwWorldID(GT_INVALID), m_pWorldServer(NULL), m_pDBServer(NULL)
{

}

GameWorld::~GameWorld()
{
	SAFE_DEL(m_pWorldServer);
	SAFE_DEL(m_pDBServer);
}

//------------------------------------------------------------------------------
// 初始化，nIndex代表游戏世界的序列号
//------------------------------------------------------------------------------
BOOL GameWorld::Init(INT nIndex)
{
	// 得到world主键的名字
	TCHAR szWorld[X_SHORT_NAME] = {_T('\0')};
	_stprintf(szWorld, _T("world_%d"), nIndex);

	TObjRef<VarContainer> pVar = "SectionContainer";

	// 得到游戏世界的名称和ID
	LPCTSTR szWorldName = pVar->GetString(_T("name"), szWorld);
	memcpy(m_szWorldName, szWorldName, sizeof(m_szWorldName));
	m_dwWorldID = pVar->GetDword(_T("id"), szWorld);

	// 生成数据库对象
	if (!InitDB(szWorld))
	{
		return FALSE;
	}
	
	// 生成world和db对象
	LPCTSTR szWorldIP = pVar->GetString(_T("loong_world_ip"), szWorld);
	LPCTSTR szDBIP = pVar->GetString(_T("loong_db_ip"), szWorld);

	DWORD dwLoongWorldID = g_SectionMgr.CreateNewClientID();
	m_pWorldServer = new GameServerWorld(dwLoongWorldID, IUTIL->UnicodeToUnicode8(szWorldIP));

	DWORD dwLoongDBID = g_SectionMgr.CreateNewClientID();
	m_pDBServer = new GameServerDB(dwLoongDBID, IUTIL->UnicodeToUnicode8(szDBIP));

	return TRUE;
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
VOID GameWorld::Update()
{
	m_pWorldServer->Update();
	m_pDBServer->Update();
}

//------------------------------------------------------------------------------
// 生成该World的静态信息
//------------------------------------------------------------------------------
VOID GameWorld::GenerateWorldStaticInfo(LPBYTE &pBuff, DWORD& dwSize)
{
	// 静态信息包括：world的名字，loongworld的ip，loongdb的ip
	LPBYTE pOriBuff = pBuff;

	// 首先载入world的名字
	memcpy(pBuff, m_szWorldName, X_SHORT_NAME*sizeof(TCHAR));
	pBuff += X_SHORT_NAME*sizeof(TCHAR);

	// 再载入world的ID
	*(DWORD*)pBuff = m_dwWorldID;
	pBuff += sizeof(DWORD);

	// 再载入loongworld的ip
	strncpy((LPSTR)pBuff, GetWorld()->GetIP(), X_SHORT_NAME);
	pBuff += X_SHORT_NAME*sizeof(CHAR);

	// 再载入loongdb的ip
	strncpy((LPSTR)pBuff, GetDB()->GetIP(), X_SHORT_NAME);
	pBuff += X_SHORT_NAME*sizeof(CHAR);

	// 增加size
	dwSize += pBuff - pOriBuff;
}

//-------------------------------------------------------------------------------
// 生成该world的动态信息
//-------------------------------------------------------------------------------
VOID GameWorld::GenerateWorldDynamicInfo(LPBYTE &pBuff, DWORD& dwSize)
{
	// 动态信息包括：world的id，loongworld的动态信息，loongdb的动态信息
	LPBYTE pOriBuff = pBuff;

	// 加载ID
	*(DWORD*)pBuff = m_dwWorldID;
	pBuff += sizeof(DWORD);

	// 加载loongworld的动态信息
	tagServerInfo* pInfoWorld = (tagServerInfo*)pBuff;
	GetWorld()->FillServerInfo(pInfoWorld);
	pBuff += sizeof(tagServerInfo);

	// 加载loongdb的动态信息
	tagServerInfo* pInfoDB = (tagServerInfo*)pBuff;
	GetDB()->FillServerInfo(pInfoDB);
	pBuff += sizeof(tagServerInfo);

	// 累积size
	dwSize += pBuff - pOriBuff;
}

BOOL GameWorld::InitDB( LPCTSTR szWorld )
{
	TObjRef<VarContainer> pVar = "SectionContainer";
	LPCTSTR szIp	= pVar->GetString(_T("db_host"), szWorld);
	LPCTSTR szDBName= pVar->GetString(_T("db_name"), szWorld);
	LPCTSTR szUser	= pVar->GetString(_T("db_user"), szWorld);
	LPCTSTR szPwd	= pVar->GetString(_T("db_psd"), szWorld);
	INT		nPort	= pVar->GetInt(_T("db_port"), szWorld);
	m_pDB = new ServerDB;
	BOOL bRtv = m_pDB->Init(szIp, szUser, szPwd, szDBName, nPort);
	if (!bRtv)
	{
		SAFE_DEL(m_pDB);
	}
	return bRtv;
}

VOID GameWorld::SendWorldMsg( tagNetCmd* pCmd )
{
	g_SectionMgr.SendMsg(m_pWorldServer->GetID(), pCmd, pCmd->dwSize);
}