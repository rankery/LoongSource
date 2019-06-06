//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: TradeYuanBao.h
// author: wjiang
// actor:
// data: 2009-3-3
// last:
// brief: 元宝交易类
//-------------------------------------------------------------------------------------------------------
#pragma once

#include "../WorldDefine/mall_define.h"
#include "../WorldDefine/currency_define.h"
#include "currency.h"
#include <set>

//--------------------------------------------------------------------------------------------
// 玩家元宝交易的交易账户
//--------------------------------------------------------------------------------------------
struct tagYuanBaoAccount
{
	DWORD				dwRoleID;				//玩家ID
	Currency<INT>		YuanBao;				//元宝数量
	Currency<INT64>		Silver;					//金钱数量
	BOOL				bSellOrder;				//是否有出售订单
	BOOL				bBuyOrder;				//是否有收购订单
	DWORD				dwQuestTick;			//查询订单时间（防止客户端频繁查询本周订单）
	tagYuanBaoAccount(DWORD dwID):YuanBao(0, MAX_YB_NUM), Silver(0, MAX_SILVER_NUM)
	{
		dwRoleID = dwID;
		bSellOrder = FALSE;
		bBuyOrder = FALSE;
		dwQuestTick = 0;
	}
	tagYuanBaoAccount(DWORD dwID, INT32 nYuanBao, INT32 nGold, INT32 nSilver, BOOL bSell, BOOL bBuy):YuanBao(nYuanBao, MAX_YB_NUM),
					Silver((MGold2Silver(nGold)+(INT64)nSilver), MAX_SILVER_NUM)
	{
		dwRoleID = dwID;
		bSellOrder = bSell;
		bBuyOrder = bBuy;
		dwQuestTick = 0;
	}

	// 设计客户端查询时间
	VOID			SetQuestTick(DWORD dwTick) { dwQuestTick = dwTick; }
	DWORD			GetQuestTick() { return dwQuestTick; }

	// 设置是否提交过订单
	VOID			SetSellOrder(BOOL bSubmit);
	VOID			SetBuyOrder(BOOL bSubmit);

	// 得到账户当前金钱数
	inline INT		GetAccountYB() { return YuanBao.GetCur(); }
	inline INT64	GetAccountSilver() { return Silver.GetCur(); }
	// 向账户存钱
	BOOL			IncAccountYB(INT nYuanBao, DWORD dwCmdID, BOOL bNoticeClient=FALSE);
	BOOL			IncAccountSilver(INT64 n64Silver, DWORD dwCmdID, BOOL bNoticeClient=FALSE);
	// 向账户取钱
	BOOL			DecAccountYuanBao(INT32 nYuanBao, DWORD dwCmdID, BOOL bNoticeClient=FALSE);
	BOOL			DecAccountSilver(INT64 n64Silver, DWORD dwCmdID, BOOL bNoticeClient=FALSE);
	// 可以存入的金钱数目
	INT64			GetCanIncAccountSilver()	{ return Silver.GetMax() - GetAccountSilver(); }
	INT				GetCanIncAccountYB()	    { return YuanBao.GetMax() - GetAccountYB(); }
};

//--------------------------------------------------------------------------------------------
// 元宝寄售订单排序
//--------------------------------------------------------------------------------------------
struct SellOrderCmp
{
	bool operator()(tagYuanBaoOrder* pOrder1, tagYuanBaoOrder* pOrder2) const
	{
		if(pOrder1->nPrice < pOrder2->nPrice)
			return TRUE;
		else if(pOrder1->nPrice == pOrder2->nPrice)
		{
			if(pOrder1->dwID < pOrder2->dwID)
				return TRUE;
		}
		return FALSE;
	}
};

//--------------------------------------------------------------------------------------------
// 元宝收购订单排序
//--------------------------------------------------------------------------------------------
struct BuyOrderCmp
{
	bool operator()(tagYuanBaoOrder* pOrder1, tagYuanBaoOrder* pOrder2) const
	{
		if(pOrder1->dwID < pOrder2->dwID)
			return TRUE;
		return FALSE;
	}
};

//--------------------------------------------------------------------------------------------
// 玩家元宝交易类
//--------------------------------------------------------------------------------------------
class CTradeYB
{
public:
	CTradeYB() {}
	~CTradeYB();
	BOOL	Init();
	VOID	Destroy();
	VOID	OnHour();

