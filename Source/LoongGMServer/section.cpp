//-----------------------------------------------------------------------------
//!\file section.cpp
//!\brief 大区
//!
//!\date 2009-04-20
//! last 2009-04-20
//!\author zhangrong
//!
//! Copyright (c) 1985-2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "server.h"
#include "section.h"
#include "game_world.h"
#include "game_server.h"
#include "section_mgr.h"
#include "../ServerDefine/msg_rt_s.h"

//-----------------------------------------------------------------------------
//    
//-----------------------------------------------------------------------------
Section::Section(DWORD dwSectionID, LPCTSTR szSectionName) : m_dwSectionID(dwSectionID), m_pLoginServer(NULL)
{
	memcpy(m_szSectionName, szSectionName, sizeof(m_szSectionName));
}

//-----------------------------------------------------------------
//    
//-----------------------------------------------------------------
BOOL Section::Init()
{
	// 获得路径
	TCHAR tszFilePath[X_LONG_STRING] = {0};
	GetModuleFileName(NULL, tszFilePath, X_LONG_STRING);
	int nCount = _tcslen(tszFilePath);
	while( tszFilePath[nCount-1] != '\\' )
	{
		nCount--;
	}
	tszFilePath[nCount] = '\0';

	// 生成大区配置文件名
	TCHAR szSectionCfg[MAX_PATH] = {_T('\0')};
	_stprintf(szSectionCfg, _T("%ssection_%s"), tszFilePath, m_szSectionName);
	TObjRef<Util> pUtil = "Util";
	TCHAR tszFile[MAX_PATH] = {0};
	if (!pUtil->GetIniPath(tszFile, szSectionCfg))
		return FALSE;

	// 读取大区配置文件
	CreateObj("SectionContainer", "VarContainer");
	TObjRef<VarContainer> pVar = "SectionContainer";
	pVar->Load("VirtualFileSys", tszFile);

	// 首先初始化login
	DWORD	dwLoginID	=	g_SectionMgr.CreateNewClientID();
	LPCTSTR	szIP		=	pVar->GetString(_T("ip"), _T("login"));
	m_pLoginServer = new GameServerLogin(dwLoginID, IUTIL->UnicodeToUnicode8(szIP));

	// 再生成所有的world
	INT nWorldNum = pVar->GetInt(_T("num"), _T("world"));

	for(INT n = 0; n < nWorldNum; ++n)
	{
		GameWorld* pWorld = new GameWorld;
		if( !pWorld->Init(n) )
		{
			SAFE_DEL(pWorld);
			return FALSE;
		}

		// 加入到列表中
		m_mapAllWorld.Add(pWorld->GetID(), pWorld);
	}

	// 进行Login的ID的大区注册
	g_SectionMgr.RegisterClientID(dwLoginID, m_dwSectionID);

	// 注册World
	GameWorld* pWorld = NULL;
	TMap<DWORD, GameWorld*>::TMapIterator it = m_mapAllWorld.Begin();
	while( m_mapAllWorld.PeekNext(it, pWorld) )
	{
		// 注册ID和游戏世界的对应
		m_mapIDWorldID.Add(pWorld->GetWorld()->GetID(), pWorld->GetID());
		m_mapIDWorldID.Add(pWorld->GetDB()->GetID(), pWorld->GetID());

		// 注册ID和大区的对应
		g_SectionMgr.RegisterClientID(pWorld->GetWorld()->GetID(), m_dwSectionID);
		g_SectionMgr.RegisterClientID(pWorld->GetDB()->GetID(), m_dwSectionID);
	}

	// 删除变量管理器
	KillObj("SectionContainer");

	return TRUE;
}

//-----------------------------------------------------------------
// 销毁
//-----------------------------------------------------------------
VOID Section::Destroy()
{
	// 先删除Login
	SAFE_DEL(m_pLoginServer);

	// 再删除管理的各个World
	GameWorld* pWorld = NULL;
	TMap<DWORD, GameWorld*>::TMapIterator it = m_mapAllWorld.Begin();
	while( m_mapAllWorld.PeekNext(it, pWorld) )
	{
		SAFE_DEL(pWorld);
	}
	m_mapAllWorld.Clear();
}

//-----------------------------------------------------------------
// 更新  
//-----------------------------------------------------------------
VOID Section::Update()
{
	// 先更新login
	m_pLoginServer->Update();

	// 再更新所有的world
	GameWorld* pWorld = NULL;
	TMap<DWORD, GameWorld*>::TMapIterator it = m_mapAllWorld.Begin();

	while( m_mapAllWorld.PeekNext(it, pWorld) )
	{
		pWorld->Update();
	}
}

