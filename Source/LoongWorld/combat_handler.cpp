//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: combat_handler.cpp
// author: Aslan
// actor:
// data: 2008-09-25
// last:
// brief: 战斗系统管理器
//-----------------------------------------------------------------------------
#include "stdafx.h"

#include "../WorldDefine/msg_combat.h"
#include "../ServerDefine/log_cmdid_define.h"

#include "unit.h"
#include "map.h"
#include "creature.h"
#include "role.h"
#include "combat_handler.h"
#include "script_mgr.h"
#include "title_mgr.h"

//-----------------------------------------------------------------------------
// 使用技能
//-----------------------------------------------------------------------------
INT CombatHandler::UseSkill(DWORD dwSkillID, DWORD dwTargetUnitID, DWORD dwSerial)
{
	Skill* pSkill = m_pOwner->GetSkill(dwSkillID);
	if( !P_VALID(pSkill) ) return E_UseSkill_SkillNotExist;

	Unit* pTargetUnit = m_pOwner->GetMap()->FindUnit(dwTargetUnitID);

	// 检查能否对该目标发动技能
	INT nRet = CanCastSkill(pSkill, dwTargetUnitID);
	if( E_Success != nRet )	return nRet;

	// 该机能有武器限制，则减少相应武器崭新度
	if( EITE_Null != pSkill->GetProto()->nWeaponLimit && m_pOwner->IsRole() )
		((Role*)m_pOwner)->GetItemMgr().ProcEquipNewness();

	// 检查该技能是否能够移动施放
	if( !pSkill->IsMoveable() )
	{
		m_pOwner->GetMoveData().StopMoveForce();
	}

	// 如果目标存在且不是自己，则改变面向
	if( P_VALID(pTargetUnit) && m_pOwner->GetID() != pTargetUnit->GetID() )
	{
		m_pOwner->GetMoveData().SetFaceTo(pTargetUnit->GetCurPos() - m_pOwner->GetCurPos());
	}


	// 打断使用技能打断的buff
	m_pOwner->OnInterruptBuffEvent(EBIF_InterCombat);

	// 设置参数，准备发动
	m_dwSkillID			=	dwSkillID;
	m_dwTargetUnitID	=	dwTargetUnitID;
	m_dwSkillSerial		=	dwSerial;

	// 如果该技能需要起手，则设置起手倒计时，否则进入技能作用阶段
	m_nSkillPrepareCountDown = (INT)(pSkill->GetPrepareTime() * m_fSkillPrepareModPct);

	if( m_nSkillPrepareCountDown > 0 )
	{
		m_bSkillPreparing	=	TRUE;
		m_bSkillOperating	=	FALSE;
	}
	else
	{
		// 可以发动，设置技能冷却
		m_pOwner->StartSkillCoolDown(pSkill);

		m_bSkillPreparing	=	FALSE;
		m_bSkillOperating	=	TRUE;
		m_nSkillOperateTime	=	0;
		m_nSkillCurDmgIndex	=	0;

		// 计算目标
		CalSkillTargetList();
	}

	return nRet;
}

//-------------------------------------------------------------------------------
// 使用物品
//-------------------------------------------------------------------------------
INT CombatHandler::UseItem(INT64 n64ItemID, DWORD dwTargetUnitID, DWORD dwSerial, DWORD &dwTypeID, bool& bImmediate)
{
	if( GT_INVALID == dwTargetUnitID )
		dwTargetUnitID = m_pOwner->GetID();

	// 检查是不是玩家
	if( !m_pOwner->IsRole() ) return E_UseItem_TargetInvalid;
	Role* pOwnerRole = static_cast<Role*>(m_pOwner);

	// 检查物品是否在背包里
	tagItem* pItem = pOwnerRole->GetItemMgr().GetBagItem(n64ItemID); 
	if( !P_VALID(pItem) ) return E_UseItem_ItemNotExist;

	// 检查能否使用物品
	INT nRet = E_Success;
	BOOL bIgnore = FALSE;		// 是否忽略能否使用物品的通用判断

	if(P_VALID(pItem->pScript) && P_VALID(pOwnerRole->GetMap()))
	{
		// 检查脚本的物品使用限制
		nRet = pItem->pScript->CanUseItem(pOwnerRole->GetMap(), pItem->dwTypeID, dwTargetUnitID, bIgnore);

		// 重新获取物品指针
		pItem = pOwnerRole->GetItemMgr().GetBagItem(n64ItemID); 
		if( !P_VALID(pItem) ) return E_UseItem_ItemNotExist;
	}

	// 检查使用物品的通用判断
	if(!bIgnore && E_Success == nRet)
		nRet = CanUseItem(pItem);

	if( E_Success != nRet ) return nRet;

	// 如果该物品不能移动使用，则停下
	if( !pItem->pProtoType->bMoveable )
	{
		m_pOwner->GetMoveData().StopMoveForce();
	}

	// 打断使用物品打断的buff
	m_pOwner->OnInterruptBuffEvent(EBIF_InterCombat);

	// 如果检查通过，则设置上相应的参数，准备发动
	m_n64ItemID			=	n64ItemID;
	m_dwItemSerial		=	dwSerial;
	dwTypeID			=	pItem->dwTypeID;
	m_dwTargetUnitIDItem = dwTargetUnitID;

	// 如果使用物品需要起手，则设置起手倒计时，否则进入作用阶段
	m_nItemPrepareCountDown = pItem->pProtoType->nPrepareTime;
	if( m_nItemPrepareCountDown > 0 )
	{
		m_bItemPreparing	=	TRUE;
		m_bItemOperating	=	FALSE;
		bImmediate			=	false;
	}
	else
	{
		m_bItemPreparing	=	FALSE;
		m_bItemOperating	=	TRUE;
		bImmediate			=	true;
	}

	return nRet;
}

//-----------------------------------------------------------------------------
// 更新技能起手，如果起手结束了，则切换到进行状态
//-----------------------------------------------------------------------------
VOID CombatHandler::UpdateSkillPrepare()
{
	if( !IsUseSkill() ) return;
	if( !IsSkillPreparing() ) return;

	// 减去Tick时间
	m_nSkillPrepareCountDown -= TICK_TIME;

	// 起手时间结束，切换到更新状态
	if( m_nSkillPrepareCountDown <= 0 )
	{
		Skill* pSkill = m_pOwner->GetSkill(m_dwSkillID);

		if( P_VALID(pSkill) )
		{
			m_pOwner->StartSkillCoolDown(pSkill);
		}

		m_bSkillPreparing = FALSE;
		m_bSkillOperating = TRUE;
		m_nSkillOperateTime = 0;
		m_nSkillCurDmgIndex = 0;

		// 计算目标
		CalSkillTargetList();
	}
}

//-----------------------------------------------------------------------------
// 更新起手，如果起手结束了，则切换到进行状态
//-----------------------------------------------------------------------------
VOID CombatHandler::UpdateItemPrepare()
{
	if( !IsUseItem() ) return;
	if( !IsItemPreparing() ) return;

	// 减去Tick时间
	m_nItemPrepareCountDown -= TICK_TIME;

	// 起手时间结束，切换到更新状态
	if( m_nItemPrepareCountDown <= 0 )
	{
		m_bItemPreparing = FALSE;
		m_bItemOperating = TRUE;
	}
}


//-------------------------------------------------------------------------------
// 更新技能操作，如果到了计算伤害的时候，则计算伤害，如果伤害计算完了，则计算buff
//-------------------------------------------------------------------------------
VOID CombatHandler::UpdateSkillOperate()
{
	if( !IsUseSkill() ) return;
	if( !IsSkillOperating() ) return;

	// 首先找到这个技能
	Skill* pSkill = m_pOwner->GetSkill(m_dwSkillID);
	if( !P_VALID(pSkill) )
	{
		EndUseSkill();
		return;
	}

	Map* pMap = m_pOwner->GetMap();
	if( !P_VALID(pMap) ) return;

	// 得到技能总伤害次数
	INT nDmgTimes = pSkill->GetDmgTimes();

	if (m_pOwner->IsRole())
	{
		((Role*)m_pOwner)->GetTitleMgr()->SigEvent(ETE_USE_SKILL, pSkill->GetTypeID(), GT_INVALID);
	}

	// 如果伤害次数为0，说明该技能无伤害，则直接进入到计算buff阶段
	if( nDmgTimes <= 0 )
	{
		// 计算buff
		m_pOwner->OnActiveSkillBuffTrigger(pSkill, m_listTargetID, ETEE_Use);
		// 计算消耗
		CalculateCost(pSkill);
		// 结束
		EndUseSkill();
		return;
	}

	// 伤害次数不为0，则检测当前时间到了哪次伤害
	m_nSkillOperateTime += TICK_TIME;

	for(; m_nSkillCurDmgIndex < nDmgTimes; m_nSkillCurDmgIndex++)
	{
		// 本tick完成不了如此多的伤害计算，等到下个tick
		if( pSkill->GetDmgTime(m_nSkillCurDmgIndex) > m_nSkillOperateTime )
			break;

		// 时间到了，则开始计算伤害
		TList<DWORD>::TListIterator it = m_listTargetID.Begin();
		DWORD dwTargetID = GT_INVALID;

		while( m_listTargetID.PeekNext(it, dwTargetID) )
		{
			// 找到这个目标
			Unit* pTarget = pMap->FindUnit(dwTargetID);

			if( !P_VALID(pTarget) ) continue;

			// 计算伤害
			CalculateDmg(pSkill, pTarget);
		}
	}

	// 检测所有伤害是否已经计算完毕
	if( m_nSkillCurDmgIndex >= nDmgTimes )
	{
		// 计算主动技能Buff
		m_pOwner->OnActiveSkillBuffTrigger(pSkill, m_listHitedTarget, ETEE_Hit);
		m_pOwner->OnActiveSkillBuffTrigger(pSkill, m_listDodgedTarget, ETEE_Dodged);
		m_pOwner->OnActiveSkillBuffTrigger(pSkill, m_listBlockedTarget, ETEE_Blocked);
		m_pOwner->OnActiveSkillBuffTrigger(pSkill, m_listCritedTarget, ETEE_Crit);
		m_pOwner->OnActiveSkillBuffTrigger(pSkill, m_listHitedTarget, ETEE_Use);

		// 找到目标
		Unit* pTarget = pMap->FindUnit(m_dwTargetUnitID);
		if( P_VALID(pTarget) )
		{
			// Buff触发
			if( m_dwTargetEffectFlag & ETEF_Hited )
			{
				// 命中
				m_pOwner->OnBuffTrigger(pTarget, ETEE_Hit);
			}
			else
			{
				// 被闪避
				m_pOwner->OnBuffTrigger(pTarget, ETEE_Dodged);
			}

			if( m_dwTargetEffectFlag & ETEF_Block )
			{
				// 被格挡
				m_pOwner->OnBuffTrigger(pTarget, ETEE_Blocked);
			}

			if( m_dwTargetEffectFlag & ETEF_Crited )
			{
				// 暴击
				m_pOwner->OnBuffTrigger(pTarget, ETEE_Crit);
			}

			// 计算被动技能和装备Buff
			if( m_pOwner->IsRole() )
			{
				// 针对第一目标进行计算
				Role* pOwnerRole = static_cast<Role*>(m_pOwner);

				if( m_dwTargetEffectFlag & ETEF_Hited )
				{
					// 命中
					pOwnerRole->OnPassiveSkillBuffTrigger(pTarget, ETEE_Hit);
					pOwnerRole->OnEquipmentBuffTrigger(NULL, ETEE_Hit);
				}
				else
				{
					// 被闪避
					pOwnerRole->OnPassiveSkillBuffTrigger(pTarget, ETEE_Dodged);
					pOwnerRole->OnEquipmentBuffTrigger(NULL, ETEE_Dodged);
				}

				if( m_dwTargetEffectFlag & ETEF_Block )
				{
					// 被格挡
					pOwnerRole->OnPassiveSkillBuffTrigger(pTarget, ETEE_Blocked);
					pOwnerRole->OnEquipmentBuffTrigger(NULL, ETEE_Blocked);
				}

				if( m_dwTargetEffectFlag & ETEF_Crited )
				{
					// 暴击
					pOwnerRole->OnPassiveSkillBuffTrigger(pTarget, ETEE_Crit);
					pOwnerRole->OnEquipmentBuffTrigger(NULL, ETEE_Crit);
				}
			}
		}

		// 计算消耗
		CalculateCost(pSkill);

		if (m_pOwner->IsRole())
		{
			Role *pRole = dynamic_cast<Role *>(m_pOwner);
			if (NULL != pRole)
			{
				pRole->GetTitleMgr()->SigEvent(ETE_USE_SKILL, pSkill->GetID(), GT_INVALID);
			}
		}

		// 技能结束
		EndUseSkill();
	}
}

