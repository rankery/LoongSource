//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: ai_transition.h
// author: wjiang
// actor:
// data: 2009-5-31
// last:
// brief: AI状态转换器
//-------------------------------------------------------------------------------------------------------


#include "stdafx.h"
#include "ai_transition.h"
#include "creature_ai.h"
#include "creature.h"

AITransition* AggressiveTransition::Instance()
{
	static AggressiveTransition instance;
	return &instance;
}

//------------------------------------------------------------------------------------------
// 攻击型怪物状态转换
//------------------------------------------------------------------------------------------
AIStateType	AggressiveTransition::Transition(AIController* pAI, AIStateType eCurState)
{
	switch(eCurState)
	{
	case AIST_Idle:
		return IdleTransition(pAI);

	case AIST_Pursue:
		return PursueTransition(pAI);

	case AIST_Attack:
		return AttackTransition(pAI);

	case AIST_Return:
		return ReturnTransition(pAI);

	default:
		return eCurState;
	}
}

//------------------------------------------------------------------------------------------
// 空闲状态转换
//------------------------------------------------------------------------------------------
AIStateType AggressiveTransition::IdleTransition(AIController* pAI)
{
	// 索敌
	if( pAI->GetMaxEnmityUnitID() != GT_INVALID )
	{
		return AIST_Pursue;
	}

	return AIST_Idle;
}

//------------------------------------------------------------------------------------------
// 追击状态转换
//------------------------------------------------------------------------------------------
AIStateType AggressiveTransition::PursueTransition(AIController* pAI)
{
	// 如果现在target非法，则切换成返回状态
	if( !P_VALID(pAI->GetTargetUnitID()) )
	{
		return AIST_Return;
	}

	// 如果当前目标还存在，则检测技能
	if( pAI->IsInAttackDist(pAI->GetTargetUnitID()) )
	{
		return AIST_Attack;
	}

	return AIST_Pursue;
}

//------------------------------------------------------------------------------------------
// 攻击状态转换
//------------------------------------------------------------------------------------------
AIStateType AggressiveTransition::AttackTransition(AIController* pAI)
{
	Creature* pOwner = pAI->GetOwner();
	
	// 如果技能还没有完成
	if( pOwner->GetCombatHandler().IsValid() )
	{
		return AIST_Attack;
	}

	// 如果没有目标了
	if( !P_VALID(pAI->GetTargetUnitID()) )
	{
		return AIST_Return;
	}

	// 如果当前目标不在攻击范围之内了
	if( !pAI->IsInAttackDist(pAI->GetTargetUnitID()) )
	{
		return AIST_Pursue;
	}

	return AIST_Attack;
}

//------------------------------------------------------------------------------------------
// 返回状态转换
//------------------------------------------------------------------------------------------
AIStateType AggressiveTransition::ReturnTransition(AIController* pAI)
{
	if( pAI->IsArrivedReturnPos() )
	{
		return AIST_Idle;
	}

	return AIST_Return;
}

AITransition* GuardTransition::Instance()
{
	static GuardTransition instance;
	return &instance;
}

//------------------------------------------------------------------------------------------
// 防御型怪物状态转换
//------------------------------------------------------------------------------------------
AIStateType GuardTransition::Transition(AIController* pAI, AIStateType eCurState)
{
	switch(eCurState)
	{
	case AIST_Idle:
		return IdleTransition(pAI);

	case AIST_Attack:
		return AttackTransition(pAI);

	case AIST_Return:
		return ReturnTransition(pAI);

	default:
		return eCurState;
	}
}

//------------------------------------------------------------------------------------------
// 空闲状态转换
//------------------------------------------------------------------------------------------
AIStateType GuardTransition::IdleTransition(AIController* pAI)
{
	// 索敌
	if( pAI->GetMaxEnmityUnitID() != GT_INVALID )
	{
		return AIST_Attack;
	}

	return AIST_Idle;

}

