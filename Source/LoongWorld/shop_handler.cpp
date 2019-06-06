//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: shop_handler.h
// author: Sxg
// actor:
// data: 2008-10-28
// last:
// brief: 商店消息处理
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "../WorldDefine/shop_define.h"
#include "../WorldDefine/msg_shop.h"
#include "player_session.h"
#include "role.h"

//-----------------------------------------------------------------------------
// 获取物品(非装备)店中刷新商品列表
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleGetShopItems(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_GetShopItems);

	// 消息合法性验证
	if(pRecv->byShelf >= MAX_SHOP_SHELF_NUM)
	{
		return GT_INVALID;
	}

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	DWORD dwErrorCode = pRole->GetShopItems(pRecv->dwNPCID, pRecv->byShelf);

	// 处理结果反馈
	pRole->SendShopFeedbackMsg(dwErrorCode, pRecv->dwNPCID);
	
	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 获取装备店装备列表
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleGetShopEquips(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_GetShopEquips);

	// 消息合法性验证
	if(pRecv->byShelf >= MAX_SHOP_SHELF_NUM)
	{
		return GT_INVALID;
	}

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	DWORD dwErrorCode = pRole->GetShopEquips(pRecv->dwNPCID, pRecv->byShelf);

	// 处理结果反馈
	pRole->SendShopFeedbackMsg(dwErrorCode, pRecv->dwNPCID);

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 购买物品
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleBuyShopItem(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_BuyShopItem);

	// 消息合法性验证
	if(pRecv->byShelf >= MAX_SHOP_SHELF_NUM
		|| pRecv->n16ItemNum < 0 || MIsEquipment(pRecv->dwTypeID))
	{
		return GT_INVALID;
	}

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	DWORD dwErrorCode =	pRole->BuyShopItem(pRecv->dwNPCID, pRecv->byShelf, 
										pRecv->dwTypeID, pRecv->n16ItemNum);

	// 处理结果反馈
	pRole->SendShopFeedbackMsg(dwErrorCode, pRecv->dwNPCID);

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 购买装备
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleBuyShopEquip(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_BuyShopEquip);

	// 消息合法性验证
	if(pRecv->byShelf >= MAX_SHOP_SHELF_NUM)
	{
		return GT_INVALID;
	}

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	DWORD dwErrorCode = pRole->BuyShopEquip(pRecv->dwNPCID, pRecv->byShelf, 
										pRecv->dwTypeID, pRecv->n64Serial);

	// 处理结果反馈
	pRole->SendShopFeedbackMsg(dwErrorCode, pRecv->dwNPCID);

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 出售物品&装备
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleSellToShop(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_SellToShop);

	// 消息合法性验证: 无

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	DWORD dwErrorCode = pRole->SellToShop(pRecv->dwNPCID, pRecv->n64Serial);

	// 处理结果反馈
	pRole->SendShopFeedbackMsg(dwErrorCode, pRecv->dwNPCID);

	return dwErrorCode;
}