//------------------------------------------------------------------
// 开启Login
//------------------------------------------------------------------
BOOL Section::StartLoginServer()
{
	if( m_pLoginServer->IsValid() ) return TRUE;
	return m_pLoginServer->StartServer();
}

//------------------------------------------------------------------
// 开启World
//------------------------------------------------------------------
BOOL Section::StartWorld(DWORD dwWorldID, EServerType eType)
{
	GameWorld* pWorld = GetWorld(dwWorldID);
	if( !P_VALID(pWorld) ) return FALSE;

	BOOL bRet = TRUE;
	switch (eType)
	{
	case EST_DB:
		if( !pWorld->GetDB()->IsValid() )
		{
			bRet = pWorld->GetDB()->StartServer();
		}
		break;

	case EST_World:
		if( !pWorld->GetWorld()->IsValid() )
		{
			bRet = pWorld->GetWorld()->StartServer();
		}
		break;
	}

	return bRet;
}

//------------------------------------------------------------------
// 关闭login
//------------------------------------------------------------------
BOOL Section::CloseLoginServer(BOOL bForce/* =FALSE */)
{
	if( !m_pLoginServer->IsValid() ) return TRUE;

	// 不是强制关闭，则给login本身发送消息
	if( !bForce )
	{
		// 通过g_sectionMgr发送消息
		tagNSS_Close send;
		g_SectionMgr.SendMsg(m_pLoginServer->GetID(), &send, sizeof(tagNSS_Close));
		return TRUE;
	}
	// 强制关闭，则通知本地守护进程杀掉进程
	else
	{
		return m_pLoginServer->ShutdownServer();
	}
}

//-------------------------------------------------------------------
// 关闭World
//-------------------------------------------------------------------
BOOL Section::CloseWorld(DWORD dwWorldID, EServerType eType, BOOL bForce/* =FALSE */)
{
	// 找到World
	GameWorld* pWorld = GetWorld(dwWorldID);
	if( !P_VALID(pWorld) ) return FALSE;

	// 不是强制关闭
	if( !bForce )
	{
		// 通过g_SectionMgr发送消息
		tagNSS_Close send;
		switch (eType)
		{
		case EST_DB:
			g_SectionMgr.SendMsg((pWorld->GetDB()->GetID()), &send, sizeof(tagNSS_Close));
			break;

		case EST_World:
			g_SectionMgr.SendMsg((pWorld->GetWorld()->GetID()), &send, sizeof(tagNSS_Close));
			break;
		}
		
		return TRUE;
	}
	else
	{
		BOOL bRet = TRUE;

		switch (eType)
		{
		case EST_DB:
			if( pWorld->GetDB()->IsValid() )
			{
				bRet = pWorld->GetDB()->ShutdownServer();
			}
			break;

		case EST_World:
			if( pWorld->GetWorld()->IsValid() )
			{
				bRet = pWorld->GetWorld()->ShutdownServer();
			}
			break;
		}

		return bRet;
	}
}

//-------------------------------------------------------------------
// 向loginserver发送系统指令
//-------------------------------------------------------------------
BOOL Section::SendSystemCmd2LoginServer(tagNetCmd* pMsg)
{
	g_SectionMgr.SendMsg(m_pLoginServer->GetID(), pMsg, sizeof(tagNLSS_SystemCmd));
	
	return TRUE;
}

//-------------------------------------------------------------------
// 向world发送系统指令
//-------------------------------------------------------------------
BOOL Section::SendSystemCmd2World(DWORD dwWorldID, EServerType eType, tagNetCmd* pMsg)
{
	// 找到World
	GameWorld* pWorld = GetWorld(dwWorldID);
	if( !P_VALID(pWorld) ) return FALSE;

	if (eType == EST_DB)
	{
		pWorld->GetDB()->ExecuteCmd(pMsg);
	}
	else if (eType == EST_World)
	{
		pWorld->GetWorld()->ExecuteCmd(pMsg);
	}
	else
		return FALSE;

	return TRUE;
}

BOOL Section::SetMaxNum(DWORD dwWorldID, LPVOID pMsg)
{
	// 找到World
	GameWorld* pWorld = GetWorld(dwWorldID);
	if( !P_VALID(pWorld) ) return FALSE;

	g_SectionMgr.SendMsg(pWorld->GetWorld()->GetID(), pMsg, sizeof(tagNSS_MaxNum));

	return TRUE;
}