//-----------------------------------------------------------------------------
// 更新使用物品效果
//-----------------------------------------------------------------------------
VOID CombatHandler::UpdateItemOperate()
{
	if( !IsUseItem() ) return;
	if( !IsItemOperating() ) return;
	if( !m_pOwner->IsRole() ) return;

	Role* pOwnerRole = static_cast<Role*>(m_pOwner);

	// 首先找到这个物品
	tagItem* pItem = pOwnerRole->GetItemMgr().GetBagItem(m_n64ItemID); 
	if( !P_VALID(pItem) )
	{
		EndUseItem();
		return;
	}

	DWORD	dwTypeID = pItem->dwTypeID;
	Map* pMap = pOwnerRole->GetMap();
	if( !P_VALID(pMap) ) return;

	// 发送命中目标给客户端
	tagNS_HitTarget send;
	send.dwRoleID = m_pOwner->GetID();
	send.dwSrcRoleID = m_pOwner->GetID();
	send.eCause = EHTC_Item;
	send.dwMisc = pItem->dwTypeID;
	send.dwSerial = m_dwItemSerial;
	pMap->SendBigVisTileMsg(m_pOwner, &send, send.dwSize);

	// 计算buff
	pOwnerRole->OnActiveItemBuffTrigger(pItem, ETEE_Use);

	// 计算物品的脚本使用效果
	if(P_VALID(pItem->pScript) && P_VALID(pOwnerRole->GetMap()))
	{
		pItem->pScript->UseItem(pOwnerRole->GetMap(), pItem->dwTypeID, m_dwTargetUnitIDItem);
	}

	// 称号消息
	pOwnerRole->GetTitleMgr()->SigEvent(ETE_USE_ITEM, dwTypeID, GT_INVALID);

	// 加入物品公共冷却时间
	pOwnerRole->GetItemMgr().Add2CDTimeMap(dwTypeID);

	// 处理物品消失
	pOwnerRole->GetItemMgr().ItemUsedFromBag(m_n64ItemID, 1, (DWORD)ELCLD_Item_Use);

	EndUseItem();
}

//-----------------------------------------------------------------------------------
// 取消技能使用
//-----------------------------------------------------------------------------------
VOID CombatHandler::CancelSkillUse(DWORD dwSkillTypeID)
{
	if( !IsValid() || !IsUseSkill() ) return;

	Skill* pSkill = m_pOwner->GetSkill(m_dwSkillID);
	if( !P_VALID(pSkill) || pSkill->GetTypeID() != dwSkillTypeID )
		return;

	BOOL bCanCancel = FALSE;

	// 如果技能正在起手，一定可以
	if( IsSkillPreparing() )
	{
		bCanCancel = TRUE;
	}
	// 如果正在释放，则只有普通攻击才可以
	else
	{
		if( ESSTE_Default == pSkill->GetTypeEx() )
		{
			bCanCancel = TRUE;
		}
	}

	// 如果可以取消
	if( bCanCancel )
	{
		tagNS_SkillInterrupt send;
		send.dwRoleID = m_pOwner->GetID();
		send.dwSkillID = dwSkillTypeID;

		if( P_VALID(m_pOwner->GetMap()) )
		{
			m_pOwner->GetMap()->SendBigVisTileMsg(m_pOwner, &send, send.dwSize);
		}
		EndUseSkill();
	}
}

//-----------------------------------------------------------------------------------
// 取消物品释放
//-----------------------------------------------------------------------------------
VOID CombatHandler::CancelItemUse(INT64 n64ItemSerial)
{
	if( !IsValid() || !IsUseItem() ) return;

	if( m_n64ItemID != n64ItemSerial ) return;

	BOOL bCanCancel = FALSE;

	// 物品只有在起手时才能取消
	if( IsItemPreparing() )
	{
		bCanCancel = TRUE;
	}

	if( bCanCancel )
	{
		tagNS_UseItemInterrupt send;
		send.dwRoleID = m_pOwner->GetID();
		send.n64ItemID = m_n64ItemID;
		send.dwTypeID = GT_INVALID;

		if( P_VALID(m_pOwner->GetMap()) )
		{
			m_pOwner->GetMap()->SendBigVisTileMsg(m_pOwner, &send, send.dwSize);
		}
		EndUseItem();
	}
}

//-----------------------------------------------------------------------------------
// 打断起手
//-----------------------------------------------------------------------------------
BOOL CombatHandler::InterruptPrepare(EInterruptType eType, BOOL bOrdinary, BOOL bForce)
{
	if( FALSE == IsValid() || FALSE == IsPreparing() )
		return TRUE;

	BOOL bSkill = FALSE;		// 是技能在起手还是物品在起手
	DWORD dwSkillTypeID = GT_INVALID;
	if( IsSkillPreparing() )	bSkill = TRUE;
	else						bSkill = FALSE;

	// 通过是使用物品还是使用技能来判断打断值
	BOOL bMoveInterrupt = FALSE;
	INT nInterruptSkillRate = 0;

	if( bSkill )
	{
		Skill* pSkill = m_pOwner->GetSkill(m_dwSkillID);
		if( P_VALID(pSkill) )
		{
			const tagSkillProto* pProto = pSkill->GetProto();
			if( P_VALID(pProto) )
			{
				bMoveInterrupt = pProto->bInterruptMove;
				nInterruptSkillRate = (bOrdinary ? pProto->nInterruptSkillOrdRate : pProto->nInterruptSkillSpecRate);
			}
			dwSkillTypeID = pSkill->GetTypeID();
		}
	}
	else
	{
		Role* pRole = static_cast<Role*>(m_pOwner);
		tagItem* pItem = pRole->GetItemMgr().GetBagItem(m_n64ItemID);
		if( P_VALID(pItem) )
		{
			bMoveInterrupt = pItem->pProtoType->bInterruptMove;
			nInterruptSkillRate = pItem->pProtoType->nInterruptSkillOrdRate;
		}
	}

	BOOL bCanInterrupt = FALSE;	// 是否能够打断

	if( bForce )
	{
		bCanInterrupt = TRUE;
	}
	else
	{
		// 尝试打断
		switch(eType)
		{
		case EIT_Move:
			{
				if( bMoveInterrupt )
				{
					bCanInterrupt = TRUE;
				}
			}
			break;

		case EIT_Skill:
			{
				// 普通攻击打断几率
				if( IUTIL->Rand() % 10000 < nInterruptSkillRate )
				{
					bCanInterrupt = TRUE;
				}
			}
			break;

		default:
			break;
		}
	}

	if( bCanInterrupt )
	{
		// 发送打断给周围玩家
		if( bSkill )
		{
			tagNS_SkillInterrupt send;
			send.dwRoleID = m_pOwner->GetID();
			send.dwSkillID = dwSkillTypeID;

			if( P_VALID(m_pOwner->GetMap()) )
			{
				m_pOwner->GetMap()->SendBigVisTileMsg(m_pOwner, &send, send.dwSize);
			}
			EndUseSkill();
		}
		else
		{
			tagNS_UseItemInterrupt send;
			send.dwRoleID = m_pOwner->GetID();
			send.n64ItemID = m_n64ItemID;
			send.dwTypeID = GT_INVALID;

			if( P_VALID(m_pOwner->GetMap()) )
			{
				m_pOwner->GetMap()->SendBigVisTileMsg(m_pOwner, &send, send.dwSize);
			}
			EndUseItem();
		}

		return TRUE;
	}

	return FALSE;
}

