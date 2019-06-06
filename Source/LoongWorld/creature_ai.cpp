//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: creature_ai.cpp
// author: Aslan
// actor:
// data: 2008-09-09
// last:
// brief: 游戏内生物AI类，一个有限状态机，包括多种AI状态
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "../WorldDefine/creature_define.h"
#include "../ServerDefine/NPCTeam_define.h"

#include "unit.h"
#include "role.h"
#include "creature.h"
#include "creature_ai.h"
#include "ai_transition.h"
#include "ai_trigger.h"
#include "path_finder.h"
#include "NPCTeam.h"

//------------------------------------------------------------------------------
// 空闲AI状态
//------------------------------------------------------------------------------
AIStateIdle* AIStateIdle::Instance()
{
	static AIStateIdle instance;
	return &instance;
}

VOID AIStateIdle::OnEnter(AIController* pAI)
{
	pAI->ClearAllEnmity();
	pAI->SetEnterCombatTick(0);

	pAI->SetIsPatroling(FALSE);
	pAI->SetIsReversePatrol(FALSE);
	pAI->ReSetPatrolRestTick();
	pAI->ReSetLookForTargetTick();
}

VOID AIStateIdle::OnExit(AIController* pAI)
{
	AITriggerMgr* pAITrigger = pAI->GetAITriggerMgr();
	if( P_VALID(pAITrigger) )
		pAITrigger->SetTriggerActive(ETEE_DetectTarget);
}

VOID AIStateIdle::Update(AIController* pAI)
{
	// 更新巡逻
	pAI->UpdatePatrol();

	// 更新索敌
	pAI->UpdateLookForEnemy();
}

BOOL AIStateIdle::OnEvent(AIController* pAI, Unit* pSrc, INT nEventType, DWORD dwEventMisc1, DWORD dwEventMisc2)
{
	return TRUE;
}

//--------------------------------------------------------------------------------
// 追击状态
//--------------------------------------------------------------------------------
AIStatePursue* AIStatePursue::Instance()
{
	static AIStatePursue instance;
	return &instance;
}

VOID AIStatePursue::OnEnter(AIController* pAI)
{
	Creature* pOwner = pAI->GetOwner();
	if( !P_VALID(pOwner) ) return;

	pAI->ReSetLockTargetTime();
	pAI->SetTargetUnitID(pAI->GetMaxEnmityUnitID());

	Unit* pUnit = pAI->GetOwner()->GetMap()->FindUnit(pAI->GetTargetUnitID());
	if( P_VALID(pUnit) )
	{
		pAI->StartPursue(pUnit);
	}
	else
	{
		pAI->SetPursueTargetPos(pAI->GetOwner()->GetCurPos());
	}

	if( 0 == pAI->GetEnterCombatTick() )
	{
		pAI->SetEnterCombatTick(g_world.GetWorldTick());
		pAI->SetEnterCombatPos(pAI->GetOwner()->GetCurPos());

		const CreatureScript *pScript = pOwner->GetScript();
		if (P_VALID(pScript))
		{
			pScript->OnEnterCombat(pOwner);
		}
	}
}

VOID AIStatePursue::OnExit(AIController* pAI)
{

}

VOID AIStatePursue::Update(AIController* pAI)
{
	Creature* pOwner = pAI->GetOwner();
	if( !P_VALID(pOwner) ) return;

	// 更新锁敌倒计时
	pAI->CountDownNextAttackWaitTick();
	BOOL bTargetChange = pAI->UpdateLockTarget();

	// 如果没找到，直接返回
	if( !P_VALID(pAI->GetTargetUnitID()) ) return;

	// 找到真正的target
	Unit* pTarget = pAI->GetPursueTarget();
	if( !P_VALID(pTarget) )  return;

	// 如果目标改变了，则重新发起追踪
	if( bTargetChange )
	{
		pAI->StartPursue(pTarget);
	}
	else
	{
		pAI->UpdatePursue(pTarget);
	}
}

BOOL AIStatePursue::OnEvent(AIController* pAI, Unit* pSrc, INT nEventType, DWORD dwEventMisc1, DWORD dwEventMisc2)
{
	return TRUE;
}

//---------------------------------------------------------------------------------
// 攻击状态
//---------------------------------------------------------------------------------
AIStateAttack* AIStateAttack::Instance()
{
	static AIStateAttack instance;
	return &instance;
}

VOID AIStateAttack::OnEnter(AIController* pAI)
{
	Creature* pOwner = pAI->GetOwner();
	if( !P_VALID(pOwner) ) return;

	if( 0 == pAI->GetEnterCombatTick() )
	{
		pAI->SetEnterCombatTick(g_world.GetWorldTick());
		pAI->SetEnterCombatPos(pAI->GetOwner()->GetCurPos());

		const CreatureScript *pScript = pOwner->GetScript();
		if (P_VALID(pScript))
		{
			pScript->OnEnterCombat(pOwner);
		}
	}

	AITriggerMgr* pAITrigger = pAI->GetAITriggerMgr();
	if( P_VALID(pAITrigger) )
	{
		pAITrigger->SetTriggerActive(ETEE_InterCombatTime);
	}
}

VOID AIStateAttack::OnExit(AIController* pAI)
{

}

VOID AIStateAttack::Update(AIController* pAI)
{
	Creature* pOwner = pAI->GetOwner();
	if( !P_VALID(pOwner) ) return;

	BOOL bTargetChange = pAI->UpdateLockTarget();

	CombatHandler& handler = pOwner->GetCombatHandler();

	// 如果当前技能不在释放了
	if( !handler.IsValid() )
	{
		pAI->CountDownNextAttackWaitTick();

		if( P_VALID(pAI->GetTargetUnitID()) && pAI->GetNextAttackWaitTick() < 0 )
		{
			// 可以再次发动
			if( pAI->IsInAttackDist(pAI->GetTargetUnitID()) )
			{
				INT nRet = pAI->AIUseSkill(pAI->GetAIUseSkillID(), pAI->GetTargetUnitID());

				if( E_Success == nRet )
					pOwner->OnAIAttack();
			}
		}
	}
}

BOOL AIStateAttack::OnEvent(AIController* pAI, Unit* pSrc, INT nEventType, DWORD dwEventMisc1, DWORD dwEventMisc2)
{
	return TRUE;
}

//-----------------------------------------------------------------------------------
// 逃跑状态
//-----------------------------------------------------------------------------------
AIStateFlee* AIStateFlee::Instance()
{
	static AIStateFlee instance;
	return &instance;
}

VOID AIStateFlee::OnEnter(AIController* pAI)
{
	pAI->SetFleeTime(EFT_FirstEnter);
}

VOID AIStateFlee::OnExit(AIController* pAI)
{
	pAI->SetIsArrivedFleePos(FALSE);
}

VOID AIStateFlee::Update(AIController* pAI)
{
	// 递减进入逃跑状态的时间
	pAI->CountDownNextFleeTick();

	if(50 > pAI->GetFleeTick())
		return;

	if( EFT_FirstEnter == pAI->GetFleeTime() )
	{
		pAI->CalFleePos();
		pAI->StartFlee(pAI->GetFleePos());

		pAI->SetFleeTime(EFT_Enter);
	}

	// 已经到达逃跑的目标点
	if( pAI->IsArrivedFleePos() )
	{
		pAI->CalFleePos();
		pAI->StartFlee(pAI->GetFleePos());
	}
	else
	{
		pAI->UpdateFlee();
	}
}

BOOL AIStateFlee::OnEvent(AIController* pAI, Unit* pSrc, INT nEventType, DWORD dwEventMisc1, DWORD dwEventMisc2)
{
	return TRUE;
}

//-----------------------------------------------------------------------------------
// 呼救状态
//-----------------------------------------------------------------------------------
AIStateCallHelp* AIStateCallHelp::Instance()
{
	static AIStateCallHelp instance;
	return &instance;
}

VOID AIStateCallHelp::OnEnter(AIController* pAI)
{
	pAI->SetFleeTime(EFT_FirstEnter);
}

VOID AIStateCallHelp::OnExit(AIController* pAI)
{
	pAI->SetIsArrivedFleePos(FALSE);
}

VOID AIStateCallHelp::Update(AIController* pAI)
{
	Creature* pOwner = pAI->GetOwner();
	if( !P_VALID(pOwner) ) return;

	// 寻找求救目标
	if( EFT_FirstEnter == pAI->GetFleeTime() )
	{
		if( pAI->CalHelpPos() )
		{
			pAI->StartFlee(pAI->GetFleePos());
			pAI->SetFleeTime(EFT_Enter);
		}
		else
		{
			pAI->SetFleeTime(EFT_CallHelp);
		}
	}

	// 已经到达逃跑的目标点
	if( pAI->IsArrivedFleePos() )
	{
		if( EFT_CallHelp != pAI->GetFleeTime() )
		{
			pAI->SetFleeTime(EFT_CallHelp);
			if( P_VALID(pAI->GetHelperID()))
			{
				Creature* pCreature = pOwner->GetMap()->FindCreature(pAI->GetHelperID());
				if( P_VALID(pCreature) && !pCreature->IsDead() )
				{
					if( P_VALID(pCreature->GetAI()) )
					{
						Unit* pTarget = pOwner->GetMap()->FindRole(pAI->GetTargetUnitID());
						pCreature->GetAI()->AddEnmity(pTarget, 1);
					}
				}
			}
		}
	}
	else
	{
		if( pAI->NeedReCalHelpPos() )
		{
			if( pAI->CalHelpPos() )
				pAI->StartFlee(pAI->GetFleePos());
			else
				pAI->SetFleeTime(EFT_CallHelp);
		}
		pAI->UpdateFlee();
	}
}

