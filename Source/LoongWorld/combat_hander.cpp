//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: combat_handler.cpp
// author: Aslan
// actor:
// data: 2008-09-16
// last:
// brief: 玩家间交易消息处理
//-----------------------------------------------------------------------------
#include "StdAfx.h"

#include "../WorldDefine/msg_combat.h"
#include "../WorldDefine/msg_map.h"

#include "role_mgr.h"
#include "player_session.h"
#include "map.h"
#include "unit.h"
#include "role.h"

//--------------------------------------------------------------------------------
// 进入战斗状态
//--------------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleEnterCombat(tagNetCmd* pCmd)
{
	Role* pRole = GetRole();
	if( !P_VALID(pRole) ) return GT_INVALID;

	// 设置人物现在处于战斗状态
	pRole->SetRoleState(ERS_Combat);
	return 0;
}

//--------------------------------------------------------------------------------
// 离开战斗状态
//--------------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleLeaveCombat(tagNetCmd* pCmd)
{
	Role* pRole = GetRole();
	if( !P_VALID(pRole) ) return GT_INVALID;

	// 设置人物现在处于战斗状态
	pRole->UnsetRoleState(ERS_Combat);
	return 0;
}


//--------------------------------------------------------------------------------
// 使用技能
//--------------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleSkill(tagNetCmd* pCmd)
{
	tagNC_Skill* pRecv = (tagNC_Skill*)pCmd;

	Role* pRole = GetRole();
	if( !P_VALID(pRole) ) return GT_INVALID;

	Map* pMap = pRole->GetMap();
	if( !P_VALID(pMap) ) return GT_INVALID;

	// 看能否同步到发招地点
	if (pRole->GetMoveData().IsInValidDistance(pRecv->vSrcPos))
	{
		pRole->GetMoveData().ForceTeleport(pRecv->vSrcPos,TRUE);
	}
	else
	{
		return GT_INVALID;
	}
		

	// 使用技能
	INT nRet = pRole->GetCombatHandler().UseSkill(Skill::GetIDFromTypeID(pRecv->dwSkillID), pRecv->dwTargetRoleID, pRecv->dwSerial);

	if( E_Success != nRet )
	{
		tagNS_Skill send;
		send.dwSrcRoleID = pRole->GetID();
		send.dwTarRoleID = pRecv->dwTargetRoleID;
		send.dwSkillID = pRecv->dwSkillID;
		send.dwSerial = pRecv->dwSerial;
		send.dwErrorCode = DWORD(nRet);

		SendMessage(&send, send.dwSize);
	}

	else
	{
		pRole->StopMount();
		tagNS_Skill send;
		send.dwSrcRoleID = pRole->GetID();
		send.dwTarRoleID = pRecv->dwTargetRoleID;
		send.dwSkillID = pRecv->dwSkillID;
		send.dwSerial = pRecv->dwSerial;
		send.nSpellTime = pRole->GetCombatHandler().GetSkillPrepareCountDown();
		send.dwErrorCode = E_Success;

		if( send.nSpellTime > TICK_TIME )
		{
			send.nSpellTime -= TICK_TIME;
		}

		if( P_VALID(pRole->GetMap()) )
		{
			pRole->GetMap()->SendBigVisTileMsg(pRole, &send, send.dwSize);
		}
	}

	return 0;
}

//--------------------------------------------------------------------------------
// 打断技能
//--------------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleInterruptSkill(tagNetCmd* pCmd)
{
	tagNC_SkillInterrupt* pRecv = (tagNC_SkillInterrupt*)pCmd;

	Role *pRole = GetRole();
	if( !P_VALID(pRole)) return GT_INVALID;

	pRole->GetCombatHandler().CancelSkillUse(pRecv->dwSkillID);

	return 0;	
}

