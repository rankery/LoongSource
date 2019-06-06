#include "StdAfx.h"
#include "StateShutdown_PET.h"
#include "FSM_PET.h"
#include "Pet.h"
#include "CombatEvent.h"

StateShutdown_PET::StateShutdown_PET(void)
{
}

StateShutdown_PET::~StateShutdown_PET(void)
{
}

void StateShutdown_PET::Active( tagNetCmd* pNetCmd, NavMap* pNav, Pet* pRole )
{
    pRole->PlayTrack(_T("std"),true);
}

void StateShutdown_PET::Update( NavMap* pNav, Pet* pRole )
{

}

void StateShutdown_PET::OnNetCmd( tagNetCmd* pNetCmd )
{
    m_pFSM->Change2State(pNetCmd);
}

void StateShutdown_PET::OnGameEvent( tagGameEvent* pEvent )
{
    m_pFSM->Change2State(pEvent);
}