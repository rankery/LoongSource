//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: guild_CofC.h
// author: Sunnee
// actor:
// data: 2009-07-03
// last:
// brief: 商会
//-----------------------------------------------------------------------------
#pragma once

struct tagCofCSellGood
{
	DWORD		dwGoodID;
	INT32		nCost;
	BYTE		byRemainNum;
};

struct tagCofCBuyGood
{
	DWORD		dwGoodID;
	INT32		nCost;
};

class GuildCofC
{
public:
	GuildCofC();
	~GuildCofC();

public:
	static GuildCofC* Create(DWORD dwNPCID, DWORD dwCofCID);
	static VOID	 Delete(GuildCofC *&pCofC) { SAFE_DEL(pCofC); }

public:
	BOOL	Init(DWORD dwNPCID, DWORD dwCofCID);
	VOID	Update();
	VOID	Destory();

	// 定时刷新商货
	VOID	RefreshSellGoods();
	VOID	RefreshBuyGoods();

	// 取得商店中所有物品信息
	BYTE	GetGoodsNum()	{ return m_mapGoodSell.Size(); }
	VOID	GetGoodsInfo(DWORD dwRoleID, OUT tagCommerceGoodInfo* pGoods, INT& byNum);

	// 商店卖出商货
	DWORD	SellItem(INT32 nTael, DWORD dwGoodID, BYTE byBuyNum, OUT INT32 &nCostTael);
	// 获取指定商货在该商店的收购价
	INT32	GetGoodPrice(DWORD dwGoodID);

	// 移出观察者列表
	VOID	RemoveObserver(DWORD dwRoleID)	{ m_listObserver.Erase(dwRoleID); }

	// 更新商会以及跑商容器中商货
	DWORD	SendCommerceGoodsInfo2Role(Role* pRole, DWORD dwGoodID = GT_INVALID);

	// 取得商会属性
	DWORD	GetCofCID()		{ return m_dwCofCID; }
	BYTE	GetHolderCity()	{ return m_byHolderCity; }

private:
	BOOL	IsInitOK() const { return m_bInitOK; }
	VOID	CalRefreshPrice(INT32& nCurPrice, INT32 nLowPrice, INT32 nHighPrice);
	VOID	SendMsg2Observer(LPVOID pMsg, DWORD dwSize);

private:
	typedef TMap<DWORD, tagCofCSellGood*>	MapSellGood;
	typedef TMap<DWORD, tagCofCBuyGood*>	MapBuyGood;
	typedef TList<DWORD>					ListObserver;

	BOOL			m_bInitOK;

	DWORD			m_dwNPCID;
	DWORD			m_dwCofCID;
	BYTE			m_byHolderCity;

	MapSellGood		m_mapGoodSell;
	MapBuyGood		m_mapGoodBuy;
	ListObserver	m_listObserver;

	tagDWORDTime	m_dwSyncTime;
};