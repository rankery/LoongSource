//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: guild_CofC.cpp
// author: Sunnee
// actor:
// data: 2009-07-03
// last:
// brief: 商会
//-----------------------------------------------------------------------------

#include "StdAfx.h"

#include "../ServerDefine/guild_define.h"
#include "../WorldDefine/msg_guild.h"

#include "guild_CofC.h"
#include "att_res.h"
#include "world.h"
#include "role.h"
#include "role_mgr.h"
#include "guild.h"
#include "guild_mgr.h"
#include "guild_commodity.h"

//-----------------------------------------------------------------------------
// 构造与析构
//-----------------------------------------------------------------------------
GuildCofC::GuildCofC()
{
	m_bInitOK		= FALSE;
	m_dwNPCID		= GT_INVALID;
	m_dwCofCID		= GT_INVALID;
	m_byHolderCity	= 0;
	m_dwSyncTime	= 0;
	m_mapGoodSell.Clear();
	m_listObserver.Clear();
}

GuildCofC::~GuildCofC()
{
	Destory();
}

//-----------------------------------------------------------------------------
// 初始化、更新和销毁
//-----------------------------------------------------------------------------
BOOL GuildCofC::Init( DWORD dwNPCID, DWORD dwCofCID )
{
	if (!GT_VALID(dwNPCID) || !GT_VALID(dwCofCID))
	{
		return FALSE;
	}

	// 读取商货信息
	if (!g_attRes.LoadCofCGoodInfo(dwCofCID, m_mapGoodSell, m_mapGoodBuy))
	{
		return FALSE;
	}

	// 确定是否是特产商会
	const tagCofCSPProto* pCofCSPProto = g_attRes.GetCofCSPProto(dwCofCID);
	if (P_VALID(pCofCSPProto))
	{
		m_byHolderCity = pCofCSPProto->byHolderID;
	}
	
	m_listObserver.Clear();
	m_dwNPCID		= dwNPCID;
	m_dwCofCID		= dwCofCID;
	m_dwSyncTime	= 0;
	m_bInitOK		= TRUE;

	return TRUE;
}

VOID GuildCofC::Update()
{
	tagDWORDTime dwTime = g_world.GetWorldTime();
	BOOL bChange = FALSE;

	// 更新商货价格
	if (dwTime.sec == 0 && (DWORD)dwTime != (DWORD)m_dwSyncTime)
	{
		m_dwSyncTime = dwTime;

		if (dwTime.min % 5 == 0)
		{
			RefreshBuyGoods();
			bChange = TRUE;
		}

		// 更新商货数量
		if (dwTime.min % 10 == 0)
		{
			RefreshSellGoods();
			bChange = TRUE;
		}

		// 向观察者发送更新
		if (bChange)
		{
			SendCommerceGoodsInfo2Role(NULL);
		}
	}
}

VOID GuildCofC::Destory()
{
	tagCofCSellGood* pSellGood = NULL;
	MapSellGood::TMapIterator iter = m_mapGoodSell.Begin();
	while (m_mapGoodSell.PeekNext(iter, pSellGood))
	{
		SAFE_DEL(pSellGood);
	}
	m_mapGoodSell.Clear();

	m_dwNPCID	= GT_INVALID;
	m_bInitOK	= FALSE;
}

//-----------------------------------------------------------------------------
// 定时刷新商货
//-----------------------------------------------------------------------------
VOID GuildCofC::RefreshSellGoods()
{
	tagCofCSellGood* pSellGood = NULL;
	MapSellGood::TMapIterator iter = m_mapGoodSell.Begin();
	while (m_mapGoodSell.PeekNext(iter, pSellGood))
	{
		if (!P_VALID(pSellGood))
		{
			continue;
		}
		const tagCommodityProto* pProto = g_attRes.GetCommodityProto(pSellGood->dwGoodID);
		if (!P_VALID(pProto))
		{
			continue;
		}
		pSellGood->byRemainNum = pProto->byRefreshNum;
		CalRefreshPrice(pSellGood->nCost, pProto->nLowPrice, pProto->nHighPrice);
	}
}