//--------------------------------------------------------------------------------
// 使用物品
//--------------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleUseItem(tagNetCmd *pCmd)
{
	tagNC_UseItem* pRecv = (tagNC_UseItem*)pCmd;

	Role *pRole = GetRole();
	if( !P_VALID(pRole))
		return GT_INVALID;

	Map* pMap = pRole->GetMap();
	if( !P_VALID(pMap) ) return GT_INVALID;

	DWORD dwTypeID = GT_INVALID;
	bool bImmediate = false;

	// 使用物品
	INT nRet = pRole->GetCombatHandler().UseItem(pRecv->n64ItemID, pRecv->dwTargetRoleID, pRecv->dwSerial, dwTypeID, bImmediate);

	if( E_Success != nRet)
	{
		tagNS_UseItem send;
		send.dwSrcRoleID = pRole->GetID();
		send.dwTarRoleID = pRecv->dwTargetRoleID;
		send.n64ItemID = pRecv->n64ItemID;
		send.dwTypeID = dwTypeID;
		send.dwSerial = pRecv->dwSerial;
		send.dwErrorCode = DWORD(nRet);
		send.bInmmediate = bImmediate;

		SendMessage(&send, send.dwSize);
	}
	else
	{
		if( !bImmediate )
		{
			// 如果不是立即可以使用的物品，则需要停下来
			if (pRole->GetMoveData().IsInValidDistance(pRecv->vSrcPos))
			{
				pRole->GetMoveData().ForceTeleport(pRecv->vSrcPos, TRUE);
			}
		}

		tagNS_UseItem send;
		send.dwSrcRoleID = pRole->GetID();
		send.dwTarRoleID = pRecv->dwTargetRoleID;
		send.n64ItemID = pRecv->n64ItemID;
		send.dwTypeID = dwTypeID;
		send.dwSerial = pRecv->dwSerial;
		send.nSpellTime = pRole->GetCombatHandler().GetItemPrepareCountDown();
		send.dwErrorCode = E_Success;
		send.bInmmediate = bImmediate;

		if( send.nSpellTime > TICK_TIME )
		{
			send.nSpellTime -= TICK_TIME;
		}

		if( P_VALID(pRole->GetMap()) )
		{
			pRole->GetMap()->SendBigVisTileMsg(pRole, &send, send.dwSize);
		}
	}

	return 0;
}

//--------------------------------------------------------------------------------
// 打断使用物品
//--------------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleInterruptUseItem(tagNetCmd* pCmd)
{
	tagNC_UseItemInterrupt* pRecv = (tagNC_UseItemInterrupt*)pCmd;

	Role *pRole = GetRole();
	if( !P_VALID(pRole)) return GT_INVALID;

	pRole->GetCombatHandler().CancelItemUse(pRecv->n64ItemID);

	return 0;	
}

//------------------------------------------------------------------------
// 绑定复活地图
//------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleBindRebornMap(tagNetCmd* pCmd)
{
	MGET_MSG(p, pCmd, NC_BindRebornMap);
	
	Role *pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}
	
	tagNS_BindRebornMap send;
	send.dwErrorCode = pRole->SetRebornMap(p->dwNPCID, send.dwBornMapID, send.vBornPos);

	SendMessage(&send, send.dwSize);

	return send.dwErrorCode;
}

//------------------------------------------------------------------------
// 复活
//------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleRevive(tagNetCmd* pCmd)
{
	MGET_MSG(p, pCmd, NC_RoleRevive);

	// 检查消息合法性
	if(p->eType < ERRT_Start || p->eType > ERRT_End)
	{
		return GT_INVALID;
	}
	
	Role *pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	tagNS_RoleRevive send;
	send.dwRoleID		= pRole->GetID();
	send.dwErrorCode	= pRole->Revive(p->eType, p->n64ItemID);

	if( P_VALID(pRole->GetMap()) )
	{
		pRole->GetMap()->SendBigVisTileMsg(pRole, &send, send.dwSize);
	}

	if(E_Success == send.dwErrorCode)
	{
		// 复活到指定地点
		pRole->GotoNewMap(pRole->m_Revive.dwMapID, 
			pRole->m_Revive.fX, pRole->m_Revive.fY, pRole->m_Revive.fZ);
	}

	return send.dwErrorCode;
}

//------------------------------------------------------------------------------
// 玩家点掉Buff
//------------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleCancelBuff(tagNetCmd* pCmd)
{
	tagNC_CancelBuff* pRecv = (tagNC_CancelBuff*)pCmd;

	Role* pRole = GetRole();
	if( !P_VALID(pRole) ) return GT_INVALID;

	// 取消Buff
	pRole->CancelBuff(Buff::GetIDFromTypeID(pRecv->dwBuffTypeID));

	return 0;
}
