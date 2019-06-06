//-----------------------------------------------------------------------------
//!\file world_db.h
//!\author Aslan
//!
//!\date 2009-06-09
//! last 2009-06-09
//!
//!\brief 游戏世界数据库
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once
#include "../ServerDefine/bill_define.h"

class World;

//-----------------------------------------------------------------------------
// 游戏世界数据库
//-----------------------------------------------------------------------------
class WorldDB
{
public:
	//-------------------------------------------------------------------------
	// CONSTRUCT
	//-------------------------------------------------------------------------
	WorldDB(World* pWorldID) :
	  m_n64ItemSerial(GT_INVALID), m_pWorld(pWorldID){}
	~WorldDB() {}

	//-------------------------------------------------------------------------
	// 初始化和销毁
	//-------------------------------------------------------------------------
	BOOL	Init(INT nIndex);
	VOID	Update();
	VOID	Destroy();

	//-------------------------------------------------------------------------
	// 是否断开连接
	//-------------------------------------------------------------------------
	BOOL	IsConLost()				{ return m_DB.IsConnLost(); }

	//--------------------------------------------------------------------------
	// 划拨函数
	//--------------------------------------------------------------------------
	BOOL	GetCentralbillingInfo(INT& nNum);

	BOOL	GiveYuanbaoToAccount(DWORD dwAccountID, INT nNum, Connection* pCon);
	BOOL	GiveItemToAccountBaibao(DWORD dwAccountID, DWORD dwItemTypeID, INT nNum, Connection* pCon, OUT INT64& n64SerialID);

	BOOL	MoveBillInfo2Log(DWORD dwTokenID, Connection* pCon);

	BOOL	SetCentralbillingState(DWORD dwTokenID, INT nState, Connection* pCon);

	//添加元宝划拨log
	BOOL	InsertBillYuanbaoLog(const LPCSTR szTokenID,DWORD dwAccountID, INT nNum );
	BOOL	InsertBillItemLog(const LPCSTR szTokenID,DWORD dwAccountID, DWORD dwItemTypeID,INT nNum );
	//--------------------------------------------------------------------------
	// 辅助函数
	//--------------------------------------------------------------------------
	DWORD	GetAccountID(const LPCSTR szAccountName);

	BOOL	LockTables(LPCSTR szTableNames);
	BOOL	UnlockTables();
	//--------------------------------------------------------------------------
	// 账号控制函数
	//--------------------------------------------------------------------------
	BOOL	InsertAccout(DWORD dwAccountID, LPCSTR szAccountName);

	//--------------------------------------------------------------------------
	// 物品ID相关
	//--------------------------------------------------------------------------
	BOOL	GetCurMinItemSerialInDB(INT64& n64ItemSerial);
	BOOL	GetMinItemSerialInTable(LPCSTR szTable, INT64& n64ItemSerial);
	INT64	GenerateNew64ItemSerial()	{ return --m_n64ItemSerial; }


	//得到玩家现在的元宝数
	DWORD	GetCurAccountYuanbao(DWORD dwAccountID);
private:
	//--------------------------------------------------------------------------
	// 重连
	//--------------------------------------------------------------------------
	VOID	Reconnect();
	
private:
	DataBase				m_DB;				// Beton数据库
	World*					m_pWorld;

	INT64					m_n64ItemSerial;	// 物品64位序列号

	tagBillInfo				m_sBillInfo[BILLINFO_PERTICK];
};