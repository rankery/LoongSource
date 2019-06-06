//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: mall_handler.cpp
// author: Sxg
// actor:
// data: 2009-2-3
// last:
// brief: 商城消息处理
//-----------------------------------------------------------------------------
#include "StdAfx.h"

#include "../WorldDefine/msg_mall.h"
#include "player_session.h"
#include "role.h"
#include "mall.h"
#include "world_net_cmd_mgr.h"

//-----------------------------------------------------------------------------
// 常量
//-----------------------------------------------------------------------------
const UINT32 MAX_REQUEST_GPINFO_PER_TICK = 100;

//-----------------------------------------------------------------------------
// 获取商场商品
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleMallGet(tagNetCmd* pCmd)
{
	//MGET_MSG(pRecv, pCmd, NC_MallGet);

	// 消息合法性验证: 无

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	tagNS_OpenMall send;
	send.dwErrorCode = pRole->GetMallAll(send.dwMallTime);
	SendMessage(&send, send.dwSize);

	return send.dwErrorCode;
}

//-----------------------------------------------------------------------------
// 更新商城
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleMallUpdate(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_MallUpdate);

	// 消息合法性验证
	if(GT_INVALID == pRecv->dwMallTime)
	{
		return GT_INVALID;
	}

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	//tagNS_MallUpdate send;
	//send.dwMallTime = GT_INVALID;
	//pRole->UpdateMallAll(send.dwMallTime, pRecv->dwMallTime);
	//SendMessage(&send, send.dwSize);

	tagNS_OpenMall send;
	send.dwErrorCode = E_Success;
	pRole->UpdateMallAll(send.dwMallTime, pRecv->dwMallTime);
	SendMessage(&send, send.dwSize);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 购买物品
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleMallBuyItem(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_MallBuyItem);

	// 消息合法性验证
	if(pRecv->n16BuyNum <= 0 || pRecv->nPrice <= 0)
	{
		return GT_INVALID;
	}

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	tagNS_MallBuyItem send;
	send.dwErrorCode = pRole->BuyMallItem(pRecv->dwTypeID, pRecv->nPrice, pRecv->n16BuyNum, pRecv->nIndexInServer);
	SendMessage(&send, send.dwSize);

	return send.dwErrorCode;
}

//-----------------------------------------------------------------------------
// 购买礼品包
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleMallBuyPack(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_MallBuyPack);

	// 消息合法性验证
	if(pRecv->n16BuyNum != 1 || pRecv->nPrice <= 0)
	{
		return GT_INVALID;
	}

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	tagNS_MallBuyPack send;
	send.dwErrorCode = pRole->BuyMallPack(pRecv->dwTypeID, pRecv->nPrice, pRecv->nIndexInServer);
	SendMessage(&send, send.dwSize);

	return send.dwErrorCode;
}

//-----------------------------------------------------------------------------
// 向好友赠送物品
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleMallPresentItem(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_MallPresentItem);

	// 消息合法性验证
	if(pRecv->n16BuyNum <= 0 || pRecv->nPrice <= 0)
	{
		return GT_INVALID;
	}

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	MGET_MSG_STRING(strLeave, pRecv->szLeave, pRecv->dwSize, NC_MallPresentItem, szLeave);

	tagNS_MallPresentItem send;
	send.dwErrorCode = pRole->BuyMallItem(pRecv->dwRoleTgtID, strLeave.c_str(), 
								pRecv->dwTypeID, pRecv->nPrice, pRecv->n16BuyNum, pRecv->nIndexInServer);
	SendMessage(&send, send.dwSize);

	return send.dwErrorCode;
}

//-----------------------------------------------------------------------------
// 向好友赠送礼品包
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleMallPresentPack(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_MallPresentPack);

	// 消息合法性验证
	if(pRecv->n16BuyNum <= 0 || pRecv->nPrice <= 0)
	{
		return GT_INVALID;
	}

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	MGET_MSG_STRING(strLeave, pRecv->szLeave, pRecv->dwSize, NC_MallPresentPack, szLeave);

	tagNS_MallPresentPack send;
	send.dwErrorCode = pRole->BuyMallPack(pRecv->dwRoleTgtID, strLeave.c_str(), 
								pRecv->dwTypeID, pRecv->nPrice, pRecv->nIndexInServer);
	SendMessage(&send, send.dwSize);

	return send.dwErrorCode;
}

