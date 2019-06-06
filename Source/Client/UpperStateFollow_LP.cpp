#include "StdAfx.h"
#include "UpperStateFollow_LP.h"
#include "LocalPlayer.h"
#include "FSM_LP.h"
#include "RoleMgr.h"
#include "CombatEvent.h"

const float FOLLOW_VALID_DIST = 100.0f;

UpperStateFollow_LP::UpperStateFollow_LP(void)
{}

UpperStateFollow_LP::~UpperStateFollow_LP(void)
{}

void UpperStateFollow_LP::Active( tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole )
{
	ASSERT(pEvent->strEventName==_T("tagFollowRoleEvent"));
	tagFollowRoleEvent* pFollowEvent=(tagFollowRoleEvent*)pEvent;

	m_targetRoleID=pFollowEvent->dwTargetRoleID;

	Role* pTarget=RoleMgr::Inst()->FindRole(m_targetRoleID);
	if(pTarget==NULL)
	{
		m_pFSM->Change2UpperState_Idle();
		return;
	}

	SendMouseMoveEvent(pRole,pTarget);
	m_lastMoveTime=Kernel::Inst()->GetAccumTime();
}

void UpperStateFollow_LP::Update(NavMap* pNav,LocalPlayer* pRole)
{
	if(Kernel::Inst()->GetAccumTime()-m_lastMoveTime>1.0f)
	{
		m_lastMoveTime=Kernel::Inst()->GetAccumTime();

		Role* pTarget=RoleMgr::Inst()->FindRole(m_targetRoleID);
		if(pTarget==NULL)
		{
			m_pFSM->Change2UpperState_Idle();
			return;
		}

		SendMouseMoveEvent(pRole,pTarget);
	}
}

void UpperStateFollow_LP::OnGameEvent( tagGameEvent* pEvent )
{
	if(pEvent->strEventName==_T("tagUseSkillEvent"))
	{
		m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_Skill,pEvent);
	}
	else if(pEvent->strEventName==_T("tagUseItemEvent"))
	{
		m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_UseItem,pEvent);
	}
	else if(pEvent->strEventName==_T("tagFollowRoleEvent"))
	{
		m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_Follow,pEvent);
	}
	else if(pEvent->strEventName==_T("tagTalkNPCEvent"))
	{
		m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_TalkNPC,pEvent);
	}
	else if(pEvent->strEventName==_T("tagPickupItemEvent"))
	{
		m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_PickupItem,pEvent);
	}
	else if(pEvent->strEventName==_T("tagPlayPersonalityActEvent"))
	{
		m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_StyleAction,pEvent);
	}
}

void UpperStateFollow_LP::OnNetCmd( tagNetCmd* pNetCmd )
{}


void UpperStateFollow_LP::SendMouseMoveEvent(LocalPlayer* pSelf,Role* pTarget)
{
	// 如果在有效距离之内，不再移动
	const float dist = Vec3Dist(pSelf->GetPos(),pTarget->GetPos());
	if(dist<=FOLLOW_VALID_DIST)
		return;

	tagMouseMoveEvent event;
	event.start=pSelf->GetPos();
	const Vector3 dir=(pTarget->GetPos()-pSelf->GetPos())/dist;
	event.end=pTarget->GetPos()-FOLLOW_VALID_DIST*dir;
	event.bUserCtrl=false;
	event.validDist=0.0f;
	event.bNeedSendStopCmd=true;
	m_pFSM->OnGameEvent(&event);
}

bool UpperStateFollow_LP::IfCanChange2UpperState( FSM_LP::EUpperState state,tagGameEvent* pEvent )
{
	return true;
}

bool UpperStateFollow_LP::IfCanChange2LowerState( FSM_LP::ELowerState state,tagGameEvent* pEvent )
{
	return true;
}

void UpperStateFollow_LP::OnChangeLowerState( FSM_LP::ELowerState newState,tagGameEvent* pEvent )
{
	switch(newState)
	{
	case FSM_LP::ELowerState_Jump:
	case FSM_LP::ELowerState_Slide:
	case FSM_LP::ELowerState_Drop:
	case FSM_LP::ELowerState_VDrop:
	case FSM_LP::ELowerState_Stand:
	case FSM_LP::ELowerState_Float:
	case FSM_LP::ELowerState_WaterStand:
		break;
	case FSM_LP::ELowerState_MouseMove:
	case FSM_LP::ELowerState_MouseSwim:
	case FSM_LP::ELowerState_MouseWaterWalk:
		{
			tagMouseMoveEvent* pMouseMove=(tagMouseMoveEvent*)pEvent;
			if(pMouseMove->bUserCtrl)
				m_pFSM->Change2UpperState_Idle();
		}
		break;
	default:
		{
			m_pFSM->Change2UpperState_Idle();
		}
		break;
	}
}

bool UpperStateFollow_LP::IfCanPlayLowerAni()
{
	return true;
}

bool UpperStateFollow_LP::IfLowerCanSetYaw()
{
	return true;
}
