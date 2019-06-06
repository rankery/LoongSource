#include "StdAfx.h"
#include "UpperStateIdle_RP.h"
#include "FSM_RP.h"
#include "Player.h"

UpperStateIdle_RP::UpperStateIdle_RP(void)
{}

UpperStateIdle_RP::~UpperStateIdle_RP(void)
{}

void UpperStateIdle_RP::Active( tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole )
{
	pRole->GetAniPlayer()->Sync2Lower();
}

void UpperStateIdle_RP::Update(NavMap* pNav,Player* pRole)
{
}

void UpperStateIdle_RP::OnGameEvent( tagGameEvent* pEvent )
{
	if(pEvent->strEventName==_T("tagBeAttackEvent"))
	{
		m_pFSM->TryChange2UpperState(FSM_RP::EUpperState_BeAttack,pEvent);
	}
}

void UpperStateIdle_RP::OnNetCmd( tagNetCmd* pNetCmd )
{
	m_pFSM->Change2UpperState(pNetCmd);
}

bool UpperStateIdle_RP::IfCanPlayLowerAni()
{
	return true;
}

bool UpperStateIdle_RP::IfLowerCanSetYaw()
{
	return true;
}