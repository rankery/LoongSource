#include "StdAfx.h"
#include "UpperStateTalkNPC_LP.h"
#include "RoleMgr.h"
#include "FSM_LP.h"
#include "LocalPlayer.h"
#include "CombatEvent.h"
#include "QuestEvent.h"
#include "CombatSysUtil.h"
#include "CombatSys.h"
#include "NPC.h"

const float TALK_DIST=200.0f;

UpperStateTalkNPC_LP::UpperStateTalkNPC_LP(void)
: m_lastUpdatetime(0.0f)
{}

UpperStateTalkNPC_LP::~UpperStateTalkNPC_LP(void)
{}

void UpperStateTalkNPC_LP::Active( tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole )
{
	ASSERT(pEvent->strEventName==_T("tagTalkNPCEvent"));
	tagTalkNPCEvent* pTalkEvent=(tagTalkNPCEvent*)pEvent;

	m_npcID=pTalkEvent->dwNPCID;
	m_npcTypeID=pTalkEvent->dwTypeID;

	//--检查目标是否存在
	Role* pNPC=RoleMgr::Inst()->FindRole(m_npcID);
	if(pNPC==NULL)
	{
		m_pFSM->Change2UpperState_Idle();
		return;
	}

	FaceToTarget();

	//--判断是否在对话范围内
	if(Vec3DistSq(pRole->GetPos(),pNPC->GetPos())<TALK_DIST*TALK_DIST)
	{
		//停止移动
		m_pFSM->Change2LowerState_Stand();
		
		SendTalkNPCEvent();
		m_pFSM->Change2UpperState_Idle();
		return;
	}

	//--追踪目标
	tagPursueEvent event;
	event.start=pRole->GetPos();
	event.targetType=tagPursueEvent::ETargetType_Role;
	event.targetRoleID=m_npcID;
	event.validDist=TALK_DIST;
	event.bNeedSendStopCmd=true;
	m_pFSM->Change2LowerState(FSM_LP::ELowerState_Pursue,&event);
}

void UpperStateTalkNPC_LP::Update( NavMap* pNav,LocalPlayer* pRole )
{
	float curTime = Kernel::Inst()->GetAccumTime();
	if( curTime-m_lastUpdatetime > 0.1f )//每隔0.1秒更新
	{
		FaceToTarget();
		m_lastUpdatetime = curTime;
	}
}

void UpperStateTalkNPC_LP::OnGameEvent( tagGameEvent* pEvent )
{
	if(pEvent->strEventName==_T("tagUseSkillEvent"))
	{
		SendStopPursueEvent();
		m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_Skill,pEvent);
	}
	else if(pEvent->strEventName==_T("tagUseItemEvent"))
	{
		SendStopPursueEvent();
		m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_UseItem,pEvent);
	}
	else if(pEvent->strEventName==_T("tagFollowRoleEvent"))
	{
		SendStopPursueEvent();
		m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_Follow,pEvent);
	}
	else if(pEvent->strEventName==_T("tagTalkNPCEvent"))
	{
		SendStopPursueEvent();
		m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_TalkNPC,pEvent);
	}
	else if(pEvent->strEventName==_T("tagPickupItemEvent"))
	{
		SendStopPursueEvent();
		m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_PickupItem,pEvent);
	}
	else if(pEvent->strEventName==_T("tagPlayPersonalityActEvent"))
	{
		SendStopPursueEvent();
		m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_StyleAction,pEvent);
	}
	else if(pEvent->strEventName==_T("tagPursueFinishEvent"))
	{
		tagPursueFinishEvent* pFinishEvent=(tagPursueFinishEvent*)pEvent;
		if(pFinishEvent->bSuccessed)
		{
			SendTalkNPCEvent();
			m_pFSM->Change2UpperState_Idle();
		}
		else
		{
			m_pFSM->Change2UpperState_Idle();
		}
	}
}

void UpperStateTalkNPC_LP::OnNetCmd( tagNetCmd* pNetCmd )
{}

void UpperStateTalkNPC_LP::SendTalkNPCEvent()
{
	tagStartNPCTalkEvent event(_T("tagStartNPCTalkEvent"),NULL);
	event.globalID=m_npcID;
	event.typeID=m_npcTypeID;
	TObjRef<GameFrameMgr>()->SendEvent(&event);
}

void UpperStateTalkNPC_LP::SendStopPursueEvent()
{
	tagStopPursueEvent event;
	m_pFSM->OnGameEvent(&event);
}

bool UpperStateTalkNPC_LP::IfCanChange2UpperState( FSM_LP::EUpperState state,tagGameEvent* pEvent )
{
	return true;
}

bool UpperStateTalkNPC_LP::IfCanChange2LowerState( FSM_LP::ELowerState state,tagGameEvent* pEvent )
{
	return true;
}

void UpperStateTalkNPC_LP::OnChangeLowerState( FSM_LP::ELowerState newState,tagGameEvent* pEvent )
{
	switch(newState)
	{
	case FSM_LP::ELowerState_Pursue:
	case FSM_LP::ELowerState_Jump:
	case FSM_LP::ELowerState_Slide:
	case FSM_LP::ELowerState_VDrop:
	case FSM_LP::ELowerState_Drop:
	case FSM_LP::ELowerState_Stand:
		break;
	default:
		{
			m_pFSM->Change2UpperState_Idle();
		}
		break;
	}
}

bool UpperStateTalkNPC_LP::IfCanPlayLowerAni()
{
	return true;
}

bool UpperStateTalkNPC_LP::IfLowerCanSetYaw()
{
	return true;
}

void UpperStateTalkNPC_LP::FaceToTarget()
{
	LocalPlayer* pRole = m_pFSM->GetLocalPlayer();

	CombatSys* pCombatSys = (CombatSys*)TObjRef<GameFrameMgr>()->GetFrame(_T("CombatSys"));
	if( P_VALID( pCombatSys ) && GT_INVALID != pCombatSys->GetCurTargetID() )
	{
		Role* pTarget = RoleMgr::Inst()->FindRole( pCombatSys->GetCurTargetID() );
		if( P_VALID(pTarget)
			&& pTarget->IsAttributeInited()
			&& !RoleMgr::Inst()->IsOutFaceDist( pTarget->GetID() ) )
		{
			if( pTarget->IS_KIND_OF( NPC ) )
			{
				NPC* pNpc = (NPC*)pTarget;
				if( pNpc->IsNPC() || pNpc->IsMonster() )
				{
					// 朝向目标
					Vector3 targetPos;
					pTarget->GetHeadPos(targetPos);
					pRole->FaceTo(targetPos,FALSE);

					return;
				}
			}
			else if( pTarget->IS_KIND_OF( Player ) )//玩家
			{
				// 朝向目标
				Vector3 targetPos;
				pTarget->GetHeadPos(targetPos);
				pRole->FaceTo(targetPos,FALSE);

				return;
			}
		}
	}

	pRole->FaceBack();
}

void UpperStateTalkNPC_LP::OnExit()
{
	m_pFSM->GetLocalPlayer()->FaceBack();
}