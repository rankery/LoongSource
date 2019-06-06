#include "StdAfx.h"
#include "StateDrop.h"
#include "ClientApp.h"
#include "Role.h"
#include "FSM.h"
#include "GameEvent.h"

StateDrop::StateDrop(void)
{
}

StateDrop::~StateDrop(void)
{
}

void StateDrop::Active( tagGameEvent* pEvent )
{
	ASSERT(pEvent->event==EGE_Drop);
	tagDropEvent* pDropEvent=(tagDropEvent*)pEvent;

	NavMap* pNav=App()->GetMapView()->GetNav();
	Role* pRole=m_pFSM->GetRole();

	m_collider.Init(pDropEvent->start,pDropEvent->xzDir,pRole->GetRoleSize(),pDropEvent->xzSpeed,pDropEvent->ySpeed);

	m_startTime=pDropEvent->startTime;

	pRole->PlayTrack(_T("std1"),true);
}

void StateDrop::Update()
{
	float nowTime=Kernel::Inst()->GetAccumTime();
	if(nowTime>m_startTime)
	{
		NavMap* pNav=App()->GetMapView()->GetNav();
		Role* pRole=m_pFSM->GetRole();

		Vector3 nowPos;
		float endTime;
		float cdCarrierTime;
		DWORD dwCarrierObjID;
		ENavResult result=m_collider.Update(pNav->GetCollider(),m_startTime,nowTime-m_startTime,NULL,nowPos,endTime,dwCarrierObjID,cdCarrierTime);
		m_startTime=nowTime;
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
		case ENR_Landed:
			{
				m_pFSM->Change2IdleState();
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
}

void StateDrop::OnGameEvent( tagGameEvent* pEvent )
{

}