//-------------------------------------------------------------------------------
// 打断释放
//-------------------------------------------------------------------------------
BOOL CombatHandler::InterruptOperate(EInterruptType eType, DWORD dwMisc, BOOL bForce/* =FALSE */)
{
	if( FALSE == IsValid() || FALSE == IsSkillOperating() )
		return TRUE;

	if( EIT_Move == eType )
	{
		EMoveState eState = (EMoveState)dwMisc;

		// 走和游泳相关的移动，则只有移动打断的普通攻击才打断
		if( EMS_Walk			== eState ||
			EMS_Swim			== eState ||
			EMS_CreaturePatrol	== eState ||
			EMS_CreatureWalk	== eState ||
			EMS_CreatureFlee	== eState )
		{
			Skill* pSkill = m_pOwner->GetSkill(m_dwSkillID);
			if( P_VALID(pSkill) && ESSTE_Default == pSkill->GetTypeEx() && !pSkill->IsMoveable() )
			{
				EndUseSkill();
				return TRUE;
			}
		}
		// 其它移动方式，则只要是普通攻击就打断
		else
		{
			Skill* pSkill = m_pOwner->GetSkill(m_dwSkillID);
			if( P_VALID(pSkill) && ESSTE_Default == pSkill->GetTypeEx() )
			{
				EndUseSkill();
				return TRUE;
			}
		}
	}

	return FALSE;
}


//-------------------------------------------------------------------------------
// 是否可以使用技能
//-------------------------------------------------------------------------------
INT CombatHandler::CanCastSkill(Skill* pSkill, DWORD dwTargetUnitID)
{
	if( !P_VALID(pSkill) )
		return E_SystemError;

	if( CheckSkillConflict(pSkill) ) return E_UseSkill_Operating;

	INT nRet = E_Success;

	nRet = CheckSkillAbility(pSkill);
	if( E_Success != nRet ) return nRet;

	nRet = CheckOwnerLimitSkill();
	if( E_Success != nRet ) return nRet;

	nRet = CheckSkillLimit(pSkill);
	if( E_Success != nRet ) return nRet;

	nRet = CheckTargetLimit(pSkill, dwTargetUnitID);
	if( E_Success != nRet ) return nRet;

	nRet = CheckCostLimit(pSkill);
	if( E_Success != nRet ) return nRet;

	nRet = CheckVocationLimit(pSkill);
	if( E_Success != nRet ) return nRet;

	nRet = CheckMapLimit(pSkill);
	if( E_Success != nRet ) return nRet;

	const SkillScript* pScript = pSkill->GetSkillScript();
	if (P_VALID(pScript))
	{
		nRet = pScript->CanCastSkill(m_pOwner->GetMap(), pSkill->GetID(), m_pOwner->GetID(), dwTargetUnitID);
		if( E_Success != nRet ) return nRet;
	}

	return nRet;
}

//-------------------------------------------------------------------------------
// 测试技能本身是否能够使用
//-------------------------------------------------------------------------------
INT CombatHandler::CheckSkillAbility(Skill* pSkill)
{
	if( !P_VALID(pSkill) ) return E_UseSkill_SkillNotExist;

	// 被动技能不可以使用
	if( pSkill->IsPassive() )
		return E_UseSkill_PassiveSkill;

	// 如果技能的目标类型不是战斗目标或非战斗目标，则不可以使用
	ESkillTargetType eTargetType = pSkill->GetTargetType();
	if( ESTT_Combat != eTargetType && ESTT_NoCombat != eTargetType )
		return E_UseSkill_SkillTargetInvalid;

	// 技能的冷却时间还没到，则不可以使用
	if( pSkill->GetCoolDownCountDown() > 0 )
		return E_UseSkill_CoolDowning;

	return E_Success;
}

//-------------------------------------------------------------------------------
// 测试技能使用者是否能够使用技能
//-------------------------------------------------------------------------------
INT CombatHandler::CheckOwnerLimitSkill()
{
	// 是否处在不能使用技能的状态
	if( m_pOwner->IsInStateCantCastSkill() )
		return E_UseSkill_UseLimit;

	return E_Success;
}

//-------------------------------------------------------------------------------
// 测试技能本身使用限制
//-------------------------------------------------------------------------------
INT CombatHandler::CheckSkillLimit(Skill* pSkill)
{
	if( !P_VALID(pSkill) ) return E_UseSkill_SkillNotExist;

	const tagSkillProto* pProto = pSkill->GetProto();
	if( !P_VALID(pProto) ) return E_UseSkill_SkillNotExist;

	// 体力低于或者真气低于
	if( pProto->nUseHPPctLimit > 0 )
	{
		if( m_pOwner->GetAttValue(ERA_MaxHP) <= 0 )
			return E_UseSkill_UseLimit;

		if( (FLOAT)m_pOwner->GetAttValue(ERA_HP) / (FLOAT)m_pOwner->GetAttValue(ERA_MaxHP) * 10000.0f < pProto->nUseHPPctLimit )
			return E_UseSkill_UseLimit;

	}
	if( pProto->nUseMPPctLimit > 0 )
	{
		if( m_pOwner->GetAttValue(ERA_MaxMP) <= 0 )
			return E_UseSkill_UseLimit;

		if( (FLOAT)m_pOwner->GetAttValue(ERA_MP) / (FLOAT)m_pOwner->GetAttValue(ERA_MaxMP) * 10000.0f < pProto->nUseHPPctLimit )
			return E_UseSkill_UseLimit;

	}

	// 性别限制
	if( pProto->eSexLimit != ESSL_Null )
	{
		if( ESSL_Man == pProto->eSexLimit )
		{
			if( 1 != m_pOwner->GetSex() )
				return E_UseSkill_SexLimit;
		}
		else if( ESSL_Woman == pProto->eSexLimit )
		{
			if( 0 != m_pOwner->GetSex() )
				return E_UseSkill_SexLimit;
		}
		else
		{

		}
	}

	// 职业限制

	// 武器限制
	if( EITE_Null != pProto->nWeaponLimit && m_pOwner->IsRole() )
	{
		Role* pRole = static_cast<Role*>(m_pOwner);
		tagEquip* pWeapon = pRole->GetItemMgr().GetEquipBarEquip((INT16)EEP_RightHand);
		if( !P_VALID(pWeapon) || pWeapon->pProtoType->eTypeEx != pProto->nWeaponLimit )
		{
			return E_UseSkill_WeaponLimit;
		}
	}

	// 特殊Buff限制
	if( P_VALID(pProto->dwBuffLimitID) )
	{
		if( !m_pOwner->IsHaveBuff(pProto->dwBuffLimitID) )
		{
			return E_UseSkill_SelfBuffLimit;
		}
	}

	// 检查自身状态限制
	DWORD dwSelfStateFlag = m_pOwner->GetStateFlag();
	if( (dwSelfStateFlag & pProto->dwSelfStateLimit) != dwSelfStateFlag )
	{
		return E_UseSkill_SelfStateLimit;
	}

	return E_Success;
}

//-------------------------------------------------------------------------------
// 测试目标限制
//-------------------------------------------------------------------------------
INT CombatHandler::CheckTargetLimit(Skill* pSkill, DWORD dwTargetUnitID)
{
	if( !P_VALID(pSkill) )
		return E_UseSkill_SkillNotExist;

	const tagSkillProto* pProto = pSkill->GetProto();
	if( !P_VALID(pProto) ) return E_UseSkill_SkillNotExist;

	// 如果TargetUnitID是GT_INVALID，则需要特殊判断一下
	if( GT_INVALID == dwTargetUnitID )
	{
		if( ESOPT_Explode == pSkill->GetOPType() && 0.0f == pSkill->GetOPDist() )
		{
			return E_Success;
		}
		else
		{
			return E_UseSkill_SkillTargetInvalid;
		}
	}

	Unit* pTarget = m_pOwner->GetMap()->FindUnit(dwTargetUnitID);
	if( !P_VALID(pTarget) ) return E_UseSkill_SkillTargetInvalid;

	// 目标对象逻辑限制
	INT nRet = CheckTargetLogicLimit(pSkill, pTarget);
	if( nRet != E_Success )	return nRet;

	// 位置限制，距离限制和范围判断
	if( m_pOwner != pTarget )
	{
		// 位置限制
		if( ESPT_NUll != pProto->ePosType )
		{
			if( ESPT_Front == pProto->ePosType )
			{
				if( FALSE == m_pOwner->IsInFrontOfTarget(*pTarget) )
					return E_UseSkill_PosLimitFront;
			}
			else if( ESPT_Back == pProto->ePosType )
			{
				if( TRUE == m_pOwner->IsInFrontOfTarget(*pTarget) )
					return E_UseSkill_PosLimitBack;
			}
		}

		// 目标距离判断
		if( FALSE == m_pOwner->IsInCombatDistance(*pTarget, pSkill->GetOPDist()) )
			return E_UseSkill_DistLimit;

		// 射线检测
		if( m_pOwner->IsRayCollide(*pTarget) )
			return E_UseSkill_RayLimit;
	}

	return E_Success;
}

