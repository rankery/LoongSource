//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: guild_commodity.h
// author: Sunnee
// actor:
// data: 2009-07-03
// last:
// brief: 跑商任务用容器
//-----------------------------------------------------------------------------
#pragma once

#include "../ServerDefine/guild_define.h"

class Role;

class GuildCommodity
{
public:
	GuildCommodity();
	~GuildCommodity();

	BOOL	Init(DWORD dwRoleID, INT nLevel, const tagTaelInfo* pTaelInfo = NULL, const tagRedoundInfo* pRedoundInfo = NULL);
	VOID	Destory();

	// 从数据库中载入跑商信息
	DWORD	LoadCommodityInfo(tagGuildCommerceInfo* pLoadInfo);

	// 获取容器中商货信息(包括商银)
	DWORD	GetCommodityInfo(tagCommerceGoodInfo* pGoods, INT& nGoodNum, INT32& nTael, INT32& nLevel);
	
	// 取得跑商容器信息
	const tagTaelInfo* GetTaelInfo()		{ return &m_sTaelInfo; }
	const tagRedoundInfo* GetRedoundInfo()	{ return &m_sRedoundInfo; }
	INT   GetLevel()						{ return m_nLevel; }
	INT32 GetTael()							{ return m_nCurTael; }
	INT	  GetGoodsNum()						{ return m_mapGoods.Size(); }
	INT   GetTaelProgress()					{ return (INT)(((FLOAT)m_nCurTael / (FLOAT)m_sTaelInfo.nPurposeTael) * 100.0f); }
	INT32 GetGain();

	DWORD IsFull(DWORD dwGoodID, BYTE byNum);
	DWORD IsExist(DWORD dwGoodID, BYTE byNum);

	// 当前商银操作(只会与商会交互,暂时不记录Log)
	BOOL	IncTael(INT32 nTael);
	BOOL	DecTael(INT32 nTael);

	// 容器物品操作
	BOOL	AddGood(DWORD dwGoodID, BYTE byNum, INT32 nPrice = 0);
	BOOL	RemoveGood(DWORD dwGoodID, BYTE byNum, INT32 nPrice = 0);

	// 死亡惩罚
	VOID	DeadPenalty();

private:
	// 数据库操作
	VOID	SaveTael2DB();
	VOID	SaveCommodity2DB();

private:
	typedef TMap<DWORD, tagCommerceGoodInfo*> MapGoods;

	DWORD					m_dwOwnerID;			// 所有者
	INT						m_nLevel;				// 接取跑商任务时的等级
	tagTaelInfo				m_sTaelInfo;			// 跑商任务初始信息
	tagRedoundInfo			m_sRedoundInfo;			// 奖励信息

	INT32					m_nCurTael;				// 容器中商银数量
	MapGoods				m_mapGoods;				// 商货表
};