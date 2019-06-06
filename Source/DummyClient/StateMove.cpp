#include "StdAfx.h"
#include "StateMove.h"
#include "ClientApp.h"
#include "Role.h"
#include "FSM.h"
#include "GameEvent.h"

StateMove::StateMove(void)
{
}

StateMove::~StateMove(void)
{
}

void StateMove::Active( tagGameEvent* pEvent )
{
	ASSERT(pEvent->event==EGE_Move);
	tagMoveEvent* pMoveEvent=(tagMoveEvent*)pEvent;

	m_event=*pMoveEvent;
	
	NavMap* pNav=App()->GetMapView()->GetNav();
	Role* pRole=m_pFSM->GetRole();

	m_collider.Init(m_event.start,m_event.end,pRole->GetRoleSize(),pRole->GetMoveSpeed(),true);

	Vector3 dir=pMoveEvent->end-pMoveEvent->start;
	pRole->SetYaw(CalcYaw(dir));

	pRole->PlayTrack(_T("run1"),true);
}

void StateMove::Update()
{
	NavMap* pNav=App()->GetMapView()->GetNav();
	Role* pRole=m_pFSM->GetRole();

	float deltaTime=Kernel::Inst()->GetDeltaTime();
	float startTime=Kernel::Inst()->GetAccumTime()-deltaTime;

	Vector3 nowPos;
	float endTime;
	DWORD dwCarrierObjID;
	int result=m_collider.Update(pNav->GetCollider(),startTime,deltaTime,nowPos,endTime,dwCarrierObjID);
	pRole->SetPos(nowPos);

	switch(result)
	{
	case ENR_ToBeContinued:
		{
		}
		break;
	case ENR_Arrive:
	case ENR_Blocking:
		{
			m_pFSM->Change2IdleState();
		}
		break;
	case ENR_WillCarry:
		{
			tagStandOnCarrierEvent event;
			event.dwCarrierObjID=dwCarrierObjID;
			event.startTime=endTime;
			m_pFSM->Change2State(&event);
		}
		break;
	case ENR_WillDrop:
		{
			tagDropEvent dropEvent;
			dropEvent.start=nowPos;
			dropEvent.xzDir=m_event.end-m_event.start;
			dropEvent.xzSpeed=pRole->GetDropXzSpeed();
			dropEvent.ySpeed=0.0f;
			dropEvent.startTime=endTime;
			m_pFSM->Change2State(&dropEvent);
		}
		break;
	case ENR_WillSlide:
		{
			tagSlideEvent slideEvent;
			slideEvent.start=nowPos;
			m_pFSM->Change2State(&slideEvent);
		}
		break;
	default:
		{
			m_pFSM->Change2IdleState();
		}
		break;
	}
}

void StateMove::OnGameEvent( tagGameEvent* pEvent )
{
	if(pEvent->event==EGE_Move)
	{
		m_pFSM->Change2State(pEvent);
	}
	else if(pEvent->event==EGE_Jump)
	{
		Role* pRole=m_pFSM->GetRole();

		tagJumpEvent event;
		event.start=m_pFSM->GetRole()->GetPos();
		event.xzDir=m_event.end-m_event.start;
		event.xzSpeed=pRole->GetJumpXzSpeed();
		event.ySpeed=pRole->GetJumpYSpeed();
		m_pFSM->Change2State(&event);
	}
}
