//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: shop.h
// author: Sxg
// actor:
// data: 2008-10-22
// last:
// brief: 商店类
//-----------------------------------------------------------------------------
#pragma once

#include "container_template.h"
#include "../WorldDefine/shop_define.h"

class Role;
//-----------------------------------------------------------------------------
// 方法中传出参数结构定义
//-----------------------------------------------------------------------------
struct tagShopSell
{
	INT32				nSilverCost;
	ECurCostType		eCostType;
	INT32				nSpecCost;
	INT16				n16RemainNum;
	BYTE				byIndex;
	BYTE				byDummy;
	TList<tagItem*>		listItem;

	tagShopSell()
	{
		nSilverCost		= 0;
		eCostType		= ECCT_Null;
		nSpecCost		= 0;
		n16RemainNum	= 0;
		byIndex			= 0;
	}
};

struct tagSellItem: public tagShopSell
{
	tagItem				*pItemSell;

	tagSellItem(): tagShopSell() { pItemSell = NULL; }
};

struct tagSellEquip: public tagShopSell
{
	tagEquip			*pEquipSell;
	tagEquip			*pEquipNew;

	tagSellEquip(): tagShopSell() { pEquipSell = NULL; pEquipNew = NULL; }
};


//-----------------------------------------------------------------------------
// 商店基类
//-----------------------------------------------------------------------------
class Shop
{
protected:
	Shop(DWORD dwNPCID, const tagShopProto *pShopProto);
	virtual ~Shop();

public:
	static Shop* Create(DWORD dwNPCID, DWORD dwShopID);
	static VOID	 Delete(Shop *&pShop) { SAFE_DEL(pShop); }

public:
	virtual VOID	Update() = 0;
	virtual INT16	GetGoodsNum(BYTE byShelf) const = 0;

	virtual BOOL	ActiveTreasure(DWORD dwTypeID, DWORD dwNameID) { ASSERT(0); return FALSE;}

	virtual VOID	GetRareItems(OUT tagMsgShopItem* pRareItems, INT16 n16RareItemNum, BYTE byShelf) { ASSERT(0); }
	virtual VOID	GetRareEquips(OUT tagMsgShopEquip* pEquips, BYTE byShelf, INT16 &n16RareEquipNum) { ASSERT(0); }
	
	virtual DWORD	SellItem(Role *pRole, BYTE byShelf, DWORD dwTypeID, INT16 n16BuyNum, OUT tagSellItem &SellItem) { ASSERT(0); return GT_INVALID; }
	virtual DWORD	SellEquip(Role *pRole, BYTE byShelf, DWORD dwTypeID, INT64 n64Serial, OUT tagSellEquip &SellEquip) { ASSERT(0); return GT_INVALID; }

	virtual INT16	GetRareGoodsNum(BYTE byShelf) const
	{ return GetGoodsNum(byShelf) - m_n16RareIndexStart[byShelf]; }

	BOOL	IsEquipShop() const
	{ ASSERT(P_VALID(m_pShopProto)); return m_pShopProto->bEquip; }

	BOOL	IsTreasury() const
	{ ASSERT(P_VALID(m_pShopProto)); return m_pShopProto->bClanTreasury; }

	

protected:
	DWORD	CheckSpecCost(Role *pRole, ECurCostType eCostType, INT nSpecCost, 
					DWORD dwCostItemID, BYTE byRepLevelLimit, OUT TList<tagItem*> &listItem);

private:
	BOOL	IsInitOK() const { return m_bInitOK; }

protected:
	BOOL				m_bInitOK;

	DWORD				m_dwNPCID;			// NPC ID
	const tagShopProto	*m_pShopProto;		// 指向商店静态数据

	INT16	m_n16RareIndexStart[MAX_SHOP_SHELF_NUM];	// 稀有商品在第x页中的起始下标
};

//-----------------------------------------------------------------------------
// 物品(非装备)商店
//-----------------------------------------------------------------------------
class ItemShop: public Shop
{
friend class Shop;

typedef Container<tagShopItem, DWORD>	ShopItemCon;

private:
	ItemShop(DWORD dwNPCID, const tagShopProto *pShopProto);
	virtual ~ItemShop();

private:
	virtual VOID Update();

	virtual INT16 GetGoodsNum(BYTE byShelf) const
	{ ASSERT(byShelf < MAX_SHOP_SHELF_NUM); return m_Shelf[byShelf].GetCurSpaceSize(); }

	virtual VOID	GetRareItems(OUT tagMsgShopItem* pRareItems, INT16 n16RareItemNum, BYTE byShelf);

	virtual DWORD	SellItem(Role *pRole, BYTE byShelf, DWORD dwTypeID, INT16 n16BuyNum, OUT tagSellItem &SellItem);
	
private:
	BOOL Init(const tagShopProto *pShopProto);

private:
	ShopItemCon		m_Shelf[MAX_SHOP_SHELF_NUM];	// 商品分页
};

//-----------------------------------------------------------------------------
// 装备商店
//-----------------------------------------------------------------------------
class EquipShop: public Shop
{
friend class Shop;

typedef Container<tagShopEquip, DWORD>	ShopEquipCon;

private:
	EquipShop(DWORD dwNPCID, const tagShopProto *pShopProto);
	virtual ~EquipShop();

private:
	virtual VOID Update();

	virtual INT16 GetGoodsNum(BYTE byShelf) const
	{ ASSERT(byShelf < MAX_SHOP_SHELF_NUM); return m_Shelf[byShelf].GetCurSpaceSize(); }

	virtual VOID	GetRareEquips(OUT tagMsgShopEquip* pEquips, BYTE byShelf, INT16 &n16RareEquipNum);

	virtual DWORD	SellEquip(Role *pRole, BYTE byShelf, DWORD dwTypeID, INT64 n64Serial, OUT tagSellEquip &SellEquip);

private:
	BOOL Init(const tagShopProto *pShopProto);

private:
	ShopEquipCon	m_Shelf[MAX_SHOP_SHELF_NUM];	// 商品分页
};