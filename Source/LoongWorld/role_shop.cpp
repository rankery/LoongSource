//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: role_shop.h
// author: Sxg
// actor:
// data: 2008-10-28
// last:
// brief: 商店处理
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "../WorldDefine/msg_shop.h"
#include "../ServerDefine/log_cmdid_define.h"
#include "role.h"
#include "creature.h"
#include "shop.h"

//-----------------------------------------------------------------------------
// 获取物品(非装备)店中刷新商品列表
//-----------------------------------------------------------------------------
DWORD Role::GetShopItems(DWORD dwNPCID, BYTE byShelf)
{
	// 获得地图
	Map *pMap = GetMap();
	if(!P_VALID(pMap))
	{
		ASSERT(P_VALID(pMap));
		return GT_INVALID;
	}

	// 找到NPC，并检查合法性
	Creature* pNPC = pMap->FindCreature(dwNPCID);
	if(!P_VALID(pNPC)
		|| !pNPC->IsFunctionNPC(EFNPCT_Shop) 
		|| !pNPC->CheckNPCTalkDistance(this))
	{
		return E_Shop_NPCNotValid;
	}

	// 找到商店
	Shop *pShop = pMap->GetShop(pNPC->GetTypeID());
	if(!P_VALID(pShop) || pShop->IsEquipShop())
	{
		return E_Shop_NotValid;
	}

	INT16 n16RareItemNum = pShop->GetRareGoodsNum(byShelf);
	
	// 没有刷新物品
	if(0 == n16RareItemNum)
	{
		tagNS_GetShopItems Send;
		Send.dwNPCID	= dwNPCID;
		Send.byShelf	= byShelf;
		Send.dwShopID	= pNPC->GetShopID();
		Send.n16RareItemNum = 0;

		SendMessage(&Send, Send.dwSize);
		return E_Success;
	}

	// 有刷新物品
	INT32 nSzMsg = sizeof(tagNS_GetShopItems) - 1 + sizeof(tagMsgShopItem) * n16RareItemNum;

	MCREATE_MSG(pSend, nSzMsg, NS_GetShopItems);
	pSend->dwSize	= nSzMsg;
	pSend->dwNPCID	= dwNPCID;
	pSend->byShelf	= byShelf;
	pSend->dwShopID	= pNPC->GetShopID();
	pSend->n16RareItemNum = n16RareItemNum;

	pShop->GetRareItems((tagMsgShopItem*)pSend->byData, n16RareItemNum, byShelf);
	
	SendMessage(pSend, pSend->dwSize);

	MDEL_MSG(pSend);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 获取所有装备
//-----------------------------------------------------------------------------
DWORD Role::GetShopEquips(DWORD dwNPCID, BYTE byShelf)
{
	// 获得地图
	Map *pMap = GetMap();
	if(!P_VALID(pMap))
	{
		ASSERT(P_VALID(pMap));
		return GT_INVALID;
	}

	// 找到NPC，并检查合法性
	Creature* pNPC = pMap->FindCreature(dwNPCID);
	if(!P_VALID(pNPC)
		|| !pNPC->IsFunctionNPC(EFNPCT_Shop) 
		|| !pNPC->CheckNPCTalkDistance(this))
	{
		return E_Shop_NPCNotValid;
	}

	// 找到商店
	Shop *pShop = pMap->GetShop(pNPC->GetTypeID());
	if(!P_VALID(pShop) || !pShop->IsEquipShop())
	{
		return E_Shop_NotValid;
	}

	INT16 n16RareEquipNum = pShop->GetRareGoodsNum(byShelf);

	// 没有刷新物品
	if(0 == n16RareEquipNum)
	{
		tagNS_GetShopEquips Send;
		Send.dwNPCID		= dwNPCID;
		Send.byShelf		= byShelf;
		Send.dwShopID		= pNPC->GetShopID();
		Send.n16EquipNum	= 0;

		SendMessage(&Send, Send.dwSize);
		return E_Success;
	}

	INT32 nSzMsg = sizeof(tagNS_GetShopEquips) - 1 + sizeof(tagMsgShopEquip) * n16RareEquipNum;

	MCREATE_MSG(pSend, nSzMsg, NS_GetShopEquips);
	pSend->dwSize	= nSzMsg;
	pSend->dwNPCID	= dwNPCID;
	pSend->byShelf	= byShelf;
	pSend->dwShopID	= pNPC->GetShopID();
	pSend->n16EquipNum = n16RareEquipNum;

	pShop->GetRareEquips((tagMsgShopEquip*)pSend->byData, byShelf, pSend->n16EquipNum);

	// 重新计算大小
	if(pSend->n16EquipNum != n16RareEquipNum)
	{
		pSend->dwSize -= (n16RareEquipNum - pSend->n16EquipNum) * sizeof(tagMsgShopEquip);
	}

	SendMessage(pSend, pSend->dwSize);

	MDEL_MSG(pSend);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 购买物品(非装备)
//-----------------------------------------------------------------------------
DWORD Role::BuyShopItem(DWORD dwNPCID, BYTE byShelf, DWORD dwTypeID, INT16 n16ItemNum)
{
	// 行囊是否解锁
	if(!GetRoleStateEx().IsInState(ERSE_BagPsdPass))
	{
		return E_Con_PswNotPass;
	}
	
	// 获得地图
	Map *pMap = GetMap();
	if(!P_VALID(pMap))
	{
		ASSERT(P_VALID(pMap));
		return GT_INVALID;
	}

	// 找到NPC，并检查合法性
	Creature* pNPC = pMap->FindCreature(dwNPCID);
	if(!P_VALID(pNPC)
		|| !pNPC->IsFunctionNPC(EFNPCT_Shop) 
		|| !pNPC->CheckNPCTalkDistance(this))
	{
		return E_Shop_NPCNotValid;
	}

	// 找到商店
	Shop *pShop = pMap->GetShop(pNPC->GetTypeID());
	if(!P_VALID(pShop) || pShop->IsEquipShop())
	{
		return E_Shop_NotValid;
	}
	
	// 检查玩家背包是否有地方
	if(GetItemMgr().GetBagFreeSize() < 1)
	{
		return E_Shop_NotEnough_SpaceInBag;
	}

	// 同类物品数量达到上限
	if (!GetItemMgr().CanAddMaxHoldItem(dwTypeID, n16ItemNum))
	{
		return E_Shop_ItemMaxHold;
	}

	tagSellItem SellItem;
	DWORD dwErrorCode = pShop->SellItem(this, byShelf, dwTypeID, n16ItemNum, SellItem);
	if(dwErrorCode != E_Success
		&& dwErrorCode != E_Shop_ItemNotEnough)
	{
		return dwErrorCode;
	}

	// 从玩家背包中扣除金钱
	if(SellItem.nSilverCost > 0)
	{
		GetCurMgr().DecBagSilver(SellItem.nSilverCost, ELCID_Shop_BuyItem);
	}

	// 扣除特殊消耗
	if(SellItem.nSpecCost > 0)
	{
		if(ECCT_ItemExchange == SellItem.eCostType)
		{
			GetItemMgr().DelBagSameItem(SellItem.listItem, SellItem.nSpecCost, ELCID_Shop_BuyItem);
		}
		else if(SellItem.eCostType != ECCT_Null)
		{
			GetCurMgr().DecCurrency(SellItem.eCostType, SellItem.nSpecCost, ELCID_Shop_BuyItem);
		}
	}

	// 将装备放到玩家背包中
	if(P_VALID(SellItem.pItemSell))
	{
		GetItemMgr().Add2Bag(SellItem.pItemSell, ELCID_Shop_BuyItem, TRUE);
	}

	// 发送更新后商店物品 -- 只有刷新物品要更新物品个数
	if(SellItem.n16RemainNum != GT_INVALID)
	{
		tagNS_BuyShopItem Send;
		Send.dwNPCID		= dwNPCID;
		Send.byShelf		= byShelf;
		Send.n16RemainNum	= SellItem.n16RemainNum;
		Send.dwTypeID		= dwTypeID;
		Send.byIndex		= SellItem.byIndex;

		SendMessage(&Send, Send.dwSize);
	}
	

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 买装备
//-----------------------------------------------------------------------------
DWORD Role::BuyShopEquip(DWORD dwNPCID, BYTE byShelf, DWORD dwTypeID, INT64 n64Serial)
{
	// 行囊是否解锁
	if(!GetRoleStateEx().IsInState(ERSE_BagPsdPass))
	{
		return E_Con_PswNotPass;
	}
	
	// 获得地图
	Map *pMap = GetMap();
	if(!P_VALID(pMap))
	{
		ASSERT(P_VALID(pMap));
		return GT_INVALID;
	}

	// 找到NPC，并检查合法性
	Creature* pNPC = pMap->FindCreature(dwNPCID);
	if(!P_VALID(pNPC)
		|| !pNPC->IsFunctionNPC(EFNPCT_Shop) 
		|| !pNPC->CheckNPCTalkDistance(this))
	{
		return E_Shop_NPCNotValid;
	}

	// 找到商店
	Shop *pShop = pMap->GetShop(pNPC->GetTypeID());
	if(!P_VALID(pShop) || !pShop->IsEquipShop())
	{
		return E_Shop_NotValid;
	}

	// 检查玩家背包是否有地方
	if(GetItemMgr().GetBagFreeSize() < 1)
	{
		return E_Shop_NotEnough_SpaceInBag;
	}

	// 同类物品数量达到上限
	if (!GetItemMgr().CanAddMaxHoldItem(dwTypeID, 1))
	{
		return E_Shop_ItemMaxHold;
	}

	tagSellEquip SellEquip;
	DWORD dwErrorCode = pShop->SellEquip(this, byShelf, dwTypeID, n64Serial, SellEquip);
	if(dwErrorCode != E_Success
		&& dwErrorCode != E_Shop_Equip_Sold
		&& dwErrorCode != E_Shop_CreateEquip_Failed)
	{
		return dwErrorCode;
	}

	// 从玩家背包中扣除金钱
	if(SellEquip.nSilverCost > 0)
	{
		GetCurMgr().DecBagSilver(SellEquip.nSilverCost, ELCID_Shop_BuyEquip);
	}

	// 扣除特殊消耗
	if(SellEquip.nSpecCost > 0)
	{
		if(ECCT_ItemExchange == SellEquip.eCostType)
		{
			GetItemMgr().DelBagSameItem(SellEquip.listItem, SellEquip.nSpecCost, ELCID_Shop_BuyEquip);
		}
		else if(SellEquip.eCostType != ECCT_Null)
		{
			GetCurMgr().DecCurrency(SellEquip.eCostType, SellEquip.nSpecCost, ELCID_Shop_BuyEquip);
		}
	}

	// 将装备放到玩家背包中
	if(P_VALID(SellEquip.pEquipSell))
	{
		tagItem *pItem = SellEquip.pEquipSell;
		GetItemMgr().Add2Bag(pItem, ELCID_Shop_BuyEquip, TRUE);
	}

	// 卖出的为普通武器
	if(GT_INVALID == SellEquip.n16RemainNum)
	{
		return dwErrorCode;
	}
	
	// 发送更新后商店物品 -- 只更新稀有武器
	if(P_VALID(SellEquip.pEquipNew))
	{
		INT32 nMsgSz = sizeof(tagNS_BuyShopEquip) - 1 + SIZE_EQUIP;

		MCREATE_MSG(pSend, nMsgSz, NS_BuyShopEquip);
		pSend->dwNPCID		= dwNPCID;
		pSend->byShelf		= byShelf;
		pSend->n16RemainNum	= SellEquip.n16RemainNum;
		pSend->dwSize		= nMsgSz;
		pSend->byIndex		= SellEquip.byIndex;
		
		IFASTCODE->MemCpy(pSend->byData, SellEquip.pEquipNew, SIZE_EQUIP);
		
		SendMessage(pSend, pSend->dwSize);

		MDEL_MSG(pSend);
	}
	else
	{
		// 刷新装备，且没有待售装备，更新个数
		tagNS_BuyShopEquip Send;
		Send.dwNPCID		= dwNPCID;
		Send.byShelf		= byShelf;
		Send.n16RemainNum	= SellEquip.n16RemainNum;
		Send.byIndex		= SellEquip.byIndex;

		ASSERT(0 == SellEquip.n16RemainNum);

		SendMessage(&Send, Send.dwSize);

		GetShopEquips(dwNPCID,byShelf);
	}

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 向商店出售物品&装备
//-----------------------------------------------------------------------------
DWORD Role::SellToShop(DWORD dwNPCID, INT64 n64Serial)
{
	// 行囊是否解锁
	if(!GetRoleStateEx().IsInState(ERSE_BagPsdPass))
	{
		return E_Con_PswNotPass;
	}
	
	// 获得地图
	Map *pMap = GetMap();
	if(!P_VALID(pMap))
	{
		ASSERT(P_VALID(pMap));
		return GT_INVALID;
	}

	// 找到NPC，并检查合法性
	Creature* pNPC = pMap->FindCreature(dwNPCID);
	if(!P_VALID(pNPC)
		|| !pNPC->IsFunctionNPC(EFNPCT_Shop) 
		|| !pNPC->CheckNPCTalkDistance(this))
	{
		return E_Shop_NPCNotValid;
	}

	// 找到商店
	Shop *pShop = pMap->GetShop(pNPC->GetTypeID());
	if(!P_VALID(pShop))
	{
		return E_Shop_NotValid;
	}

	// 找到待售物品
	tagItem *pItem = GetItemMgr().GetBagItem(n64Serial);
	if(!P_VALID(pItem))
	{
		return E_Shop_ItemNotFind;
	}

	// 是否可出售
	if(!GetItemMgr().CanSell(*pItem))
	{
		return E_Shop_ItemCannotSell;
	}

	// 计算售价//??没有计算税收影响
	INT64 n64Price = pItem->pProtoType->nBasePrice * pItem->n16Num;
	if(MIsEquipment(pItem->dwTypeID))
	{
		FLOAT fFactor;
		MCalPriceFactor(fFactor, ((tagEquip*)pItem)->equipSpec.byQuality);
		n64Price = (INT64)(n64Price * fFactor);
		if(n64Price < 0)
		{
			ASSERT(0);
			n64Price = 0;
		}
	}

	// 从玩家身上删除物品
	DWORD dwErrorCode = GetItemMgr().DelFromBag(n64Serial, ELCID_Shop_Sell, GT_INVALID, TRUE);
	if(dwErrorCode != E_Success)
	{
		return dwErrorCode;
	}

	// 防沉迷
	float fEarnRate = GetEarnRate() / 10000.0f;
	n64Price = (INT64)(n64Price * fEarnRate);

	// 玩家获得金钱
	GetCurMgr().IncBagSilver(n64Price, ELCID_Shop_Sell);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 商店消息处理结构反馈给客户端
//-----------------------------------------------------------------------------
VOID Role::SendShopFeedbackMsg(DWORD dwErrorCode, DWORD dwNPCID)
{
	// 成功也会返回，客户端需要做音效处理
	tagNS_FeedbackFromShop Send;
	Send.dwErrorCode	= dwErrorCode;
	Send.dwNPCID		= dwNPCID;

	SendMessage(&Send, Send.dwSize);
}