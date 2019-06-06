#include "StdAfx.h"
#include "StateSwim.h"
#include "ClientApp.h"
#include "Role.h"
#include "FSM.h"
#include "GameEvent.h"

StateSwim::StateSwim(void)
{
}

StateSwim::~StateSwim(void)
{
}

void StateSwim::Active( tagGameEvent* pEvent )
{
	ASSERT(pEvent->event==EGE_Swim);
	tagSwimEvent* pSwimEvent=(tagSwimEvent*)pEvent;

	m_event=*pSwimEvent;
	
	NavMap* pNav=App()->GetMapView()->GetNav();
	Role* pRole=m_pFSM->GetRole();

	m_ret=pNav->GetCollider()->Swim(pSwimEvent->start,pSwimEvent->end,pRole->GetRoleSize(),100,m_movePath);

	if(m_movePath.size()==0)
	{
		m_pFSM->Change2IdleState();
		return;
	}

	m_moveStartTime=Kernel::Inst()->GetAccumTime();
	m_moveStart = pSwimEvent->start;
	m_moveDest.x=m_movePath.back().pos.x;
	m_moveDest.y=m_movePath.back().pos.y;
	m_moveDest.z=m_movePath.back().pos.z;
	m_moveDir=m_moveDest-m_moveStart;
	Vec3Normalize(m_moveDir);
	m_moveTime=m_movePath.back().dist/pRole->GetMoveSpeed();

	pRole->SetYaw(CalcYaw(m_moveDir));
	pRole->PlayTrack(_T("swm2"),true);
}

void StateSwim::Update()
{
	NavMap* pNav=App()->GetMapView()->GetNav();
	Role* pRole=m_pFSM->GetRole();

	Vector3 nowPos;
	float runTime=Kernel::Inst()->GetAccumTime()-m_moveStartTime;
	if(runTime>=m_moveTime)//finish
	{
		pNav->GetCollider()->CalcMovePos(m_moveStart,m_movePath,1.0f,nowPos);
		pRole->SetPos(nowPos);

		if(m_ret==NavCollider::ERet_SwimGround)
			m_pFSM->Change2IdleState();
		else
			m_pFSM->Change2FloatState();
	}
	else
	{// ÒÆ¶¯ÖĞ

		float r=runTime/m_moveTime;
		pNav->GetCollider()->CalcMovePos(m_moveStart,m_movePath,r,nowPos);
		pRole->SetPos(nowPos);
	}
}

void StateSwim::OnGameEvent( tagGameEvent* pEvent )
{
	if(pEvent->event==EGE_Move)
	{
		tagMoveEvent* pMoveEvent=(tagMoveEvent*)pEvent;
		tagSwimEvent event;
		event.start=m_pFSM->GetRole()->GetPos();
		event.end=pMoveEvent->end;
		m_pFSM->Change2State(&event);
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