//------------------------------------------------------------------------------------------
// 攻击状态转换
//------------------------------------------------------------------------------------------
AIStateType GuardTransition::AttackTransition(AIController* pAI)
{
	Creature* pOwner = pAI->GetOwner();

	// 如果技能还没有完成
	if( pOwner->GetCombatHandler().IsValid() )
	{
		return AIST_Attack;
	}

	// 如果没有目标了
	if( !P_VALID(pAI->GetTargetUnitID()) )
	{
		return AIST_Return;
	}

	// 如果目标不在攻击范围内了，则清空它的仇恨
	if( !pAI->IsInAttackDist(pAI->GetTargetUnitID()) )
	{
		pAI->SetEnmityActive(pAI->GetTargetUnitID(), FALSE);
		return AIST_Attack;
	}

	return AIST_Attack;
}

//------------------------------------------------------------------------------------------
// 返回状态转换
//------------------------------------------------------------------------------------------
AIStateType GuardTransition::ReturnTransition(AIController* pAI)
{
	if( pAI->IsArrivedReturnPos() )
	{
		return AIST_Idle;
	}

	return AIST_Return;
}

AITransition* BarbetteTransition::Instance()
{
	static BarbetteTransition instance;
	return &instance;
}

//------------------------------------------------------------------------------------------
// 炮塔状态转换
//------------------------------------------------------------------------------------------
AIStateType BarbetteTransition::Transition(AIController* pAI, AIStateType eCurState)
{
	switch(eCurState)
	{
	case AIST_Idle:
		return IdleTransition(pAI);

	case AIST_Attack:
		return AttackTransition(pAI);

	default:
		return eCurState;
	}
}

//------------------------------------------------------------------------------------------
// 炮塔型空闲状态转换
//------------------------------------------------------------------------------------------
AIStateType BarbetteTransition::IdleTransition(AIController* pAI)
{
	// 索敌
	if( pAI->GetMaxEnmityUnitID() != GT_INVALID )
	{
		pAI->SetTargetUnitID(pAI->GetMaxEnmityUnitID());
		return AIST_Attack;
	}

	return AIST_Idle;
}

//------------------------------------------------------------------------------------------
// 炮塔型攻击状态转换
//------------------------------------------------------------------------------------------
AIStateType BarbetteTransition::AttackTransition(AIController* pAI)
{
	Creature* pOwner = pAI->GetOwner();

	// 如果技能还没有完成
	if( pOwner->GetCombatHandler().IsValid() )
	{
		return AIST_Attack;
	}

	// 如果没有目标了
	if( !P_VALID(pAI->GetTargetUnitID()) )
	{
		return AIST_Idle;
	}

	// 如果当前目标不在范围内，则清空其仇恨
	if( !pAI->IsInAttackDist(pAI->GetTargetUnitID()) )
	{
		pAI->SetEnmityActive(pAI->GetTargetUnitID(), FALSE);
		return AIST_Attack;
	}

	return AIST_Attack;
}


AITransition* LamsterTransition::Instance()
{
	static LamsterTransition instance;
	return &instance;
}

//------------------------------------------------------------------------------------------
// 逃跑型怪物状态转换
//------------------------------------------------------------------------------------------
AIStateType	LamsterTransition::Transition(AIController* pAI, AIStateType eCurState)
{
	switch(eCurState)
	{
	case AIST_Idle:
		return IdleTransition(pAI);

	case AIST_Pursue:
		return PursueTransition(pAI);

	case AIST_Attack:
		return AttackTransition(pAI);

	case AIST_Return:
		return ReturnTransition(pAI);

	case AIST_Flee:
		return FleeTransition(pAI);

	default:
		return eCurState;
	}
}

//------------------------------------------------------------------------------------------
// 空闲状态转换
//------------------------------------------------------------------------------------------
AIStateType LamsterTransition::IdleTransition(AIController* pAI)
{
	// 索敌
	if( pAI->GetMaxEnmityUnitID() != GT_INVALID )
	{
		return AIST_Pursue;
	}

	return AIST_Idle;
}

//------------------------------------------------------------------------------------------
// 追击状态转换
//------------------------------------------------------------------------------------------
AIStateType LamsterTransition::PursueTransition(AIController* pAI)
{
	// 如果现在target非法，则切换成返回状态
	if( !P_VALID(pAI->GetTargetUnitID()) )
	{
		return AIST_Return;
	}

	// 如果当前目标还存在，则检测技能
	if( pAI->IsInAttackDist(pAI->GetTargetUnitID()) )
	{
		return AIST_Attack;
	}

	return AIST_Pursue;
}