BOOL AIStateCallHelp::OnEvent(AIController* pAI, Unit* pSrc, INT nEventType, DWORD dwEventMisc1, DWORD dwEventMisc2)
{
	return TRUE;
}


//------------------------------------------------------------------------------------
// 返回状态
//------------------------------------------------------------------------------------
AIStateReturn* AIStateReturn::Instance()
{
	static AIStateReturn instance;
	return &instance;
}

VOID AIStateReturn::OnEnter(AIController* pAI)
{
	Creature* pOwner = pAI->GetOwner();
	if( !P_VALID(pOwner) ) return;

	pAI->StartReturn();
	pOwner->SetState(ES_Invincible);
	pOwner->SetTagged(GT_INVALID);
	pAI->ClearAllEnmity();
	pOwner->RemoveAllBuff(FALSE);
	pAI->SetTargetUnitID(GT_INVALID);
	pAI->SetFleeTime(EFT_NotEnter);

	const CreatureScript *pScript = pOwner->GetScript();
	if (P_VALID(pScript))
	{
		pScript->OnLeaveCombat(pOwner);
	}

	AITriggerMgr* pAITrigger = pAI->GetAITriggerMgr();
	if( P_VALID(pAITrigger) )
	{
		pAITrigger->Refresh();
	}
}

VOID AIStateReturn::OnExit(AIController* pAI)
{
	Creature* pOwner = pAI->GetOwner();
	if( !P_VALID(pOwner) ) return;

	pOwner->SetAttValue(ERA_HP, pOwner->GetAttValue(ERA_MaxHP));
	pOwner->SetAttValue(ERA_MP, pOwner->GetAttValue(ERA_MaxMP));
	pOwner->UnsetState(ES_Invincible);
	pOwner->SetTagged(GT_INVALID);
}

VOID AIStateReturn::Update(AIController* pAI)
{
	pAI->UpdateReturn();
}

BOOL AIStateReturn::OnEvent(AIController* pAI, Unit* pSrc, INT nEventType, DWORD dwEventMisc1, DWORD dwEventMisc2)
{
	return TRUE;
}

//--------------------------------------------------------------------------------------
// 跟随状态
//--------------------------------------------------------------------------------------
AIStateFollow* AIStateFollow::Instance()
{
	static AIStateFollow instance;
	return &instance;
}

VOID AIStateFollow::OnEnter(AIController* pAI)
{

}

VOID AIStateFollow::OnExit(AIController* pAI)
{

}

VOID AIStateFollow::Update(AIController* pAI)
{
	
}


BOOL AIStateFollow::OnEvent(AIController* pAI, Unit* pSrc, INT nEventType, DWORD dwEventMisc1, DWORD dwEventMisc2)
{
	return TRUE;
}

//--------------------------------------------------------------------------------------
// 拉开距离
//--------------------------------------------------------------------------------------
AIStateSpaceOut* AIStateSpaceOut::Instance()
{
	static AIStateSpaceOut instance;
	return &instance;
}

VOID AIStateSpaceOut::OnEnter(AIController* pAI)
{
	pAI->StartSpaceOut();
}

VOID AIStateSpaceOut::OnExit(AIController* pAI)
{
	pAI->SetIfArrivedPos(FALSE);
}

VOID AIStateSpaceOut::Update(AIController* pAI)
{
	pAI->UpdateSpaceOut();
}


BOOL AIStateSpaceOut::OnEvent(AIController* pAI, Unit* pSrc, INT nEventType, DWORD dwEventMisc1, DWORD dwEventMisc2)
{
	return TRUE;
}

//---------------------------------------------------------------------------------------
// 脚本状态
//---------------------------------------------------------------------------------------
AIStateScript* AIStateScript::Instance()
{
	static AIStateScript instance;
	return &instance;
}

VOID AIStateScript::OnEnter(AIController* pAI)
{
	Creature* pOwner = pAI->GetOwner();
	if( !P_VALID(pOwner) ) return;

	AITriggerMgr* pAITrigger = pAI->GetAITriggerMgr();
	if( P_VALID(pAITrigger) )
	{
		pAITrigger->Pause();		// 暂停触发器
	}

	const CreatureScript* pScript = pOwner->GetScript();
	if( P_VALID(pScript) ) pScript->OnEnterCurAI(pOwner);
}

VOID AIStateScript::OnExit(AIController* pAI)
{
	Creature* pOwner = pAI->GetOwner();
	if( !P_VALID(pOwner) ) return;

	AITriggerMgr* pAITrigger = pAI->GetAITriggerMgr();
	if( P_VALID(pAITrigger) )
	{
		pAITrigger->UnPause();		// 恢复触发器
	}

	const CreatureScript* pScript = pOwner->GetScript();
	if( P_VALID(pScript) ) pScript->OnLeaveCurAI(pOwner);
}

VOID AIStateScript::Update(AIController* pAI)
{
	Creature* pOwner = pAI->GetOwner();
	if( !P_VALID(pOwner) ) return;

	const CreatureScript* pScript = pOwner->GetScript();
	if( P_VALID(pScript) ) pScript->OnUpdateCurAI(pOwner);
}


BOOL AIStateScript::OnEvent(AIController* pAI, Unit* pSrc, INT nEventType, DWORD dwEventMisc1, DWORD dwEventMisc2)
{
	Creature* pOwner = pAI->GetOwner();
	if( !P_VALID(pOwner) ) return FALSE;

	const CreatureScript* pScript = pOwner->GetScript();
	if( P_VALID(pScript) ) return pScript->OnEventCurAI(pOwner);
	
	return FALSE;
}

//---------------------------------------------------------------------------------------
// AI控制器
//---------------------------------------------------------------------------------------
AIController::AIController(Creature* pCreature, const tagMapWayPointInfoList* patrolList) : m_pOwner(pCreature), m_pCurrentState(NULL), 
												  m_pPreState(NULL), m_pGlobalState(NULL), m_pProto(NULL), m_pAIProto(NULL), m_pAITrigger(NULL), 
												  m_pScript(NULL), m_pTransition(NULL), m_nScriptUpdateTimer(GT_INVALID), m_nScriptUpdateCountDown(0), 
												  m_pWPArray(NULL), m_nWPNum(0), m_nWPIndex(0), m_bReversePatrol(FALSE), m_bIsPatroling(FALSE), 
												  m_nPatrolRestTick(0), m_nLookForTargetTick(0), m_bPursueFailed(FALSE), m_bCanNotPursue(FALSE), m_vPursueTargetPos(0.0f, 0.0f, 0.0f), 
												  m_dwTargeUnitID(GT_INVALID), m_nCheckAIEventTick(0), m_dwEnterCombatTick(0), m_dwUseSkillID(GT_INVALID),
												  m_nNextAttackWaitTick(0), m_dwSerial(0), m_vFleePos(0.0f, 0.0f, 0.0f), m_vPosEnterCombat(0.0f, 0.0f, 0.0f), m_eFleeTime(EFT_NotEnter),
												  m_dwMaxEnmityUnitID(GT_INVALID), m_nLockTargetTick(0), m_mapEnmity(), m_pPathFinder(NULL), m_dwFleeTick(0), m_bArrivedFleePos(FALSE),
												  m_dwSpaceOutTick(CREATURE_SPACE_OUT_MIN), m_bArrivedPos(FALSE)

{
	// 初始化静态属性
	m_pProto = m_pOwner->GetProto();

	// 初始化脚本
	m_pScript = pCreature->GetScript();

	// 初始化巡逻
	InitPatrol(patrolList);

	// 初始化AI属性
	InitAIProto();

	// 初始化触发器管理器
	InitTriggerMgr();

	// 初始化状态管理器
	InitAITransition();

	// 初始化寻路
	InitPathFinder();

	// 初始化为Idle状态
	m_eCurAIState = AIST_Idle;
	m_pCurrentState = AIStateIdle::Instance();

	// 初始化战斗属性
	ReSetPatrolRestTick();
	ReSetLookForTargetTick();
	ReSetLockTargetTime();
}

