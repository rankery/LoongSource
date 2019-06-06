#include "StdAfx.h"
#include "StateFloat.h"
#include "Role.h"
#include "GameEvent.h"
#include "FSM.h"
#include "ClientApp.h"
#include "Role.h"


const float DOWN_SPEED=300.0f;	//下沉速度，每秒多少个世界单位
const float UP_SPEED=200.0f;	//上浮速度，每秒多少个世界单位

StateFloat::StateFloat(void)
{
}

StateFloat::~StateFloat(void)
{
}

void StateFloat::Active( tagGameEvent* pEvent )
{
	Role* pRole=m_pFSM->GetRole();
	NavMap* pNav=App()->GetMapView()->GetNav();

	pRole->PlayTrack(_T("swm1"),true);

	m_startH=pRole->GetPos().y;
	m_endH=m_startH-100.0f;

	m_startTime=Kernel::Inst()->GetAccumTime();
	m_totalTime=(m_startH-m_endH)/DOWN_SPEED;

	m_step=EStep_Floating;
}

void StateFloat::Update()
{
	Role* pRole=m_pFSM->GetRole();
	NavMap* pNav=App()->GetMapView()->GetNav();

	if(m_step==EStep_GoDown)
	{
		float r=(Kernel::Inst()->GetAccumTime()-m_startTime)/m_totalTime;
		if(r>1)r=1;

		Vector3 curPos=pRole->GetPos();
		curPos.y=LinearLerp(m_startH,m_endH,r);
		pRole->SetPos(curPos);

		if(r>=1)
		{
			m_startH=m_endH;
			m_endH=pNav->GetCollider()->CalcSwimFootHeight(curPos,pRole->GetRoleSize().y,curPos.y);

			m_startTime=Kernel::Inst()->GetAccumTime();
			m_totalTime=fabs(m_startH-m_endH)/UP_SPEED;

			m_step=EStep_FloatUp;
		}
	}
	else if(m_step==EStep_FloatUp)
	{
		float r=(Kernel::Inst()->GetAccumTime()-m_startTime)/m_totalTime;
		if(r>1)r=1;

		Vector3 curPos=pRole->GetPos();
		curPos.y=LinearLerp(m_startH,m_endH,r);
		pRole->SetPos(curPos);

		if(r>=1)
			m_step=EStep_Floating;
	}
}

void StateFloat::OnGameEvent( tagGameEvent* pEvent )
{
	if(pEvent->event==EGE_Move)
	{
		tagMoveEvent* pMoveEvent=(tagMoveEvent*)pEvent;
		tagSwimEvent event;
		event.start=m_pFSM->GetRole()->GetPos();
		event.end=pMoveEvent->end;
		m_pFSM->Change2State(&event);
	}
}