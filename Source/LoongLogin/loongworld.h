//-----------------------------------------------------------------------------
//!\file loongworld.h
//!\author Aslan
//!
//!\date 2008-11-30
//! last 2008-11-30
//!
//!\brief 管理某个单独的游戏世界
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "..\ServerDefine\login_issue.h"

#include "Mutex.h"
#include "loongworld_mgr.h"
#include "player_mgr.h"
#include "player.h"

class Player;

class LoongWorld
{

public:
	//--------------------------------------------------------------------------
	// CONSTRUCT
	//--------------------------------------------------------------------------
    LoongWorld();
    ~LoongWorld();

    //--------------------------------------------------------------------------
	// 初始化，更新及销毁
	//--------------------------------------------------------------------------
	BOOL			Init(INT nIndex);
	VOID			Update();
	VOID			Destroy();

	//--------------------------------------------------------------------------
	// 开启和关闭
	//--------------------------------------------------------------------------
	BOOL			WorldLogin(DWORD dwIP, INT nPort, DWORD* pOLAccountIDs, INT nOLAccountIDNum);
	BOOL			WorldLogout();

	//--------------------------------------------------------------------------
	// 各种Get
	//--------------------------------------------------------------------------
	DWORD			GetID()				{ return m_Data.dwNameCrc; }
	//temp add by songg
	DWORD			GetWorldID()		{ return m_Data.dwWorldID; }
	EWorldStatus	GetStatus()			{ return m_Data.eStatus; }
	INT				GetMaxOnlineNum()	{ return m_Data.nMaxOnlineNum; }
	INT				GetCurrOnlineNum()	{ return m_Data.nCurOnlineNum; }
	INT				GetCurrOnlineDisplay()	{ return m_Data.nCurOnlineDisplay; }
	LPCTSTR			GetName()			{ return m_Data.szName; }
	INT				GetListQueue()		{ return m_listQueue.Size(); }
	DWORD			GetDBUpdateTime()	{ return m_dwDBUpdateTime;	}
	DWORD			GetDBInsertTime()	{ return m_dwDBInsertTime;	}
	//--------------------------------------------------------------------------
	// 各种Set
	//--------------------------------------------------------------------------
	VOID			SetDBUpdateTime(DWORD dwTime)	{ m_dwDBUpdateTime = dwTime;}
	VOID			SetDBInsertTime(DWORD dwTime)	{ m_dwDBInsertTime = dwTime;}

	BOOL			IsValid()			{ return m_Data.bValid; }
	BOOL			IsAutoSeal()		{ return m_Data.bAutoSeal; }

	//--------------------------------------------------------------------------
	// 更新状态
	//--------------------------------------------------------------------------
	VOID			UpdateStatus(EWorldStatus eStatus, INT nCurOnline, INT nMaxOnline, INT nCurOnlineDisplay=-1);

	//--------------------------------------------------------------------------
	// 辅助函数
	//--------------------------------------------------------------------------
	VOID			KickPlayer(Player* pPlayer);
	VOID			AddPlayer(Player* pPlayer);

	//----------------------------------------------------------------------------
	// 玩家请求登入结果
	//----------------------------------------------------------------------------
	VOID			PlayerWillLoginRet(DWORD dwAccountID, DWORD dwErrorCode);

	//----------------------------------------------------------------------------
	// 向LoongWorld发送踢人消息
	//----------------------------------------------------------------------------
	VOID			SendKickPlayerMsg(tagNLW_KickPlayer& send) { sLoongWorldMgr.SendMsg(m_Data.dwNameCrc, &send, send.dwSize); }

private:
	//----------------------------------------------------------------------------
	// 更新所有连接，排队列表和等待列表
	//----------------------------------------------------------------------------
	VOID			UpdateSession();
	VOID			UpdateInsertPlayer();
	VOID			UpdateQueuedPlayer();
	VOID			UpdateWaitingPlayer();
	VOID			UpdateKickedPlayer();

	//----------------------------------------------------------------------------
	// 加入各个列表
	//----------------------------------------------------------------------------
	VOID			AddIntoQueue(Player* pPlayer);
	VOID			AddIntoWaitingMap(Player* pPlayer);

private:
    TObjRef<Util>					m_pUtil;
	
	//---------------------------------------------------------------------------------
	// 游戏世界状态
	//---------------------------------------------------------------------------------
	tagLoongWorldData				m_Data;
	DWORD							m_dwTime;					// 用于定时更新排队列表

	DWORD							m_dwDBUpdateTime;			// 世界状态更新时间
	DWORD							m_dwDBInsertTime;			// 世界状态插入时间
	//----------------------------------------------------------------------------------
	// 玩家列表
	//----------------------------------------------------------------------------------
	TList<Player*>					m_listInsertPlayer;			// 插入列表
	TList<Player*>					m_listQueue;				// 排队列表
	TMap<DWORD, Player*>			m_mapWaitting;				// 等待进入游戏世界的玩家列表
	TList<Player*>					m_listKickPlayer;			// 由于种种原因被踢掉玩家

};

//----------------------------------------------------------------------------------------
// 踢掉该游戏世界中的一个玩家
//----------------------------------------------------------------------------------------
inline VOID LoongWorld::KickPlayer(Player* pPlayer)
{
	if( !P_VALID(pPlayer) ) return;

	sPlayerMgr.Kick(pPlayer->GetCDIndex());
	m_listKickPlayer.PushBack(pPlayer);
}

//----------------------------------------------------------------------------------------
// 将玩家加入该游戏世界列表
//----------------------------------------------------------------------------------------
inline VOID LoongWorld::AddPlayer(Player* pPlayer)
{
	if( P_VALID(pPlayer) )
	{
		m_listInsertPlayer.PushFront(pPlayer);
	}
}

//-----------------------------------------------------------------------------------------
// 更新状态
//-----------------------------------------------------------------------------------------
inline VOID LoongWorld::UpdateStatus(EWorldStatus eStatus, INT nCurOnline, INT nMaxOnline, INT nCurOnlineDisplay/*=-1*/)
{
	m_Data.eStatus			=	eStatus;
	m_Data.nMaxOnlineNum	=	nMaxOnline;
	m_Data.nCurOnlineNum	=	nCurOnline;
	m_Data.nCurOnlineDisplay=	nCurOnlineDisplay;
}




