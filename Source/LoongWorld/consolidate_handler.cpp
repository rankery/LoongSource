//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: exchange_handler.cpp
// author: Wjiang
// actor:
// data: 2008-08-22
// last:
// brief: 装备强化
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "player_session.h"
#include "../ServerDefine/consolidate_define.h"
#include "../WorldDefine/msg_compose.h"
#include "../WorldDefine/compose_define.h"
#include "map.h"
#include "role.h"



//------------------------------------------------------------------------
// 物品强化（铭纹）
//------------------------------------------------------------------------
DWORD	PlayerSession::HandleRolePosyEquip(tagNetCmd* pCmd)
{
	// 接收网络消息
	MGET_MSG(pRecv, pCmd, NC_ConsolidatePosy);

	// 获取角色
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 输入材料个数
	INT nStuffNum = 1 + ((INT)pRecv->dwSize - sizeof(tagNC_ConsolidatePosy)) / sizeof(INT64);

	DWORD	dwErrorCode = pRole->PosyEquip(pRecv->dwNPCID, pRecv->dwFormulaID, pRecv->n64ItemID, \
												pRecv->n64IMID, pRecv->n64StuffID, nStuffNum, pRecv->dwID);

	if(GT_INVALID == dwErrorCode)
	{
		return dwErrorCode;
	}

	tagNS_ConsolidatePosy	send;
	send.dwErrorCode = dwErrorCode;
	SendMessage(&send, send.dwSize);

	return dwErrorCode;
}

//------------------------------------------------------------------------
// 物品强化（镌刻）
//------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleEngraveEquip(tagNetCmd* pCmd)
{
	// 接收网络消息
	MGET_MSG(pRecv, pCmd, NC_ConsolidateEngrave);

	// 获取角色
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 输入材料个数
	INT nStuffNum = 1 + ((INT)pRecv->dwSize - sizeof(tagNC_ConsolidateEngrave)) / sizeof(INT64);

	DWORD	dwErrorCode = pRole->EngraveEquip(pRecv->dwNPCID, pRecv->dwFormulaID, pRecv->n64ItemID, \
													pRecv->n64IMID, pRecv->n64StuffID, nStuffNum, pRecv->dwID);

	if(GT_INVALID == dwErrorCode)
	{
		return dwErrorCode;
	}

	tagNS_ConsolidateEngrave	send;
	send.dwErrorCode = dwErrorCode;
	SendMessage(&send, send.dwSize);

	return dwErrorCode;
}

//------------------------------------------------------------------------
// 物品强化（镶嵌）
//------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleInlayEquip(tagNetCmd* pCmd)
{
	// 接收网络消息
	MGET_MSG(pRecv, pCmd, NC_Inlay);

	// 获取角色
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	DWORD	dwErrorCode = pRole->InlayEquip(pRecv->n64SrcItemID, pRecv->n64DstItemID, pRecv->dwID);

	if(GT_INVALID == dwErrorCode)
	{
		return dwErrorCode;
	}

	tagNS_Inlay	send;
	send.dwErrorCode = dwErrorCode;
	SendMessage(&send, send.dwSize);

	return dwErrorCode;
}

//------------------------------------------------------------------------
// 物品强化（烙印）
//------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleBrandEquip(tagNetCmd* pCmd)
{
	// 接收网络消息
	MGET_MSG(pRecv, pCmd, NC_Brand);

	// 获取角色
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	DWORD	dwErrorCode = pRole->BrandEquip(pRecv->n64SrcItemID, pRecv->n64DstItemID, pRecv->n64IMID, pRecv->dwID);

	if(GT_INVALID == dwErrorCode)
	{
		return dwErrorCode;
	}

	tagNS_Brand	send;
	send.dwErrorCode = dwErrorCode;
	SendMessage(&send, send.dwSize);

	return dwErrorCode;
}