//-------------------------------------------------------------------------------
// 测试目标逻辑限制
//-------------------------------------------------------------------------------
INT CombatHandler::CheckTargetLogicLimit(Skill* pSkill, Unit* pTarget)
{
	if( !P_VALID(pSkill) || !P_VALID(pTarget) )
		return E_UseSkill_SkillNotExist;

	const tagSkillProto* pProto = pSkill->GetProto();
	if( !P_VALID(pProto) ) return E_UseSkill_SkillNotExist;

	// 检测目标是否不能被使用技能
	if( pTarget->IsInStateCantBeSkill() )
	{
		return E_UseSkill_TargetLimit;
	}

	// 首先检测与目标的类型标志tbc:inves
	DWORD dwTargetFlag = m_pOwner->TargetTypeFlag(pTarget);
	if( !(dwTargetFlag & pProto->dwTargetLimit) )
		return E_UseSkill_TargetLimit;

	// 再检测目标的状态限制
	DWORD dwTargetStatFlag = pTarget->GetStateFlag();
	if( (dwTargetStatFlag & pProto->dwTargetStateLimit) != dwTargetStatFlag )
	{
		return E_UseSkill_TargetStateLimit;
	}

	// 检测目标Buff限制
	if( P_VALID(pProto->dwTargetBuffLimitID) )
	{
		if( !pTarget->IsHaveBuff(pProto->dwTargetBuffLimitID) )
		{
			return E_UseSkill_TargetBuffLimit;
		}
	}	

	// 再检测敌我判断
	if( m_pOwner != pTarget )
	{
		DWORD dwFriendEnemyFlag = m_pOwner->FriendEnemy(pTarget);

		DWORD dwFriendEnemyLimit = 0;

		if( pProto->bFriendly )		dwFriendEnemyLimit |= ETFE_Friendly;
		if( pProto->bHostile )		dwFriendEnemyLimit |= ETFE_Hostile;
		if( pProto->bIndependent )	dwFriendEnemyLimit |= ETFE_Independent;

		if( !(dwFriendEnemyLimit & dwFriendEnemyFlag) )
		{
			return E_UseSkill_TargetLimit;
		}
	}

	// 判断成功
	return E_Success;
}

//----------------------------------------------------------------------------------------
// 检测地图中技能限制
//----------------------------------------------------------------------------------------
INT CombatHandler::CheckMapLimit(Skill* pSkill)
{
	// 判断地图限制
	if(P_VALID(m_pOwner->GetMap()))
	{
		BOOL bUesAble = m_pOwner->GetMap()->CanUseSkill(pSkill->GetID());
		if( !bUesAble )	return E_UseSkill_MapLimit;
	}

	return E_Success;
}

//----------------------------------------------------------------------------------------
// 测试技能使用冲突，返回TRUE为冲突，FALSE为非冲突
//----------------------------------------------------------------------------------------
BOOL CombatHandler::CheckSkillConflict(Skill* pSkill)
{
	ASSERT( P_VALID(pSkill) );

	if( !IsValid() ) return FALSE;		// 当前没有使用任何技能和任何物品

	if( IsPreparing() ) return TRUE;	// 当前正在起手，则不能使用任何技能

	if( IsUseSkill() )
	{
		// 当前正在使用技能，则查看该技能是否是普通攻击
		Skill* pCurSkill = m_pOwner->GetSkill(m_dwSkillID);
		if( P_VALID(pSkill) && ESSTE_Default != pCurSkill->GetTypeEx() )
		{
			return TRUE;
		}
		else
		{
			EndUseSkill();
			return FALSE;
		}
	}

	return FALSE;
}

//-------------------------------------------------------------------------------
// 测试技能消耗
//-------------------------------------------------------------------------------
INT CombatHandler::CheckCostLimit(Skill* pSkill)
{
	// 检测体力消耗
	INT nHPCost = pSkill->GetCost(ESCT_HP);
	if( nHPCost > 0 && m_pOwner->GetAttValue(ERA_HP) < nHPCost )
		return E_UseSkill_CostLimit;


	// 检测真气消耗
	INT nMPCost = pSkill->GetCost(ESCT_MP);
	if( nMPCost > 0 && m_pOwner->GetAttValue(ERA_MP) < nMPCost )
		return E_UseSkill_CostLimit;


	// 检测怒气消耗
	INT nRageCost = pSkill->GetCost(ESCT_Rage);
	if( nRageCost > 0 && m_pOwner->GetAttValue(ERA_Rage) < nRageCost )
		return E_UseSkill_CostLimit;


	// 检测持久消耗
	INT nEnduranceCost = pSkill->GetCost(ESCT_Endurance);
	if( nEnduranceCost > 0 && m_pOwner->GetAttValue(ERA_Endurance) < nEnduranceCost )
		return E_UseSkill_CostLimit;


	// 检测活力消耗
	INT nValicityCost = pSkill->GetCost(ESCT_Valicity);
	if( nValicityCost > 0 && m_pOwner->GetAttValue(ERA_Vitality) < nValicityCost )
		return E_UseSkill_CostLimit;

	return E_Success;
}

//-------------------------------------------------------------------------------
// 测试职业限制
//-------------------------------------------------------------------------------
INT CombatHandler::CheckVocationLimit(Skill* pSkill)
{
	//ASSERT(P_VALID(pSkill));
	if (!P_VALID(pSkill)) return E_UseSkill_SkillNotExist;

	if (!m_pOwner->IsRole()) return E_Success;

	const tagSkillProto* pProto = pSkill->GetProto();
	if( !P_VALID(pProto) ) return E_UseSkill_SkillNotExist;

	//INT nClass = (INT)((Role*)m_pOwner)->GetClass();
	//INT nClassEx = (INT)((Role*)m_pOwner)->GetClassEx();
	INT nClass = (INT)(static_cast<Role*> (m_pOwner)->GetClass());
	INT nClassEx = (INT)(static_cast<Role*> (m_pOwner)->GetClassEx());

	INT nTmpClass =  1 << ( nClass - 1 );
	INT nTmpClassEx = 0;
	INT nTmp = 0;

	if ( (INT)nClassEx != (INT)EHV_Base )
	{
		nTmpClassEx = 1 << ( nClassEx + 8 );
	}

	nTmp = nTmpClass + nTmpClassEx;

	if ( !( nTmp & pProto->dwVocationLimit ) )
		return E_UseSkill_VocationLimit;

	return E_Success;
}

//-------------------------------------------------------------------------------
// 计算攻击目标，放入到list中 
//-------------------------------------------------------------------------------
VOID CombatHandler::CalSkillTargetList()
{
	m_listTargetID.Clear();
	m_listHitedTarget.Clear();
	m_listDodgedTarget.Clear();
	m_listBlockedTarget.Clear();
	m_listCritedTarget.Clear();
	m_dwTargetEffectFlag = 0;

	// 根据该技能的攻击距离和攻击范围来判断
	Skill* pSkill = m_pOwner->GetSkill(m_dwSkillID);
	if( !P_VALID(pSkill) ) return;

	// 得到目标对象
	Unit* pTarget = NULL;
	if( GT_INVALID == m_dwTargetUnitID )	// 如果没有选目标，则目标就是自己
	{
		pTarget = m_pOwner;
	}
	else									// 如果选了目标，则找到目标
	{
		pTarget = m_pOwner->GetMap()->FindUnit(m_dwTargetUnitID);
	}
	if( !P_VALID(pTarget) ) return;

	// 根据作用类型，作用距离和作用半径来使用技能
	ESkillOPType eOPType = pSkill->GetOPType();
	FLOAT fOPDist = pSkill->GetOPDist();
	FLOAT fOPRadius = pSkill->GetOPRadius();

	// 先将目标加进去
	if( m_pOwner != pTarget )
	{
		m_listTargetID.PushBack(pTarget->GetID());
		m_dwTargetEffectFlag = CalculateSkillEffect(pSkill, pTarget);
	}

	// 爆炸效果
	if( ESOPT_Explode == eOPType )
	{
		// 如果攻击范围为0，则直接返回
		if( 0.0f == fOPRadius )
			return;

		// 如果攻击范围不为0，则以目标为球心检测
		FLOAT fOPRadiusSQ = fOPRadius * fOPRadius;

		tagVisTile* pVisTile[ED_End] = {0};

		// 得到攻击范围内的vistile列表
		pTarget->GetMap()->GetVisTile(pTarget->GetCurPos(), fOPRadius, pVisTile);
		Role*		pRole		= NULL;
		Creature*	pCreature	= NULL;

		for(INT n = ED_Center; n < ED_End; n++)
		{
			if( !P_VALID(pVisTile[n]) ) continue;

			// 首先检测人物
			TMap<DWORD, Role*>& mapRole = pVisTile[n]->mapRole;
			TMap<DWORD, Role*>::TMapIterator it = mapRole.Begin();

			while( mapRole.PeekNext(it, pRole) )
			{
				// 和目标一样，不做处理
				if( pRole == pTarget || pRole == m_pOwner ) continue;

				// 目标对象限制判断
				if( E_Success != CheckTargetLogicLimit(pSkill, pRole) )
					continue;

				// 技能距离判断
				FLOAT fDistSQ = Vec3DistSq(pTarget->GetCurPos(), pRole->GetCurPos());
				if( fDistSQ > fOPRadiusSQ  ) continue;

				// 射线检测

				// 判断通过，则将玩家加入到列表中
				m_listTargetID.PushBack(pRole->GetID());

				// 计算技能作用结果
				CalculateSkillEffect(pSkill, pRole);
			}

			// 再检测生物
			TMap<DWORD, Creature*>& mapCreature = pVisTile[n]->mapCreature;
			TMap<DWORD, Creature*>::TMapIterator it2 = mapCreature.Begin();

			while( mapCreature.PeekNext(it2, pCreature) )
			{
				// 和目标一样，不做处理
				if( pCreature == pTarget || pCreature == m_pOwner ) continue;

				// 目标对象限制判断
				if( E_Success != CheckTargetLogicLimit(pSkill, pCreature) )
					continue;

				// 技能距离判断
				FLOAT fDistSQ = Vec3DistSq(pTarget->GetCurPos(), pCreature->GetCurPos());
				if( fDistSQ > fOPRadiusSQ  ) continue;

				// 射线检测

				// 判断通过，则将生物加入到列表中
				m_listTargetID.PushBack(pCreature->GetID());

				// 计算技能作用结果
				CalculateSkillEffect(pSkill, pCreature);
			}
		}
	}

	// 矩形
	else if( ESOPT_Rect == eOPType )
	{
		// 如果攻击范围或者攻击距离为0，则直接返回
		if( 0.0f == fOPRadius || 0.0f == fOPDist )	return;

		// 如果攻击范围不为0，则以目标为球心检测
		FLOAT fOPRadiusSQ = fOPRadius * fOPRadius;
		FLOAT fOPDistSQ = fOPDist * fOPDist;

		// 如果攻击范围和攻击距离均不为0，则以自身为基准检测
		FLOAT fTargetX = pTarget->GetCurPos().x;
		FLOAT fTargetY = pTarget->GetCurPos().y;
		FLOAT fTargetZ = pTarget->GetCurPos().z;
		FLOAT fSrcX = m_pOwner->GetCurPos().x;
		FLOAT fSrcY = m_pOwner->GetCurPos().y;
		FLOAT fSrcZ = m_pOwner->GetCurPos().z;

		// 自身到第一目标点的向量
		FLOAT fX2 = fTargetX - fSrcX;
		FLOAT fY2 = fTargetY - fSrcY;
		FLOAT fZ2 = fTargetZ - fSrcZ;

		// 如果目标就是自身，那么直接取自身的朝向向量
		if( m_pOwner == pTarget )
		{
			fX2 = m_pOwner->GetFaceTo().x;
			fZ2 = m_pOwner->GetFaceTo().z;
		}

		if( abs(fX2) < 0.001f && abs(fZ2) < 0.001f )
			return;

		// 自身到第一目标点的距离的平方
		FLOAT fDistSQ2 = fX2*fX2 + fY2*fY2 + fZ2*fZ2;

		tagVisTile* pVisTile[ED_End] = {0};

		// 得到vistile列表
		pTarget->GetMap()->GetVisTile(m_pOwner->GetVisTileIndex(), pVisTile);
		Role*		pRole		= NULL;
		Creature*	pCreature	= NULL;

		for(INT n = ED_Center; n < ED_End; n++)
		{
			if( !P_VALID(pVisTile[n]) ) continue;

			// 首先检测人物
			TMap<DWORD, Role*>& mapRole = pVisTile[n]->mapRole;
			TMap<DWORD, Role*>::TMapIterator it = mapRole.Begin();

			while( mapRole.PeekNext(it, pRole) )
			{
				// 和目标一样，不做处理
				if( pRole == pTarget || pRole == m_pOwner ) continue;

				// 目标对象限制判断
				if( E_Success != CheckTargetLogicLimit(pSkill, pRole) )
					continue;

				// 自身到当前点的向量
				FLOAT fX1 = pRole->GetCurPos().x - fSrcX;
				FLOAT fY1 = pRole->GetCurPos().y - fSrcY;
				FLOAT fZ1 = pRole->GetCurPos().z - fSrcZ;

				// 先检查方位 cos(a) > 0 
				if( fX1*fX2	+ fZ1*fZ2 < 0.0f )
					continue;

				FLOAT fDist1 = fX1*fX2 + fY1*fY2 + fZ1*fZ2;
				FLOAT fDistSQ1 = fDist1 * fDist1;

				// 检查投影距离
				FLOAT fProjDistSQ = fDistSQ1 / fDistSQ2;
				if( fProjDistSQ > fOPDistSQ )
					continue;

				// 检查点到直线距离
				if( fX1*fX1 + fY1*fY1 + fZ1*fZ1 - fProjDistSQ > fOPRadiusSQ)
					continue;

				// 射线检测

				// 判断通过，则将玩家加入到列表中
				m_listTargetID.PushBack(pRole->GetID());

				// 计算技能作用结果
				CalculateSkillEffect(pSkill, pRole);
			}

			// 再检测生物
			TMap<DWORD, Creature*>& mapCreature = pVisTile[n]->mapCreature;
			TMap<DWORD, Creature*>::TMapIterator it2 = mapCreature.Begin();

			while( mapCreature.PeekNext(it2, pCreature) )
			{
				// 和目标一样，不做处理
				if( pCreature == pTarget || pCreature == m_pOwner ) continue;

				// 目标对象限制判断
				if( E_Success != CheckTargetLogicLimit(pSkill, pCreature) )
					continue;

				// 自身到当前点的向量
				FLOAT fX1 = pCreature->GetCurPos().x - fSrcX;
				FLOAT fY1 = pCreature->GetCurPos().y - fSrcY;
				FLOAT fZ1 = pCreature->GetCurPos().z - fSrcZ;

				// 先检查方位 cos(a) > 0 
				if( fX1*fX2	+ fZ1*fZ2 < 0.0f )
					continue;

				// 检查投影距离
				FLOAT fProjDistSQ = (fX1*fX2 + fY1*fY2 + fZ1*fZ2) / fDistSQ2;
				if( fProjDistSQ > fOPDistSQ )
					continue;

				// 检查点到直线距离
				if( fX1*fX1 + fY1*fY1 + fZ1*fZ1 - fProjDistSQ > fOPRadiusSQ)
					continue;

				// 射线检测

				// 判断通过，则将玩家加入到列表中
				m_listTargetID.PushBack(pCreature->GetID());

				// 计算技能作用结果
				CalculateSkillEffect(pSkill, pCreature);
			}
		}
	}
}