BOOL Section::UpdateMall(DWORD dwWorldID, LPVOID pMsg)
{
	GameWorld* pWorld = GetWorld(dwWorldID);
	if( !P_VALID(pWorld) ) return FALSE;

	g_SectionMgr.SendMsg(pWorld->GetWorld()->GetID(), pMsg, sizeof(tagNSS_UpdateMall));

	return TRUE;
}

BOOL Section::AccountForbid(LPVOID pMsg, DWORD dwSize)
{
	GameServerLogin* pLoginServer = GetLoginServer();
	if( !P_VALID(pLoginServer) ) return FALSE;

	g_SectionMgr.SendMsg(pLoginServer->GetID(), pMsg, dwSize);

	return TRUE;
}

BOOL Section::IPBlacklist(LPVOID pMsg)
{
	GameServerLogin* pLoginServer = GetLoginServer();
	if( !P_VALID(pLoginServer) ) return FALSE;

	g_SectionMgr.SendMsg(pLoginServer->GetID(), pMsg, sizeof(tagNSS_ForbidIP));
	return TRUE;
}
//---------------------------------------------------------------------------
// 生成该大区的静态信息
//---------------------------------------------------------------------------
VOID Section::GenerateSectionStaticInfo(LPBYTE &pBuff, DWORD& dwSize)
{
	// 静态信息：大区名，login的ip，world数量，每个world的信息

	// 首先加载大区名
	memcpy(pBuff, m_szSectionName, X_SHORT_NAME*sizeof(TCHAR));
	pBuff	+=	X_SHORT_NAME*sizeof(TCHAR);
	dwSize	+=	X_SHORT_NAME*sizeof(TCHAR);

	// 加载大区的ID
	*(DWORD*)pBuff = m_dwSectionID;
	pBuff	+=	sizeof(DWORD);
	dwSize	+=	sizeof(DWORD);

	// 再加载login的ip
	strncpy((LPSTR)pBuff, m_pLoginServer->GetIP(), X_SHORT_NAME);
	pBuff	+=	X_SHORT_NAME*sizeof(CHAR);
	dwSize	+=	X_SHORT_NAME*sizeof(CHAR);

	// world的数量
	*(DWORD*)pBuff = GetWorldNum();
	pBuff	+=	sizeof(DWORD);
	dwSize	+=	sizeof(DWORD);

	// 加载各个world
	GameWorld* pWorld = NULL;
	TMap<DWORD, GameWorld*>::TMapIterator it = m_mapAllWorld.Begin();

	while( m_mapAllWorld.PeekNext(it, pWorld) )
	{
		pWorld->GenerateWorldStaticInfo(pBuff, dwSize);
	}
}

//---------------------------------------------------------------------------
// 生成该大区的动态信息
//---------------------------------------------------------------------------
VOID Section::GenerateSectionDynamicInfo(LPBYTE &pBuff, DWORD& dwSize)
{
	// 动态信息：大区id，login的状态，world数量，每个world的信息

	// 首先加载大区id
	*(DWORD*)pBuff = m_dwSectionID;
	pBuff	+=	sizeof(DWORD);
	dwSize	+=	sizeof(DWORD);

	// 再加载login的状态
	tagServerInfo* pInfoLogin = (tagServerInfo*)pBuff;
	m_pLoginServer->FillServerInfo(pInfoLogin);
	pBuff	+=	sizeof(tagServerInfo);
	dwSize	+=	sizeof(tagServerInfo);

	// world的数量
	*(DWORD*)pBuff = GetWorldNum();
	pBuff	+=	sizeof(DWORD);
	dwSize	+=	sizeof(DWORD);

	// 加载各个world
	GameWorld* pWorld = NULL;
	TMap<DWORD, GameWorld*>::TMapIterator it = m_mapAllWorld.Begin();

	while( m_mapAllWorld.PeekNext(it, pWorld) )
	{
		pWorld->GenerateWorldDynamicInfo(pBuff, dwSize);
	}
}

//---------------------------------------------------------------------------
// 得到LoongWorld
//---------------------------------------------------------------------------
GameServerWorld* Section::GetWorldServer(DWORD dwWorldID)
{
	GameWorld* pWorld = GetWorld(dwWorldID);

	if( !P_VALID(pWorld) ) return NULL;

	return pWorld->GetWorld();
}