	tagYuanBaoAccount*		GetYBAccount(DWORD dwRoleID) { return m_mapYBAccount.Peek(dwRoleID); }
	tagYuanBaoOrder*		GetYBSellOrder(DWORD dwRoleID);
	tagYuanBaoOrder*		GetYBBuyOrder(DWORD dwRoleID);
	tagYuanBaoAccount*		CreateTradeAccount(DWORD dwRoleID);
	tagYuanBaoAccount*		CreateTradeAccount(const tagYBAccount *pYBAccount);
	tagYuanBaoOrder*		CreateYBOrder(DWORD dwRoleID, EYBOTYPE eYBOType, INT nPrice, INT nNum);
	VOID					LoadYOOrder(tagYuanBaoOrder* pYBOrder);
	VOID					RefreshYBOrder(tagYuanBaoOrder* pYBOrder, DWORD dwRoleID, INT nPrice, INT nNum, BOOL bNoticeClient);
	VOID					DeleteYBOrder(tagYuanBaoOrder* pYBOrder, EYBOMODE eYBOMode);
	VOID					DealYBSell(tagYuanBaoOrder* pYBOrder);
	VOID					DealYBBuy(tagYuanBaoOrder* pYBBuyOrder);
	VOID					DealYBTrade(tagYuanBaoOrder* pYBOrder, EYBOTYPE eYBOType);
	DWORD					IsCanTradeYB();

	VOID					SynSellPriceList(Role *pRole);
	VOID					SynBuyPriceList(Role *pRole);
	VOID					SynYBAccount(Role *pRole);

	VOID					SetMaxOrderIndex(DWORD dwIndex)	{ m_dwMaxOrderID = dwIndex; };

private:
	VOID					CalYBSellList();
	VOID					CalYBBuyList();
	VOID					InserYBSellList(INT nPrice, INT nNum);
	VOID					InserYBBuyList(INT nPrice, INT nNum);
	VOID					RemoveYBBuyList(INT nPrice, INT nNum, DWORD dwRoleID, BOOL bNoticeClient=FALSE);
	VOID					RemoveYBSellList(INT nPrice, INT nNum, DWORD dwRoleID, BOOL bNoticeClient=FALSE);

private:
	DWORD										m_dwMaxOrderID;					// 最大订单索引
	std::set<tagYuanBaoOrder*, SellOrderCmp>	m_setYBSell;					// 元宝寄售订单
	std::set<tagYuanBaoOrder*, BuyOrderCmp>		m_setYBBuy;						// 元宝收购订单
	TMap<DWORD, tagYuanBaoAccount*>				m_mapYBAccount;					// 玩家元宝交易账户
	TMap<DWORD,	DWORD>							m_mapOrder2Role;				// 订单ID与玩家ID的对应索引
	TMap<INT, INT>								m_mapYBSellList;				// 元宝寄售列表（方便同步客户端 单价 数量）
	TMap<INT, INT>								m_mapYBBuyList;					// 元宝收购列表（方便同步客户端 单价 数量）
};

inline tagYuanBaoOrder* CTradeYB::GetYBSellOrder(DWORD dwRoleID)
{
	tagYuanBaoOrder* pSellOrder = (tagYuanBaoOrder*)GT_INVALID;
	std::set<tagYuanBaoOrder*, SellOrderCmp>::iterator it = m_setYBSell.begin();
	while(it != m_setYBSell.end())
	{
		pSellOrder = *it;
		if(pSellOrder->dwRoleID == dwRoleID)
		{
			return pSellOrder;
		}
		++it;
	}

	return (tagYuanBaoOrder*)GT_INVALID;
}

inline tagYuanBaoOrder* CTradeYB::GetYBBuyOrder(DWORD dwRoleID)
{
	tagYuanBaoOrder* pBuyOrder = (tagYuanBaoOrder*)GT_INVALID;
	std::set<tagYuanBaoOrder*, BuyOrderCmp>::iterator it = m_setYBBuy.begin();
	while(it != m_setYBBuy.end())
	{
		pBuyOrder = *it;
		if(pBuyOrder->dwRoleID == dwRoleID)
		{
			return pBuyOrder;
		}
		++it;
	}

	return (tagYuanBaoOrder*)GT_INVALID;
}

extern CTradeYB g_tradeYB;