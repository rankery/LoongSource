//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: shop.cpp
// author: Sxg
// actor:
// data: 2008-10-22
// last:
// brief: 商店类
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "../WorldDefine/time.h"
#include "../WorldDefine/msg_shop.h"
#include "../ServerDefine/base_define.h"
#include "shop.h"
#include "role.h"
#include "att_res.h"
#include "item_creator.h"
#include "clan_treasury.h"

//*********************************** Shop **************************************

//-----------------------------------------------------------------------------
// 构造&析构函数
//-----------------------------------------------------------------------------
Shop::Shop(DWORD dwNPCID, const tagShopProto *pShopProto)
{
	ASSERT(P_VALID(pShopProto));

	// 检查商品个数
	INT16 n16TotalNum = 0;
	for(INT32 i=0; i<MAX_SHOP_SHELF_NUM; ++i)
	{
		ASSERT(pShopProto->n16Num[i] >= 0);
		n16TotalNum += pShopProto->n16Num[i];
	}

	ASSERT(n16TotalNum <= MAX_SHOP_TOTAL_ITEM);
	
	// 初始化
	m_bInitOK		= FALSE;
	m_dwNPCID		= dwNPCID;
	m_pShopProto	= pShopProto;

	ZeroMemory(m_n16RareIndexStart, sizeof(m_n16RareIndexStart));
}

Shop::~Shop()
{
}

//-----------------------------------------------------------------------------
// 创建商店
//-----------------------------------------------------------------------------
Shop* Shop::Create(DWORD dwNPCID, DWORD dwShopID)
{
	const tagShopProto *pShopProto = g_attRes.GetShopProto(dwShopID);
	if(!P_VALID(pShopProto))
	{
		ASSERT(P_VALID(pShopProto));
		return NULL;
	}

	Shop *pShop = NULL;

	if (pShopProto->bClanTreasury)
	{
		if (pShopProto->bEquip)
		{
			pShop = new EquipClanTreasury(dwNPCID, pShopProto);
		}
		else
		{
			pShop = new ItemClanTreasury(dwNPCID, pShopProto);
		}
	}
	else
	{
		if(pShopProto->bEquip)
		{
			pShop = new EquipShop(dwNPCID, pShopProto);
		}
		else
		{
			pShop = new ItemShop(dwNPCID, pShopProto);
		}
	}

	
	if(!pShop->IsInitOK())
	{
		ASSERT(pShop->IsInitOK());
		SAFE_DEL(pShop);
	}

	return pShop;
}

//-----------------------------------------------------------------------------
// 检查特殊消耗
//-----------------------------------------------------------------------------
DWORD Shop::CheckSpecCost(Role *pRole, ECurCostType eCostType, INT nSpecCost, 
			  DWORD dwCostItemID, BYTE byRepLevelLimit, OUT TList<tagItem*> &listItem)
{
	ASSERT(P_VALID(pRole));
	
	if(nSpecCost < 0)
	{
		ASSERT(nSpecCost >= 0);
		return GT_INVALID;
	}
	
	listItem.Clear();
	
	switch(eCostType)
	{
	case ECCT_Null:			// 没有特殊消耗
		break;
	case ECCT_BagSilver:	// 消耗背包金钱有单独字段
		return GT_INVALID;
		break;
	case ECCT_ItemExchange:	// 物品兑换
		if(pRole->GetItemMgr().GetBagSameItemList(listItem, dwCostItemID, nSpecCost) < nSpecCost)
		{
			return E_Item_NotEnough;
		}
		break;
	default:				// 声望等其他类型
		ECLanType eClanType = MTRANS_ECCT2ECLT(eCostType);
		if( JDG_VALID(ECLT, eClanType) && byRepLevelLimit > pRole->GetClanData().RepGetLvl(eClanType))
		{
			return E_Shop_RepLevel_Low;
		}

		// 特殊消耗品数量 -- 使用通用接口(策划调整后，程序不用修改)
		if(!pRole->GetCurMgr().IsEnough(eCostType, nSpecCost))
		{
			return E_Shop_NotEnough_SpecCost;
		}
		break;
	}
	
	return E_Success;
}


//*********************************** ItemShop **********************************