//-----------------------------------------------------------------------------------------
// 刷新
//-----------------------------------------------------------------------------------------
VOID AIController::Refresh()
{
	// 重置属性
	m_eCurAIState				=	AIST_Idle;
	m_pCurrentState				=	AIStateIdle::Instance();
	m_pPreState					=	NULL;
	m_pGlobalState				=	NULL;
	m_nScriptUpdateTimer		=	GT_INVALID;
	m_nScriptUpdateCountDown	=	0;
	m_nWPIndex					=	0;
	m_bReversePatrol			=	FALSE;
	m_bIsPatroling				=	FALSE;
	m_nPatrolRestTick			=	0;
	m_nLookForTargetTick		=	0;
	m_vPursueTargetPos			=	Vector3(0.0f, 0.0f, 0.0f);
	m_bPursueFailed				=	FALSE;
	m_bCanNotPursue				=	FALSE;
	m_dwTargeUnitID				=	GT_INVALID;
	m_nCheckAIEventTick			=	0;
	m_dwEnterCombatTick			=	0;
	m_dwUseSkillID				=	GT_INVALID;
	m_nNextAttackWaitTick		=	0;
	m_dwSerial					=	0;
	m_vFleePos					=	Vector3(0.0f, 0.0f, 0.0f);
	m_vPosEnterCombat			=	Vector3(0.0f, 0.0f, 0.0f);
	m_dwMaxEnmityUnitID			=	GT_INVALID;
	m_nLockTargetTick			=	0;
	m_eFleeTime					=	EFT_NotEnter;
	m_bArrivedFleePos			=	FALSE;


	// 重新初始化属性
	InitAIProto();

	// 初始化触发器管理器
	InitTriggerMgr();

	// 初始化状态管理器
	InitAITransition();

	// 重置战斗属性
	ReSetPatrolRestTick();
	ReSetLookForTargetTick();
	ReSetLockTargetTime();
}

//-----------------------------------------------------------------------------------------
// 初始化巡逻
//-----------------------------------------------------------------------------------------
VOID AIController::InitPatrol(const tagMapWayPointInfoList* patrolList)
{
	if( !P_VALID(m_pProto) ) return;

	// 导航点巡逻
	if( ECPT_Path == m_pProto->ePatrolType && P_VALID(patrolList) && patrolList->list.Size() >= 1 )
	{
		INT nWPNum = patrolList->list.Size();

		m_pWPArray = new Vector3[nWPNum];
		m_nWPNum = nWPNum;
		m_nWPIndex = 0;

		tagWayPointInfo info;
		INT nIndex = 0;
		TList<tagWayPointInfo>::TListIterator it = patrolList->list.Begin();

		while( patrolList->list.PeekNext(it, info) )
		{
			m_pWPArray[nIndex] = info.vPos;
			++nIndex;
		}

		m_bReversePatrol = FALSE;
	}
	else
	{
		m_pWPArray = NULL;
		m_nWPNum = 0;
		m_nWPIndex = 0;
	}
}

//-----------------------------------------------------------------------------------------
// 初始化AI静态属性
//-----------------------------------------------------------------------------------------
VOID AIController::InitAIProto()
{
	if( !P_VALID(m_pProto) ) return;

	// 得到静态AI属性
	switch(m_pProto->eAICreateType)
	{
	case EACT_Indicate:
		{
			// 指定方式，则直接根据怪物AI的ID得到AI静态属性
			m_pAIProto = g_attRes.GetCreatureAI(m_pProto->dwAIID);
		}
		break;

	case EACT_Random:
		{
			// 随机方式，则随机的给怪物增加一个AI
			m_pAIProto = g_attRes.RandGetCreatureAI();
		}
		break;

	case EACT_GroupRandom:
		{
			// 组随机方式，则在一个组种随机的选择一个AI
			m_pAIProto = g_attRes.RandGetCreatureAIInGroup(m_pProto->dwAIID);
		}
		break;
	}

	// 如果有AI静态属性，则根据这个属性给怪物增加buff和技能
	if( P_VALID(m_pAIProto) )
	{
		// 首先给怪物添加buff
		for(INT n = 0; n < X_MAX_CREATURE_AI_BUFF; n++)
		{
			if( GT_INVALID == m_pAIProto->dwBuffTypeID[n] )
				break;

			// 找到buff属性
			const tagBuffProto* pBuffProto = g_attRes.GetBuffProto(m_pAIProto->dwBuffTypeID[n]);
			if( !P_VALID(pBuffProto) ) continue;

			// 添加buff
			m_pOwner->TryAddBuff(m_pOwner, pBuffProto, NULL, NULL, NULL);
		}

		// 再给怪物添加技能
		for(INT n = 0; n < X_MAX_CREATURE_AI_SKILL; n++)
		{
			if( GT_INVALID == m_pAIProto->dwSkillTypeID[n] )
				break;

			// 找到技能属性和触发器属性
			const tagSkillProto* pSkillProto = g_attRes.GetSkillProto(m_pAIProto->dwSkillTypeID[n]);

			if( !P_VALID(pSkillProto) ) continue;

			// 生成技能
			DWORD dwSkillID = Skill::GetIDFromTypeID(pSkillProto->dwID);
			INT nLevel = Skill::GetLevelFromTypeID(pSkillProto->dwID);
			Skill* pSkill = new Skill(dwSkillID, nLevel, 0, 0, 0);
			m_pOwner->AddSkill(pSkill);
		}
	}
}

//------------------------------------------------------------------------------------------
// 初始化触发器管理器
//-------------------------------------------------------------------------------------------
VOID AIController::InitTriggerMgr()
{
	if( P_VALID(m_pAITrigger) ) SAFE_DEL(m_pAITrigger);

	if( P_VALID(m_pAIProto) )
	{
		m_pAITrigger = new AITriggerMgr;
		m_pAITrigger->Init(m_pOwner, m_pAIProto);
	}
}

//------------------------------------------------------------------------------------------
// 初始化
//------------------------------------------------------------------------------------------
VOID AIController::InitAITransition()
{
	m_pTransition = GetTransitionByType();
}

//------------------------------------------------------------------------------------------
// 初始化寻路
//------------------------------------------------------------------------------------------
VOID AIController::InitPathFinder()
{
	if( m_pOwner->NeedCollide() )
		m_pPathFinder = new NPCPathFinderCol();
	else
		m_pPathFinder = new NPCPathFinder();

	m_pPathFinder->Init(GetOwner());
}

//------------------------------------------------------------------------------------------
// 销毁
//------------------------------------------------------------------------------------------
VOID AIController::Destroy()
{
	SAFE_DEL(m_pAITrigger);
	SAFE_DEL(m_pWPArray);
	SAFE_DEL(m_pPathFinder);
}

//------------------------------------------------------------------------------------------
// 更新AI状态
//------------------------------------------------------------------------------------------
VOID AIController::Update()
{
	// 如果处于不能更新AI的状态，则返回
	if( IsInStateCantUpdateAI() ) return;

	// 更新状态机
	UpdateAIController();

	// 更新切换
	UpdateTransition();

	// 更新当前状态
	UpdateCurrentState();

	// 更新触发器
	UpdateTriggerMgr();
}

//------------------------------------------------------------------------------------------
// 处理事件
//------------------------------------------------------------------------------------------
VOID AIController::OnEvent(Unit* pSrc, INT nEventType, DWORD dwEventMisc1/* =0 */, DWORD dwEventMisc2/* =0 */)
{
	// 如果当前状态不存在，则用全局状态处理
	if( !P_VALID(m_pCurrentState) )
	{
		if( P_VALID(m_pGlobalState) )
		{
			m_pGlobalState->OnEvent(this, pSrc, nEventType, dwEventMisc1, dwEventMisc2);
		}
	}
	// 弱当前状态不处理该事件，则用全局状态处理
	else
	{
		if( FALSE == m_pCurrentState->OnEvent(this, pSrc, nEventType, dwEventMisc1, dwEventMisc2) )
		{
			if( P_VALID(m_pGlobalState) )
			{
				m_pGlobalState->OnEvent(this, pSrc, nEventType, dwEventMisc1, dwEventMisc2);
			}
		}
	}
}

//------------------------------------------------------------------------------------------
// 根据怪物移动类型得到状态转换器
//------------------------------------------------------------------------------------------
AITransition* AIController::GetTransitionByType()
{
	const tagCreatureProto* pProto = m_pOwner->GetProto();
	if( !P_VALID(pProto) ) return NULL;

	switch(pProto->eAIAction)
	{
	case AIAT_Attack:
		return AggressiveTransition::Instance();
		
	case AIAT_Guard:
		return GuardTransition::Instance();

	case AIAT_Barbette:
		return BarbetteTransition::Instance();

	case AIAT_Lamster:
		return LamsterTransition::Instance();

	case AIAT_CallHelp:
		return CallHelpTransition::Instance();

	case AIAT_SpaceOut:
		return SpaceOutTransition::Instance();

	default:
		return NULL;
	}
}