//-------------------------------------------------------------------------------
// 计算技能效果
//-------------------------------------------------------------------------------
DWORD CombatHandler::CalculateSkillEffect(Skill* pSkill, Unit* pTarget)
{
	DWORD dwTargetEffectFlag = 0;

	DWORD dwTargetID = pTarget->GetID();

	INT nDmgTimes = pSkill->GetDmgTimes();

	// 无伤害技能，必命中
	if( nDmgTimes <= 0 )
	{
		m_listHitedTarget.PushBack(dwTargetID);
		dwTargetEffectFlag |= ETEF_Hited;

		tagNS_HitTarget send;
		send.dwRoleID = pTarget->GetID();
		send.dwSrcRoleID = m_pOwner->GetID();
		send.eCause = EHTC_Skill;
		send.dwMisc = pSkill->GetTypeID();
		send.dwSerial = m_dwSkillSerial;

		if( P_VALID(pTarget->GetMap()) )
		{
			pTarget->GetMap()->SendBigVisTileMsg(pTarget, &send, send.dwSize);
		}

		pTarget->OnBeAttacked(m_pOwner, pSkill, TRUE, FALSE, FALSE);
		return dwTargetEffectFlag;
	}

	// 计算命中
	BOOL bHit = CalculateHit(pSkill, pTarget);
	if( FALSE == bHit )
	{
		// 未命中
		m_listDodgedTarget.PushBack(dwTargetID);
	}
	else
	{
		// 命中
		m_listHitedTarget.PushBack(dwTargetID);
		dwTargetEffectFlag |= ETEF_Hited;

		tagNS_HitTarget send;
		send.dwRoleID = pTarget->GetID();
		send.dwSrcRoleID = m_pOwner->GetID();
		send.eCause = EHTC_Skill;
		send.dwMisc = pSkill->GetTypeID();
		send.dwSerial = m_dwSkillSerial;

		if( P_VALID(pTarget->GetMap()) )
		{
			pTarget->GetMap()->SendBigVisTileMsg(pTarget, &send, send.dwSize);
		}

		// 计算格挡
		BOOL bBlocked = CalculateBlock(pSkill, pTarget);
		if( TRUE == bBlocked )
		{
			// 被格挡
			m_listBlockedTarget.PushBack(dwTargetID);
			dwTargetEffectFlag |= ETEF_Block;
		}
		else
		{
			// 计算暴击
			BOOL bCrit = CalculateCritRate(pSkill, pTarget);
			if( TRUE == bCrit )
			{
				m_listCritedTarget.PushBack(dwTargetID);
				dwTargetEffectFlag |= ETEF_Crited;
			}
		}
	}

	// 被攻击方的被攻击触发
	pTarget->OnBeAttacked(m_pOwner, pSkill,
		dwTargetEffectFlag & ETEF_Hited, dwTargetEffectFlag & ETEF_Block, dwTargetEffectFlag & ETEF_Crited);

	return dwTargetEffectFlag;
}


//--------------------------------------------------------------------------------
// 计算命中
//--------------------------------------------------------------------------------
BOOL CombatHandler::CalculateHit(Skill* pSkill, Unit* pTarget)
{
	FLOAT fHit = 0.0f;

	// 外功攻击
	if( pSkill->IsExAttackSkill() )
	{
		// 命中率=0.9×[1-（防御方当前闪避-攻击方当前命中）/10000]×（1-攻击方攻击失误率）÷（1+防御方近、远程躲避率）+技能命中率
		fHit = 0.9f * (1.0f - (FLOAT)(pTarget->GetAttValue(ERA_Dodge) - m_pOwner->GetAttValue(ERA_HitRate)) / 10000.0f);
		if( fHit < 0.0f ) fHit = 0.0f;
		if( fHit > 1.0f ) fHit = 1.0f;

		fHit = fHit	* (1.0f - FLOAT(m_pOwner->GetAttValue(ERA_Attack_MissRate)) / 10000.0f);

		// 根据是近身还是远程决定选择哪个躲避率
		if( pSkill->IsMelee() )
		{
			fHit = fHit / (1.0f + FLOAT(pTarget->GetAttValue(ERA_CloseAttack_DodgeRate)) / 10000.0f);
		}
		else if( pSkill->IsRanged() )
		{
			fHit = fHit / (1.0f + FLOAT(pTarget->GetAttValue(ERA_RemoteAttack_DodgeRate)) / 10000.0f);
		}
	}

	// 内功攻击
	else if( pSkill->IsInAttackSkill() )
	{
		// 命中率=1×[1-（防御方当前闪避-攻击方当前命中）/8500]×（1-攻击方攻击失误率）+技能命中率
		fHit = 1.0f - (FLOAT)(pTarget->GetAttValue(ERA_Dodge) - m_pOwner->GetAttValue(ERA_HitRate)) / 8500.0f;
		if( fHit < 0.0f ) fHit = 0.0f;
		if( fHit > 1.0f ) fHit = 1.0f;

		fHit = fHit * (1.0f - FLOAT(m_pOwner->GetAttValue(ERA_Attack_MissRate)) / 10000.0f);
	}

	// 绝技攻击
	else if( pSkill->IsStuntSkill() )
	{
		// 命中率=100%×（1-攻击方攻击失误率）+技能命中率
		fHit = 1.0f - FLOAT(m_pOwner->GetAttValue(ERA_Attack_MissRate)) / 10000.0f;
	}

	// else
	else
	{

	}

	// 增加技能命中率
	fHit += pSkill->GetHit();

	// 范围：30——100%
	if( fHit < 0.3f ) fHit = 0.3f;
	if( fHit > 1.0f ) fHit = 1.0f;

	// 随机看是否能命中
	return IUTIL->Probability(INT(fHit*100.0f));
}

