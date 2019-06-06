#pragma once

#include "shop.h"


//-----------------------------------------------------------------------------
// 氏族宝库（非装备）
//-----------------------------------------------------------------------------
class ItemClanTreasury: public Shop
{
	friend class Shop;

	typedef Container<tagTreasuryItem, DWORD>	TreasuryItemCon;
public:
	virtual BOOL	ActiveTreasure(DWORD dwTypeID, DWORD dwNameID);

private:
	ItemClanTreasury(DWORD dwNPCID, const tagShopProto *pShopProto);
	virtual ~ItemClanTreasury();

private:
	virtual VOID Update();

	virtual INT16 GetGoodsNum(BYTE byShelf) const
	{ ASSERT(byShelf < MAX_SHOP_SHELF_NUM); return m_n16ActNum[byShelf]; }

	virtual INT16	GetRareGoodsNum(BYTE byShelf) const
	{ return m_n16ActRareNum[byShelf]; }

	virtual VOID	GetRareItems(OUT tagMsgShopItem* pRareItems, INT16 n16RareItemNum, BYTE byShelf);

	virtual DWORD	SellItem(Role *pRole, BYTE byShelf, DWORD dwTypeID, INT16 n16BuyNum, OUT tagSellItem &SellItem);

private:
	BOOL Init(const tagShopProto *pShopProto);

private:
	TreasuryItemCon		m_Shelf[MAX_SHOP_SHELF_NUM];		// 商品分页
	INT16				m_n16ActNum[MAX_SHOP_SHELF_NUM];		// 激活珍宝数目
	INT16				m_n16ActRareNum[MAX_SHOP_SHELF_NUM];	// 激活稀有珍宝数目
};

//-----------------------------------------------------------------------------
// 氏族宝库（装备）
//-----------------------------------------------------------------------------
class EquipClanTreasury: public Shop
{
	friend class Shop;

	typedef Container<tagTreasuryEquip, DWORD>	TreasuryEquipCon;
public:
	virtual BOOL	ActiveTreasure(DWORD dwTypeID, DWORD dwNameID);

private:
	EquipClanTreasury(DWORD dwNPCID, const tagShopProto *pShopProto);
	virtual ~EquipClanTreasury();

private:
	virtual VOID Update();

	virtual INT16 GetGoodsNum(BYTE byShelf) const
	{ ASSERT(byShelf < MAX_SHOP_SHELF_NUM); return m_n16ActNum[byShelf]; }

	virtual INT16	GetRareGoodsNum(BYTE byShelf) const
	{ return m_n16ActRareNum[byShelf]; }

	virtual VOID	GetRareEquips(OUT tagMsgShopEquip* pEquips, BYTE byShelf, INT16 &n16RareEquipNum);

	virtual DWORD	SellEquip(Role *pRole, BYTE byShelf, DWORD dwTypeID, INT64 n64Serial, OUT tagSellEquip &SellEquip);

private:
	BOOL Init(const tagShopProto *pShopProto);

private:
	TreasuryEquipCon	m_Shelf[MAX_SHOP_SHELF_NUM];			// 商品分页
	INT16				m_n16ActNum[MAX_SHOP_SHELF_NUM];		// 激活珍宝数目
	INT16				m_n16ActRareNum[MAX_SHOP_SHELF_NUM];	// 激活稀有珍宝数目
};