//------------------------------------------------------------------------
// 龙附
//------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleLoongSoul(tagNetCmd* pCmd)
{
	// 接收网络消息
	MGET_MSG(pRecv, pCmd, NC_LoongSoul);

	// 获取角色
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	DWORD	dwErrorCode = pRole->LoongSoulEquip(pRecv->n64SrcItemID, pRecv->n64DstItemID, pRecv->dwID);

	if(GT_INVALID == dwErrorCode)
	{
		return dwErrorCode;
	}

	tagNS_LoongSoul	send;
	send.dwErrorCode = dwErrorCode;
	SendMessage(&send, send.dwSize);

	return dwErrorCode;
}

//------------------------------------------------------------------------
// 淬火
//------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleQuench(tagNetCmd* pCmd)
{
	// 接收网络消息
	MGET_MSG(pRecv, pCmd, NC_ConsolidateQuench);

	// 获取角色
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 输入材料个数
	INT nStuffNum = 1 + ((INT)pRecv->dwSize - sizeof(tagNC_ConsolidateEngrave)) / sizeof(INT64);

	DWORD	dwErrorCode = pRole->QuenchEquip(pRecv->dwNPCID, pRecv->dwFormulaID, pRecv->n64ItemID, 
											pRecv->n64IMID, pRecv->n64StuffID, nStuffNum, pRecv->dwID);

	if(GT_INVALID == dwErrorCode)
	{
		return dwErrorCode;
	}

	tagNS_ConsolidateQuench	send;
	send.dwErrorCode = dwErrorCode;
	SendMessage(&send, send.dwSize);

	return dwErrorCode;
}

//------------------------------------------------------------------------
// 生产合成物品
//------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleProduceItem(tagNetCmd* pCmd)
{
	// 接收网络消息
	MGET_MSG(pRecv, pCmd, NC_Produce);

	// 获取角色
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 找到合成配方数据
	const tagProduceProtoSer *pProduceProto = g_attRes.GetProduceProto(pRecv->dwFormulaID);
	if(!P_VALID(pProduceProto))
		return GT_INVALID;

	// 输入材料个数
	INT nStuffNum = 1 + ((INT)pRecv->dwSize - sizeof(tagNC_Produce)) / sizeof(INT64);
	
	DWORD	dwErrorCode = GT_INVALID;
	if(!MIsEquipment(pProduceProto->dwProItemTypeID) )
	{
		dwErrorCode = pRole->ProduceItem(pRecv->dwNPCID, Skill::GetIDFromTypeID(pRecv->dwSkillID), pRecv->n64ItemID, pRecv->dwFormulaID,\
			pRecv->n64IMID, pRecv->n64StuffID, nStuffNum, pRecv->dwID);
	}
	else 
	{
		dwErrorCode = pRole->ProduceEquip(pRecv->dwNPCID, Skill::GetIDFromTypeID(pRecv->dwSkillID), pRecv->n64ItemID, pRecv->dwFormulaID,\
			pRecv->n64IMID, pRecv->n64StuffID, nStuffNum, pRecv->dwID);
	}

	if(GT_INVALID == dwErrorCode)
	{
		return dwErrorCode;
	}

	tagNS_Produce	send;
	send.dwErrorCode = dwErrorCode;
	SendMessage(&send, send.dwSize);

	return dwErrorCode;
}

// Jason 2009-12-6 处理装备潜力值提升协议
DWORD	PlayerSession::HandleRoleAddEquipPotVal(tagNetCmd* pCmd)
{
	// 接收网络消息
	MGET_MSG(pRecv, pCmd, NC_AddPot);

	// 获取角色
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	DWORD	dwPotValAdded = 0;
	DWORD	dwErrorCode = GT_INVALID;
	if( MIsEquipment( pRecv->n64DstItemID ) )
	{
		dwErrorCode = pRole->RaiseEquipPotVal( pRecv->n64SrcItemID,pRecv->n64DstItemID,pRecv->n64IMID,dwPotValAdded );
	}

	//返回码 0：成功； 1：完美； 2：失败 
	//if( E_Compose_Consolidate_Perfect == dwErrorCode )
	//	dwErrorCode = 1;
	//else if( E_Compose_Consolidate_Success == dwErrorCode )
	//	dwErrorCode = 0;
	//else
	//	dwErrorCode = 2;

	tagNS_AddPot msg;
	msg.dwErrorCode	= dwErrorCode;
	msg.dwPotValue	= dwPotValAdded;
	SendMessage  ( &msg,msg.dwSize );

	return dwErrorCode;
}

