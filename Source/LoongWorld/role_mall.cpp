//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: role_mall.cpp
// author: Sxg
// actor:
// data: 2009-2-1
// last:
// brief: 商城处理
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "../WorldDefine/msg_mall.h"
#include "../WorldDefine/mall_define.h"
#include "../ServerDefine/msg_yuanbao.h"
#include "../ServerDefine/log_cmdid_define.h"

#include "role.h"
#include "mall.h"
#include "role_mgr.h"
#include "TradeYuanBao.h"
#include "guild.h"
#include "guild_mgr.h"
#include "world.h"

//-----------------------------------------------------------------------------
// 获取商城中所有商品
//-----------------------------------------------------------------------------
DWORD Role::GetMallAll(OUT DWORD &dwMallTime)
{
	// 判断商城是否开放
	if(!g_mall.IsInitOK())
	{
		return E_Mall_Close;
	}

	// 获取服务器商城加载时间
	dwMallTime = g_mall.GetMallTime();

	INT nGoodsNum, nSzMsg;

	// 普通商品
	nGoodsNum = g_mall.GetItemNum();
	if(nGoodsNum > 0)
	{
		nSzMsg = sizeof(tagNS_MallItem) - 1 + sizeof(tagMallItemProto) * nGoodsNum;
		MCREATE_MSG(pSend, nSzMsg, NS_MallItem);
		pSend->nItemNum = nGoodsNum;
		g_mall.GetAllItems(pSend->byData);

		SendMessage(pSend, pSend->dwSize);
		MDEL_MSG(pSend);
	}

	// 礼品包
	nGoodsNum = g_mall.GetPackNum();
	if(nGoodsNum > 0)
	{
		nSzMsg = sizeof(tagNS_MallPack) - 1 + sizeof(tagMallPackProto) * nGoodsNum;
		MCREATE_MSG(pSend, nSzMsg, NS_MallPack);
		pSend->nPackNum = nGoodsNum;
		g_mall.GetAllPacks(pSend->byData);

		SendMessage(pSend, pSend->dwSize);
		MDEL_MSG(pSend);
	}
	
	// 免费领取商品(只有1个)
	nGoodsNum = g_mall.GetFreeItemNum();
	if(nGoodsNum > 0)
	{
		tagNS_MallFreeItem send;
		g_mall.GetFreeItem(&send.freeItem);
		SendMessage(&send, send.dwSize);
	}

	return E_Success;
}

//-----------------------------------------------------------------------------
// 更新商城中有出售个数限制的所有商品个数
//-----------------------------------------------------------------------------
DWORD Role::UpdateMallAll(OUT DWORD &dwNewMallTime, IN DWORD dwOldMallTime)
{
	// 判断商城是否开放
	if(!g_mall.IsInitOK())
	{
		return E_Mall_Close;
	}

	// 获取服务器商城加载时间
	dwNewMallTime = g_mall.GetMallTime();

	//-- 检查客户端的商城静态属性是否与服务器相同

	// 1.不同：重新发送商店原型信息
	if(dwNewMallTime != dwOldMallTime)
	{
		return GetMallAll(dwNewMallTime);
	}

	// 2.相同：只刷新有个数限制的商品个数信息
	INT nGoodsNum, nSzMsg;

	// 普通商品
	nGoodsNum = g_mall.GetItemNum();
	if(nGoodsNum > 0)
	{
		nSzMsg = sizeof(tagNS_MallUpdateItem) - 1 + sizeof(tagMallUpdate) * nGoodsNum;
		MCREATE_MSG(pSend, nSzMsg, NS_MallUpdateItem);
		g_mall.UpdateAllItems(pSend->byData, pSend->nItemNum);

		// 重新计算消息大小
		if(pSend->nItemNum > 0)
		{
			pSend->dwSize = sizeof(tagNS_MallUpdateItem) - 1 + sizeof(tagMallUpdate) * pSend->nItemNum;
			SendMessage(pSend, pSend->dwSize);
		}

		MDEL_MSG(pSend);
	}

	// 礼品包
	nGoodsNum = g_mall.GetPackNum();
	if(nGoodsNum > 0)
	{
		nSzMsg = sizeof(tagNS_MallUpdatePack) - 1 + sizeof(tagMallUpdate) * nGoodsNum;
		MCREATE_MSG(pSend, nSzMsg, NS_MallUpdatePack);
		g_mall.UpdateAllPacks(pSend->byData, pSend->nItemNum);

		// 重新计算消息大小
		if(pSend->nItemNum > 0)
		{
			pSend->dwSize = sizeof(tagNS_MallUpdatePack) - 1 + sizeof(tagMallUpdate) * pSend->nItemNum;
			SendMessage(pSend, pSend->dwSize);
		}

		MDEL_MSG(pSend);
	}

	return E_Success;
}

