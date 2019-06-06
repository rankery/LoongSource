//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: ai_trigger.cpp
// author: wjiang
// actor:
// data: 2009-6-25
// last:
// brief: 怪物AI触发器
//-----------------------------------------------------------------------------

#include "StdAfx.h"
#include "ai_trigger.h"
#include "unit.h"
#include "creature.h"
#include "creature_ai.h"
#include "map.h"
#include "NPCTeam.h"


#include "../WorldDefine/trigger_define.h"

AITrigger::AITrigger():m_bActive(FALSE),m_nActiveTick(1),m_dwTriggeredTimes(0),
									m_dwSkillID(GT_INVALID),m_pProto(NULL)
{

}

//------------------------------------------------------------------------------
// Init
//------------------------------------------------------------------------------
BOOL AITrigger::Init(DWORD dwCreatureTypeID, DWORD dwTypeID, INT nTargetType, DWORD dwSkillID)
{
	m_pProto = g_attRes.GetTriggerProto(dwTypeID);
	if(!P_VALID(m_pProto))
	{
		IMSG(_T("can not fine Trigger proto <TypeID: %u>\r\n"), dwTypeID);
		return FALSE;
	}

	m_dwSkillID = dwSkillID;
	m_eTargeType = (ECreatureTargetFriendEnemy)nTargetType;

	return TRUE;
}

//------------------------------------------------------------------------------
// 触发器刷新
//------------------------------------------------------------------------------
VOID AITrigger::Refresh()
{
	m_bActive			=		FALSE;
	m_nActiveTick		=		1;
	m_dwTriggeredTimes	=		0;
}

//------------------------------------------------------------------------------
// 检测触发器触发时间
//------------------------------------------------------------------------------
BOOL AITrigger::TestTimeIntervalTrigger()
{
	// 是否满足间隔触发
	if(0 != m_pProto->dwEventMisc1)
	{
		if(0 != (m_nActiveTick % m_pProto->dwEventMisc1))
			return FALSE;
	}

	// 是否超过触发次数限制
	if(0 != m_pProto->dwEventMisc2)
	{
		if(m_dwTriggeredTimes >= m_pProto->dwEventMisc2)
			return FALSE;
	}

	return TRUE;
}

//------------------------------------------------------------------------------
// 触发器的状态条件检测
//------------------------------------------------------------------------------
BOOL AITrigger::TestStateTrigger(Creature* pOwner, DWORD& dwTargetID)
{
	Unit* pTarget = pOwner->GetMap()->FindUnit(dwTargetID);

	if(P_VALID(pTarget) && pTarget->IsDead())
		return FALSE;

	// 检测同队成员体力低于一定值
	if(ETST_TeammateHPLower == m_pProto->eStateType)
	{
		NPCTeam* pTeam = pOwner->GetTeamPtr();
		if(!P_VALID(pTeam))		return FALSE;

		pTarget = pTeam->IsTeamateHPLower(m_pProto->dwStateMisc1);
		if(!P_VALID(pTarget))	return FALSE;
		
		dwTargetID = pTarget->GetID();
		return TRUE;
	}

	// 检测同队成员真气低于一定值
	if(ETST_TeammateMPLower == m_pProto->eStateType)
	{
		NPCTeam* pTeam = pOwner->GetTeamPtr();
		if(!P_VALID(pTeam))		return FALSE;

		pTarget = pTeam->IsTeamateMPLower(m_pProto->dwStateMisc1);
		if(!P_VALID(pTarget))	return FALSE;

		dwTargetID = pTarget->GetID();
		return TRUE;
	}

	// 检测目标距离是否在某一范围之内
	if(ETST_Distance == m_pProto->eStateType)
	{
		// 小于最大距离
		if(P_VALID(pTarget) && pOwner->IsInDistance(*pTarget, m_pProto->dwStateMisc2*TILE_SCALE) )
		{
			// 大于最小距离
			if(!pOwner->IsInDistance(*pTarget, m_pProto->dwStateMisc1*TILE_SCALE) )
				return TRUE;
		}

		return FALSE;
	}

	if(!pOwner->TestStateTrigger(pTarget, m_pProto, NULL))	
		return FALSE;

	return TRUE;
}

//------------------------------------------------------------------------------
// 触发器的时间条件检测
//------------------------------------------------------------------------------
BOOL AITrigger::TestEventTrigger(Creature* pOwner)
{
	if(!pOwner->TestEventTrigger(NULL, m_pProto, NULL, GT_INVALID, GT_INVALID))	
		return FALSE;

	return TRUE;
}

