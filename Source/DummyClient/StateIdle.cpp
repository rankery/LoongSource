#include "StdAfx.h"
#include "StateIdle.h"
#include "Role.h"
#include "GameEvent.h"
#include "FSM.h"
#include "ClientApp.h"

StateIdle::StateIdle(void)
{
}

StateIdle::~StateIdle(void)
{
}

void StateIdle::Active( tagGameEvent* pEvent )
{
	Role* pRole=m_pFSM->GetRole();
	pRole->PlayTrack(_T("std1"),true);
}

void StateIdle::Update()
{

}

void StateIdle::OnGameEvent( tagGameEvent* pEvent )
{
	if(pEvent->event==EGE_Move)
		m_pFSM->Change2State(FSM::EState_Move,pEvent);
}