//-----------------------------------------------------------------------------
// 获取免费物品
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleMallFreeGetItem(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_MallFreeGetItem);

	// 消息合法性验证: 无

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	tagNS_MallFreeGetItem send;
	send.dwErrorCode = pRole->GetMallFreeItem(pRecv->dwTypeID);
	SendMessage(&send, send.dwSize);

	return send.dwErrorCode;
}

//-----------------------------------------------------------------------------
// 发起团购
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleMallLaunchGroupPurchase(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_LaunchGroupPurchase);

	// 消息合法性验证
	if(pRecv->nPrice <= 0)
	{
		return GT_INVALID;
	}

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	tagNS_LaunchGroupPurchase send;
	send.dwErrorCode = g_mall.LaunchGroupPurchase(pRole, pRecv->dwTypeID, pRecv->byScope, pRecv->nIndexInServer, pRecv->nPrice);
	SendMessage(&send, send.dwSize);

	return send.dwErrorCode;
}

//-----------------------------------------------------------------------------
// 响应团购
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleMallRespondGroupPurchase(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_RespondGroupPurchase);

	// 消息合法性验证
	if(pRecv->nPrice <= 0)
	{
		return GT_INVALID;
	}

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	tagNS_RespondGroupPurchase send;
	send.nGroupPurchaseKey = pRecv->dwRoleID;
	send.nGroupPurchaseKey = (send.nGroupPurchaseKey << 32) | pRecv->dwTypeID;

	send.dwErrorCode = g_mall.RespondGroupPurchase(pRole, pRecv->dwGuildID, pRecv->dwTypeID, pRecv->dwRoleID, pRecv->nPrice);

	SendMessage(&send, send.dwSize);

	return send.dwErrorCode;
}

//-----------------------------------------------------------------------------
// 获取玩家所在帮派的团购信息
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleMallGetGroupPurchaseInfo(tagNetCmd* pCmd)
{
	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 判断服务器是否繁忙
	UINT32 nCount = g_worldNetCmdMgr.GetRunTimesPerTick(pCmd->dwID);
	if (nCount < 0)
	{
		IMSG(_T("Msg runtimes status error!\r\n"));
		return GT_INVALID;
	}

	if (nCount > MAX_REQUEST_GPINFO_PER_TICK)
	{
		tagNS_GetSimGPInfo send;
		send.dwErrorCode = E_GroupPurchase_ServerBusy;
		SendMessage(&send, send.dwSize);
		return send.dwErrorCode;
	}

	return g_mall.GetAllSimGPInfo(pRole);
}

//-----------------------------------------------------------------------------
// 获取指定团购信息中的响应者列表
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleMallGetParticipators(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_GetParticipators);

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	return g_mall.GetParticipators(pRole, pRecv->dwGuildID, pRecv->dwTypeID, pRecv->dwRoleID);
}

//-----------------------------------------------------------------------------
// 商城物品兑换
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleMallItemExchange(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_MallItemExchange);

	// 消息合法性验证
	if(pRecv->n16BuyNum <= 0 || pRecv->nPrice <= 0)
	{
		return GT_INVALID;
	}

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	tagNS_MallItemExchange send;
	send.dwErrorCode = pRole->MallItemExchange(pRecv->dwMallID, pRecv->nPrice, pRecv->n16BuyNum, pRecv->nIndexInServer);
	SendMessage(&send, send.dwSize);

	return send.dwErrorCode;
}

