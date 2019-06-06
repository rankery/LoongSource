#include "StdAfx.h"
#include "UpperStateTie_LP.h"
#include "FSM_LP.h"
#include "LocalPlayer.h"
#include "CombatEvent.h"
#include "..\WorldDefine\role_att.h"

UpperStateTie_LP::UpperStateTie_LP(void)
{}

UpperStateTie_LP::~UpperStateTie_LP(void)
{}

void UpperStateTie_LP::Active( tagNetCmd* pNetCmd,NavMap* pNav,LocalPlayer* pRole )
{
}

void UpperStateTie_LP::Update(NavMap* pNav,LocalPlayer* pRole)
{}

void UpperStateTie_LP::OnGameEvent( tagGameEvent* pEvent )
{
	if(pEvent->strEventName==_T("tagUseSkillEvent"))
	{
		m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_Skill,pEvent);
	}
	else if(pEvent->strEventName==_T("tagUseItemEvent"))
	{
		m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_UseItem,pEvent);
	}
}

void UpperStateTie_LP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==pNetCmd->Crc32("NS_UnSetState"))
	{
		tagNS_UnSetState* pCmd=(tagNS_UnSetState*)pNetCmd;
		if(pCmd->eState==ES_Tie)
		{
			Role* pRole = m_pFSM->GetLocalPlayer();
			tagNS_SetState setStateCmd;
			setStateCmd.dwRoleID=pRole->GetID();
			if( pRole->IsInState(ES_Dizzy) ) 
			{
				setStateCmd.eState=ES_Dizzy;
				m_pFSM->Change2UpperState( FSM_LP::EUpperState_Dazzy, &setStateCmd );
			}
			else if( pRole->IsInState(ES_Spor) ) 
			{
				setStateCmd.eState=ES_Spor;
				m_pFSM->Change2UpperState( FSM_LP::EUpperState_Sopor, &setStateCmd );
			}
			else
				m_pFSM->Change2UpperState_Idle();
		}
	}
}

bool UpperStateTie_LP::IfCanChange2UpperState( FSM_LP::EUpperState state,tagGameEvent* pEvent )
{
	return true;
}

bool UpperStateTie_LP::IfCanChange2LowerState( FSM_LP::ELowerState state,tagGameEvent* pEvent )
{
	return false;
}

bool UpperStateTie_LP::IfCanPlayLowerAni()
{
	return false;
}

bool UpperStateTie_LP::IfLowerCanSetYaw()
{
	return false;
}	

void UpperStateTie_LP::OnExit()
{
	m_pFSM->ReplayLowerAni();
}