//----------------------------------------------------------------------------
// 得到LoongDB
//----------------------------------------------------------------------------
GameServerDB* Section::GetDBServer(DWORD dwWorldID)
{
	GameWorld* pWorld = GetWorld(dwWorldID);

	if( !P_VALID(pWorld) ) return NULL;

	return pWorld->GetDB();
}

//----------------------------------------------------------------------------
// 查找Login
//----------------------------------------------------------------------------
GameServerLogin* Section::FindLoginServer(DWORD dwID)
{
	if( m_pLoginServer->GetID() != dwID )
	{
		return NULL;
	}

	return m_pLoginServer;
}

//-----------------------------------------------------------------------------
// 查找World
//-----------------------------------------------------------------------------
GameServerWorld* Section::FindWorldServer(DWORD dwID)
{
	// 先查看这个ID是属于哪个游戏世界的
	DWORD dwWorldID = m_mapIDWorldID.Peek(dwID);
	if( !P_VALID(dwWorldID) ) return NULL;

	// 找到world
	GameWorld* pWorld = GetWorld(dwWorldID);
	if( !P_VALID(pWorld) ) return NULL;

	// 查找是不是LoongWorld的ID
	if( pWorld->GetWorld()->GetID() != dwID )
	{
		return NULL;
	}

	return pWorld->GetWorld();
}

//------------------------------------------------------------------------------
// 查找DB
//------------------------------------------------------------------------------
GameServerDB* Section::FindDBServer(DWORD dwID)
{
	// 先查看这个ID是属于哪个游戏世界的
	DWORD dwWorldID = m_mapIDWorldID.Peek(dwID);
	if( !P_VALID(dwWorldID) ) return NULL;

	// 找到world
	GameWorld* pWorld = GetWorld(dwWorldID);
	if( !P_VALID(pWorld) ) return NULL;

	// 查找是不是LoongDB的ID
	if( pWorld->GetDB()->GetID() != dwID )
	{
		return NULL;
	}

	// 返回LoongDB指针
	return pWorld->GetDB();
}

GameWorld* Section::FindGameWorld(DWORD dwID)
{
	DWORD dwWorldID = m_mapIDWorldID.Peek(dwID);
	if (!GT_VALID(dwWorldID))
	{
		return NULL;
	}
	return m_mapAllWorld.Peek(dwWorldID);
}

//-------------------------------------------------------------------------------
// 根据ID查找Server
//-------------------------------------------------------------------------------
GameServer* Section::FindServer(DWORD dwID)
{
	// 首先查找是不是Login
	if( m_pLoginServer->GetID() == dwID )
		return m_pLoginServer;

	// 不是login，则查找时哪个游戏世界的
	DWORD dwWorldID = m_mapIDWorldID.Peek(dwID);
	if( !P_VALID(dwWorldID) ) return NULL;

	// 找到world
	GameWorld* pWorld = GetWorld(dwWorldID);
	if( !P_VALID(pWorld) ) return NULL;

	// 查看是不是LoongWorld的ID
	if( pWorld->GetWorld()->GetID() == dwID )
	{
		return pWorld->GetWorld();
	}
	// 查看是不是LoongDB的ID
	else if( pWorld->GetDB()->GetID() == dwID )
	{
		return pWorld->GetDB();
	}
	// 都没有查找到，返回NULL
	else
	{
		return NULL;
	}
}

BOOL Section::ForAllGameWorld( DWORD* pWorldIDs, INT nNum, FUN_GAMEWORLD pFun,  PVOID pPara)
{
	BOOL bRtv = TRUE;
	if (!GT_VALID(*pWorldIDs))
	{
		GameWorld* pWorld = NULL;
		TMap<DWORD, GameWorld*>::TMapIterator itr = m_mapAllWorld.Begin();
		while( m_mapAllWorld.PeekNext(itr, pWorld) )
		{
			if (P_VALID(pWorld))
			{
				(pWorld->*pFun)((DWORD)pPara);
			}
			else
			{
				bRtv = FALSE;
			}
		}
	}
	else
	{
		GameWorld* pWorld = NULL;
		for (INT i=0; i<nNum; ++i)
		{
			pWorld = m_mapAllWorld.Peek(pWorldIDs[i]);
			if (P_VALID(pWorld))
			{
				(pWorld->*pFun)((DWORD)pPara);
			}
			else
			{
				bRtv = FALSE;
			}
		}
	}
	return bRtv;
}