VOID GuildCofC::RefreshBuyGoods()
{
	// 收购商货价格
	tagCofCBuyGood* pBuyGood = NULL;
	MapBuyGood::TMapIterator iter_buy = m_mapGoodBuy.Begin();
	while (m_mapGoodBuy.PeekNext(iter_buy, pBuyGood))
	{
		if (!P_VALID(pBuyGood))
		{
			continue;
		}
		const tagCommodityProto* pProto = g_attRes.GetCommodityProto(pBuyGood->dwGoodID);
		if (!P_VALID(pProto))
		{
			continue;
		}

		CalRefreshPrice(pBuyGood->nCost, pProto->nLowPrice, pProto->nHighPrice);
	}
}

//-----------------------------------------------------------------------------
// 取得商店中所有物品信息
//-----------------------------------------------------------------------------
VOID GuildCofC::GetGoodsInfo( DWORD dwRoleID, OUT tagCommerceGoodInfo* pGoods, INT& byNum )
{
	byNum = 0;

	tagCofCSellGood* pSellGood = NULL;
	MapSellGood::TMapIterator iter = m_mapGoodSell.Begin();
	while (m_mapGoodSell.PeekNext(iter, pSellGood))
	{
		pGoods[byNum++] = *(tagCommerceGoodInfo*)pSellGood;
	}

	// 加入观察者列表
	if (GT_VALID(dwRoleID) && !m_listObserver.IsExist(dwRoleID))
	{
		m_listObserver.PushBack(dwRoleID);
	}
}

