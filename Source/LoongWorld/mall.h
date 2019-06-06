//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: mall.h
// author: Sxg
// actor:
// data: 2009-1-22
// last:
// brief: 商城类 -- 可动态更新商场物品
//-----------------------------------------------------------------------------
#pragma once

class Role;
class GuildPurchase;

struct tagItem;
//-----------------------------------------------------------------------------
// 方法中传出参数定义
//-----------------------------------------------------------------------------
struct tagMallItemSell
{
	tagItem		*pItem;
	tagItem		*pPresent;
	INT			nYuanBaoNeed;		//兑换时代表赠点数量
	INT32		nExVolumeAssign;
	BYTE		byRemainNum;
	
	tagMallItemSell() { ZeroMemory(this, sizeof(*this)); }
};

struct tagMallPackSell
{
	tagItem		*pItem[MALL_PACK_ITEM_NUM];
	tagItem		*pPresent;
	INT			nYuanBaoNeed;		//兑换时代表赠点数量
	INT32		nExVolumeAssign;
	BYTE		byRemainNum;
	
	tagMallPackSell() { ZeroMemory(this, sizeof(*this)); }
};

//-----------------------------------------------------------------------------
// 商城商品类型
//----------------------------------------------------------------------------
enum EMallItemType
{
	EMIT_Item,			// 商品
	EMIT_Pack,			// 打包商品
	EMIT_FreeItem,		// 免费物品

	EMIT_End
};

//-----------------------------------------------------------------------------
// 商城类
//-----------------------------------------------------------------------------
class Mall
{
public:
	Mall();
	~Mall();

	// 打开关闭商城
	BOOL Init();
	VOID Destroy();

	// 更新资源，并重新打开商城
	DWORD ReInit();

	// 团购信息更新
	VOID Update();

public:
	BOOL IsInitOK()			const { return m_bInitOK; }
	INT  GetItemNum()		const { return m_nItemNum; }
	INT  GetPackNum()		const { return m_nPackNum; }
	INT	 GetFreeItemNum()	const { return (m_pMallFreeItem->pMallFreeItem->dwTypeID != GT_INVALID) ? 1 : 0; }

	DWORD GetMallTime()		const { return m_dwLoadTime; }

	const tagMallGoods* GetMallItem(BYTE byIndex, EMallItemType eType = EMIT_Item);

public:
	// 同步
	DWORD GetAllItems(OUT LPVOID pData);
	DWORD GetAllPacks(OUT LPVOID pData);
	DWORD GetFreeItem(OUT LPVOID pData);
	DWORD UpdateAllItems(OUT LPVOID pData, OUT INT &nRefreshNum);
	DWORD UpdateAllPacks(OUT LPVOID pData, OUT INT &nRefreshNum);
		
	// 出售
	DWORD SellItem(Role *pRole, DWORD dwToRoleID, DWORD dwCmdID, DWORD dwID, INT nIndex, 
				INT nUnitPrice, INT16 n16BuyNum, OUT tagMallItemSell &itemSell);
	DWORD SellPack(Role *pRole, DWORD dwToRoleID, DWORD dwCmdID, DWORD dwID, BYTE byIndex, 
				INT nUnitPrice, OUT tagMallPackSell &packSell, BOOL bNeedCheckBagSpace);

	// 兑换
	DWORD ExchangeItem(Role *pRole, DWORD dwCmdID, DWORD dwID, BYTE byIndex, 
		INT nPrice, INT16 n16BuyNum, OUT tagMallItemSell &itemSell);
	DWORD ExchangePack(Role *pRole, DWORD dwCmdID, DWORD dwID, BYTE byIndex, 
		INT nPrice, OUT tagMallPackSell &packSell);

	// 免费发放
	DWORD GrantFreeItem(Role *pRole, DWORD dwID, OUT tagMallItemSell &itemSell);

	// 帮派团购信息管理
	DWORD LoadAllGPInfo(INT nGPInfoNum, LPVOID pData);
	DWORD LaunchGroupPurchase(Role *pRole, DWORD dwID, BYTE byScope,
		BYTE byIndex, INT nUnitPrice);
	DWORD RespondGroupPurchase(Role *pRole, DWORD dwGuildID, DWORD dwID, DWORD dwRoleID,
		INT nPrice);
	DWORD GetAllSimGPInfo(Role* pRole);
	DWORD GetParticipators(Role* pRole, DWORD dwGuildID, DWORD dwID, DWORD dwRoleID);
	VOID RemoveGuildPurchaseInfo(DWORD dwGuildID);

private:
	VOID InitItem();
	VOID InitPack();
	BOOL CheckPack();

public:
	// log
	VOID LogMallSell(DWORD dwBuyRoleID, DWORD dwToRoleID, 
					 const tagItem& item, INT64 n64Serial, INT16 n16Num, 
					 DWORD dwFstGainTime, INT nCostYuanBao, INT nCostExVolume, DWORD dwCmdID);
	VOID LogMallSellPack(DWORD dwPackID, DWORD dwBuyRoleID, DWORD dwToRoleID, INT nCostYuanBao);

private:
	BOOL			m_bInitOK;			// 商城开放状态
	DWORD			m_dwLoadTime;		// 读取商城物品原型时间(tagDwordTime)
	DWORD			m_dwTimeKeeper;		// 计时器
	BYTE			m_byMinuteTime;		// 分钟计时

private:
	INT				m_nItemNum;			// 物品个数
	INT				m_nPackNum;			// 礼品包个数
	tagMallGoods	*m_pMallItem;		// 物品数组指针
	tagMallGoods	*m_pMallPack;		// 礼品包数组指针
	tagMallGoods	*m_pMallFreeItem;	// 免费领取物品

private:
	// 帮派团购信息管理
	TMap<DWORD, GuildPurchase*>	m_mapGuildPurchase;

private:
	// 统计
	INT				m_nFreeGrantNum;
};

extern Mall g_mall;