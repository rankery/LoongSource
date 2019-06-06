#include "StdAfx.h"
#include "StateMoveOnCarrier.h"
#include "ClientApp.h"
#include "Role.h"
#include "FSM.h"
#include "GameEvent.h"

StateMoveOnCarrier::StateMoveOnCarrier(void)
{
}

StateMoveOnCarrier::~StateMoveOnCarrier(void)
{
}

void StateMoveOnCarrier::Active( tagGameEvent* pEvent )
{
	ASSERT(pEvent->event==EGE_Move);
	tagMoveEvent* pMoveEvent=(tagMoveEvent*)pEvent;

	m_event=*pMoveEvent;
	
	NavMap* pNav=App()->GetMapView()->GetNav();
	Role* pRole=m_pFSM->GetRole();

	NavCarrierNode* pCarrier=pNav->GetCollider()->FindCarrier(pMoveEvent->dwCarrierObjID);
	if(pCarrier==NULL)
	{
		m_pFSM->Change2IdleState();
		return;
	}

	Vector3 localStart;
	pCarrier->PosToLocal(pMoveEvent->start,Kernel::Inst()->GetAccumTime(),localStart);
	Vector3 localDir=pMoveEvent->end-pMoveEvent->start;
	pCarrier->DirToLocal(localDir,Kernel::Inst()->GetAccumTime(),localDir);

	//localStart=Vector3(503.87604f,301.27859f,413.51764f);
	//localDir=Vector3(-640.543884f,-2261.277588f,-1390.359253f);
	m_dropDir=localDir;

	m_collider.Init(pMoveEvent->dwCarrierObjID,localStart,localDir,pRole->GetRoleSize(),pRole->GetMoveSpeed());

	Vector3 dir=pMoveEvent->end-pMoveEvent->start;
	pRole->SetYaw(CalcYaw(dir));

	pRole->PlayTrack(_T("run1"),true);
}

void StateMoveOnCarrier::Update()
{
	NavMap* pNav=App()->GetMapView()->GetNav();
	Role* pRole=m_pFSM->GetRole();

	float deltaTime=Kernel::Inst()->GetDeltaTime();
	float startTime=Kernel::Inst()->GetAccumTime()-deltaTime;

	Vector3 nowPos;
	DWORD dwCarrierObjID;
	float endTime;
	int result=m_collider.Update(pNav->GetCollider(),startTime,deltaTime,nowPos,endTime,dwCarrierObjID);
	pRole->SetPos(nowPos);

	switch(result)
	{
	case ENR_ToBeContinued:
		{
		}
		break;
	case ENR_Blocking:
		{
			tagStandOnCarrierEvent event;
			event.dwCarrierObjID=m_event.dwCarrierObjID;
			event.startTime=endTime;
			m_pFSM->Change2State(&event);
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
	case ENR_CarryLanded:
		{
			m_pFSM->Change2IdleState();
		}
		break;
	case ENR_WillDrop:
		{
			NavMap* pNav=App()->GetMapView()->GetNav();
			NavCarrierNode* pCarrier=pNav->GetCollider()->FindCarrier(m_event.dwCarrierObjID);
			if(pCarrier==NULL)
			{
				return;
			}

			tagDropEvent dropEvent;
			dropEvent.start=nowPos;
			dropEvent.xzDir=m_dropDir;
			dropEvent.xzSpeed=pRole->GetDropXzSpeed();
			dropEvent.ySpeed=0.0f;
			dropEvent.startTime=endTime;
			pCarrier->AdjustJumpDirAndSpeed(endTime,dropEvent.xzDir,dropEvent.xzSpeed,dropEvent.ySpeed);
			m_pFSM->Change2State(&dropEvent);
		}
		break;
	default:
		{
			m_pFSM->Change2IdleState();
		}
		break;
	}
}

void StateMoveOnCarrier::OnGameEvent( tagGameEvent* pEvent )
{
	if(pEvent->event==EGE_Move)
	{
		tagMoveEvent* pMoveEvent=(tagMoveEvent*)pEvent;
		pMoveEvent->dwCarrierObjID=m_event.dwCarrierObjID;
		m_pFSM->Change2State(FSM::EState_MoveOnCarrier,pEvent);
	}
	else if(pEvent->event==EGE_Jump)
	{
		NavMap* pNav=App()->GetMapView()->GetNav();
		NavCarrierNode* pCarrier=pNav->GetCollider()->FindCarrier(m_event.dwCarrierObjID);
		if(pCarrier==NULL)
		{
			return;
		}

		Role* pRole=m_pFSM->GetRole();

		tagJumpEvent event;
		event.start=m_pFSM->GetRole()->GetPos();
		event.xzDir=m_event.end-m_event.start;
		event.xzSpeed=pRole->GetJumpXzSpeed();
		event.ySpeed=pRole->GetJumpYSpeed();
		pCarrier->AdjustJumpDirAndSpeed(Kernel::Inst()->GetAccumTime(),event.xzDir,event.xzSpeed,event.ySpeed);
		m_pFSM->Change2State(&event);
	}
}
