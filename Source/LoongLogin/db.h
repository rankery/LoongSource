//-----------------------------------------------------------------------------
//!\file db.h
//!\author Aslan
//!
//!\date 2008-11-30
//! last 2008-11-30
//!
//!\brief 数据库类
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Singleton.h"
#include "../ServerDefine/login_define.h"

class Player;
struct tagFatigueInfo;

//-----------------------------------------------------------------------------
// 数据库类
//-----------------------------------------------------------------------------
class DB : public Singleton<DB>
{
public:
    //-------------------------------------------------------------------------
	// CONSTURCT
	//-------------------------------------------------------------------------
	DB() { }
	~DB() { Destroy(); }

    //-------------------------------------------------------------------------
	// 初始化和销毁
	//-------------------------------------------------------------------------
    BOOL	Init();
    VOID	Destroy();
    
	//-------------------------------------------------------------------------
	// 各种数据库操作函数
	//-------------------------------------------------------------------------
	VOID	PlayerLogin(DWORD dwAccountID, DWORD dwWorldNameCrc);
	VOID	PlayerLogout(DWORD dwAccountID);
	VOID	PlayerEnterWorld(DWORD dwAccountID, DWORD dwWorldNameCrc);
	VOID	PlayerOutWorld(DWORD dwAccountID, DWORD dwWorldNameCrc);
	VOID	FixPlayerLoginStatus(DWORD dwAccountID, EPlayerLoginStatus eDest);
	VOID	FixOneWorldPlayerLoginStatus( DWORD dwWorldNameCrc, EPlayerLoginStatus eSrc, EPlayerLoginStatus eDest );
	VOID	ResetAllPlayerLoginStatus(EPlayerLoginStatus eSrc, EPlayerLoginStatus eDest);
	VOID	ResetPlayersLoginStatus(DWORD* pdwAccountIDs, INT nNum, EPlayerLoginStatus eDest);

	BOOL	GetPlayerMibao(DWORD dwAccountID, CHAR szMibao[MIBAO_LEN]);


	BOOL	LoadAllFatigueInfo(TMap<DWORD, tagFatigueInfo*> &mapFatigueInfo);
//	VOID	UpdateFatigueTimeTable();
//	BOOL	LoadLoginStatus(TMap<DWORD, tagFatigueInfo*> &mapFatigueInfo);
//	VOID	SaveFatigueInfo(tagFatigueInfo* pToSave);
	BOOL	LoadCacheAccountData(TSafeMap<DWORD, tagAccountData*> &mapAccountData, TSafeMap<DWORD, DWORD> &mapNameCrc2AccountID);
	BOOL	LoadOnLineGuardAccountIDs(TMap<DWORD, DWORD> &mapAccounts);

	VOID	SaveFatigueInfo(tagFatigueInfo* pFatigueInfo);
	BOOL	LoadFatigueInfo(DWORD dwAccountNameCrc, tagFatigueInfo* pFatigueInfo);
	BOOL	IfWorldForbid(DWORD dwAccountID,DWORD dwWorldNameCrc);
	BOOL	IfHaveWorldForbid(DWORD dwAccountID);
	BOOL	IfIPForbid(DWORD dwIP);

	DWORD	ForbidAccount(DWORD dwAccountID, DWORD dwForbidM, DWORD dwWorldNameCrc = 0);
	DWORD	ForbidAccount(LPCTSTR szAccountName, DWORD dwForbidM, DWORD &dwAccountID, DWORD dwWorldNameCrc = 0);

	DWORD	RemoveAccountForbid(DWORD dwAccountID,DWORD dwForbidM,DWORD dwWorldNameCrc = 0);
	DWORD	RemoveAccountForbid(LPCTSTR szAccountName,DWORD dwForbidM,DWORD dwWorldNameCrc = 0);

	DWORD	SetAccountChenMi(DWORD dwAccountID,DWORD dwChenMi,DWORD dwWorldNameCrc = 0);

	DWORD	ForbidIP(DWORD dwIP);
	DWORD	RemoveIPForbid(DWORD dwIP);

//	tstring GetAccountName(DWORD dwAccountID);
	VOID	UpdateAccountLoginInfo( DWORD dwAccountID, DWORD &dwIP);
	VOID	LogPlayerAction( DWORD dwAccountID, LPCSTR szAccountName, DWORD dwIP, LPCSTR szAction );
	VOID	InsertKickLog(const CHAR* pAccountName, DWORD dwAccountID, DWORD dwTime, UINT16 u16ErrCode, BOOL bSeal);

	BOOL    UpdateWorldState(DWORD dwWorldID,INT iRoleNum, SHORT sState);
	BOOL    InsertWorldState(DWORD dwWorldID);
	BOOL    ClearWorldStateTable();//清空世界状态表

	BOOL	InsertWorldStateLog(DWORD dwWorldID,INT iRoleNum, SHORT sState);
	//-------------------------------------------------------------------------
	// 辅助函数
	//-------------------------------------------------------------------------
	INT		GetAvailableStreamNum()			{ return m_DB.GetAvailableStreamNum(); }
	INT		GetAllStreamNum()				{ return m_DB.GetAllStreamNum(); }
	INT		GetUnhandledAsynStreamNum()		{ return m_DB.GetUnhandledAsynStreamNum(); }
	DWORD   GetWorldStateUpdateTime()		{ return m_dwWorldStateUpdateTime;}
	DWORD   GetWorldStateInsertTime()		{ return m_dwWorldStateInsertTime;}

private:
	DataBase		m_DB;					// Beton数据库

	DWORD			m_dwWorldStateUpdateTime;		// 世界状态更新时间（毫秒）

	DWORD			m_dwWorldStateInsertTime;		// 世界状态更新时间（毫秒）

 //   //从数据库中查询玩家数据
 //   BOOL QueryPlayerData(Player* pPlayer);
	////从数据库中查询玩家帐号名称
	
 //   //更新数据库中玩家数据
 //   BOOL UpdatePlayerData(Player* pPlayer);
 //   //将Log插入数据库
	
 //   //当玩家成功登录游戏世界，更新数据库玩家数据
	//BOOL UpdatePlayerIntoWorld(EPlayerLoginStatus status, DWORD dwLoginTime, DWORD dwAccULSec, DWORD dwAccOLSec, DWORD dwAccountID);
 //   //当玩家离开游戏世界，更新数据库玩家数据
 //   BOOL UpdatePlayerOutWorld(EPlayerLoginStatus status, DWORD dwLogoutTime, DWORD dwOnlineTime, DWORD dwAccOLSec, DWORD dwAccountID);

	////当LoongWorld当掉,清空该服务器上登录的玩家
	//VOID UpdatePlayer_LoongWorld(EPlayerLoginStatus status, DWORD dwWorldNameCrc, DWORD dwLogoutTime);
};

//Beton回调函数
VOID DBCallBack(DataBase* pDB, INT nReason, INT nParam);

#define sBeton DB::getSingleton()