//----------------------------------------------------------------------------
// 计算格挡
//----------------------------------------------------------------------------
BOOL CombatHandler::CalculateBlock(Skill* pSkill, Unit* pTarget)
{
	// 只有攻击放处在防御方前面时，防御方才可以格挡
	if( FALSE == m_pOwner->IsInFrontOfTarget(*pTarget) )
		return FALSE;

	// 格挡率
	FLOAT fBlock = 0.0f;

	// 外功攻击
	if( pSkill->IsExAttackSkill() )
	{
		// 远程攻击
		if( pSkill->IsMelee() )
		{
			// 基础格挡率=（防御方当前外功防御-（攻击方外功攻击+攻击方内功攻击）/4）/30000
			// 格挡率=[1+（防御方当前防御技巧-攻击方当前攻击技巧）/6000] ×基础格挡率+防御方格档几率加乘
			FLOAT fBaseBlock = (FLOAT(pTarget->GetAttValue(ERA_ExDefense)) - FLOAT(m_pOwner->GetAttValue(ERA_ExAttack) + m_pOwner->GetAttValue(ERA_InAttack)) / 4.0f) / 30000.0f;
			fBlock = (1.0f + FLOAT(pTarget->GetAttValue(ERA_DefenseTec) - m_pOwner->GetAttValue(ERA_AttackTec)) / 6000.0f) * fBaseBlock + (FLOAT)pTarget->GetAttValue(ERA_Block_Rate) / 10000.0f;
		}

		// 远程攻击
		else if( pSkill->IsRanged() )
		{
			// 格挡率=0
			fBlock = 0.0f;
		}		
	}

	// 内功攻击
	else if( pSkill->IsInAttackSkill() )
	{
		// 基础格挡率=（防御方当前内功防御-（攻击方外功攻击+攻击方内功攻击）/4）/30000
		// 格挡率=[1+（防御方当前防御技巧-攻击方当前攻击技巧）/6600] ×基础格挡率+防御方格档几率加乘
		FLOAT fBaseBlock = (FLOAT(pTarget->GetAttValue(ERA_InDefense)) - FLOAT(m_pOwner->GetAttValue(ERA_ExAttack) + m_pOwner->GetAttValue(ERA_InAttack)) / 4.0f) / 30000.0f;
		fBlock = (1.0f + FLOAT(pTarget->GetAttValue(ERA_DefenseTec) - m_pOwner->GetAttValue(ERA_AttackTec)) / 6600.0f) * fBaseBlock + (FLOAT)pTarget->GetAttValue(ERA_Block_Rate) / 10000.0f;
	}

	// 绝技攻击
	else if( pSkill->IsStuntSkill() )
	{
		fBlock = 0.0f;
	}

	// else
	else
	{

	}

	// 范围：0——100%
	if( fBlock < 0.0f ) fBlock = 0.0f;
	if( fBlock > 1.0f ) fBlock = 1.0f;

	// 随机看是否能命中
	return IUTIL->Probability(INT(fBlock*100.0f));
}

//-----------------------------------------------------------------------------
// 计算致命率
//-----------------------------------------------------------------------------
BOOL CombatHandler::CalculateCritRate(Skill* pSkill, Unit* pTarget)
{
	// 基础致命率=（攻击方当前攻击技巧/150000）^0.5
	// 致命率=基础致命率×(攻击方当前攻击技巧/被攻击方当前防御技巧)^0.5+致命加成/1000+技能附带致命率
	FLOAT fCrit = m_pOwner->GetAttValue(ERA_AttackTec) / 150000.0f;
	fCrit = IFASTCODE->FastSqrt(fCrit * ((FLOAT)m_pOwner->GetAttValue(ERA_AttackTec) / (FLOAT)pTarget->GetAttValue(ERA_DefenseTec)));
	fCrit = fCrit + (FLOAT)m_pOwner->GetAttValue(ERA_Crit_Rate) / 1000.0f + pSkill->GetCrit();

	// 范围：0——100%
	if( fCrit < 0.0f ) fCrit = 0.0f;
	if( fCrit > 1.0f ) fCrit = 1.0f;

	// 随机看是否能命中
	return IUTIL->Probability(INT(fCrit*100.0f));
}

//-----------------------------------------------------------------------------
// 计算致命量
//-----------------------------------------------------------------------------
FLOAT CombatHandler::CalculateCritAmount(Skill* pSkill, Unit* pTarget)
{
	// 外功攻击
	if( pSkill->IsExAttackSkill() )
	{
		// 致命伤害量加成=（攻击方当前外功攻击/2000）^0.25+攻击方致命量加成（装备、技能、物品等影响）
		return pow(FLOAT(m_pOwner->GetAttValue(ERA_ExAttack) / 2000.0f), 0.25f) + (FLOAT)m_pOwner->GetAttValue(ERA_Crit_Amount) / 10000.0f;
	}

	// 内功攻击
	else if( pSkill->IsInAttackSkill() )
	{
		// 致命伤害量加成=（攻击方当前内功攻击/8000）^0.25+攻击方致命量加成（装备、技能、物品等影响）
		return pow(FLOAT(m_pOwner->GetAttValue(ERA_InAttack) / 8000.0f), 0.25f) + (FLOAT)m_pOwner->GetAttValue(ERA_Crit_Amount) / 10000.0f;
	}

	// 绝技攻击
	else if( pSkill->IsStuntSkill() )
	{
		// 致命伤害量加成=（（攻击方当前内功攻击+攻击方当前外功攻击）/1000）^0.25+攻击方致命量加成（装备、技能、物品等影响）
		return pow(FLOAT((m_pOwner->GetAttValue(ERA_InAttack) + m_pOwner->GetAttValue(ERA_ExAttack)) / 1000.0f), 0.25f) + (FLOAT)m_pOwner->GetAttValue(ERA_Crit_Amount) / 10000.0f;
	}

	// else
	else
	{

	}

	return 0.0f;
}

//-----------------------------------------------------------------------------
// 计算技能伤害
//-----------------------------------------------------------------------------
VOID CombatHandler::CalculateDmg(Skill* pSkill, Unit* pTarget)
{
	// 目标已经死亡，直接返回
	if( pTarget->IsDead() ) return;

	DWORD dwTargetID = pTarget->GetID();

	// 暴击参数
	bool bCrit = false;
	FLOAT fCrit = 0.0f;

	// 首先判断该目标是否被闪避了
	if( m_listDodgedTarget.IsExist(dwTargetID) )
	{
		// 发送未命中消息
		tagNS_RoleHPChange send;
		send.dwRoleID = pTarget->GetID();
		send.dwSrcRoleID = m_pOwner->GetID();
		send.eCause = ERHPCC_SkillDamage;
		send.bMiss = true;
		send.dwMisc = pSkill->GetTypeID();
		send.dwMisc2 = m_nSkillCurDmgIndex;
		send.dwSerial = m_dwSkillSerial;

		if( P_VALID(pTarget->GetMap()) )
		{
			pTarget->GetMap()->SendBigVisTileMsg(pTarget, &send, send.dwSize);
		}
		return;
	}

	// 再判断是否被格挡了
	else if( m_listBlockedTarget.IsExist(dwTargetID) )
	{
		// 发送格挡消息
		tagNS_RoleHPChange send;
		send.dwRoleID = pTarget->GetID();
		send.dwSrcRoleID = m_pOwner->GetID();
		send.eCause = ERHPCC_SkillDamage;
		send.bBlocked = true;
		send.dwMisc = pSkill->GetTypeID();
		send.dwMisc2 = m_nSkillCurDmgIndex;
		send.dwSerial = m_dwSkillSerial;

		if( P_VALID(pTarget->GetMap()) )
		{
			pTarget->GetMap()->SendBigVisTileMsg(pTarget, &send, send.dwSize);
		}
		return;
	}

	// 再判断是否暴击
	else if( m_listCritedTarget.IsExist(dwTargetID) )
	{
		// 计算暴击参数
		bCrit = true;
		fCrit = CalculateCritAmount(pSkill, pTarget);
	}

	FLOAT fBaseDmg		=	CalBaseDmg(pSkill);						// 基础伤害
	FLOAT fAttDefCoef	=	CalAttackDefenceCoef(pSkill, pTarget);	// 攻防影响
	//FLOAT fMoraleCoef	=	CalMoraleCoef(pTarget);					// 士气影响
	FLOAT fDerateCoef	=	CalDerateCoef(pSkill, pTarget);			// 减免影响
	FLOAT fInjuryCoef	=	CalInjuryCoef();						// 内伤影响
	FLOAT fLevelCoef	=	CalLevelCoef(pSkill, pTarget);			// 等级影响

	// 最终伤害
	FLOAT fDmg = fBaseDmg * fAttDefCoef /** fMoraleCoef*/ * fDerateCoef * fInjuryCoef * fLevelCoef;
	fDmg = fDmg + (FLOAT)m_pOwner->GetAttValue(ERA_ExDamage) - (FLOAT)pTarget->GetAttValue(ERA_ExDamage_Absorb);

	// 计算暴击参数
	INT nDmg = INT(fDmg * (1.0f + fCrit));
	if( nDmg < 1 ) nDmg = 1;
	
	// 击飞 = 可击飞 + 暴击+20%
	// 击飞的距离先确定为15格子
	// 玩家不可击飞,怪物均可击飞
	if ( pTarget->IsCreature() && bCrit && fDmg >= pTarget->GetAttValue(ERA_HP)*0.2)
	{
		// 是否可被击飞
		Creature *pCreature = static_cast<Creature*>(pTarget);
		if ( 1 == pCreature->GetProto()->bCanHitFly )
			m_pOwner->HitFly(pTarget);
	}

	// 减血
	pTarget->ChangeHP(-nDmg, m_pOwner, pSkill, NULL, bCrit, m_dwSkillSerial, m_nSkillCurDmgIndex);
}