//------------------------------------------------------------------------------
// 触发器Update
//------------------------------------------------------------------------------
VOID AITrigger::Update(Creature* pOwner)
{
	if(!m_bActive)	return;

	// 累计激活时间
	++m_nActiveTick;

	// 检测触发器触发时间
	if( !TestTimeIntervalTrigger() ) return;

	// 得到目标ID
	DWORD dwTargetID = pOwner->GetAI()->GetTargetIDByType(m_eTargeType);

	// 检测触发
	if( TestEventTrigger(pOwner) && TestStateTrigger(pOwner, dwTargetID) )
	{
		// 触发事件
		OnTrigger(pOwner, dwTargetID);
	}
}

//------------------------------------------------------------------------------
// 激活触发器
//------------------------------------------------------------------------------
VOID AITrigger::SetTriggerActive(INT eEventType)
{
	if( eEventType != m_pProto->eEventType ) return;

	// 激活触发器
	SetActive(TRUE);
}

//------------------------------------------------------------------------------
// 触发器触发
//------------------------------------------------------------------------------
VOID AITrigger::OnEvent(INT eEventType, Creature* pOwner)
{
	if( eEventType != m_pProto->eEventType )	return;

	// 测试触发器的事件条件是否满足
	if( !TestEventTrigger(pOwner) )	return;

	// 激活触发器
	SetActive(TRUE);

	// 得到目标ID
	DWORD dwTargetID = pOwner->GetAI()->GetTargetIDByType(m_eTargeType);

	// 检测触发器的状态条件是否满足
	if( TestStateTrigger(pOwner, dwTargetID) )
	{
		// 触发事件
		OnTrigger(pOwner, dwTargetID);
	}
}

//------------------------------------------------------------------------------
// 触发器事件触发
//------------------------------------------------------------------------------
VOID AITrigger::OnTrigger(Creature* pOwner, DWORD dwTargetID)
{
	if( E_Success == pOwner->GetAI()->AIUseSkill(m_dwSkillID, dwTargetID) )
	{
		// 累计触发次数
		++m_dwTriggeredTimes;
	}
	else
	{
		// 下一tick继续触发
		--m_nActiveTick;
	}
}


//------------------------------------------------------------------------------
// 触发器管理类析构
//------------------------------------------------------------------------------
AITriggerMgr::~AITriggerMgr()
{
	std::list<AITrigger*>::iterator it = m_listTrigger.begin();
	while( it != m_listTrigger.end() )
	{
		delete *it;
		++it;
	}

	m_listTrigger.clear();
}

//------------------------------------------------------------------------------
// 触发器管理类Init
//------------------------------------------------------------------------------
BOOL AITriggerMgr::Init(Creature* pOwner, const tagCreatureAI* m_pAIProto)
{
	m_pOwner = pOwner;
	m_bPaused = FALSE;

	for(INT i = 0; i < X_MAX_CREATURE_AI_SKILL; ++i)
	{
		if( GT_INVALID == m_pAIProto->dwTriggerID[i] )
			break;

		AITrigger* pAITrigger = new AITrigger;
	
		DWORD dwSkillID = Skill::GetIDFromTypeID(m_pAIProto->dwSkillTypeID[i]);
		if( !pAITrigger->Init(pOwner->GetTypeID(), m_pAIProto->dwTriggerID[i], m_pAIProto->nTargetType[i], dwSkillID) )
		{
			SAFE_DEL(pAITrigger);
		}
		else
		{
			m_listTrigger.push_back(pAITrigger);
		}
	}

	return TRUE;
}

//------------------------------------------------------------------------------
// 激活触发器
//------------------------------------------------------------------------------
VOID AITriggerMgr::SetTriggerActive(INT eEventType)
{
	std::list<AITrigger*>::iterator it = m_listTrigger.begin();
	while (it != m_listTrigger.end())
	{
		(*it)->SetTriggerActive(eEventType);
		++it;
	}
}

//------------------------------------------------------------------------------
// 触发器管理类Update
//------------------------------------------------------------------------------
VOID AITriggerMgr::Update()
{
	std::list<AITrigger*>::iterator it = m_listTrigger.begin();
	while (it != m_listTrigger.end())
	{
		(*it)->Update(m_pOwner);
		++it;
	}
}

//------------------------------------------------------------------------------
// 触发器管理类刷新
//------------------------------------------------------------------------------
VOID AITriggerMgr::Refresh()
{
	std::list<AITrigger*>::iterator it = m_listTrigger.begin();
	while (it != m_listTrigger.end())
	{
		(*it)->Refresh();
		++it;
	}
}


//------------------------------------------------------------------------------------------
// 触发器触发
//------------------------------------------------------------------------------------------
VOID AITriggerMgr::OnEvent(INT eEventType)
{
	if( IsPaused() ) return;

	std::list<AITrigger*>::iterator it = m_listTrigger.begin();
	while (it != m_listTrigger.end())
	{
		(*it)->OnEvent(eEventType, m_pOwner);
		++it;
	}
}
