#include "StdAfx.h"
#include "StateJump.h"
#include "ClientApp.h"
#include "Role.h"
#include "FSM.h"
#include "GameEvent.h"

StateJump::StateJump(void)
{
}

StateJump::~StateJump(void)
{
}

void StateJump::Active( tagGameEvent* pEvent )
{
	ASSERT(pEvent->event==EGE_Jump);
	tagJumpEvent* pJumpEvent=(tagJumpEvent*)pEvent;

	m_event=*pJumpEvent;

	NavMap* pNav=App()->GetMapView()->GetNav();
	Role* pRole=m_pFSM->GetRole();

	m_collider.Init(pJumpEvent->start,pJumpEvent->xzDir,pRole->GetRoleSize(),pJumpEvent->xzSpeed,pJumpEvent->ySpeed);

	pRole->PlayTrack(_T("std1"),true);
}

void StateJump::Update()
{
	NavMap* pNav=App()->GetMapView()->GetNav();
	Role* pRole=m_pFSM->GetRole();

	float deltaTime=Kernel::Inst()->GetDeltaTime();
	float startTime=Kernel::Inst()->GetAccumTime()-deltaTime;

	Vector3 nowPos;
	float endTime;
	float cdCarrierTime;
	DWORD dwCarrierObjID;
	ENavResult result=m_collider.Update(pNav->GetCollider(),startTime,deltaTime,NULL,nowPos,endTime,dwCarrierObjID,cdCarrierTime);
	pRole->SetPos(nowPos);

	switch(result)
	{
	case ENR_ToBeContinued:
		{
		}
		break;
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
	case ENR_WillVDrop:
		{
			NavMap* pNav=App()->GetMapView()->GetNav();
			NavCarrierNode* pCarrier=NULL;
			if(dwCarrierObjID!=-1)
				pCarrier=pNav->GetCollider()->FindCarrier(dwCarrierObjID);

			tagVDropEvent dropEvent;
			dropEvent.start=nowPos;
			dropEvent.xzDir=MathConst::Zero3;
			dropEvent.xzSpeed=0.0f;
			dropEvent.ySpeed=0.0f;
			dropEvent.startTime=endTime;
			if(pCarrier!=NULL)
				pCarrier->AdjustJumpDirAndSpeed(cdCarrierTime,dropEvent.xzDir,dropEvent.xzSpeed,dropEvent.ySpeed);

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
	case ENR_Landed:
		{
			m_pFSM->Change2IdleState();
		}
		break;
	default:
		{
			m_pFSM->Change2IdleState();
		}
		break;
	}
}

void StateJump::OnGameEvent( tagGameEvent* pEvent )
{

}