//------------------------------------------------------------------------------
// 计算基础伤害
//------------------------------------------------------------------------------
inline FLOAT CombatHandler::CalBaseDmg(Skill* pSkill)
{
	FLOAT	fBaseDmg	=	1.0f;

	FLOAT	fWeaponDmg	=	FLOAT(IUTIL->RandomInRange(m_pOwner->GetAttValue(ERA_WeaponDmgMin), m_pOwner->GetAttValue(ERA_WeaponDmgMax))) / (FLOAT)pSkill->GetDmgTimes();
	FLOAT	fWeaponSoul	=	(FLOAT)m_pOwner->GetAttValue(ERA_WeaponSoul) / (FLOAT)pSkill->GetDmgTimes();
	INT		nSkillDmg	=	pSkill->GetDmg(m_nSkillCurDmgIndex);

	// 外功攻击基础伤害
	// （武器伤害/技能伤害次数+技能威力）×（1+攻击方当前外功攻击/10000）+攻击方当前外功攻击×0.02/技能攻击次数
	if( pSkill->IsExAttackSkill() )
	{
		if( nSkillDmg > 100000 )
		{
			// 取的是武器伤害的倍数
			fBaseDmg = fWeaponDmg * (FLOAT(nSkillDmg - 100000) / 10000.0f);
		}
		else
		{
			// 取的是技能伤害
			fBaseDmg = fWeaponDmg + (FLOAT)nSkillDmg;
		}

		fBaseDmg = fBaseDmg * (1.0f + (FLOAT)m_pOwner->GetAttValue(ERA_ExAttack) / 10000.0f);
		fBaseDmg = fBaseDmg + (FLOAT)m_pOwner->GetAttValue(ERA_ExAttack) * 0.02f / (FLOAT)pSkill->GetDmgTimes();
	}

	// 内功攻击基础伤害
	// （武魂/技能攻击次数+技能威力）×（1+攻击方当前内功攻击/10000）+攻击方当前内功攻击×0.02/技能攻击次数
	else if( pSkill->IsInAttackSkill() )
	{
		if( nSkillDmg > 100000 )
		{
			// 取的是武器伤害的倍数
			fBaseDmg = fWeaponSoul * ((FLOAT)(nSkillDmg - 100000) / 10000.0f);
		}

		else
		{
			// 取的是技能伤害
			fBaseDmg = fWeaponSoul + (FLOAT)nSkillDmg;
		}

		fBaseDmg = fBaseDmg * (1.0f + (FLOAT)m_pOwner->GetAttValue(ERA_InAttack) / 10000.0f);
		fBaseDmg = fBaseDmg + (FLOAT)m_pOwner->GetAttValue(ERA_InAttack) * 0.02f / (FLOAT)pSkill->GetDmgTimes();

	}

	// 绝技攻击基础伤害
	// （绝技技能威力+武器伤害/技能攻击次数+武魂/技能攻击次数）×（1+（当前外功功攻击+当前内攻攻击）/10000）+（攻击方当前内功攻击+攻击方当前外功攻击）×0.2
	else if( pSkill->IsStuntSkill() )
	{
		if( nSkillDmg > 100000 )
		{
			// 取的是武器伤害的倍数
			fBaseDmg = (fWeaponDmg + fWeaponSoul) * (FLOAT(nSkillDmg - 100000) / 10000.0f);
		}
		else
		{
			// 取的是技能伤害
			fBaseDmg = fWeaponDmg + fWeaponSoul + (FLOAT)nSkillDmg;
		}

		fBaseDmg = fBaseDmg * (1.0f + FLOAT(m_pOwner->GetAttValue(ERA_InAttack) + m_pOwner->GetAttValue(ERA_ExAttack)) / 10000.0f);
		fBaseDmg = fBaseDmg + FLOAT(m_pOwner->GetAttValue(ERA_InAttack) + m_pOwner->GetAttValue(ERA_ExAttack)) * 0.2f;
	}

	return fBaseDmg;
}

//-----------------------------------------------------------------------------
// 计算攻防影响
//-----------------------------------------------------------------------------
inline FLOAT CombatHandler::CalAttackDefenceCoef(Skill* pSkill, Unit* pTarget)
{
	// 外功攻击攻防影响
	// [1+（攻击方外功攻击-防御方外功防御）/（1000+攻方等级*25）]×[1+（攻击方攻击技巧-防御方防御技巧）/（1000+攻方等级*25）]
	// 乘法的两个因子分别在0.5——2.0之间
	if( pSkill->IsExAttackSkill() )
	{
		FLOAT fCoef1 = 1.0f + FLOAT(m_pOwner->GetAttValue(ERA_ExAttack) - pTarget->GetAttValue(ERA_ExDefense)) / (1000.0f + FLOAT(m_pOwner->GetLevel())*25.0f);
		FLOAT fCoef2 = 1.0f + FLOAT(m_pOwner->GetAttValue(ERA_AttackTec) - pTarget->GetAttValue(ERA_DefenseTec)) / (1000.0f + FLOAT(m_pOwner->GetLevel())*25.0f);
		if( fCoef1 < 0.5f ) fCoef1 = 0.5f;
		if( fCoef1 > 2.0f ) fCoef1 = 2.0f;
		if( fCoef2 < 0.5f ) fCoef2 = 0.5f;
		if( fCoef2 > 2.0f ) fCoef2 = 2.0f;

		return fCoef1 * fCoef2;
	}

	// 内功攻击攻防影响
	// [1+（攻击方内功攻击-防御方内功防御）/（880+攻方等级*22）]×[1+（攻击方攻击技巧-防御方防御技巧）/（1400+攻方等级*35）]
	// 乘法的两个因子分别在0.5——2.0之间
	else if( pSkill->IsInAttackSkill() )
	{
		FLOAT fCoef1 = 1.0f + FLOAT(m_pOwner->GetAttValue(ERA_InAttack) - pTarget->GetAttValue(ERA_InDefense)) / (880.0f + FLOAT(m_pOwner->GetLevel())*22.0f);
		FLOAT fCoef2 = 1.0f + FLOAT(m_pOwner->GetAttValue(ERA_AttackTec) - pTarget->GetAttValue(ERA_DefenseTec)) / (1400.0f + FLOAT(m_pOwner->GetLevel())*35.0f);
		if( fCoef1 < 0.5f ) fCoef1 = 0.5f;
		if( fCoef1 > 2.0f ) fCoef1 = 2.0f;
		if( fCoef2 < 0.5f ) fCoef2 = 0.5f;
		if( fCoef2 > 2.0f ) fCoef2 = 2.0f;

		return fCoef1 * fCoef2;
	}

	// 绝技攻击攻防影响
	// 外功攻防影响+内功攻防影响
	else if( pSkill->IsStuntSkill() )
	{
		FLOAT fCoef1 = 1.0f + FLOAT(m_pOwner->GetAttValue(ERA_ExAttack) - pTarget->GetAttValue(ERA_ExDefense)) / 1000.0f;
		FLOAT fCoef2 = 1.0f + FLOAT(m_pOwner->GetAttValue(ERA_AttackTec) - pTarget->GetAttValue(ERA_DefenseTec)) / 1000.0f;
		FLOAT fCoef3 = 1.0f + FLOAT(m_pOwner->GetAttValue(ERA_InAttack) - pTarget->GetAttValue(ERA_InDefense)) / 800.0f;
		FLOAT fCoef4 = 1.0f + FLOAT(m_pOwner->GetAttValue(ERA_AttackTec) - pTarget->GetAttValue(ERA_DefenseTec)) / 2000.0f;
		if( fCoef1 < 0.5f ) fCoef1 = 0.5f;
		if( fCoef1 > 2.0f ) fCoef1 = 2.0f;
		if( fCoef2 < 0.5f ) fCoef2 = 0.5f;
		if( fCoef2 > 2.0f ) fCoef2 = 2.0f;
		if( fCoef3 < 0.5f ) fCoef3 = 0.5f;
		if( fCoef3 > 2.0f ) fCoef3 = 2.0f;
		if( fCoef4 < 0.5f ) fCoef4 = 0.5f;
		if( fCoef4 > 2.0f ) fCoef4 = 2.0f;

		return fCoef1 * fCoef2 * fCoef3 * fCoef4;
	}
	else
	{
		return 0.0f;
	}
}

//-----------------------------------------------------------------------
// 计算士气影响
//-----------------------------------------------------------------------
inline FLOAT CombatHandler::CalMoraleCoef(Unit* pTarget)
{
	// 士气影响=[1+（攻击方士气-防御方士气）/100]
	return FLOAT(m_pOwner->GetAttValue(ERA_Morale) - pTarget->GetAttValue(ERA_Morale)) / 100.0f;
}

//------------------------------------------------------------------------
// 计算减免影响
//------------------------------------------------------------------------
inline FLOAT CombatHandler::CalDerateCoef(Skill* pSkill, Unit* pTarget)
{
	FLOAT fDerateCoef = 1.0f;

	ERoleAttribute eAtt = pTarget->SkillDmgType2ERA(pSkill->GetDmgType());
	if( ERA_Null == eAtt ) return fDerateCoef;

	// 外功攻击减免
	// 1 - 防御方伤害减免 - 防御方护甲减免
	if( pSkill->IsExAttackSkill() )
	{
		// 先计算护甲
		FLOAT fArmor = 1.0f;
		if( ERA_Derate_Ordinary == eAtt )
		{
			// 普通伤害护甲减免 = 护甲值/（31×防御方等级+267）
			fArmor = GetTargetArmor(*pTarget) / (31.0f * (FLOAT)pTarget->GetLevel() + 267.0f);
		}
		else
		{
			// 外功伤害护甲减免 = 护甲值/（46.5×防御方等级+400）
			fArmor = GetTargetArmor(*pTarget) / (46.5f * (FLOAT)pTarget->GetLevel() + 400.0f);
		}

		// 外功伤害减免=（##伤害减免 + 外功伤害减免 + 全部伤害减免– 人物等级）/ 1000
		fDerateCoef = fArmor + FLOAT(pTarget->GetAttValue(eAtt) + pTarget->GetAttValue(ERA_Derate_ExAttack) + pTarget->GetAttValue(ERA_Derate_ALL) - pTarget->GetLevel()) / 1000.0f;
	}

	// 内功攻击减免
	// 1 - 防御方伤害减免 - 防御方护甲减免
	else if( pSkill->IsInAttackSkill() )
	{
		// 内功伤害护甲减免 = 护甲值/（93×防御方等级+800）
		FLOAT fArmor = GetTargetArmor(*pTarget) / (93.0f * (FLOAT)pTarget->GetLevel() + 800.0f);
		// 内功伤害减免=（##伤害减免 + 内功伤害减免 + 全部伤害减免 – 人物等级）/ 1000
		fDerateCoef = fArmor + FLOAT(pTarget->GetAttValue(eAtt) + pTarget->GetAttValue(ERA_Derate_InAttack) + pTarget->GetAttValue(ERA_Derate_ALL) - pTarget->GetLevel()) / 1000.0f;
	}

	// 绝技攻击减免
	// 减免影响=1-防御方伤害减免
	else if( pSkill->IsStuntSkill() )
	{

		// 绝技伤害减免 = （绝技伤害减免 + 全部伤害减免 – 人物等级）/ 1000
		fDerateCoef = (FLOAT)(pTarget->GetAttValue(eAtt) + pTarget->GetAttValue(ERA_Derate_ALL) - pTarget->GetLevel()) / 1000.0f;
	}

	// 计算最终值
	fDerateCoef = 1.0f - fDerateCoef;
	if( fDerateCoef < 0.2f ) fDerateCoef = 0.2f;
	if( fDerateCoef > 2.0f ) fDerateCoef = 2.0f;

	return fDerateCoef;
}