//------------------------------------------------------------------------------------------
// 攻击状态转换
//------------------------------------------------------------------------------------------
AIStateType LamsterTransition::AttackTransition(AIController* pAI)
{
	Creature* pOwner = pAI->GetOwner();

	// 如果技能还没有完成
	if( pOwner->GetCombatHandler().IsValid() )
	{
		return AIST_Attack;
	}

	// 如果没有目标了
	if( !P_VALID(pAI->GetTargetUnitID()) )
	{
		return AIST_Return;
	}

	// 如果当前目标不在攻击范围之内了
	if( !pAI->IsInAttackDist(pAI->GetTargetUnitID()) )
	{
		return AIST_Pursue;
	}

	// 当前体力小于基础体力20％
	if(EFT_NotEnter == pAI->GetFleeTime() && (pOwner->GetBaseAttValue(ERA_MaxHP) * 0.2f) > pOwner->GetAttValue(ERA_HP))
	{
		DWORD	dwFleeTick = IUTIL->Rand() % 50 + 100;
		pAI->SetFleeTick(dwFleeTick);
		return AIST_Flee;
	}

	return AIST_Attack;
}

//------------------------------------------------------------------------------------------
// 逃跑状态转换
//------------------------------------------------------------------------------------------
AIStateType LamsterTransition::FleeTransition(AIController* pAI)
{
	// 逃跑状态持续时间结束
	if( pAI->GetFleeTick() <= 0 )
	{
		return AIST_Pursue;
	}

	return AIST_Flee;
}

//------------------------------------------------------------------------------------------
// 返回状态转换
//------------------------------------------------------------------------------------------
AIStateType LamsterTransition::ReturnTransition(AIController* pAI)
{
	if( pAI->IsArrivedReturnPos() )
	{
		return AIST_Idle;
	}

	return AIST_Return;
}


AITransition* CallHelpTransition::Instance()
{
	static CallHelpTransition instance;
	return &instance;
}

//------------------------------------------------------------------------------------------
// 呼救型怪物状态转换
//------------------------------------------------------------------------------------------
AIStateType	CallHelpTransition::Transition(AIController* pAI, AIStateType eCurState)
{
	switch(eCurState)
	{
	case AIST_Idle:
		return IdleTransition(pAI);

	case AIST_Pursue:
		return PursueTransition(pAI);

	case AIST_Attack:
		return AttackTransition(pAI);

	case AIST_Return:
		return ReturnTransition(pAI);

	case AIST_SOS:
		return CallForHelpTransition(pAI);

	default:
		return eCurState;
	}
}

//------------------------------------------------------------------------------------------
// 空闲状态转换
//------------------------------------------------------------------------------------------
AIStateType CallHelpTransition::IdleTransition(AIController* pAI)
{
	// 索敌
	if( pAI->GetMaxEnmityUnitID() != GT_INVALID )
	{
		return AIST_Pursue;
	}

	return AIST_Idle;
}

//------------------------------------------------------------------------------------------
// 追击状态转换
//------------------------------------------------------------------------------------------
AIStateType CallHelpTransition::PursueTransition(AIController* pAI)
{
	// 如果现在target非法，则切换成返回状态
	if( !P_VALID(pAI->GetTargetUnitID()) )
	{
		return AIST_Return;
	}

	// 如果当前目标还存在，则检测技能
	if( pAI->IsInAttackDist(pAI->GetTargetUnitID()) )
	{
		return AIST_Attack;
	}

	return AIST_Pursue;
}

//------------------------------------------------------------------------------------------
// 攻击状态转换
//------------------------------------------------------------------------------------------
AIStateType CallHelpTransition::AttackTransition(AIController* pAI)
{
	Creature* pOwner = pAI->GetOwner();

	// 如果技能还没有完成
	if( pOwner->GetCombatHandler().IsValid() )
	{
		return AIST_Attack;
	}

	// 如果没有目标了
	if( !P_VALID(pAI->GetTargetUnitID()) )
	{
		return AIST_Return;
	}

	// 如果当前目标不在攻击范围之内了
	if( !pAI->IsInAttackDist(pAI->GetTargetUnitID()) )
	{
		return AIST_Pursue;
	}

	// 当前体力小于基础体力20％
	if(EFT_NotEnter == pAI->GetFleeTime() && (pOwner->GetBaseAttValue(ERA_MaxHP) * 0.2f) > pOwner->GetAttValue(ERA_HP))
	{
		return AIST_SOS;
	}

	return AIST_Attack;
}