//-----------------------------------------------------------------------------
// 购买商城物品
//-----------------------------------------------------------------------------
DWORD Role::BuyMallItem(DWORD dwID, INT nUnitPrice, INT16 n16BuyNum, INT nIndex)
{
	// 判断商城是否开放
	if(!g_mall.IsInitOK())
	{
		return E_Mall_Close;
	}

	// 行囊是否解锁
	if(!GetRoleStateEx().IsInState(ERSE_BagPsdPass))
	{
		return E_Con_PswNotPass;
	}
	
	// 预检查背包中是否有空位
	if(GetItemMgr().GetBagFreeSize() < 1)
	{
		return E_Bag_NotEnoughSpace;
	}

	tagMallItemSell sItemSell;

	// 商城中商品相关检查
	DWORD dwErrorCode = g_mall.SellItem(this, GetID(), ELCID_Mall_BuyItem, 
										dwID, nIndex, nUnitPrice, n16BuyNum, sItemSell);

	// 处理结果
	if(E_Success == dwErrorCode	&& sItemSell.nYuanBaoNeed > 0 && P_VALID(sItemSell.pItem))
	{
		// 元宝已在商城中扣除
		
		// 将物品放到玩家背包中
		GetItemMgr().Add2Bag(sItemSell.pItem, ELCID_Mall_BuyItem, TRUE);
		
		// 如果有赠品，则放到百宝贷中
		if(P_VALID(sItemSell.pPresent))
		{
			// 百宝袋历史记录
			GetItemMgr().ProcBaiBaoRecord(sItemSell.pPresent->dwTypeID, 
							GetNameID(), GT_INVALID, EBBRT_Mall, sItemSell.pPresent->dw1stGainTime);

			GetItemMgr().Add2BaiBao(sItemSell.pPresent, ELCID_Mall_BuyItem_Add);
		}

		// 回馈玩家赠点
		if (sItemSell.nExVolumeAssign > 0)
		{
			GetCurMgr().IncExchangeVolume(sItemSell.nExVolumeAssign, ELCID_Mall_BuyItem);
		}
	}

	// 发送更新后商店物品 -- 只有刷新物品要更新物品个数
	if((E_Success == dwErrorCode || E_Mall_Item_NotEnough == dwErrorCode) 
		&& sItemSell.byRemainNum != (BYTE)GT_INVALID)
	{
		INT nSzMsg = sizeof(tagNS_MallUpdateItem) - 1 + sizeof(tagMallUpdate);
		MCREATE_MSG(pSend, nSzMsg, NS_MallUpdateItem);
		
		pSend->nItemNum = 1;
		MTRANS_POINTER(p, pSend->byData, tagMallUpdate);
		p->byRemainNum	= sItemSell.byRemainNum;
		p->dwID			= dwID;

		SendMessage(pSend, pSend->dwSize);

		MDEL_MSG(pSend);
	}

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 购买商城礼品包
//-----------------------------------------------------------------------------
DWORD Role::BuyMallPack(DWORD dwID, INT nUnitPrice, BYTE byIndex)
{
	// 判断商城是否开放
	if(!g_mall.IsInitOK())
	{
		return E_Mall_Close;
	}

	// 行囊是否解锁
	if(!GetRoleStateEx().IsInState(ERSE_BagPsdPass))
	{
		return E_Con_PswNotPass;
	}

	// 预检查背包中是否有空位
	if(GetItemMgr().GetBagFreeSize() < 1)
	{
		return E_Bag_NotEnoughSpace;
	}

	tagMallPackSell sPackSell;

	// 商城中商品相关检查
	DWORD dwErrorCode = g_mall.SellPack(this, GetID(), ELCID_Mall_BuyPack, 
										dwID, byIndex, nUnitPrice, sPackSell, TRUE);

	// 处理结果
	if(E_Success == dwErrorCode	&& sPackSell.nYuanBaoNeed > 0 && P_VALID(sPackSell.pItem[0]))
	{
		// 将物品放到玩家背包中
		for(INT i=0; i<MALL_PACK_ITEM_NUM; ++i)
		{
			if(!P_VALID(sPackSell.pItem[i]))
			{
				break;
			}

			GetItemMgr().Add2Bag(sPackSell.pItem[i], ELCID_Mall_BuyPack, TRUE);
		}

		// 如果有赠品，则放到百宝贷中
		if(P_VALID(sPackSell.pPresent))
		{
			// 百宝袋历史记录
			GetItemMgr().ProcBaiBaoRecord(sPackSell.pPresent->dwTypeID, 
								GetNameID(), GT_INVALID, EBBRT_Mall, sPackSell.pPresent->dw1stGainTime);

			GetItemMgr().Add2BaiBao(sPackSell.pPresent, ELCID_Mall_BuyPack_Add);
		}

		// 回馈玩家赠点
		if (sPackSell.nExVolumeAssign > 0)
		{
			GetCurMgr().IncExchangeVolume(sPackSell.nExVolumeAssign, ELCID_Mall_BuyPack);
		}
	}

	// 发送更新后商店物品 -- 只有刷新物品要更新物品个数
	if((E_Success == dwErrorCode || E_Mall_Pack_NotEnough == dwErrorCode) 
		&& sPackSell.byRemainNum != (BYTE)GT_INVALID)
	{
		INT nSzMsg = sizeof(tagNS_MallUpdatePack) - 1 + sizeof(tagMallUpdate);
		MCREATE_MSG(pSend, nSzMsg, NS_MallUpdatePack);

		pSend->nItemNum = 1;
		MTRANS_POINTER(p, pSend->byData, tagMallUpdate);
		p->byRemainNum	= sPackSell.byRemainNum;
		p->dwID			= dwID;

		SendMessage(pSend, pSend->dwSize);

		MDEL_MSG(pSend);
	}

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 赠送商城物品给好友
//-----------------------------------------------------------------------------
DWORD Role::BuyMallItem(DWORD dwTgtRoleID, LPCTSTR szLeaveWord, 
						DWORD dwID, INT nUnitPrice, INT16 n16BuyNum, INT nIndex)
{
	// 判断商城是否开放
	if(!g_mall.IsInitOK())
	{
		return E_Mall_Close;
	}

	// 行囊是否解锁
	if(!GetRoleStateEx().IsInState(ERSE_BagPsdPass))
	{
		return E_Con_PswNotPass;
	}

	// 检查好友ID在游戏世界中是否存在
	if(!g_roleMgr.IsRoleBelongToWorld(dwTgtRoleID))
	{
		return E_Role_Not_ExistIn_World;
	}

	tagMallItemSell sItemSell;

	// 商城中商品相关检查
	DWORD dwErrorCode = g_mall.SellItem(this, dwTgtRoleID, ELCID_Mall_PresentItem, 
										dwID, nIndex, nUnitPrice, n16BuyNum, sItemSell);

	// 处理结果
	if(E_Success == dwErrorCode	&& sItemSell.nYuanBaoNeed > 0 && P_VALID(sItemSell.pItem))
	{
		// 百宝袋历史记录
		GetItemMgr().ProcBaiBaoRecord(sItemSell.pItem->dwTypeID, dwTgtRoleID, 
							GetNameID(), EBBRT_Friend, sItemSell.pItem->dw1stGainTime, szLeaveWord);

		// 将物品放到好友百宝袋中
		Role *pFriend = g_roleMgr.GetRolePtrByID(dwTgtRoleID);
		if(P_VALID(pFriend))
		{
			pFriend->GetItemMgr().Add2BaiBao(sItemSell.pItem, ELCID_Mall_PresentItem, GetID());
		}
		else
		{
			// 存储到item_baibao表中
			ItemMgr::InsertBaiBao2DB(sItemSell.pItem, dwTgtRoleID, ELCID_Mall_PresentItem);
			
			// 删除物品
			::Destroy(sItemSell.pItem);
		}

		// 如果有赠品，则放到好友百宝袋中
		if(P_VALID(sItemSell.pPresent))
		{
			// 百宝袋历史记录
			GetItemMgr().ProcBaiBaoRecord(sItemSell.pPresent->dwTypeID, dwTgtRoleID, 
										GetNameID(), EBBRT_Mall, sItemSell.pPresent->dw1stGainTime);

			if(P_VALID(pFriend))
			{
				pFriend->GetItemMgr().Add2BaiBao(sItemSell.pPresent, ELCID_Mall_PresentItem_Add);
			}
			else
			{
				// 存储到item_baibao表中
				ItemMgr::InsertBaiBao2DB(sItemSell.pPresent, dwTgtRoleID, ELCID_Mall_PresentItem_Add);

				// 删除物品
				::Destroy(sItemSell.pPresent);
			}
		}
		
		// 向买家回馈赠点
		if (sItemSell.nExVolumeAssign > 0)
		{
			GetCurMgr().IncExchangeVolume(sItemSell.nExVolumeAssign, ELCID_Mall_PresentItem);
		}
	}

	// 发送更新后商店物品 -- 只有刷新物品要更新物品个数
	if((E_Success == dwErrorCode || E_Mall_Item_NotEnough == dwErrorCode) 
		&& sItemSell.byRemainNum != (BYTE)GT_INVALID)
	{
		INT nSzMsg = sizeof(tagNS_MallUpdateItem) - 1 + sizeof(tagMallUpdate);
		MCREATE_MSG(pSend, nSzMsg, NS_MallUpdateItem);

		pSend->nItemNum = 1;
		MTRANS_POINTER(p, pSend->byData, tagMallUpdate);
		p->byRemainNum	= sItemSell.byRemainNum;
		p->dwID			= dwID;

		SendMessage(pSend, pSend->dwSize);

		MDEL_MSG(pSend);
	}

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 赠送商品礼品包给好友
//-----------------------------------------------------------------------------
DWORD Role::BuyMallPack(DWORD dwTgtRoleID, LPCTSTR szLeaveWord, 
						DWORD dwID, INT nUnitPrice, BYTE byIndex)
{
	// 判断商城是否开放
	if(!g_mall.IsInitOK())
	{
		return E_Mall_Close;
	}

	// 行囊是否解锁
	if(!GetRoleStateEx().IsInState(ERSE_BagPsdPass))
	{
		return E_Con_PswNotPass;
	}

	// 检查好友ID在游戏世界中是否存在
	if(!g_roleMgr.IsRoleBelongToWorld(dwTgtRoleID))
	{
		return E_Role_Not_ExistIn_World;
	}

	tagMallPackSell sPackSell;

	// 商城中商品相关检查
	DWORD dwErrorCode = g_mall.SellPack(this, dwTgtRoleID, ELCID_Mall_PresentPack, 
										dwID, byIndex, nUnitPrice, sPackSell, FALSE);

	// 处理结果
	if(E_Success == dwErrorCode	&& sPackSell.nYuanBaoNeed > 0 && P_VALID(sPackSell.pItem[0]))
	{		
		Role *pFriend = g_roleMgr.GetRolePtrByID(dwTgtRoleID);

		// 将物品放到好友百宝袋中 -- item_baibao表
		for(INT i=0; i<MALL_PACK_ITEM_NUM; ++i)
		{
			if(!P_VALID(sPackSell.pItem[i]))
			{
				break;
			}

			// 记录到第一个物品上
			if(0 == i)
			{
				// 百宝袋历史记录
				GetItemMgr().ProcBaiBaoRecord(sPackSell.pItem[i]->dwTypeID, 
					dwTgtRoleID, GetNameID(), EBBRT_Friend, sPackSell.pItem[i]->dw1stGainTime, szLeaveWord);
			}
			else
			{
				// 百宝袋历史记录(无留言)
				GetItemMgr().ProcBaiBaoRecord(sPackSell.pItem[i]->dwTypeID, 
					dwTgtRoleID, GetNameID(), EBBRT_Friend, sPackSell.pItem[i]->dw1stGainTime);
			}

			if(P_VALID(pFriend))
			{
				pFriend->GetItemMgr().Add2BaiBao(sPackSell.pItem[i], ELCID_Mall_PresentPack, GetID());
			}
			else
			{
				// 存储到item_baibao表中
				ItemMgr::InsertBaiBao2DB(sPackSell.pItem[i], dwTgtRoleID, ELCID_Mall_PresentPack);

				// 删除物品
				::Destroy(sPackSell.pItem[i]);
			}
		}

		// 如果有赠品，则放到好友百宝贷中
		if(P_VALID(sPackSell.pPresent))
		{
			// 百宝袋历史记录
			GetItemMgr().ProcBaiBaoRecord(sPackSell.pPresent->dwTypeID, 
							dwTgtRoleID, GetNameID(), EBBRT_Mall, sPackSell.pPresent->dw1stGainTime);

			if(P_VALID(pFriend))
			{
				pFriend->GetItemMgr().Add2BaiBao(sPackSell.pPresent, ELCID_Mall_PresentPack_Add, GetID());
			}
			else
			{
				// 存储到item_baibao表中
				ItemMgr::InsertBaiBao2DB(sPackSell.pPresent, dwTgtRoleID, ELCID_Mall_PresentPack_Add);

				// 删除物品
				::Destroy(sPackSell.pPresent);
			}
		}

		// 向买家回馈赠点
		if (sPackSell.nExVolumeAssign > 0)
		{
			GetCurMgr().IncExchangeVolume(sPackSell.nExVolumeAssign, ELCID_Mall_PresentPack);
		}
	}

	// 发送更新后商店物品 -- 只有刷新物品要更新物品个数
	if((E_Success == dwErrorCode || E_Mall_Pack_NotEnough == dwErrorCode) 
		&& sPackSell.byRemainNum != (BYTE)GT_INVALID)
	{
		INT nSzMsg = sizeof(tagNS_MallUpdatePack) - 1 + sizeof(tagMallUpdate);
		MCREATE_MSG(pSend, nSzMsg, NS_MallUpdatePack);

		pSend->nItemNum = 1;
		MTRANS_POINTER(p, pSend->byData, tagMallUpdate);
		p->byRemainNum	= sPackSell.byRemainNum;
		p->dwID			= dwID;

		SendMessage(pSend, pSend->dwSize);

		MDEL_MSG(pSend);
	}

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 索取商城免费物品
//-----------------------------------------------------------------------------
DWORD Role::GetMallFreeItem(DWORD dwID)
{
	// 判断商城是否开放
	if(!g_mall.IsInitOK())
	{
		return E_Mall_Close;
	}

	// 行囊是否解锁
	if(!GetRoleStateEx().IsInState(ERSE_BagPsdPass))
	{
		return E_Con_PswNotPass;
	}
	
	// 预检查背包中是否有空位
	if(GetItemMgr().GetBagFreeSize() < 1)
	{
		return E_Bag_NotEnoughSpace;
	}

	tagMallItemSell sItemSell;

	// 商城中商品相关检查
	DWORD dwErrorCode = g_mall.GrantFreeItem(this, dwID, sItemSell);

	// 处理结果
	if(E_Success == dwErrorCode && P_VALID(sItemSell.pItem))
	{
		// 将物品放到背包中
		GetItemMgr().Add2Bag(sItemSell.pItem, ELCID_Mall_FreeItem, TRUE);
	}
	
	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 兑换商城物品
//-----------------------------------------------------------------------------
DWORD Role::MallItemExchange(DWORD dwMallID, INT nPrice, INT16 n16BuyNum, BYTE byIndex)
{
	// 判断商城是否开放
	if(!g_mall.IsInitOK())
	{
		return E_Mall_Close;
	}

	// 行囊是否解锁
	if(!GetRoleStateEx().IsInState(ERSE_BagPsdPass))
	{
		return E_Con_PswNotPass;
	}

	// 预检查背包中是否有空位
	if(GetItemMgr().GetBagFreeSize() < 1)
	{
		return E_Bag_NotEnoughSpace;
	}

	tagMallItemSell sItemSell;

	// 商城中商品相关检查
	DWORD dwErrorCode = g_mall.ExchangeItem(this, ELCLD_Mall_ExchangeItem, dwMallID, byIndex, nPrice, n16BuyNum, sItemSell);

	// 处理结果
	if(E_Success == dwErrorCode	&& sItemSell.nYuanBaoNeed > 0 && P_VALID(sItemSell.pItem))
	{
		// 将物品放到玩家背包中
		GetItemMgr().Add2Bag(sItemSell.pItem, ELCLD_Mall_ExchangeItem, TRUE);

		// 如果有赠品，则放到百宝贷中
		if(P_VALID(sItemSell.pPresent))
		{
			// 百宝袋历史记录
			GetItemMgr().ProcBaiBaoRecord(sItemSell.pPresent->dwTypeID, 
				GetNameID(), GT_INVALID, EBBRT_Mall, sItemSell.pPresent->dw1stGainTime);

			GetItemMgr().Add2BaiBao(sItemSell.pPresent, ELCLD_Mall_ExchangeItem_Add);
		}

		// 回馈玩家赠点
		if (sItemSell.nExVolumeAssign > 0)
		{
			GetCurMgr().IncExchangeVolume(sItemSell.nExVolumeAssign, ELCLD_Mall_ExchangeItem);
		}
	}

	// 发送更新后商店物品 -- 只有刷新物品要更新物品个数
	if((E_Success == dwErrorCode || E_Mall_Item_NotEnough == dwErrorCode) 
		&& sItemSell.byRemainNum != (BYTE)GT_INVALID)
	{
		INT nSzMsg = sizeof(tagNS_MallUpdateItem) - 1 + sizeof(tagMallUpdate);
		MCREATE_MSG(pSend, nSzMsg, NS_MallUpdateItem);

		pSend->nItemNum = 1;
		MTRANS_POINTER(p, pSend->byData, tagMallUpdate);
		p->byRemainNum	= sItemSell.byRemainNum;
		p->dwID			= dwMallID;

		SendMessage(pSend, pSend->dwSize);

		MDEL_MSG(pSend);
	}

	return dwErrorCode;

}

//-----------------------------------------------------------------------------
// 兑换商城打包物品
//-----------------------------------------------------------------------------
DWORD Role::MallPackExchange(DWORD dwMallID, INT nPrice, BYTE byIndex)
{
	// 判断商城是否开放
	if(!g_mall.IsInitOK())
	{
		return E_Mall_Close;
	}

	// 行囊是否解锁
	if(!GetRoleStateEx().IsInState(ERSE_BagPsdPass))
	{
		return E_Con_PswNotPass;
	}

	// 预检查背包中是否有空位
	if(GetItemMgr().GetBagFreeSize() < 1)
	{
		return E_Bag_NotEnoughSpace;
	}

	tagMallPackSell sPackSell;

	// 商城中商品相关检查
	DWORD dwErrorCode = g_mall.ExchangePack(this, ELCLD_Mall_ExchangePack, dwMallID, byIndex, nPrice, sPackSell);

	// 处理结果
	if(E_Success == dwErrorCode	&& sPackSell.nYuanBaoNeed > 0 && P_VALID(sPackSell.pItem[0]))
	{
		INT64 n64Serial = sPackSell.pItem[0]->n64Serial;
		INT16 n16BuyNum = sPackSell.pItem[0]->n16Num;
		DWORD dwFstGainTime = g_world.GetWorldTime();

		// 将物品放到玩家背包中
		for(INT i=0; i<MALL_PACK_ITEM_NUM; ++i)
		{
			if(!P_VALID(sPackSell.pItem[i]))
			{
				break;
			}

			GetItemMgr().Add2Bag(sPackSell.pItem[i], ELCLD_Mall_ExchangePack, TRUE);
		}

		// 如果有赠品，则放到百宝贷中
		if(P_VALID(sPackSell.pPresent))
		{
			// 百宝袋历史记录
			GetItemMgr().ProcBaiBaoRecord(sPackSell.pPresent->dwTypeID, 
				GetNameID(), GT_INVALID, EBBRT_Mall, dwFstGainTime);

			GetItemMgr().Add2BaiBao(sPackSell.pPresent, ELCLD_Mall_ExchangePack_Add);
		}

		// 回馈玩家赠点
		if (sPackSell.nExVolumeAssign > 0)
		{
			GetCurMgr().IncExchangeVolume(sPackSell.nExVolumeAssign, ELCLD_Mall_ExchangePack);
		}
	}

	// 发送更新后商店物品 -- 只有刷新物品要更新物品个数
	if((E_Success == dwErrorCode || E_Mall_Pack_NotEnough == dwErrorCode) 
		&& sPackSell.byRemainNum != (BYTE)GT_INVALID)
	{
		INT nSzMsg = sizeof(tagNS_MallUpdatePack) - 1 + sizeof(tagMallUpdate);
		MCREATE_MSG(pSend, nSzMsg, NS_MallUpdatePack);

		pSend->nItemNum = 1;
		MTRANS_POINTER(p, pSend->byData, tagMallUpdate);
		p->byRemainNum	= sPackSell.byRemainNum;
		p->dwID			= dwMallID;

		SendMessage(pSend, pSend->dwSize);

		MDEL_MSG(pSend);
	}

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 玩家向元宝交易账户存元宝
//-----------------------------------------------------------------------------
DWORD Role::SaveYB2Account(DWORD dwID, INT nNum)
{
	// 检查玩家背包元宝数量
	if(GetCurMgr().GetBagYuanBao() < nNum)
		return E_Trade_BagYB_NotEnough;

	tagYuanBaoAccount* pYBAccount = g_tradeYB.GetYBAccount(dwID);
	if(!P_VALID(pYBAccount))
	{
		pYBAccount = g_tradeYB.CreateTradeAccount(dwID);
		if(!P_VALID(pYBAccount))
			return GT_INVALID;
	}

	pYBAccount->IncAccountYB(nNum, (DWORD)ELCID_Trade_SaveYuanBao, TRUE);
	GetCurMgr().DecBagYuanBao(nNum, (DWORD)ELCID_Trade_SaveYuanBao);
		
	return E_Success;
}

//-----------------------------------------------------------------------------
// 玩家向元宝交易账户存金钱
//-----------------------------------------------------------------------------
DWORD Role::SaveSilver2Account(DWORD dwID, INT64 nNum)
{
	// 检查玩家背包金钱数量
	if(GetCurMgr().GetBagSilver() < nNum)
		return E_Trade_BagSilver_NotEnough;

	tagYuanBaoAccount* pYBAccount = g_tradeYB.GetYBAccount(dwID);
	if(!P_VALID(pYBAccount))
	{
		pYBAccount = g_tradeYB.CreateTradeAccount(dwID);
		if(!P_VALID(pYBAccount))
			return GT_INVALID;
	}

	pYBAccount->IncAccountSilver(nNum, (DWORD)ELCID_Trade_SaveSilver, TRUE);
	GetCurMgr().DecBagSilver(nNum, (DWORD)ELCID_Trade_SaveSilver);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 玩家向元宝交易账户取元宝
//-----------------------------------------------------------------------------
DWORD Role::DepositYBAccout(DWORD dwID, INT nNum)
{
	tagYuanBaoAccount* pYBAccount = g_tradeYB.GetYBAccount(dwID);
	if(!P_VALID(pYBAccount))
		return GT_INVALID;

	// 检查账户元宝数量
	if(pYBAccount->GetAccountYB() < nNum)
		return E_Trade_AccountYB_NotEnough;

	// 检查玩家是否提交过出售订单
	tagYuanBaoOrder *pYBOrder = g_tradeYB.GetYBSellOrder(dwID);
	if(P_VALID(pYBOrder))
		return GT_INVALID;

	pYBAccount->DecAccountYuanBao(nNum, (DWORD)ELCID_Trade_DepositYuanBao, TRUE);
	GetCurMgr().IncBagYuanBao(nNum, (DWORD)ELCID_Trade_DepositYuanBao);
	
	return E_Success;
}

//-----------------------------------------------------------------------------
// 玩家向元宝交易账户取金钱
//-----------------------------------------------------------------------------
DWORD Role::DepositSilverAccount(DWORD dwID, INT64 nNum)
{
	tagYuanBaoAccount* pYBAccount = g_tradeYB.GetYBAccount(dwID);
	if(!P_VALID(pYBAccount))
		return GT_INVALID;

	// 检查账户金钱数量
	if(pYBAccount->GetAccountSilver() < nNum)
		return E_Trade_AccountSilver_NotEnough;

	// 检查玩家是否提交过收购订单
	tagYuanBaoOrder *pYBOrder = g_tradeYB.GetYBBuyOrder(dwID);
	if(P_VALID(pYBOrder))
		return GT_INVALID;

	pYBAccount->DecAccountSilver(nNum, (DWORD)ELCID_Trade_DepositSilver, TRUE);
	GetCurMgr().IncBagSilver(nNum, (DWORD)ELCID_Trade_DepositSilver);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 同步元宝交易初始化信息
//-----------------------------------------------------------------------------
DWORD Role::GetYBTradeInfo()
{
	g_tradeYB.SynBuyPriceList(this);
	g_tradeYB.SynSellPriceList(this);
	g_tradeYB.SynYBAccount(this);
	return E_Success;
}

//-----------------------------------------------------------------------------
// 玩家提交元宝出售订单
//-----------------------------------------------------------------------------
DWORD Role::SubmitSellOrder(DWORD dwRoleID, INT nNum, INT nPrice)
{
	tagYuanBaoAccount* pYBAccount = g_tradeYB.GetYBAccount(dwRoleID);
	if(!P_VALID(pYBAccount))
		return GT_INVALID;

	if(nNum <= 0 || nPrice <= 0)
		return GT_INVALID;
		
	// 是否已经提交过出售订单
	tagYuanBaoOrder* pSellOrder = (tagYuanBaoOrder*)GT_INVALID;
	pSellOrder = g_tradeYB.GetYBSellOrder(dwRoleID);
	if(P_VALID(pSellOrder))
		return E_Trade_SellOrder_Exit;

	// 交易账户元宝是否足够
	if(pYBAccount->GetAccountYB() < nNum)
		return E_Trade_AccountYB_NotEnough;

	// 交易手续费为总价的2%
	INT nTax = nPrice * nNum * 2 / 100;
	if(nTax < 1)    nTax = 1;

	// 玩家手续费是否足够
	if (GetCurMgr().GetBagSilver() < nTax)
		return E_Trade_Tax_NotEnough;


	tagYuanBaoOrder * pYBOrder = g_tradeYB.CreateYBOrder(dwRoleID, EYBOT_SELL, nPrice, nNum);
	if(!P_VALID(pYBOrder))
		return GT_INVALID;

	// 设置账户中订单的提交状态
	pYBAccount->SetSellOrder(TRUE);

	// 扣除手续费 
	GetCurMgr().DecBagSilver(nTax, ELCID_Trade_Tax);

	// 出售元宝
	g_tradeYB.DealYBSell(pYBOrder);
	
	return E_Success;
}

//-----------------------------------------------------------------------------
// 玩家提交元宝收购订单
//-----------------------------------------------------------------------------
DWORD Role::SubmitBuyOrder(DWORD dwRoleID, INT nNum, INT nPrice)
{
	tagYuanBaoAccount *pYBAccount = g_tradeYB.GetYBAccount(dwRoleID);
	if(!P_VALID(pYBAccount))
		return GT_INVALID;

	if(nNum <= 0 || nPrice <= 0)
		return GT_INVALID;

	if(nNum * nPrice <= 0)
		return E_Trade_AccountSilver_NotEnough;

	// 是否已近提交过订单
	tagYuanBaoOrder* pBuyOrder = (tagYuanBaoOrder*)GT_INVALID;
	pBuyOrder = g_tradeYB.GetYBBuyOrder(dwRoleID);
	if(P_VALID(pBuyOrder))
		return E_Trade_BuyOrder_Exit;

	// 交易账户金钱是否足够
	if(pYBAccount->GetAccountSilver() < nNum * nPrice)
		return E_Trade_AccountSilver_NotEnough;

	// 交易手续费为总价的2%
	INT nTax = (FLOAT)(nPrice * nNum) * 0.02f;
	if(nTax < 1)    nTax = 1;

	// 玩家手续费是否足够
	if (GetCurMgr().GetBagSilver() < nTax)
		return E_Trade_Tax_NotEnough;

	tagYuanBaoOrder *pYBOrder = g_tradeYB.CreateYBOrder(dwRoleID, EYBOT_BUY, nPrice, nNum);
	if(!P_VALID(pYBOrder))
		return GT_INVALID;

	// 设置账户中订单的提交状态
	pYBAccount->SetBuyOrder(TRUE);

	// 扣除手续费 
	GetCurMgr().DecBagSilver(nTax, ELCID_Trade_Tax);

	// 购买元宝
	g_tradeYB.DealYBBuy(pYBOrder);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 删除订单
//-----------------------------------------------------------------------------
DWORD Role::DeleteOrder(DWORD dwRoleID, DWORD dwOrderID, EYBOTYPE eYBOType)
{
	if(eYBOType != EYBOT_BUY && eYBOType != EYBOT_SELL)
		return GT_INVALID;

	tagYuanBaoOrder *pYBOrder = (tagYuanBaoOrder*)GT_INVALID;
	if(eYBOType == EYBOT_BUY)
		pYBOrder = g_tradeYB.GetYBBuyOrder(dwRoleID);
	else
		pYBOrder = g_tradeYB.GetYBSellOrder(dwRoleID);

	if(!P_VALID(pYBOrder))
		return GT_INVALID;

	if(pYBOrder->dwID != dwOrderID)
		return GT_INVALID;

	g_tradeYB.DeleteYBOrder(pYBOrder, EYBOM_Cancel);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 查询一周内该玩家的元宝交易订单
//-----------------------------------------------------------------------------
DWORD Role::GetYBOrder(DWORD dwRoleID)
{
	tagYuanBaoAccount *pYBAccount = g_tradeYB.GetYBAccount(dwRoleID);
	if(!P_VALID(pYBAccount))
		return  GT_INVALID;

	DWORD dwCurTick = g_world.GetWorldTick();
	if(dwCurTick - pYBAccount->GetQuestTick() > 50)
		pYBAccount->SetQuestTick(dwCurTick);
	else
		return GT_INVALID;

	// 向数据库发送查询消息
	tagNDBC_GetRoleYBOrder	sendDB;
	sendDB.dwRoleID = dwRoleID;
	g_dbSession.Send(&sendDB, sendDB.dwSize);

	return E_Success;
}