//------------------------------------------------------------------------
// 计算内伤影响
//------------------------------------------------------------------------
inline FLOAT CombatHandler::CalInjuryCoef()
{
	// 内伤影响=1－内伤×0.1÷（4＋内伤×0.06）
	return (1.0f -(FLOAT)m_pOwner->GetAttValue(ERA_Injury) * 0.1f / (4.0f + (FLOAT)m_pOwner->GetAttValue(ERA_Injury) * 0.06));
}

//-------------------------------------------------------------------------
// 计算等级影响
//-------------------------------------------------------------------------
inline FLOAT CombatHandler::CalLevelCoef(Skill* pSkill, Unit* pTarget)
{
	// 外功攻击和内功攻击
	// 1-（防御方等级-攻击方等级）/75     取值（0.2~1.8）
	if( pSkill->IsExAttackSkill() || pSkill->IsInAttackSkill() )
	{
		FLOAT fCoef = 1.0f - FLOAT(pTarget->GetLevel() - m_pOwner->GetLevel()) / 75.0f;
		if( fCoef < 0.2f ) fCoef = 0.2f;
		if( fCoef > 1.8f ) fCoef = 1.8f;

		return fCoef;
	}
	else
	{
		return 1.0f;
	}
}

//----------------------------------------------------------------------------
// 计算技能消耗
//----------------------------------------------------------------------------
VOID CombatHandler::CalculateCost(Skill* pSkill)
{
	// 体力消耗
	INT nHPCost = pSkill->GetCost(ESCT_HP);
	if( nHPCost > 0  )
	{
		m_pOwner->ChangeHP(-nHPCost, m_pOwner);
	}

	// 真气消耗
	INT nMPCost = pSkill->GetCost(ESCT_MP);
	if( nMPCost > 0  )
	{
		m_pOwner->ChangeMP(-nMPCost);
	}

	// 怒气消耗
	INT nRageCost = pSkill->GetCost(ESCT_Rage);
	if( nRageCost > 0 )
	{
		m_pOwner->ChangeRage(-nRageCost);
	}


	// 持久消耗
	INT nEnduranceCost = pSkill->GetCost(ESCT_Endurance);
	if( nEnduranceCost > 0 )
	{
		m_pOwner->ChangeEndurance(-nEnduranceCost);
	}

	// 活力消耗
	INT nValicityCost = pSkill->GetCost(ESCT_Valicity);
	if( nValicityCost > 0  )
	{
		m_pOwner->ChangeVitality(-nValicityCost);
	}
}

//----------------------------------------------------------------------------
// 物品使用判断
//----------------------------------------------------------------------------
INT	CombatHandler::CanUseItem(tagItem *pItem)
{
	if( !P_VALID(pItem)  )
		return E_SystemError;

	if( CheckItemConflict(pItem) ) return E_UseItem_Operating;

	INT nRet = E_Success;

	nRet = CheckItemAbility(pItem);
	if( E_Success != nRet ) return nRet;

	nRet = CheckOwnerLimitItem();
	if( E_Success != nRet ) return nRet;

	nRet = CheckRoleProtoLimit(pItem);
	if( E_Success != nRet ) return nRet;

	nRet = CheckRoleStateLimit(pItem);
	if( E_Success != nRet ) return nRet;

	nRet = CheckRoleVocationLimit(pItem);
	if( E_Success != nRet ) return nRet;

	nRet = CheckMapLimit(pItem);
	if( E_Success != nRet ) return nRet;

	return nRet;
}


//----------------------------------------------------------------------------
// 检测物品本身
//----------------------------------------------------------------------------
INT	CombatHandler::CheckItemAbility(tagItem *pItem)
{
	if( !P_VALID(pItem) ) return E_UseItem_ItemNotExist;

	// 物品是否是可使用物品
	if(MIsEquipment(pItem->dwTypeID) || pItem->pProtoType->dwBuffID0 == GT_INVALID)
		return E_UseItem_ItemCanNotUse;

	// 物品的冷却时间还没到，则不可以使用
	if(((Role*)m_pOwner)->GetItemMgr().IsItemCDTime(pItem->dwTypeID))
		return E_UseItem_CoolDowning;

	return E_Success;
}

//----------------------------------------------------------------------------
// 检测使用者本身
//----------------------------------------------------------------------------
INT CombatHandler::CheckOwnerLimitItem()
{
	// 是否处在不能使用技能的状态
	if( m_pOwner->IsInStateCantCastSkill() )
		return E_UseItem_UseLimit;

	return E_Success;
}

//----------------------------------------------------------------------------
// 检测人物属性限制
//----------------------------------------------------------------------------
INT CombatHandler::CheckRoleProtoLimit(tagItem *pItem)
{
	if( !P_VALID(pItem) ) return E_UseItem_ItemNotExist;

	// 性别限制
	if( pItem->pProtoType->eSexLimit != ESL_Null )
	{
		if( ESL_Man == pItem->pProtoType->eSexLimit )
		{
			if( 1 != m_pOwner->GetSex() )
				return E_UseItem_SexLimit;
		}
		else if( ESL_Woman == pItem->pProtoType->eSexLimit )
		{
			if( 0 != m_pOwner->GetSex() )
				return E_UseItem_SexLimit;
		}
		else
		{

		}
	}

	// 等级限制
	if(pItem->pProtoType->byMinUseLevel > m_pOwner->GetLevel() 
		|| pItem->pProtoType->byMaxUseLevel < m_pOwner->GetLevel())
		return E_UseItem_LevelLimit;

	// 职业限制

	return E_Success;
}


//----------------------------------------------------------------------------
// 检测人物状态限制
//----------------------------------------------------------------------------
INT CombatHandler::CheckRoleStateLimit(tagItem *pItem)
{
	// 特殊状态限制（死亡 ，眩晕）
	DWORD dwSelfStateFlag = m_pOwner->GetStateFlag();

	if( (dwSelfStateFlag & pItem->pProtoType->dwStateLimit) != dwSelfStateFlag )
	{
		return E_UseItem_SelfStateLimit;
	}

	Role *pRole = dynamic_cast<Role *>(m_pOwner);
	if(pRole != NULL)
	{
		DWORD dwRoleState = pRole->GetState();
		if(pRole->IsInRoleState(ERS_Wedding))
		{
			return E_UseItem_SelfStateLimit;
		}
	}
	// 玩家在活动中的限制

	// 副本限制
	return E_Success;
}

//----------------------------------------------------------------------------
// 检测人物职业限制
//----------------------------------------------------------------------------
INT CombatHandler::CheckRoleVocationLimit(tagItem *pItem)
{
	if(!P_VALID(pItem)) return E_UseItem_ItemNotExist;

	if(!m_pOwner->IsRole()) return E_Success;

	INT nClass = (INT)(static_cast<Role*> (m_pOwner)->GetClass());
	INT nClassEx = (INT)(static_cast<Role*> (m_pOwner)->GetClassEx());

	INT nTmpClass =  1 << ( nClass - 1 );
	INT nTmpClassEx = 0;

	if ( (INT)nClassEx != (INT)EHV_Base )
	{
		nTmpClassEx = 1 << ( nClassEx + 8 );
	}

	if ( ( nTmpClass + nTmpClassEx ) & pItem->pProtoType->dwVocationLimit )
		return E_Success;
	else
		return E_UseItem_VocationLimit;
}

//----------------------------------------------------------------------------
// 检测地图限制
//----------------------------------------------------------------------------
INT CombatHandler::CheckMapLimit(tagItem* pItem)
{
	// 判断地图限制
	if(P_VALID(m_pOwner->GetMap()))
	{
		BOOL bUesAble = m_pOwner->GetMap()->CanUseItem(pItem->dwTypeID);
		if( !bUesAble )	return E_UseItem_MapLimit;
	}
	
	return E_Success;
}

//-------------------------------------------------------------------------------
// 测试物品使用冲突，如果冲突则为TRUE，不冲突为FALSE
//-------------------------------------------------------------------------------
BOOL CombatHandler::CheckItemConflict(tagItem* pItem)
{
	if( IsUseItem() ) return TRUE;	// 当前正在使用物品，则不能使用

	if( IsUseSkill() )
	{
		// 如果物品是起手物品，则不可以使用
		if( pItem->pProtoType->nPrepareTime > 0 ) return TRUE;
		else return FALSE;
	}

	return FALSE;
}

//-------------------------------------------------------------------------------
// 获取buff影响后的目标护甲值
//-------------------------------------------------------------------------------
FLOAT CombatHandler::GetTargetArmor(Unit &target)
{
	return (FLOAT)target.GetAttValue(ERA_Armor) * m_fTargetArmorLeftPct;
}