//------------------------------------------------------------------------------------------
// 逃跑状态转换
//------------------------------------------------------------------------------------------
AIStateType CallHelpTransition::CallForHelpTransition(AIController* pAI)
{
	// 逃跑状态持续时间结束
	if( EFT_CallHelp == pAI->GetFleeTime() )
	{
		return AIST_Pursue;
	}

	return AIST_SOS;
}

//------------------------------------------------------------------------------------------
// 返回状态转换
//------------------------------------------------------------------------------------------
AIStateType CallHelpTransition::ReturnTransition(AIController* pAI)
{
	if( pAI->IsArrivedReturnPos() )
	{
		return AIST_Idle;
	}

	return AIST_Return;
}

//------------------------------------------------------------------------------------------
// 风筝型状态转换
//------------------------------------------------------------------------------------------
AITransition* SpaceOutTransition::Instance()
{
	static SpaceOutTransition instance;
	return &instance;
}

AIStateType	SpaceOutTransition::Transition(AIController* pAI, AIStateType eCurState)
{
	switch(eCurState)
	{
	case AIST_Idle:
		return IdleTransition(pAI);

	case AIST_Pursue:
		return PursueTransition(pAI);

	case AIST_Attack:
		return AttackTransition(pAI);

	case AIST_Return:
		return ReturnTransition(pAI);

	case AIST_SpaceOut:
		return SpaceTransition(pAI);

	default:
		return eCurState;
	}
}

//------------------------------------------------------------------------------------------
// 空闲状态转换
//------------------------------------------------------------------------------------------
AIStateType SpaceOutTransition::IdleTransition(AIController* pAI)
{
	// 索敌
	if( pAI->GetMaxEnmityUnitID() != GT_INVALID )
	{
		return AIST_Pursue;
	}

	return AIST_Idle;
}

//------------------------------------------------------------------------------------------
// 追击状态转换
//------------------------------------------------------------------------------------------
AIStateType SpaceOutTransition::PursueTransition(AIController* pAI)
{
	// 如果现在target非法，则切换成返回状态
	if( !P_VALID(pAI->GetTargetUnitID()) )
	{
		return AIST_Return;
	}

	// 如果当前目标还存在，则检测技能
	if( pAI->IsInAttackDist(pAI->GetTargetUnitID()) )
	{
		return AIST_Attack;
	}

	return AIST_Pursue;
}

//------------------------------------------------------------------------------------------
// 攻击状态转换
//------------------------------------------------------------------------------------------
AIStateType SpaceOutTransition::AttackTransition(AIController* pAI)
{
	Creature* pOwner = pAI->GetOwner();
	Unit* pTarget = pAI->GetPursueTarget();

	// 如果技能还没有完成
	if( pOwner->GetCombatHandler().IsValid() )
	{
		return AIST_Attack;
	}

	// 如果没有目标了
	if( !P_VALID(pAI->GetTargetUnitID()) )
	{
		return AIST_Return;
	}

	// 如果当前目标不在攻击范围之内了
	if( !pAI->IsInAttackDist(pAI->GetTargetUnitID()) )
	{
		return AIST_Pursue;
	}

	// 满足保持距离触发间隔
	DWORD dwCombatTick = g_world.GetWorldTick() - pAI->GetEnterCombatTick();
	if(0 == dwCombatTick % pAI->GetSpaceOutTick() )
	{
		if( pOwner->IsInDistance(*pTarget, CREATURE_SPACE_OUT_CHANGE_DIST) )
		{
			if( pAI->CalSpaceOutPos() )
				return AIST_SpaceOut;
		}
	}

	return AIST_Attack;
}

AIStateType SpaceOutTransition::SpaceTransition(AIController* pAI)
{
	if( pAI->IsArrivedPos())
		return AIST_Attack;

	return AIST_SpaceOut;
}

AIStateType SpaceOutTransition::ReturnTransition(AIController* pAI)
{
	if( pAI->IsArrivedReturnPos() )
	{
		return AIST_Idle;
	}

	return AIST_Return;
}