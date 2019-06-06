#include "StdAfx.h"
#include "UpperStateDazzy_LP.h"
#include "FSM_LP.h"
#include "LocalPlayer.h"
#include "CombatEvent.h"
#include "..\WorldDefine\role_att.h"

UpperStateDazzy_LP::UpperStateDazzy_LP(void)
{}

UpperStateDazzy_LP::~UpperStateDazzy_LP(void)
{}

void UpperStateDazzy_LP::Active( tagNetCmd* pNetCmd,NavMap* pNav,LocalPlayer* pRole )
{
	pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Other,AvatarAniMixer::EAP_All,_T("ze12"),true,true);
}

void UpperStateDazzy_LP::Update(NavMap* pNav,LocalPlayer* pRole)
{}

void UpperStateDazzy_LP::OnGameEvent( tagGameEvent* pEvent )
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

void UpperStateDazzy_LP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==pNetCmd->Crc32("NS_UnSetState"))
	{
		tagNS_UnSetState* pCmd=(tagNS_UnSetState*)pNetCmd;
		if(pCmd->eState==ES_Dizzy)
		{
			Role* pRole = m_pFSM->GetLocalPlayer();
			tagNS_SetState setStateCmd;
			setStateCmd.dwRoleID=pRole->GetID();
			if( pRole->IsInState(ES_Tie) ) 
			{
				setStateCmd.eState=ES_Tie;
				m_pFSM->Change2UpperState( FSM_LP::EUpperState_Tie, &setStateCmd );
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

bool UpperStateDazzy_LP::IfCanChange2UpperState( FSM_LP::EUpperState state,tagGameEvent* pEvent )
{
	return true;
}

bool UpperStateDazzy_LP::IfCanChange2LowerState( FSM_LP::ELowerState state,tagGameEvent* pEvent )
{
	return false;
}

bool UpperStateDazzy_LP::IfCanPlayLowerAni()
{
	return false;
}

bool UpperStateDazzy_LP::IfLowerCanSetYaw()
{
	return false;
}	

void UpperStateDazzy_LP::OnExit()
{
	m_pFSM->ReplayLowerAni();
}