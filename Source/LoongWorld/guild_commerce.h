//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: guild_commerce.h
// author: Sunnee
// actor:
// data: 2009-07-03
// last:
// brief: 帮会跑商管理
//-----------------------------------------------------------------------------
#pragma once

class Guild;
class GuildCommodity;

struct tagGuildCommerceInfo;

class RankCompare
{
public:
	bool operator()(const tagCommerceRank* lhs, const tagCommerceRank* rhs)
	{
		return lhs->nTael > rhs->nTael;
	}
};

class GuildCommerce
{
public:
	GuildCommerce();
	~GuildCommerce();

	BOOL	Init(Guild* pGuild, BOOL bRequest = FALSE);
	VOID	Destory();

	BOOL	IsInitOK()		{ return m_bInitOK; }

	// 载入跑商容器信息
	DWORD	LoadCommerceInfo(tagGuildCommerceInfo* pInfo, INT nInfoNum);

	// 载入跑商排名信息(与初始化成功与否无关)
	DWORD	LoadCommerceRankInfo(tagCommerceRank* pInfo, INT nInfoNum);

	// 接取跑商任务
	DWORD	AcceptCommerce(Role* pRole);

	// 上缴商银
	DWORD	CompleteCommerce(Role* pRole, INT32& nFund);

	// 放弃跑商
	DWORD	AbandonCommerce(DWORD dwRoleID, BOOL bClearRank = FALSE);

	// 获取商货价值
	DWORD	GetCommodityGoodInfo(Role* pRole, tagCommerceGoodInfo* pGoodInfo, INT& nGoodNum, INT32& nTael, INT& nLevel);

	// 取得跑商初始信息
	DWORD	GetCommerceBeginningInfo(DWORD dwRoleID, INT& nLevel, tagTaelInfo& sTaelInfo);

	// 购买商货
	DWORD	BuyGoods(Role* pRole, DWORD dwNPCID, DWORD dwGoodID, BYTE byBuyNum);

	// 出售商货
	DWORD	SellGoods(Role* pRole, DWORD dwNPCID, DWORD dwGoodID, BYTE bySellNum);

	// 获取排行榜和嘉奖状态
	DWORD	GetCommerceRankInfo(tagCommerceRank* pRankInfo, INT& nRankNum, BOOL& bCommend);

	// 开启/关闭跑商嘉奖
	DWORD	SwitchCommendation(DWORD dwRoleID, BOOL bSwitchON);

	// 发放跑商奖励(同时清空Rank记录)
	VOID	ProvideBonus();

public:
	GuildCommodity*	GetCommodity(DWORD dwRoleID)	{ return m_mapCommodity.Peek(dwRoleID); }
	VOID	Add2CommerceRank(DWORD dwRoleID, INT32 nTael, INT nTimes = GT_INVALID, BOOL bSave2DB = TRUE);
	VOID	RemoveCommerceRank(DWORD dwRoleID);

private:
	typedef TMap<DWORD, GuildCommodity*>		MapCommodity;
	typedef std::vector<tagCommerceRank*>		VectorRank;

	BOOL				m_bInitOK;
	Guild*				m_pGuild;

	BOOL				m_bCommend;

	MapCommodity		m_mapCommodity;
	VectorRank			m_vecRank;
};