//--------------------------------------------------------------------------------------------
// 更新巡逻
//--------------------------------------------------------------------------------------------
VOID AIController::UpdatePatrol()
{
	// 如果怪物本身就是不动的，直接return
	if( ECPT_Stand == m_pProto->ePatrolType || 
		ECPT_Null == m_pProto->ePatrolType )
		return;

	// 正在移动
	if( m_bIsPatroling )
	{
		if( EMS_Stand == m_pOwner->GetMoveData().m_eCurMove )
		{
			// 已经到达终点，则原地休息一段时间
			m_bIsPatroling = FALSE;
			ReSetPatrolRestTick();
		}
		return;
	}

	// 正在休息
	if( m_nPatrolRestTick > 0 )
	{
		m_nPatrolRestTick--;
		return;
	}

	// 开始新一次的巡逻
	StartPatrol();
}

//---------------------------------------------------------------------------------------------
// 开始巡逻
//---------------------------------------------------------------------------------------------
VOID AIController::StartPatrol()
{
	switch(m_pProto->ePatrolType)
	{
		// 站立和木桩
	case ECPT_Null:
	case ECPT_Stand:
	case ECPT_Wood:
		return;
		break;

		// 范围内随机移动 
	case ECPT_Range:
		{
			Vector3 vDest = m_pOwner->GetCurPos();

			INT nRange = m_pProto->nPatrolRadius * 2;
			if( 0 >= nRange ) return;

			vDest.x = FLOAT(IUTIL->Rand() % nRange - m_pProto->nPatrolRadius) + m_pOwner->GetBornPos().x;
			vDest.z = FLOAT(IUTIL->Rand() % nRange - m_pProto->nPatrolRadius) + m_pOwner->GetBornPos().z;

			if( MoveData::EMR_Success == m_pOwner->GetMoveData().StartCreatureWalk(vDest, EMS_CreaturePatrol) )
			{
				m_bIsPatroling = TRUE;
			}
		}
		break;

		// 往返路径
	case ECPT_Path:
		{
			if( !P_VALID(m_pWPArray) || m_nWPNum <= 1 ) return;

			// 先检测一下当前导航点索引
			if( !m_bReversePatrol && (m_nWPIndex == m_nWPNum - 1) )
			{
				// 正向巡逻已经到了最终点，则折返
				m_bReversePatrol = !m_bReversePatrol;
			}
			else if( m_bReversePatrol && 0 == m_nWPIndex )
			{
				// 反向巡逻已经到了最终点，则折返
				m_bReversePatrol = !m_bReversePatrol;
			}

			// 取出导航点类表中下一个点
			if( m_bReversePatrol )
				--m_nWPIndex;
			else
				++m_nWPIndex;

			// 取出下一个点
			Vector3 vDest = m_pWPArray[m_nWPIndex];

			if( m_pOwner->IsTeam() )
			{
				NPCTeam* pTeam = m_pOwner->GetTeamPtr();
				if( P_VALID(pTeam) )
					pTeam->OrderTeammateMove(m_pOwner, vDest);

				return;
			}

			if( MoveData::EMR_Success == m_pOwner->GetMoveData().StartCreatureWalk(vDest, EMS_CreaturePatrol) )
			{
				m_bIsPatroling = TRUE;
			}
		}
		break;

	default:
		break;
	}
}

//------------------------------------------------------------------------------------
// 锁敌行为更新 0-休息 1-没有找到敌人 2-找到敌人
//------------------------------------------------------------------------------------
DWORD AIController::UpdateLookForEnemy()
{
	// 如果正在休息
	if( m_nLookForTargetTick > 0 )
	{
		m_nLookForTargetTick--;
		return 0;
	}

	// 如果休息时间到了，则开始锁敌
	ReSetLookForTargetTick();

	return StartLookForEnemy();
}

//------------------------------------------------------------------------------------
// 开始锁敌 0-休息 1-没有找到敌人 2-找到敌人
//------------------------------------------------------------------------------------
DWORD AIController::StartLookForEnemy()
{
	tagVisTile* pVisTile[ED_End] = {0};
	
	// 怪物的灵感数值
	FLOAT fMaxDist = (FLOAT)m_pOwner->GetAttValue(ERA_Inspiration);
	
	// 无灵感值,被动怪物直接返回 
	if( fMaxDist <= 0.0f  ) return 0;

	FLOAT fMaxDistSQ = fMaxDist * fMaxDist;

	// 得到视野范围内的vistile列表
	m_pOwner->GetMap()->GetVisTile(m_pOwner->GetMoveData().m_vPos, fMaxDist, pVisTile);

	Role* pBestRole = NULL;
	FLOAT fNearestDistSq = FLOAT_MAX;

	// 这个地方可以再优化一下，如果本vistile能找到，则一定是最近的
	for(INT n = ED_Center; n < ED_End; n++)
	{
		if( !P_VALID(pVisTile[n]) ) continue;

		TMap<DWORD, Role*>& mapRole = pVisTile[n]->mapRole;

		TMap<DWORD, Role*>::TMapIterator it = mapRole.Begin();
		Role* pRole = NULL;

		while( mapRole.PeekNext(it, pRole) )
		{
			// 死亡人物不检测
			if( pRole->IsDead() ) continue;

			// 是否无敌
			if( pRole->IsInState(ES_Invincible)) continue;

			// 不是敌人不检测
			if( m_pOwner->FriendEnemy(pRole) != ETFE_Hostile ) continue;

			// 检测距离
			FLOAT fDistSQ = Vec3DistSq(m_pOwner->GetCurPos(), pRole->GetCurPos());
			

			// 计算怪物对隐身目标的锁敌范围 锁敌范围 <= (怪物灵感 - 玩家隐匿值)
			FLOAT fLurkDist = fMaxDist - pRole->GetAttValue(ERA_Lurk);
			if( fLurkDist <= 100.0f  )
				fLurkDist = 100.0f;
			FLOAT fLurkDistSQ = fLurkDist * fLurkDist;

			// 如果距离小于之前最近的距离
			if( fDistSQ > fNearestDistSq || fDistSQ > fLurkDistSQ )
				continue;

			// 查看是否可达
			if( MoveData::EMR_Success == m_pOwner->GetMoveData().IsCanCreatureWalk(pRole->GetCurPos(), EMS_CreatureWalk, TRUE) )
			{
				pBestRole = pRole;
				fNearestDistSq = fDistSQ;
			}
		}
	}

	// 找到
	if( P_VALID(pBestRole) )
	{
		AddEnmity(pBestRole, 1);
		return 2;
	}

	// 没有找到
	ReSetLookForTargetTick();
	return 1;

}