//------------------------------------------------------------------------
// 点化,装备合成
//------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleDeCompose(tagNetCmd* pCmd)
{
	// 接收网络消息
	MGET_MSG(pRecv, pCmd, NC_Decomposition)

	// 获取角色
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 找到合成配方数据
	const tagDeComposeProtoSer *pComposeProto = g_attRes.GetDeComposeProto(pRecv->dwFormulaID);
	if(!P_VALID(pComposeProto))
		return GT_INVALID;

	DWORD	dwErrorCode = GT_INVALID;

	dwErrorCode = pRole->DeComposeItem(pRecv->dwNPCID, Skill::GetIDFromTypeID(pRecv->dwSkillID), pRecv->n64ItemID, pRecv->dwFormulaID,\
			pRecv->n64IMID, pRecv->n64Item, pRecv->dwID);

	if(GT_INVALID == dwErrorCode)
	{
		return dwErrorCode;
	}

	tagNS_Decomposition	send;
	send.dwErrorCode = dwErrorCode;
	SendMessage(&send, send.dwSize);

	return dwErrorCode;
}

//------------------------------------------------------------------------
// 开凿
//------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleChisel(tagNetCmd* pCmd)
{
	// 接收网络消息
	MGET_MSG(pRecv, pCmd, NC_Chisel);

	// 获取角色
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	DWORD	dwErrorCode = GT_INVALID;

	dwErrorCode = pRole->ChiselEquip(pRecv->n64SrcItemID, pRecv->n64StuffID, pRecv->dwID);

	tagNS_Chisel	send;
	send.dwErrorCode = dwErrorCode;
	SendMessage(&send, send.dwSize);

	return dwErrorCode;
}

//------------------------------------------------------------------------
// 时装染色
//------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleDyeFashion(tagNetCmd* pCmd)
{
	// 接收网络消息
	MGET_MSG(pRecv, pCmd, NC_DyeFashion);

	// 获取角色
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	DWORD	dwErrorCode = GT_INVALID;

	dwErrorCode = pRole->DyeFashion(pRecv->n64DyeSN, pRecv->n64EquipSerial, pRecv->dwID);

	tagNS_DyeFashion send;
	send.dwErrCode = dwErrorCode;
	SendMessage(&send, send.dwSize);

	return dwErrorCode;
}


DWORD PlayerSession::HandleGemRemoval(tagNetCmd * pCmd)
{
	// 接收网络消息
	MGET_MSG(pRecv, pCmd, NC_GemRemoval);

	// 获取角色
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	DWORD	dwErrorCode = GT_INVALID;

	dwErrorCode = pRole->RemoveGemFromEquip(pRecv->dwNPCID, pRecv->n64EquipID, pRecv->n16GemNum,pRecv->n64GemRomovalSign);

	tagNS_GemRemoval send;
	send.dwErrorCode = dwErrorCode;
	SendMessage(&send, send.dwSize);

	return dwErrorCode;
}


DWORD PlayerSession::HandleGetGemRemovalInfo(tagNetCmd * pCmd)
{
	// 接收网络消息
	MGET_MSG(pRecv, pCmd, NC_GetGemRemovalInfo);

	// 获取角色
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	DWORD	dwErrorCode = GT_INVALID;

	tagNS_GetGemRemovalInfo send;
	INT8 num  = 0;

	dwErrorCode = pRole->GetGemRemovalInfo(pRecv->dwNPCID, pRecv->n64EquipID, num,send.dwGemIDs);
	send.n16GemNum = num;

	send.dwErrCode = dwErrorCode;
	SendMessage(&send, send.dwSize);

	return dwErrorCode;
}