//-----------------------------------------------------------------------------
// 商城打包物品兑换
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleMallPackExchange(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_MallPackExchange);

	// 消息合法性验证
	if(pRecv->nPrice <= 0)
	{
		return GT_INVALID;
	}

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	tagNS_MallPackExchange send;
	send.dwErrorCode = pRole->MallPackExchange(pRecv->dwMallID, pRecv->nPrice, pRecv->nIndexInServer);
	SendMessage(&send, send.dwSize);

	return send.dwErrorCode;
}

//-----------------------------------------------------------------------------
// 向元宝交易账户存元宝
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleSaveYB2Account(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_SaveYB2Account);

	// 消息合法性验证
	if(pRecv->nNum <= 0)
	{
		return GT_INVALID;
	}

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	tagNS_SaveYB2Account send;
	send.dwErrorCode = pRole->SaveYB2Account(pRole->GetID(), pRecv->nNum);
	SendMessage(&send, send.dwSize);

	return send.dwErrorCode;
}

//-----------------------------------------------------------------------------
// 向元宝交易账户存金钱
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleSaveSilver2Account(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_SaveSilver2Account);

	if(pRecv->nNum <= 0)
	{
		return GT_INVALID;
	}

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	tagNS_SaveSilver2Account send;
	send.dwErrorCode = pRole->SaveSilver2Account(pRole->GetID(), pRecv->nNum);
	SendMessage(&send, send.dwSize);

	return send.dwErrorCode;
}

//-----------------------------------------------------------------------------
// 向元宝交易账户取元宝
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleDepositYBAccount(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_DepositAccountYB);

	if(pRecv->nNum <= 0)
	{
		return GT_INVALID;
	}

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	tagNS_DepositAccountYB send;
	send.dwErrorCode = pRole->DepositYBAccout(pRole->GetID(), pRecv->nNum);
	SendMessage(&send, send.dwSize);

	return send.dwErrorCode;
}

//-----------------------------------------------------------------------------
// 向元宝交易账户取金钱
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleDepositSilver(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_DepositAccountSilver);

	if(pRecv->nNum <= 0)
	{
		return GT_INVALID;
	}

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	tagNS_DepositAccountSilver send;
	send.dwErrorCode = pRole->DepositSilverAccount(pRole->GetID(), pRecv->nNum);
	SendMessage(&send, send.dwSize);

	return send.dwErrorCode;
}

//-----------------------------------------------------------------------------
// 同步元宝交易初始化信息
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleGetYBTradeInfo(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_SynYBTradeInfo);

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	pRole->GetYBTradeInfo();

	return E_Success;
}

//-----------------------------------------------------------------------------
// 玩家提交出售元宝订单
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleSubmitSellOrder(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_SubmitSellOrder);

	// 获取人物
	Role *pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	tagNS_SubmitSellOrder	send;
	send.dwErrorCode = pRole->SubmitSellOrder(pRole->GetID(), pRecv->nNum, pRecv->nPrice);
	SendMessage(&send, send.dwSize);

	return send.dwErrorCode;
}

//-----------------------------------------------------------------------------
// 玩家提交收购元宝订单
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleSubmitBuyOrder(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_SubmitBuyOrder);

	// 获取人物
	Role *pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	tagNS_SubmitBuyOrder	send;
	send.dwErrorCode = pRole->SubmitBuyOrder(pRole->GetID(), pRecv->nNum, pRecv->nPrice);
	SendMessage(&send, send.dwSize);

	return send.dwErrorCode;
}

//-----------------------------------------------------------------------------
// 玩家删除提交的订单
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleDeleteOrder(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_DeleteOrder);

	// 获取人物
	Role *pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	tagNS_DeleteOrder	send;
	send.dwErrorCode = pRole->DeleteOrder(pRole->GetID(), pRecv->dwOrderID, pRecv->eYBOType);
	SendMessage(&send, send.dwSize);

	return send.dwErrorCode;
}

//-----------------------------------------------------------------------------
// 玩家获取一周内的交易订单
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleGetYBOrder(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_GetYuanbaoOrder);

	// 获取人物
	Role *pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	pRole->GetYBOrder(pRole->GetID());

	return E_Success;
}
