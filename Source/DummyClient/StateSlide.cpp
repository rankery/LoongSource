#include "StdAfx.h"
#include "StateSlide.h"
#include "ClientApp.h"
#include "Role.h"
#include "FSM.h"


StateSlide::StateSlide(void)
{}

StateSlide::~StateSlide(void)
{}

void StateSlide::Active( tagGameEvent* pEvent )
{
	ASSERT(pEvent->event==EGE_Slide);
	tagSlideEvent* pSlideEvent=(tagSlideEvent*)pEvent;

	m_event=*pSlideEvent;

	NavMap* pNav=App()->GetMapView()->GetNav();
	Role* pRole=m_pFSM->GetRole();

	m_collider.Init(m_event.start,pRole->GetRoleSize());

	pRole->PlayTrack(_T("std1"),true);
}

void StateSlide::Update()
{
	NavMap* pNav=App()->GetMapView()->GetNav();
	Role* pRole=m_pFSM->GetRole();

	float deltaTime=Kernel::Inst()->GetDeltaTime();
	float startTime=Kernel::Inst()->GetAccumTime()-deltaTime;

	Vector3 nowPos;
	float endTime;
	int result=m_collider.Update(pNav->GetCollider(),startTime,deltaTime,nowPos,endTime);
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
	default:
		{
			m_pFSM->Change2IdleState();
		}
		break;
	}
}

void StateSlide::OnGameEvent( tagGameEvent* pEvent )
{}