//-----------------------------------------------------------------------------
// 查找追击的目标
//-----------------------------------------------------------------------------
Unit* AIController::GetPursueTarget()
{
	if( P_VALID(m_dwTargeUnitID) )
	{
		if( IS_PLAYER(m_dwTargeUnitID) )
		{
			return m_pOwner->GetMap()->FindRole(m_dwTargeUnitID);
		}
		else if( IS_CREATURE(m_dwTargeUnitID) )
		{
			return m_pOwner->GetMap()->FindCreature(m_dwTargeUnitID);
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

//------------------------------------------------------------------------------
// 找到与目标临近的点，如果找不到，返回FALSE
//------------------------------------------------------------------------------
BOOL AIController::GetPosNearPursueTarget(Unit* pTarget, Vector3& vPos)
{
	if( !P_VALID(pTarget) )
		return FALSE;

	// 计算目标位置
	const Vector3& vTargetPos = pTarget->GetCurPos();

	// 计算二维向量
	FLOAT fXDist = vTargetPos.x - m_pOwner->GetCurPos().x;
	FLOAT fZDist = vTargetPos.z - m_pOwner->GetCurPos().z;

	FLOAT fXDistABS = abs(fXDist);
	FLOAT fZDistABS = abs(fZDist);

	if( fXDistABS < 0.0001f && fZDistABS < 0.0001f )
	{
		vPos = vTargetPos;
		return TRUE;
	}

	// 近似取得x方向和z方向的距离分量
	FLOAT fX = (fXDistABS / (fXDistABS + fZDistABS)) * (CREATURE_PURSUE_NEAR_DIST + pTarget->GetSize().z + m_pOwner->GetSize().z);
	FLOAT fZ = (fZDistABS / (fXDistABS + fZDistABS)) * (CREATURE_PURSUE_NEAR_DIST + pTarget->GetSize().z + m_pOwner->GetSize().z);

	if(fX > fXDistABS && fZ > fZDistABS)
	{
		vPos = vTargetPos;
		return TRUE;
	}

	// 取得目标点
	vPos.x = ( (fXDist > 0) ? vTargetPos.x - fX : vTargetPos.x + fX );
	vPos.z = ( (fZDist > 0) ? vTargetPos.z - fZ : vTargetPos.z + fZ );

	// 取得y坐标
	if( !m_pOwner->NeedCollide() )
	{
		vPos.y = m_pOwner->GetMap()->GetGroundHeight(vPos.x, vPos.z);
	}
	else
	{
		vPos.y = vTargetPos.y;
	}

	return TRUE;	
}

//------------------------------------------------------------------------------
// 检查目标是不是在攻击范围之内
//------------------------------------------------------------------------------
BOOL AIController::IsInAttackDist(DWORD dwTargetUnitID)
{
	m_dwUseSkillID = GT_INVALID;

	// 如果这个目标不合法，则返回错误
	if( !IsTargetValid(dwTargetUnitID) ) return FALSE;

	// 得到目标
	Unit* pTarget = m_pOwner->GetMap()->FindUnit(dwTargetUnitID);
	if( !P_VALID(pTarget) ) return FALSE;

	// 检测普通攻击和远程攻击
	Skill* pMeleeSkill = m_pOwner->GetMeleeSkill();
	Skill* pRangedSkill = m_pOwner->GetRangedSkill();

	// 先检测近程攻击
	if( P_VALID(pMeleeSkill) )
	{
		if( m_pOwner->IsInCombatDistance(*pTarget, pMeleeSkill->GetOPDist()) )
		{
			m_dwUseSkillID = pMeleeSkill->GetID();
			return TRUE;
		}
	}

	// 再检测远程攻击
	if( P_VALID(pRangedSkill) )
	{
		if( m_pOwner->IsInCombatDistance(*pTarget, pRangedSkill->GetOPDist()) )
		{
			m_dwUseSkillID = pRangedSkill->GetID();
			return TRUE;
		}
	}

	return FALSE;
}

//------------------------------------------------------------------------------
// 追击目标
//------------------------------------------------------------------------------
VOID AIController::StartPursue(Unit* pTarget)
{
	if( !P_VALID(pTarget) ) return;

	// 停止寻路和其他一些状态
	m_bPursueFailed		=	FALSE;
	m_bCanNotPursue		=	FALSE;
	m_bPathFinding		=	FALSE;
	m_nCheckPursueTick	=	IUTIL->Rand() % 6 + 5;

	// 先检测直线
	Vector3 vNearPos;
	MoveData::EMoveRet eRet = m_pOwner->GetMoveData().IsCanCreatureWalk(pTarget->GetCurPos(), EMS_CreatureWalk, TRUE, &vNearPos);

	// 可达
	if( MoveData::EMR_Success == eRet )
	{
		m_pOwner->GetMoveData().StartCreatureWalk(pTarget->GetCurPos(), EMS_CreatureWalk, FALSE);
	}
	// 处在不能移动的状态
	else if( MoveData::EMR_SelfLimit == eRet )
	{
		m_bCanNotPursue = TRUE;
	}
	// 不可达
	else if( MoveData::EMR_NoArrive == eRet )
	{
		// 如果最近的距离是可以攻击的距离，则直接过去
		Skill* pMeleeSkill	=	m_pOwner->GetMeleeSkill();
		Skill* pRangedSkill	=	m_pOwner->GetRangedSkill();

		FLOAT fMaxDist = 0.0f;

		if( P_VALID(pMeleeSkill) )
		{
			if( pMeleeSkill->GetOPDist() > fMaxDist )
				fMaxDist = pMeleeSkill->GetOPDist();
		}
		if( P_VALID(pRangedSkill) )
		{
			if( pRangedSkill->GetOPDist() > fMaxDist )
				fMaxDist = pRangedSkill->GetOPDist();
		}

		// 得到路径点的最近点和目标的距离
		FLOAT fDist = Vec3Dist(vNearPos, pTarget->GetCurPos());
		Vector3 vNearPosTop = vNearPos;
		vNearPosTop.y += m_pOwner->GetSize().y;

		if( fDist < fMaxDist && !m_pOwner->GetMap()->IsRayCollide(vNearPosTop, pTarget->GetCurPosTop()) )
		{
			m_pOwner->GetMoveData().StartCreatureWalk(vNearPos, EMS_CreatureWalk, FALSE);
		}
		else
		{
			// 寻路
			Vector3 vPos;
			BOOL bRet = m_pPathFinder->FindingPath(m_pOwner->GetCurPos(), pTarget->GetCurPos());
			if( bRet && m_pPathFinder->GetCurPathPoint(m_pOwner->GetCurPos(), vPos) )
			{
				m_bPathFinding = TRUE;
				m_pOwner->GetMoveData().StartCreatureWalk(vPos, EMS_CreatureWalk, FALSE);
			}
			else
			{
				m_bPursueFailed = TRUE;
			}
		}
	}
	// 其他原因
	else
	{
		m_bPursueFailed = TRUE;
	}

	if( !m_bPursueFailed )
	{
		SetPursueTargetPos(pTarget->GetCurPos());
	}
}

//---------------------------------------------------------------------------------
// 更新追踪
//---------------------------------------------------------------------------------
VOID AIController::UpdatePursue(Unit* pTarget)
{
	// 重新追击倒计时
	--m_nCheckPursueTick;
	++m_nNoAnmityTick;

	// 如果追击失败，则取仇恨列表里面的其他目标
	if( m_bPursueFailed )
	{
		SetEnmityActive(pTarget->GetID(), FALSE);
		SetTargetUnitID(GetMaxEnmityUnitID());

		if( P_VALID(GetTargetUnitID()) )
		{
			pTarget = m_pOwner->GetMap()->FindUnit(GetTargetUnitID());
			if( P_VALID(pTarget) )
			{
				StartPursue(pTarget);
			}
		}
		return;
	}

	// 检查追击状态
	if( m_bCanNotPursue )
	{
		if( !m_pOwner->IsInStateCantMove() )
		{
			StartPursue(pTarget);
		}
		return;
	}
	else
	{
		if( m_pOwner->IsInStateCantMove() )
		{
			m_bCanNotPursue = TRUE;
			return;
		}
	}

	// 如果长时间没有增加仇恨，则追击失败
	const tagCreatureProto* pProto = pProto = m_pOwner->GetProto();
	if( !P_VALID(pProto) || m_nNoAnmityTick > pProto->nMaxPursueTick )
	{
		m_bPursueFailed = TRUE;
		return;
	}

	// 检查是否已经停下来了
	if( EMS_Stand == m_pOwner->GetMoveData().GetCurMoveState() )
	{
		if( m_pOwner->GetMoveData().IsStopped() )
		{
			// 是移动中断
			StartPursue(pTarget);
		}
		else
		{
			// 不是强制停下，说明走完了
			if( m_bPathFinding )
			{
				// 寻路中
				Vector3 vNewPos;
				if( m_pPathFinder->GetCurPathPoint(m_pOwner->GetCurPos(), vNewPos) )
				{
					m_pOwner->GetMoveData().StartCreatureWalk(vNewPos, EMS_CreatureWalk, FALSE);
					return;
				}
				else
				{
					StartPursue(pTarget);
					return;
				}
			}
			else
			{
				// 非寻路中
				StartPursue(pTarget);
				return;
			}
		}
	}

	// 检查重新检测倒计时是不是已经到了
	if( m_nCheckPursueTick < 0 )
	{
		m_nCheckPursueTick = IUTIL->Rand() % 6 + 5;
		if( abs(pTarget->GetCurPos().x - GetPursueTargetPos().x) > 50.0f || 
			abs(pTarget->GetCurPos().z - GetPursueTargetPos().z) > 50.0f )
		{
			StartPursue(pTarget);
		}
	}
}

//---------------------------------------------------------------------------------
// 使用AI技能
//---------------------------------------------------------------------------------
INT AIController::AIUseSkill(DWORD dwSkillID, DWORD dwTargetUnitID)
{
	INT nRet = m_pOwner->GetCombatHandler().UseSkill(dwSkillID, dwTargetUnitID, m_dwSerial);

	if( E_Success == nRet )
	{
		Skill* pSkill = m_pOwner->GetSkill(dwSkillID);
		ASSERT( P_VALID(pSkill) );

		m_pOwner->GetMoveData().StopMove();
		m_dwUseSkillID = dwSkillID;

		tagNS_Skill send;
		send.dwSrcRoleID = m_pOwner->GetID();
		send.dwTarRoleID = dwTargetUnitID;
		send.dwSkillID = pSkill->GetTypeID();
		send.nSpellTime = m_pOwner->GetCombatHandler().GetSkillPrepareCountDown();
		send.dwSerial = m_dwSerial;
		send.dwErrorCode = E_Success;

		if( P_VALID(m_pOwner->GetMap()) )
			m_pOwner->GetMap()->SendBigVisTileMsg(m_pOwner, &send, send.dwSize);

		// 累加序列号
		++m_dwSerial;
	}
	else
	{
	}

	return nRet;
}

//----------------------------------------------------------------------------------
// 增加仇恨
//----------------------------------------------------------------------------------
VOID AIController::AddEnmity(Unit *pUnit, INT nValue, BOOL bSyncTeam)
{
	if( !P_VALID(pUnit) || pUnit->IsDead())
		return;

	tagEnmity* pEnmity = m_mapEnmity.Peek(pUnit->GetID());
	if( P_VALID(pEnmity) )
	{
		pEnmity->nEnmity += nValue;
		if( !pEnmity->bActive ) pEnmity->bActive = TRUE;
	}
	else
	{
		pEnmity = new tagEnmity;

		pUnit->AddEnmityCreature(m_pOwner);
		pEnmity->dwID = pUnit->GetID();
		pEnmity->nEnmity += nValue;
		m_mapEnmity.Add(pUnit->GetID(), pEnmity);
	}

	// 计算最大仇恨
	CalMaxEnmity();

	// 重置仇恨无改变时间
	m_nNoAnmityTick = 0;

	// 如果是减少仇恨，重置锁敌倒计时
	if( nValue < 0 )
		m_nLockTargetTick = 0;

	if( bSyncTeam )
	{
		// 同步小队仇恨
		NPCTeam* pTeam = m_pOwner->GetTeamPtr();
		if(P_VALID(pTeam))
			pTeam->OnEvent(NTE_SynEnmity, pUnit, 1);
	}
}

//----------------------------------------------------------------------------------
// 增加仇恨值加成
//----------------------------------------------------------------------------------
VOID AIController::AddEnmityMod(Unit *pUnit, INT nValue, BOOL bSyncTeam)
{
	if( !P_VALID(pUnit) || pUnit->IsDead())
		return;

	tagEnmity* pEnmity = m_mapEnmity.Peek(pUnit->GetID());
	if( P_VALID(pEnmity) )
	{
		pEnmity->nEnmityMod += nValue;
	}
	else
	{
		pEnmity = new tagEnmity;

		pUnit->AddEnmityCreature(m_pOwner);
		pEnmity->dwID = pUnit->GetID();
		pEnmity->nEnmity += 1;
		pEnmity->nEnmityMod += nValue;
		m_mapEnmity.Add(pUnit->GetID(), pEnmity);
	}

	// 计算最大仇恨
	CalMaxEnmity();

	// 重置仇恨无改变时间
	m_nNoAnmityTick = 0;

	if( bSyncTeam )
	{
		// 同步小队仇恨
		NPCTeam* pTeam = m_pOwner->GetTeamPtr();
		if(P_VALID(pTeam))
			pTeam->OnEvent(NTE_SynEnmity, pUnit, 1);
	}
}

//----------------------------------------------------------------------------------
// 清空怪物仇恨
//----------------------------------------------------------------------------------
VOID AIController::ClearAllEnmity()
{
	tagEnmity*	pEnmity		=	(tagEnmity*)GT_INVALID;
	DWORD		dwID		=	GT_INVALID;

	m_mapEnmity.ResetIterator();
	while( m_mapEnmity.PeekNext(dwID, pEnmity) )
	{
		if( P_VALID(pEnmity) )
		{
			Unit *pUnit = m_pOwner->GetMap()->FindUnit(pEnmity->dwID);
			if( P_VALID(pUnit) )
				pUnit->DelEnmityCreature(m_pOwner);
		}
		SAFE_DEL(pEnmity);
	}

	m_mapEnmity.Clear();

	m_dwMaxEnmityUnitID = GT_INVALID;
}

//----------------------------------------------------------------------------------
// 删除一个玩家的仇恨
//----------------------------------------------------------------------------------
VOID AIController::ClearEnmity(DWORD dwRoleID)
{
	tagEnmity *pEnmity = m_mapEnmity.Peek(dwRoleID);
	if( P_VALID(pEnmity) )
	{
		m_mapEnmity.Erase(dwRoleID);
		SAFE_DEL(pEnmity);
	}

	CalMaxEnmity();
}

//----------------------------------------------------------------------------------
// 设置一个玩家的仇恨激活
//----------------------------------------------------------------------------------
VOID AIController::SetEnmityActive(DWORD dwRoleID, BOOL bActive)
{
	tagEnmity *pEnmity = m_mapEnmity.Peek(dwRoleID);
	if( P_VALID(pEnmity) )
	{
		pEnmity->bActive = bActive;
	}

	CalMaxEnmity();
}

//----------------------------------------------------------------------------------
// 计算仇恨最大的目标
//----------------------------------------------------------------------------------
VOID AIController::CalMaxEnmity()
{
	INT nMaxEnmity = 0;

	m_dwMaxEnmityUnitID		=	GT_INVALID;
	tagEnmity* pEnmity		=	(tagEnmity*)GT_INVALID;

	TMap<DWORD, tagEnmity*>::TMapIterator it = m_mapEnmity.Begin();
	while( m_mapEnmity.PeekNext(it, pEnmity) )
	{
		if( !P_VALID(pEnmity) || !pEnmity->bActive )
			continue;

		if( !IsTargetValid(pEnmity->dwID) )
		{
			pEnmity->bActive = FALSE;
			continue;
		}

		if( pEnmity->nEnmity + pEnmity->nEnmityMod > nMaxEnmity )
		{
			nMaxEnmity = pEnmity->nEnmity + pEnmity->nEnmityMod;
			m_dwMaxEnmityUnitID = pEnmity->dwID;
		}
	}
}

//----------------------------------------------------------------------------------
// 最大的目标是否合法
//----------------------------------------------------------------------------------
BOOL AIController::IsTargetValid(DWORD dwID)
{
	Unit *pUnit = m_pOwner->GetMap()->FindUnit(dwID);
	if( !P_VALID(pUnit) )
		return FALSE;

	// 是否死亡
	if( pUnit->IsDead() )
		return FALSE;

	// 是否隐身
	if( pUnit->IsInState(ES_Lurk) ) 
		return FALSE;

	// 是否无敌
	if( pUnit->IsInState(ES_Invincible) && !IsEnmityListAllInvincible() )
		return FALSE;

	Map* pMap = pUnit->GetMap();
	if( pMap != m_pOwner->GetMap() )
		return FALSE;

	return TRUE;
}

//----------------------------------------------------------------------------------
// 更新锁敌倒计时
//----------------------------------------------------------------------------------
BOOL AIController::UpdateLockTarget()
{
	// 是否切换了目标
	BOOL bTargetChange = FALSE;

	// 找到目标
	while( P_VALID(GetTargetUnitID()) )
	{
		if( !IsTargetValid(GetTargetUnitID()) )
		{
			SetEnmityActive(GetTargetUnitID(), FALSE);
			SetTargetUnitID(GetMaxEnmityUnitID());
			ReSetLockTargetTime();
			bTargetChange = TRUE;
			continue;
		}
		else
		{
			break;
		}
	}

	// 如果没有切换目标
	if( !bTargetChange )
	{
		// 已经锁定目标
		if( m_nLockTargetTick > 0 )
		{
			m_nLockTargetTick--;
		}
		else
		{
			// 如果锁定倒计时结束，则重新开始计算
			ReSetLockTargetTime();

			if( GetTargetUnitID() != GetMaxEnmityUnitID() )
			{
				// 设置最大仇恨的目标位当前目标
				SetTargetUnitID(GetMaxEnmityUnitID());
				bTargetChange = TRUE;
			}
		}
	}

	return bTargetChange;
}

//----------------------------------------------------------------------------------
// 根据目标类型获得目标ID
//----------------------------------------------------------------------------------
DWORD AIController::GetTargetIDByType(ECreatureTargetFriendEnemy eTargetType)
{
	DWORD dwTargetID = GT_INVALID;
	switch(eTargetType)
	{
	case ECFE_NULL:
		return dwTargetID;

	case ECFE_Self:
		return m_pOwner->GetID();

	case ECFE_Leader:
		{
			NPCTeam* pTeam = m_pOwner->GetTeamPtr();
			if( P_VALID(pTeam) )
			{
				dwTargetID = pTeam->GetLeaderID();
			}
			return dwTargetID;
		}

	case ECFE_RandPlayer:
		{
			dwTargetID = RandRoleInEnmityList();
			return dwTargetID;
		}
	case ECFE_MinEnimty:
		{
			dwTargetID = GetMinEnmityInEnmityList();
			return dwTargetID;
		}
	case ECFE_CurPlayer:
		{
			return m_dwTargeUnitID;
		}
	default:
		return dwTargetID;
	}
}


//----------------------------------------------------------------------------------
// 怪物仇恨列表中随机抽取一个目标
//----------------------------------------------------------------------------------
DWORD AIController::RandRoleInEnmityList()
{
	tagEnmity*	pEnmity	=	NULL;
	DWORD		dwID	=	GT_INVALID;

	if( m_mapEnmity.Empty() ) return GT_INVALID;

	m_mapEnmity.RandPeek(dwID, pEnmity);
	return dwID;
}

//----------------------------------------------------------------------------------
// 怪物仇恨列表中仇恨最小目标
//----------------------------------------------------------------------------------
DWORD AIController::GetMinEnmityInEnmityList()
{
	tagEnmity*	pEnmity		=	NULL;
	DWORD		dwID		=	GT_INVALID;
	DWORD		dwTargetID	=	GT_INVALID;
	INT			nValue		=	INT_MAX;

	TMap<DWORD, tagEnmity*>::TMapIterator it = m_mapEnmity.Begin();

	while( m_mapEnmity.PeekNext(it, dwID, pEnmity) )
	{
		if( pEnmity->nEnmity + pEnmity->nEnmityMod < nValue )
		{
			nValue = pEnmity->nEnmity + pEnmity->nEnmityMod;
			dwTargetID = dwID;
		}
	}

	return dwTargetID;
}

//----------------------------------------------------------------------------------
// 得到最大仇恨值
//----------------------------------------------------------------------------------
INT AIController::GetEnmityValue(DWORD dwID)
{
	tagEnmity* pEnmity = m_mapEnmity.Peek(dwID);
	if( P_VALID(pEnmity) )	return pEnmity->nEnmity + pEnmity->nEnmityMod;

	return 0;
}

//----------------------------------------------------------------------------------
// 得到基础仇恨值
//----------------------------------------------------------------------------------
INT AIController::GetBaseEnmityValue(DWORD dwID)
{
	tagEnmity* pEnmity = m_mapEnmity.Peek(dwID);
	if( P_VALID(pEnmity) ) return pEnmity->nEnmity;

	return 0;
}

//----------------------------------------------------------------------------------
// 设置仇恨加值
//----------------------------------------------------------------------------------
VOID AIController::ClearEnmityModValue(DWORD dwID)
{
	tagEnmity* pEnmity = m_mapEnmity.Peek(dwID);
	if( P_VALID(pEnmity) ) 
	{
		pEnmity->nEnmityMod = 0;
		CalMaxEnmity();
	}
}

//----------------------------------------------------------------------------------
// 计算得到一个怪物逃跑的目标点
//----------------------------------------------------------------------------------
VOID AIController::CalFleePos()
{
	// 第一次选择逃跑目标点
	if(EFT_FirstEnter == m_eFleeTime)
	{
		INT	  nIndex = 0;
		while(TRUE)
		{
			nIndex += 1;

			// 随机得到一个逃跑的距离
			m_vFleeDir.fx = (FLOAT)(IUTIL->Rand() % X_RANGE_FLEE_RADIUS) + X_MIN_FLEE_RADIUS;
			m_vFleeDir.fy = 0.0f;

			// 随机得到逃跑的方向向量
			FLOAT fAng = (FLOAT)(IUTIL->Rand() % 360);

			// 计算逃跑目标点
			m_vFleeDir.Vec2DRotateAroundOrigin(fAng*3.1415927f / 180.0f);

			m_vFleePos.x = m_vFleeDir.fx + m_pOwner->GetCurPos().x;
			m_vFleePos.z = m_vFleeDir.fy + m_pOwner->GetCurPos().z;

			if(m_pOwner->NeedCollide())
			{
				m_vFleePos.y = m_pOwner->GetCurPosTop().y;
				break;
			}
			else
			{
				if( m_pOwner->GetMap()->IfCanGo(m_vFleePos.x, m_vFleePos.z) )
				{
					m_vFleePos.y = m_pOwner->GetMap()->GetGroundHeight(m_vFleePos.x, m_vFleePos.z);
					break;
				}
			}

			if (nIndex > 10)	break;
		}
	}
	else
	{
		INT	  nIndex = 0;
		while(TRUE)
		{
			nIndex += 1;

			// 随机得到逃跑的方向向量
			FLOAT fAng = (FLOAT)(IUTIL->Rand() % 160) - 80.0f;

			// 计算逃跑目标点
			m_vFleeDir.Vec2DRotateAroundOrigin(fAng*3.1415927f / 180.0f);

			m_vFleePos.x = m_vFleeDir.fx + m_pOwner->GetCurPos().x;
			m_vFleePos.z = m_vFleeDir.fy + m_pOwner->GetCurPos().z;

			if(m_pOwner->NeedCollide())
			{
				m_vFleePos.y = m_pOwner->GetCurPosTop().y;
				break;
			}
			else
			{
				if( m_pOwner->GetMap()->IfCanGo(m_vFleePos.x, m_vFleePos.z) )
				{
					m_vFleePos.y = m_pOwner->GetMap()->GetGroundHeight(m_vFleePos.x, m_vFleePos.z);
					break;
				}
			}

			if (nIndex > 10)	break;
		}
	}
}

//------------------------------------------------------------------------------
// 逃跑
//------------------------------------------------------------------------------
VOID AIController::StartFlee(Vector3 vFleePos)
{
	m_bArrivedFleePos	=	FALSE;
	m_bPursueFailed		=	FALSE;
	m_bCanNotPursue		=	FALSE;
	m_bPathFinding		=	FALSE;

	// 先检测直线
	Vector3 vNearPos;
	MoveData::EMoveRet eRet = m_pOwner->GetMoveData().IsCanCreatureWalk(vFleePos, EMS_CreatureFlee, TRUE, &vNearPos);

	// 可达
	if( MoveData::EMR_Success == eRet )
	{
		m_pOwner->GetMoveData().StartCreatureWalk(vFleePos, EMS_CreatureFlee, FALSE);
	}
	// 处在不能移动的状态
	else if( MoveData::EMR_SelfLimit == eRet )
	{
		m_bCanNotPursue = TRUE;
	}
	// 不可达
	else if( MoveData::EMR_NoArrive == eRet )
	{
		// 寻路
		Vector3 vPos;
		BOOL bRet = m_pPathFinder->FindingPath(m_pOwner->GetCurPos(), vFleePos);
		if( bRet && m_pPathFinder->GetCurPathPoint(m_pOwner->GetCurPos(), vPos) )
		{
			m_bPathFinding = TRUE;
			m_pOwner->GetMoveData().StartCreatureWalk(vPos, EMS_CreatureFlee, FALSE);
		}
		else
		{
			m_bPursueFailed = TRUE;
		}
	}
	// 其他原因
	else
	{
		m_bPursueFailed = TRUE;
	}

	if( !m_bPursueFailed )
	{
		SetFleePos(vFleePos);
	}
}

//---------------------------------------------------------------------------------
// 更新逃跑
//---------------------------------------------------------------------------------
VOID AIController::UpdateFlee()
{
	// 没有找到逃跑的目标点,则重新计算
	if( m_bPursueFailed )
	{
		CalFleePos();
		StartFlee(m_vFleePos);
		return;
	}

	// 检查逃跑状态
	if( m_bCanNotPursue )
	{
		if( !m_pOwner->IsInStateCantMove() )
		{
			StartFlee(m_vFleePos);
		}
		return;
	}
	else
	{
		if( m_pOwner->IsInStateCantMove() )
		{
			m_bCanNotPursue = TRUE;
			return;
		}
	}

	// 检查是否已经停下来了
	if( EMS_Stand == m_pOwner->GetMoveData().GetCurMoveState() )
	{
		if( m_pOwner->GetMoveData().IsStopped() )
		{
			// 是移动中断
			StartFlee(m_vFleePos);
		}
		else
		{
			// 不是强制停下，说明走完了
			if( m_bPathFinding )
			{
				// 寻路中
				Vector3 vNewPos;
				if( m_pPathFinder->GetCurPathPoint(m_pOwner->GetCurPos(), vNewPos) )
				{
					m_pOwner->GetMoveData().StartCreatureWalk(vNewPos, EMS_CreatureFlee, FALSE);
					return;
				}
				else
				{
					m_bArrivedFleePos = TRUE;
					return;
				}
			}
			else
			{
				// 非寻路中
				m_bArrivedFleePos = TRUE;
				return;
			}
		}
	}
}

//----------------------------------------------------------------------------------
// 计算得到一个怪物求救的目标点
//----------------------------------------------------------------------------------
BOOL AIController::CalHelpPos()
{
	// 第一次选择逃跑目标点
	if(EFT_FirstEnter == m_eFleeTime)
	{
		tagVisTile* pVisTile[ED_End] = {0};

		// 得到视野范围内的vistile列表
		m_pOwner->GetMap()->GetVisTile(m_pOwner->GetMoveData().m_vPos, X_MAX_SOS_RADIUS, pVisTile);

		Creature* pCreature = NULL;
		FLOAT fNearestDistSq = FLOAT_MAX;

		for(INT n = ED_Center; n < ED_End; n++)
		{
			if( !P_VALID(pVisTile[n]) ) continue;

			TMap<DWORD, Creature*>& mapCreature = pVisTile[n]->mapCreature;

			TMap<DWORD, Creature*>::TMapIterator it = mapCreature.Begin();
			Creature* pCreature = NULL;

			while( mapCreature.PeekNext(it, pCreature) )
			{
				// 死亡怪物不检测
				if( pCreature->IsDead() ) continue;

				// 自己不检测
				if( pCreature->GetID() == m_pOwner->GetID() )	continue;

				const tagCreatureProto* pProto = pCreature->GetProto();
				if( !P_VALID(pProto) || AIAT_CallHelp != pProto->eAIAction ) continue;

				// 敌方不检测
				if( m_pOwner->FriendEnemy(pCreature) != ETFE_Friendly ) continue;

				// 求救目标的AI为空闲状态
				if( !P_VALID(pCreature->GetAI()) ) continue;
				if( AIST_Idle != pCreature->GetAI()->GetCurrentStateType() ) continue;

				// 检测距离
				FLOAT fDistSQ = Vec3DistSq(m_pOwner->GetCurPos(), pCreature->GetCurPos());

				// 如果距离小于之前最近的距离
				if( fDistSQ > fNearestDistSq || fDistSQ > X_MAX_SOS_RADIUS_SQ )
					continue;

				// 查看是否可达
				if( MoveData::EMR_Success == m_pOwner->GetMoveData().IsCanCreatureWalk(pCreature->GetCurPos(), EMS_CreatureWalk, TRUE) )
				{
					GetPosNearPursueTarget(pCreature, m_vFleePos);
					fNearestDistSq = fDistSQ;
					m_dwHelperID = pCreature->GetID();
				}
			}
		}

		if( fNearestDistSq != FLOAT_MAX)
			return TRUE;
	}
	else
	{
		Creature* pCreature = m_pOwner->GetMap()->FindCreature(m_dwHelperID);
		if( !P_VALID(pCreature) )
			return FALSE;

		// 查看是否可达
		if( MoveData::EMR_Success == m_pOwner->GetMoveData().IsCanCreatureWalk(pCreature->GetCurPos(), EMS_CreatureWalk, TRUE) )
		{
			GetPosNearPursueTarget(pCreature, m_vFleePos);
			return TRUE;
		}
	}

	return FALSE;
}

//----------------------------------------------------------------------------------
// 是否需要重新计算呼救目标点
//----------------------------------------------------------------------------------
BOOL AIController::NeedReCalHelpPos()
{
	Creature* pCreature = m_pOwner->GetMap()->FindCreature(m_dwHelperID);
	if( P_VALID(pCreature) )
	{
		FLOAT fDistanceX = m_vFleePos.x - pCreature->GetCurPos().x;
		FLOAT fDistanceZ = m_vFleePos.z - pCreature->GetCurPos().z;
		FLOAT fDistanceSquare = fDistanceX * fDistanceX + fDistanceZ * fDistanceZ;
		FLOAT fMaxHelpDist = CREATURE_PURSUE_NEAR_DIST + pCreature->GetSize().z + m_pOwner->GetSize().z;

		if(fDistanceSquare > fMaxHelpDist*fMaxHelpDist)
			return TRUE;
	}

	return FALSE;
}

//------------------------------------------------------------------------------
// 返回
//------------------------------------------------------------------------------
VOID AIController::StartReturn()
{
	m_bPathFinding		=	FALSE;
	m_bArrivedReturnPos	=	FALSE;

	// 先检测直线
	MoveData::EMoveRet eRet = MoveData::EMR_Success;

	if( m_pOwner->NeedCollide() )
		eRet = m_pOwner->GetMoveData().IsCanCreatureWalk(GetEnterCombatPos(), EMS_CreatureWalk, TRUE);
	else
		eRet = m_pOwner->GetMoveData().IsCanCreatureWalk(GetEnterCombatPos(), EMS_CreatureWalk, FALSE);

	// 可达
	if( MoveData::EMR_Success == eRet )
	{
		m_pOwner->GetMoveData().StartCreatureWalk(GetEnterCombatPos(), EMS_CreatureWalk, FALSE);
	}
	// 不可达
	else if( MoveData::EMR_NoArrive == eRet || MoveData::EMR_SelfLimit == eRet)
	{
		// 寻路
		Vector3 vPos;
		BOOL bRet = m_pPathFinder->FindingPath(m_pOwner->GetCurPos(), GetEnterCombatPos());
		if( bRet && m_pPathFinder->GetCurPathPoint(m_pOwner->GetCurPos(), vPos) )
		{
			m_bPathFinding = TRUE;
			m_pOwner->GetMoveData().StartCreatureWalk(vPos, EMS_CreatureWalk, FALSE);
		}
		else
		{
			m_bArrivedReturnPos = TRUE;
			m_pOwner->GetMoveData().ForceTeleport(m_pOwner->GetBornPos(),TRUE);
		}
	}
	// 其他原因
	else
	{
		m_bArrivedReturnPos = TRUE;
		m_pOwner->GetMoveData().ForceTeleport(m_pOwner->GetBornPos(),TRUE);
	}
}

VOID AIController::UpdateReturn()
{
	// 检查是否已经停下来了
	if( EMS_Stand == m_pOwner->GetMoveData().GetCurMoveState() )
	{
			// 不是强制停下，说明走完了
		if( m_bPathFinding )
		{
			// 寻路中
			Vector3 vNewPos;
			if( m_pPathFinder->GetCurPathPoint(m_pOwner->GetCurPos(), vNewPos) )
			{
				m_pOwner->GetMoveData().StartCreatureWalk(vNewPos, EMS_CreatureWalk, FALSE);
				return;
			}
			else
			{
				m_bArrivedReturnPos = TRUE;
				return;
			}
		}
		else
		{
			// 非寻路中
			m_bArrivedReturnPos = TRUE;
			return;

		}
	}
}

//------------------------------------------------------------------------------
// 计算拉开距离时的目标点
//------------------------------------------------------------------------------
BOOL AIController::CalSpaceOutPos()
{
	// 找到当前目标
	Unit* pTarget = GetPursueTarget();
	if( !P_VALID(pTarget) )  return FALSE;

	// 随机得到方向向量
	FLOAT fAng = (FLOAT)(IUTIL->Rand() % 360);

	// 计算目标点
	CVector2D vDir(CREATURE_SPACE_OUT_DIST);
	vDir.Vec2DRotateAroundOrigin(fAng*3.1415927f / 180.0f);

	m_vFleePos.x = vDir.fx + pTarget->GetCurPos().x;
	m_vFleePos.z = vDir.fy + pTarget->GetCurPos().z;

	if(m_pOwner->NeedCollide())
	{
		m_vFleePos.y = m_pOwner->GetCurPosTop().y;
		return TRUE;
	}
	else
	{
		if( m_pOwner->GetMap()->IfCanGo(m_vFleePos.x, m_vFleePos.z) )
		{
			m_vFleePos.y = m_pOwner->GetMap()->GetGroundHeight(m_vFleePos.x, m_vFleePos.z);
			return TRUE;
		}
	}

	return FALSE;
}

//------------------------------------------------------------------------------
// 与玩家拉开距离
//------------------------------------------------------------------------------
VOID AIController::StartSpaceOut()
{
	m_bArrivedReturnPos	=	FALSE;

	// 下一次时的触发间隔
	m_dwSpaceOutTick	=	IUTIL->RandomInRange(CREATURE_SPACE_OUT_MIN, CREATURE_SPACE_OUT_MAX);

	// 检测直线
	MoveData::EMoveRet eRet = m_pOwner->GetMoveData().IsCanCreatureWalk(m_vFleePos, EMS_CreatureWalk, TRUE);

	// 可达
	if( MoveData::EMR_Success == eRet )
	{
		m_pOwner->GetMoveData().StartCreatureWalk(m_vFleePos, EMS_CreatureWalk, FALSE);
	}
	// 不可达
	else if( MoveData::EMR_NoArrive == eRet || MoveData::EMR_SelfLimit == eRet)
	{
		m_bArrivedPos = TRUE;
	}
	// 其他原因
	else
	{
		m_bArrivedPos = TRUE;
	}

}

VOID AIController::UpdateSpaceOut()
{
	// 检查是否已经停下来了
	if( EMS_Stand == m_pOwner->GetMoveData().GetCurMoveState() )
	{
		if( m_pOwner->GetMoveData().IsStopped() )
		{
			// 是移动中断
			StartSpaceOut();
		}
		else
		{
			m_bArrivedPos = TRUE;
		}
	}
}

//----------------------------------------------------------------------------------
// 检测仇恨列表中的玩家是否都处于无敌状态
//----------------------------------------------------------------------------------
BOOL AIController::IsEnmityListAllInvincible()
{
	tagEnmity*	pEnmity		=	(tagEnmity*)GT_INVALID;
	Unit*		pTarget		=	NULL;	

	m_mapEnmity.ResetIterator();
	while( m_mapEnmity.PeekNext(pEnmity) )
	{
		if( !P_VALID(pEnmity) )
			continue;

		pTarget = m_pOwner->GetMap()->FindUnit(pEnmity->dwID);
		if( !P_VALID(pTarget) )
			continue;

		// 是否死亡
		if( pTarget->IsDead() )
			continue;

		// 是否隐身
		if( pTarget->IsInState(ES_Lurk) ) 
			continue;

		if( !pTarget->IsInState(ES_Invincible) )
			return FALSE;
	}

	return TRUE;
}

