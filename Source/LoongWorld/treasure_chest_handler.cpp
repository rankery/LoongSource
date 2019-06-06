//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: treasure_chest_handler.cpp
// author: twan
// actor:
// data: 2009-07-16
// last:
// brief: 客户端和服务器端间消息处理 -- 开启宝箱相关
//-----------------------------------------------------------------------------

#include "StdAfx.h"

#include "role.h"
#include "player_session.h"
#include "db_session.h"
#include "world_session.h"
#include "item_creator.h"
#include "../WorldDefine/msg_digbao.h"
#include "../ServerDefine/base_define.h"
#include "../ServerDefine/log_cmdid_define.h"
#include "TreasureChest_mgr.h"
#include "../WorldDefine/ItemDefine.h"
#include "../WorldDefine/TreasureChest_define.h"
#include "script_mgr.h"


//--------------------------------------------------------------------------
// 开启宝箱
//--------------------------------------------------------------------------
DWORD PlayerSession::HandleOpenTreasureChest(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_TreasureChest);

	//	获取角色
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 获取角色背包中的宝箱id和钥匙id
	tagItem* pItemChest = pRole->GetItemMgr().GetBagItem(pRecv->n64ChestID);
	tagItem* pItemKey	= pRole->GetItemMgr().GetBagItem(pRecv->n64KeyID);
	if ( !P_VALID(pItemChest) || !P_VALID(pItemKey)) 
	{
		tagNS_TreasureChest send;
		send.dwErrorCode = GT_INVALID;
		SendMessage(&send, send.dwSize);
		return GT_INVALID;
	}

	DWORD dwChestID =  pItemChest->dwTypeID;
	DWORD dwKeyID = pItemKey->dwTypeID;

	// 记录玩家的宝箱和钥匙id
	pRole->SetChestSerial(pRecv->n64ChestID);
	pRole->SetKeySerial(pRecv->n64KeyID);
	pRole->SetChestTypeID(dwChestID);
	pRole->SetKeyTypeID(dwKeyID);

	// 调用脚本函数
	g_ScriptMgr.GetRoleScript()->OnOpenChest(pRole, dwChestID, dwKeyID);
	return E_Success;
}

//--------------------------------------------------------------------------
// 停止随机选中物品，并且确定最后选中的物品
//--------------------------------------------------------------------------
DWORD PlayerSession::HandleStopTreasureChest(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_StopTreasureChest);

	//	获取角色
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	DWORD dwChestID =  pRole->GetChestTypeID();
	DWORD dwKeyID = pRole->GetKeyTypeID();

	// 调用脚本函数
	g_ScriptMgr.GetRoleScript()->OnStopChest(pRole, dwChestID, dwKeyID);
	return E_Success;
}

//--------------------------------------------------------------------------
// 再试一次
//--------------------------------------------------------------------------
DWORD PlayerSession::HandleAgainTreasureChest(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_AgainTreasureChest);

	//	获取角色
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 调用脚本函数
	g_ScriptMgr.GetRoleScript()->OnAgainChest(pRole);
	return E_Success;
}

//--------------------------------------------------------------------------
// 获得道具
//--------------------------------------------------------------------------
DWORD PlayerSession::HandleChestGetItem(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_ChestGetItem);

	//	获取角色
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	DWORD dwItemID = pRole->GetChestItem().dwTypeID;
	INT nNum = pRole->GetChestItem().nNum;
	DWORD dwChestID = pRole->GetChestTypeID();

	// 开宝箱的物品不对
	if (dwItemID != pRecv->dwTypeID)
	{
		tagNS_ChestGetItem send;
		send.dwErrorCode = GT_INVALID;
		SendMessage(&send, send.dwSize);
		return GT_INVALID;
	}

	// 调用脚本函数
	g_ScriptMgr.GetRoleScript()->OnGetItem(pRole, dwChestID, dwItemID, nNum);
	return E_Success;
}
	