//-----------------------------------------------------------------------------
// 商店卖出商货
//-----------------------------------------------------------------------------
DWORD GuildCofC::SellItem( INT32 nTael, DWORD dwGoodID, BYTE byBuyNum, OUT INT32 &nCostTael )
{
	// 参数检查
	if (!GT_VALID(dwGoodID) || byBuyNum<=0)
	{
		return GT_INVALID;
	}

	// 查找商货
	tagCofCSellGood* pGoodInfo = m_mapGoodSell.Peek(dwGoodID);
	if (!P_VALID(pGoodInfo))
	{
		return E_CofC_ItemNotFind;
	}

	// 检查商货个数
	if (byBuyNum > pGoodInfo->byRemainNum)
	{
		return E_CofC_ItemNotEnough;
	}

	// 检查商银
	nCostTael = byBuyNum * pGoodInfo->nCost;
	if (nTael < byBuyNum * pGoodInfo->nCost)
	{
		return E_GuildCommodity_NotEnough_Tael;
	}

	// 更新商店中物品个数
	pGoodInfo->byRemainNum	-= byBuyNum;

	// 向观察者发送更新消息
	SendCommerceGoodsInfo2Role(NULL, pGoodInfo->dwGoodID);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 获取指定商货在该商店的收购价
//-----------------------------------------------------------------------------
INT32 GuildCofC::GetGoodPrice( DWORD dwGoodID )
{
	const tagCommodityProto* pProto = g_attRes.GetCommodityProto(dwGoodID);
	if (!P_VALID(pProto))
	{
		return 0;
	}

	const FLOAT fProfit	= g_attRes.GetCofCProfit(m_dwCofCID, pProto->byHolderID);
	if (!GT_VALID((INT32)fProfit))
	{
		return 0;
	}

	tagCofCBuyGood* pBuyGood = m_mapGoodBuy.Peek(dwGoodID);
	if (!P_VALID(pBuyGood))
	{
		return 0;
	}

	return (INT32)(pBuyGood->nCost * fProfit);
}

//-----------------------------------------------------------------------------
// 创建商会
//-----------------------------------------------------------------------------
GuildCofC* GuildCofC::Create( DWORD dwNPCID, DWORD dwCofCID )
{
	GuildCofC* pCofC = new GuildCofC;
	if (!P_VALID(pCofC))
	{
		return NULL;
	}

	if (!pCofC->Init(dwNPCID, dwCofCID))
	{
		ASSERT(0);
		SAFE_DEL(pCofC);
		return NULL;
	}
	
	return pCofC;
}

//-----------------------------------------------------------------------------
// 计算出售商货刷新价格
//-----------------------------------------------------------------------------
VOID GuildCofC::CalRefreshPrice( INT32& nCurPrice, INT32 nLowPrice, INT32 nHighPrice )
{
	ASSERT(nHighPrice >= nLowPrice);
	ASSERT(nCurPrice >= nLowPrice);
	ASSERT(nCurPrice <= nHighPrice);

	INT32 nOdds	= (INT32)((FLOAT)(nHighPrice - nCurPrice) / (FLOAT)(nHighPrice - nLowPrice) * 10000.0f);

	INT32 nRandPct = IUTIL->Rand() % 10000;
	if (nRandPct < nOdds)
	{
		nCurPrice = IUTIL->RandomInRange((nHighPrice+nLowPrice)/2, nHighPrice);
	}
	else
	{
		nCurPrice = IUTIL->RandomInRange(nLowPrice, (nHighPrice+nLowPrice)/2);
	}
}

//-----------------------------------------------------------------------------
// 更新商会以及跑商容器中商货
//-----------------------------------------------------------------------------
DWORD GuildCofC::SendCommerceGoodsInfo2Role( Role* pRole, DWORD dwGoodID /*= GT_INVALID*/ )
{
	// 无观察者且无需发送
	if (!P_VALID(pRole) && m_listObserver.Empty())
	{
		return E_Success;
	}

	// 商会信息
	INT nMsgSize = sizeof(tagNS_GetCofCInfo) + sizeof(tagCommerceGoodInfo) * (MAX_COFC_GOODS_NUM - 1);
	MCREATE_MSG(pCofCInfo, nMsgSize, NS_GetCofCInfo);

	pCofCInfo->dwCofCID		= m_dwCofCID;
	pCofCInfo->byHoldCity	= m_byHolderCity;
	if (!GT_VALID(dwGoodID))
	{
		// 读取商货信息
		if (P_VALID(pRole))
		{
			GetGoodsInfo(pRole->GetID(), pCofCInfo->sGoodInfo, pCofCInfo->nGoodNum);
		}
		else
		{
			GetGoodsInfo(GT_INVALID, pCofCInfo->sGoodInfo, pCofCInfo->nGoodNum);
		}
	}
	else
	{
		// 取得指定商货信息
		tagCommerceGoodInfo* pSellGood = (tagCommerceGoodInfo*)(m_mapGoodSell.Peek(dwGoodID));
		if (P_VALID(pSellGood))
		{
			// 指定商货信息
			pCofCInfo->nGoodNum		= 1;
			pCofCInfo->sGoodInfo[0]	= *pSellGood;
		}
	}
	// 调整商会消息大小
	pCofCInfo->dwSize = sizeof(tagNS_GetCofCInfo) + sizeof(tagCommerceGoodInfo) * (pCofCInfo->nGoodNum - 1);

	// 发送商会信息
	if (P_VALID(pRole))
	{
		pRole->SendMessage(pCofCInfo, pCofCInfo->dwSize);
	}
	else
	{
		// 发送给观察者
		SendMsg2Observer(pCofCInfo, pCofCInfo->dwSize);
	}

	// 观察者商货信息更新
	if (!GT_VALID(dwGoodID))
	{
		// 读取跑商容器中商货信息
		nMsgSize = sizeof(tagNS_GetCommodityInfo) + sizeof(tagCommerceGoodInfo) * (MAX_COMMODITY_CAPACITY - 1);
		MCREATE_MSG(pCommodityInfo, nMsgSize, NS_GetCommodityInfo);

		if (P_VALID(pRole))
		{
			// 取得帮派信息
			Guild* pGuild = g_guildMgr.GetGuild(pRole->GetGuildID());
			if (!P_VALID(pGuild))
			{
				return GT_INVALID;
			}

			GuildCommodity* pCommodity = pGuild->GetGuildCommerce().GetCommodity(pRole->GetID());
			if (!P_VALID(pCommodity))
			{
				return GT_INVALID;
			}

			// 读取商货信息并发送
			if (pCommodity->GetCommodityInfo(pCommodityInfo->sGoodInfo, pCommodityInfo->nGoodNum, pCommodityInfo->nCurTael, pCommodityInfo->nLevel) == E_Success)
			{
				// 更新收购价格
				for (int n=0; n<pCommodityInfo->nGoodNum; n++)
				{
					pCommodityInfo->sGoodInfo[n].nCost = GetGoodPrice(pCommodityInfo->sGoodInfo[n].dwGoodID);
				}

				// 调整消息大小
				pCommodityInfo->dwSize = sizeof(tagNS_GetCommodityInfo) + sizeof(tagCommerceGoodInfo) * (pCommodityInfo->nGoodNum - 1);

				// 发送信息
				pRole->SendMessage(pCommodityInfo, pCommodityInfo->dwSize);
			}
		} 
		else
		{
			DWORD dwObserverID = GT_INVALID;
			ListObserver::TListIterator iter = m_listObserver.Begin();
			while (m_listObserver.PeekNext(iter, dwObserverID))
			{
				Role* pObserver = g_roleMgr.GetRolePtrByID(dwObserverID);
				if (!P_VALID(pObserver))
				{
					// 玩家不在线，移出观察者列表
					m_listObserver.Erase(dwObserverID);
					continue;
				}

				// 取得帮派信息
				Guild* pGuild = g_guildMgr.GetGuild(pObserver->GetGuildID());
				if (!P_VALID(pGuild))
				{
					// 帮派不存在
					m_listObserver.Erase(dwObserverID);
					continue;
				}

				GuildCommodity* pCommodity = pGuild->GetGuildCommerce().GetCommodity(pObserver->GetID());
				if (!P_VALID(pCommodity))
				{
					// 不在跑商状态
					m_listObserver.Erase(dwObserverID);
					continue;
				}

				// 读取商货信息并发送
				if (pCommodity->GetCommodityInfo(pCommodityInfo->sGoodInfo, pCommodityInfo->nGoodNum, pCommodityInfo->nCurTael, pCommodityInfo->nLevel) == E_Success)
				{
					// 更新收购价格
					for (int n=0; n<pCommodityInfo->nGoodNum; n++)
					{
						pCommodityInfo->sGoodInfo[n].nCost = GetGoodPrice(pCommodityInfo->sGoodInfo[n].dwGoodID);
					}

					// 调整消息大小
					pCommodityInfo->dwSize = sizeof(tagNS_GetCommodityInfo) + sizeof(tagCommerceGoodInfo) * (pCommodityInfo->nGoodNum - 1);

					// 发送信息
					pObserver->SendMessage(pCommodityInfo, pCommodityInfo->dwSize);
				}
			}
		}
	}

	return E_Success;
}

//-----------------------------------------------------------------------------
// 向所有观察者发送消息
//-----------------------------------------------------------------------------
VOID GuildCofC::SendMsg2Observer( LPVOID pMsg, DWORD dwSize )
{
	DWORD dwObserverID = GT_INVALID;
	ListObserver::TListIterator iter = m_listObserver.Begin();
	while (m_listObserver.PeekNext(iter, dwObserverID))
	{
		Role* pObserver = g_roleMgr.GetRolePtrByID(dwObserverID);
		if (!P_VALID(pObserver))
		{
			// 玩家不在线，移出观察者列表
			m_listObserver.Erase(dwObserverID);
			continue;
		}

		pObserver->SendMessage(pMsg, dwSize);
	}
}