//-----------------------------------------------------------------------------
// 构造&析构函数
//-----------------------------------------------------------------------------
ItemShop::ItemShop(DWORD dwNPCID, const tagShopProto *pShopProto)
					: Shop(dwNPCID, pShopProto)
{
	m_bInitOK = Init(pShopProto);
}

ItemShop::~ItemShop()
{
}

//-----------------------------------------------------------------------------
// 初始化
//-----------------------------------------------------------------------------
BOOL ItemShop::Init(const tagShopProto *pShopProto)
{
	ASSERT(P_VALID(pShopProto));
	ASSERT(!pShopProto->bEquip);

	// 初始化货架
	for(INT32 n=0; n<MAX_SHOP_SHELF_NUM; ++n)
	{
		m_Shelf[n].Init(EICT_Shop, pShopProto->n16Num[n], pShopProto->n16Num[n]);
	}

	// 记录各页商品个数
	INT16	n16ItemNum[MAX_SHOP_SHELF_NUM];
	ZeroMemory(n16ItemNum, sizeof(n16ItemNum));

	// 获取当前时间
	DWORD dwCurTime = g_world.GetWorldTime();
	
	// 初始化普通物品
	INT32 i,j;
	const tagShopItemProto *pItemProto = NULL;
	for(i=0; i<MAX_SHOP_COMMON_ITEM; ++i)
	{
		pItemProto = &pShopProto->Item[i];

		if(GT_INVALID == pItemProto->dwTypeID)
		{
			break;
		}
		
		tagShopItem *pItem = new tagShopItem;
		if(!P_VALID(pItem))
		{
			ASSERT(P_VALID(pItem));
			return FALSE;
		}

		pItem->dwTypeID			= pItemProto->dwTypeID;
		pItem->n16RemainNum		= GT_INVALID;
		pItem->n16Index			= GT_INVALID;
		pItem->byProtoIndex		= i;
		pItem->dwRefreshTime	= dwCurTime;
		pItem->pProto			= pItemProto;

		// 放到所属货架
		ASSERT(pItemProto->byShelf < MAX_SHOP_SHELF_NUM);
		m_Shelf[pItemProto->byShelf].Add(pItem, n16ItemNum[pItemProto->byShelf]++);
	}

	// 初始化限量物品起始下标
	IFASTCODE->MemCpy(m_n16RareIndexStart, n16ItemNum, sizeof(m_n16RareIndexStart));

	// 初始化限量物品
	const tagShopRareItemProto *pRareProto = NULL;
	for(j=0; j<MAX_SHOP_RARE_ITEM; ++i, ++j)
	{
		pRareProto = &pShopProto->RareItem[j];

		if(GT_INVALID == pRareProto->dwTypeID)
		{
			break;
		}

		tagShopItem *pItem = new tagShopItem;
		if(!P_VALID(pItem))
		{
			ASSERT(P_VALID(pItem));
			return FALSE;
		}
		
		pItem->dwTypeID			= pRareProto->dwTypeID;
		pItem->n16RemainNum		= pRareProto->byRefreshNum;
		pItem->n16Index			= GT_INVALID;
		pItem->byProtoIndex		= j;
		pItem->dwRefreshTime	= dwCurTime;
		pItem->pRareProto		= pRareProto;

		// 放到所属货架
		ASSERT(pRareProto->byShelf < MAX_SHOP_SHELF_NUM);
		m_Shelf[pRareProto->byShelf].Add(pItem, n16ItemNum[pRareProto->byShelf]++);
	}

	// 检查商店资源是否有问题
	for(INT32 n=0; n<MAX_SHOP_SHELF_NUM; ++n)
	{
		if(pShopProto->n16Num[n] != n16ItemNum[n])
		{
			ASSERT(0);
			IMSG(_T("Shop proto mybe has problem, please check<shopid:%u, shelf:%d>\n"), pShopProto->dwID, n);
			return FALSE;
		}
	}
	
	return TRUE;
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
VOID ItemShop::Update()
{
	// 获取当前时间
	DWORD dwCurTime = g_world.GetWorldTime();

	// 记录各页商品下标
	INT16	n16ItemNum[MAX_SHOP_SHELF_NUM];

	IFASTCODE->MemCpy(n16ItemNum, m_n16RareIndexStart, sizeof(m_n16RareIndexStart));

	// 检查限量物品是否需要更新
	tagShopItem *pItem = NULL;
	const tagShopRareItemProto *pRareProto = NULL;
	for(INT32 i=0; i<MAX_SHOP_RARE_ITEM; ++i)
	{
		pRareProto = &m_pShopProto->RareItem[i];
		if(GT_INVALID == pRareProto->dwTypeID)
		{
			break;
		}

		pItem = m_Shelf[pRareProto->byShelf].GetItem(n16ItemNum[pRareProto->byShelf]++);
		ASSERT(P_VALID(pItem) && pItem->dwTypeID == pRareProto->dwTypeID);

		if(CalcTimeDiff(dwCurTime, pItem->dwRefreshTime) >= pRareProto->dwRefreshTime)
		{
			pItem->n16RemainNum	= pRareProto->byRefreshNum;
			pItem->dwRefreshTime = dwCurTime;
		}
	}
}

//-----------------------------------------------------------------------------
// 获取刷新物品
//-----------------------------------------------------------------------------
VOID ItemShop::GetRareItems(OUT tagMsgShopItem* pRareItems, INT16 n16RareItemNum, BYTE byShelf)
{
	ASSERT(GetRareGoodsNum(byShelf) == n16RareItemNum);

	tagShopItem *pShopItem = NULL;
	for(INT16 i=0; i<n16RareItemNum; ++i)
	{
		pShopItem = m_Shelf[byShelf].GetItem((INT16)(m_n16RareIndexStart[byShelf] + i));
		ASSERT(P_VALID(pShopItem));

		pRareItems[i].dwTypeID		= pShopItem->dwTypeID;
		pRareItems[i].n16RemainNum	= pShopItem->n16RemainNum;
		pRareItems[i].n16Index		= pShopItem->n16Index;
		pRareItems[i].byProtoIndex	= pShopItem->byProtoIndex;
	}
}

//-----------------------------------------------------------------------------
// 出售物品
//-----------------------------------------------------------------------------
DWORD ItemShop::SellItem(Role *pRole, BYTE byShelf, DWORD dwTypeID, INT16 n16BuyNum, OUT tagSellItem &SellItem)
{
	ASSERT(P_VALID(m_pShopProto));
	ASSERT(P_VALID(pRole));

	// 查找物品
	tagShopItem *pShopItem = m_Shelf[byShelf].GetItem(dwTypeID);
	if(!P_VALID(pShopItem))
	{
		// 执行到这里，意味着消息中的dwTypeID非法
		ASSERT(P_VALID(pShopItem));
		return E_Shop_ItemNotFind;
	}

	// 检查物品个数
	if(pShopItem->n16RemainNum != GT_INVALID && pShopItem->n16RemainNum < n16BuyNum)
	{
		SellItem.n16RemainNum = pShopItem->n16RemainNum;
		return E_Shop_ItemNotEnough;
	}

	// 背包中金钱 -- 不使用通用接口
	INT nSilverCost = pShopItem->pProto->nSilver * n16BuyNum;
	if(nSilverCost > pRole->GetCurMgr().GetBagSilver())
	{
		return E_Shop_NotEnough_SilverInBag;
	}

	// 特殊消耗品数量 -- 使用通用接口(策划调整后，程序不用修改)
	INT nSpecCost = pShopItem->pProto->nCostNum * n16BuyNum;
	DWORD dwErrorCode = CheckSpecCost(pRole, m_pShopProto->eCostType, nSpecCost, 
		m_pShopProto->dwItemTypeID, pShopItem->pProto->byRepLevel, SellItem.listItem);
	if(dwErrorCode != E_Success)
	{
		return dwErrorCode;
	}

	// 创建物品
	tagItem *pItemSell = ItemCreator::Create(EICM_ShopNPC, m_dwNPCID, dwTypeID, n16BuyNum);
	if(!P_VALID(pItemSell))
	{
		ASSERT(P_VALID(pItemSell));
		return E_Shop_CreateItem_Failed;
	}

	// 更新商店中物品个数
	if(pShopItem->n16RemainNum != GT_INVALID)
	{
		pShopItem->n16RemainNum -= n16BuyNum;
	}

	// 设置传出参数
	SellItem.pItemSell		= pItemSell;
	SellItem.n16RemainNum	= pShopItem->n16RemainNum;
	SellItem.nSilverCost	= nSilverCost;
	SellItem.eCostType		= m_pShopProto->eCostType;
	SellItem.nSpecCost		= nSpecCost;
	SellItem.byIndex		= (BYTE)pShopItem->n16Index;

	return E_Success;
}


//*********************************** EquipShop **********************************

//-----------------------------------------------------------------------------
// 构造&析构函数
//-----------------------------------------------------------------------------
EquipShop::EquipShop(DWORD dwNPCID, const tagShopProto *pShopProto)
					: Shop(dwNPCID, pShopProto)
{
	m_bInitOK = Init(pShopProto);
}

EquipShop::~EquipShop()
{
	// 删除已生成的装备
	tagShopEquip *pShopEquip = NULL;
	for(INT32 i=0; i<MAX_SHOP_SHELF_NUM; ++i)
	{
		for(INT16 j=0; j<m_pShopProto->n16Num[i]; ++j)
		{
			pShopEquip = m_Shelf[i].GetItem(j);
			if(!P_VALID(pShopEquip))
			{
				break;
			}

			SAFE_DEL(pShopEquip->pEquip);
		}
	}
}

//-----------------------------------------------------------------------------
// 初始化
//-----------------------------------------------------------------------------
BOOL EquipShop::Init(const tagShopProto *pShopProto)
{
	ASSERT(P_VALID(pShopProto));
	ASSERT(pShopProto->bEquip);

	// 初始化货架
	for(INT32 n=0; n<MAX_SHOP_SHELF_NUM; ++n)
	{
		m_Shelf[n].Init(EICT_Shop, pShopProto->n16Num[n], pShopProto->n16Num[n]);
	}

	// 记录各页商品个数
	INT16	n16ItemNum[MAX_SHOP_SHELF_NUM];
	ZeroMemory(n16ItemNum, sizeof(n16ItemNum));

	// 获取当前时间
	DWORD dwCurTime = g_world.GetWorldTime();

	// 初始化普通物品
	INT32 i,j;
	const tagShopItemProto *pEquipProto = NULL;
	for(i=0; i<MAX_SHOP_COMMON_ITEM; ++i)
	{
		pEquipProto = &pShopProto->Item[i];
		if(GT_INVALID == pEquipProto->dwTypeID)
		{
			break;
		}

		tagShopEquip *pEquip = new tagShopEquip;
		if(!P_VALID(pEquip))
		{
			ASSERT(P_VALID(pEquip));
			return FALSE;
		}


		pEquip->n16RemainNum	= GT_INVALID;
		pEquip->n16Index		= GT_INVALID;
		pEquip->byProtoIndex	= i;
		pEquip->dwRefreshTime	= dwCurTime;
		pEquip->pProto			= pEquipProto;
		pEquip->pEquip			= NULL;

		// 是否是装备
		if(!MIsEquipment(pEquipProto->dwTypeID))
		{
			return FALSE;
		}

		// 生成装备
		pEquip->pEquip = (tagEquip*)ItemCreator::Create(EICM_ShopNPC, m_dwNPCID, pEquipProto->dwTypeID, 1, GT_INVALID);
		if(!P_VALID(pEquip->pEquip))
		{
			ASSERT(P_VALID(pEquip->pEquip));
			return FALSE;
		}

		// 鉴定为白色装备
		ItemCreator::IdentifyEquip(pEquip->pEquip, EIQ_White);

		// 放到所属货架
		ASSERT(pEquipProto->byShelf < MAX_SHOP_SHELF_NUM);
		m_Shelf[pEquipProto->byShelf].Add(pEquip, n16ItemNum[pEquipProto->byShelf]++);
	}

	// 初始化限量物品起始下标
	IFASTCODE->MemCpy(m_n16RareIndexStart, n16ItemNum, sizeof(m_n16RareIndexStart));

	// 初始化限量物品
	const tagShopRareItemProto *pRareProto = NULL;
	for(j=0; j<MAX_SHOP_RARE_ITEM; ++i, ++j)
	{
		pRareProto = &pShopProto->RareItem[j];
		if(GT_INVALID == pRareProto->dwTypeID)
		{
			break;
		}

		tagShopEquip *pEquip = new tagShopEquip;
		if(!P_VALID(pEquip))
		{
			ASSERT(P_VALID(pEquip));
			return FALSE;
		}

		pEquip->n16RemainNum	= pRareProto->byRefreshNum;
		pEquip->n16Index		= GT_INVALID;
		pEquip->byProtoIndex	= j;
		pEquip->dwRefreshTime	= dwCurTime;
		pEquip->pRareProto		= pRareProto;
		pEquip->pEquip			= NULL;

		// 是否是装备
		if(!MIsEquipment(pRareProto->dwTypeID))
		{
			return FALSE;
		}

		// 生成装备
		pEquip->pEquip = (tagEquip*)ItemCreator::Create(EICM_ShopNPC, m_dwNPCID, pRareProto->dwTypeID, 1, GT_INVALID);
		if(!P_VALID(pEquip->pEquip))
		{
			ASSERT(P_VALID(pEquip->pEquip));
			return FALSE;
		}

		// 鉴定为指定品级装备
		ItemCreator::IdentifyEquip(pEquip->pEquip, (EItemQuality)pRareProto->byQuality);
		
		// 放到所属货架
		ASSERT(pRareProto->byShelf < MAX_SHOP_SHELF_NUM);
		m_Shelf[pRareProto->byShelf].Add(pEquip, n16ItemNum[pRareProto->byShelf]++);
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
VOID EquipShop::Update()
{
	// 获取当前时间
	DWORD dwCurTime = g_world.GetWorldTime();

	// 记录各页商品下标
	INT16	n16ItemNum[MAX_SHOP_SHELF_NUM];

	IFASTCODE->MemCpy(n16ItemNum, m_n16RareIndexStart, sizeof(m_n16RareIndexStart));

	// 检查限量物品是否需要更新
	tagShopEquip *pEquip = NULL;
	const tagShopRareItemProto *pRareProto = NULL;
	for(INT32 i=0; i<MAX_SHOP_RARE_ITEM; ++i)
	{
		pRareProto = &m_pShopProto->RareItem[i];
		if(GT_INVALID == pRareProto->dwTypeID)
		{
			break;
		}

		pEquip = m_Shelf[pRareProto->byShelf].GetItem(n16ItemNum[pRareProto->byShelf]++);
		ASSERT(P_VALID(pEquip));

		if(CalcTimeDiff(dwCurTime, pEquip->dwRefreshTime) >= pRareProto->dwRefreshTime)
		{
			if(P_VALID(pEquip->pEquip))
			{
				ASSERT(pEquip->pEquip->dwTypeID == pRareProto->dwTypeID);

				SAFE_DEL(pEquip->pEquip);
			}

			// 生成装备
			pEquip->pEquip = (tagEquip*)ItemCreator::Create(EICM_ShopNPC, m_dwNPCID, pRareProto->dwTypeID, 1, GT_INVALID);
			//ASSERT(P_VALID(pEquip->pEquip));

			// 鉴定为指定品级装备
			ItemCreator::IdentifyEquip(pEquip->pEquip, (EItemQuality)pRareProto->byQuality);
			
			pEquip->n16RemainNum	= pRareProto->byRefreshNum;
			pEquip->dwRefreshTime	= dwCurTime;
		}
	}
}

//-----------------------------------------------------------------------------
// 获取所有装备
//-----------------------------------------------------------------------------
VOID EquipShop::GetRareEquips(OUT tagMsgShopEquip* pEquips, BYTE byShelf, INT16 &n16RareEquipNum)
{
	ASSERT(GetRareGoodsNum(byShelf) == n16RareEquipNum);

	tagShopEquip *pShopEquip = NULL;
	INT16 n16RealNum = 0;
	for(INT16 i=0; i<n16RareEquipNum; ++i)
	{
		pShopEquip = m_Shelf[byShelf].GetItem((INT16)(i + m_n16RareIndexStart[byShelf]));
		ASSERT(P_VALID(pShopEquip));

		if(P_VALID(pShopEquip->pEquip))
		{
			pEquips[n16RealNum].n16RemainNum		= pShopEquip->n16RemainNum;
			pEquips[n16RealNum].n16Index			= pShopEquip->n16Index;
			pEquips[n16RealNum].byProtoIndex		= pShopEquip->byProtoIndex;
			
			IFASTCODE->MemCpy(&pEquips[n16RealNum].Equip, pShopEquip->pEquip, SIZE_EQUIP);
			++n16RealNum;
		}
	}

	n16RareEquipNum = n16RealNum;
}

//-----------------------------------------------------------------------------
// 出售装备
//-----------------------------------------------------------------------------
DWORD EquipShop::SellEquip(Role *pRole, BYTE byShelf, DWORD dwTypeID, INT64 n64Serial, OUT tagSellEquip &SellEquip)
{
	ASSERT(P_VALID(pRole));
	ASSERT(P_VALID(m_pShopProto));
	
	// 查找装备
	tagShopEquip *pShopEquip = m_Shelf[byShelf].GetItem(dwTypeID);
	if(!P_VALID(pShopEquip))
	{
		// 执行到这里，意味着消息中的dwTypeID非法
		ASSERT(P_VALID(pShopEquip));
		return E_Shop_ItemNotFind;
	}

	// 如果是稀有装备检查指定装备是否已经被出售
	if(pShopEquip->n16RemainNum >= 0)
	{
		if(!P_VALID(pShopEquip->pEquip) || pShopEquip->pEquip->n64Serial != n64Serial)
		{
			// 设置传出参数
			SellEquip.pEquipNew		= pShopEquip->pEquip;
			SellEquip.n16RemainNum	= pShopEquip->n16RemainNum;

			return E_Shop_Equip_Sold;
		}
	}

	// 背包中金钱
	if(pShopEquip->pProto->nSilver > pRole->GetCurMgr().GetBagSilver())
	{
		return E_Shop_NotEnough_SilverInBag;
	}
	
	// 声望阶段是否满足
	DWORD dwErrorCode = CheckSpecCost(pRole, m_pShopProto->eCostType, 
									pShopEquip->pProto->nCostNum, m_pShopProto->dwItemTypeID, 
									pShopEquip->pProto->byRepLevel, SellEquip.listItem);
	if(dwErrorCode != E_Success)
	{
		return dwErrorCode;
	}

	// 检查64位id是否合法，非法的话，则重新生成一个
	if(pShopEquip->pEquip->n64Serial < MIN_ITEM_SERIAL_INTERNAL)
	{
		ItemCreator::CreateItemSerial(pShopEquip->pEquip->n64Serial);
	}
	
	// 设置传出参数
	SellEquip.pEquipSell	= pShopEquip->pEquip;
	SellEquip.nSilverCost	= pShopEquip->pProto->nSilver;
	SellEquip.eCostType		= m_pShopProto->eCostType;
	SellEquip.nSpecCost		= pShopEquip->pProto->nCostNum;

	// 更新商店中物品个数
	if(pShopEquip->n16RemainNum != GT_INVALID)
	{
		--pShopEquip->n16RemainNum;
	}

	// 创建新装备
	if(pShopEquip->n16RemainNum != 0)
	{
		pShopEquip->pEquip = (tagEquip*)ItemCreator::Create(EICM_ShopNPC, m_dwNPCID, dwTypeID);
		if(!P_VALID(pShopEquip->pEquip))
		{
			// 执行到这里,应该是内存申请失败.(原型肯定可以找到)
			ASSERT(P_VALID(pShopEquip->pEquip));
			// return E_Shop_CreateEquip_Failed;
			return E_Success;
		}

		// 鉴定为指定品级装备
		ItemCreator::IdentifyEquip(pShopEquip->pEquip, (EItemQuality)SellEquip.pEquipSell->equipSpec.byQuality);
	}
	else
	{
		pShopEquip->pEquip = NULL;
	}

	// 设置传出参数
	SellEquip.pEquipNew		= pShopEquip->pEquip;
	SellEquip.n16RemainNum	= pShopEquip->n16RemainNum;
	SellEquip.byIndex		= (BYTE)pShopEquip->n16Index;

	return E_Success;
}