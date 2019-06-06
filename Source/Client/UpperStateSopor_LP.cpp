#include "StdAfx.h"
#include "UpperStateSopor_LP.h"
#include "FSM_LP.h"
#include "LocalPlayer.h"
#include "CombatEvent.h"
#include "..\WorldDefine\role_att.h"

UpperStateSopor_LP::UpperStateSopor_LP(void)
{}

UpperStateSopor_LP::~UpperStateSopor_LP(void)
{}

void UpperStateSopor_LP::Active( tagNetCmd* pNetCmd,NavMap* pNav,LocalPlayer* pRole )
{
	pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Other,AvatarAniMixer::EAP_All,_T("ze13"),true,true);
}

void UpperStateSopor_LP::Update(NavMap* pNav,LocalPlayer* pRole)
{}

void UpperStateSopor_LP::OnGameEvent( tagGameEvent* pEvent )
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

void UpperStateSopor_LP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==pNetCmd->Crc32("NS_UnSetState"))
	{
		tagNS_UnSetState* pCmd=(tagNS_UnSetState*)pNetCmd;
		if(pCmd->eState==ES_Spor)
		{
			Role* pRole = m_pFSM->GetLocalPlayer();
			tagNS_SetState setStateCmd;
			setStateCmd.dwRoleID=pRole->GetID();
			if( pRole->IsInState(ES_Dizzy) ) 
			{
				setStateCmd.eState=ES_Dizzy;
				m_pFSM->Change2UpperState( FSM_LP::EUpperState_Dazzy, &setStateCmd );
			}
			else if( pRole->IsInState(ES_Tie) ) 
			{
				setStateCmd.eState=ES_Tie;
				m_pFSM->Change2UpperState( FSM_LP::EUpperState_Tie, &setStateCmd );
			}
			else
				m_pFSM->Change2UpperState_Idle();
		}
	}
}

bool UpperStateSopor_LP::IfCanChange2UpperState( FSM_LP::EUpperState state,tagGameEvent* pEvent )
{
	return true;
}

bool UpperStateSopor_LP::IfCanChange2LowerState( FSM_LP::ELowerState state,tagGameEvent* pEvent )
{
	return false;
}

bool UpperStateSopor_LP::IfCanPlayLowerAni()
{
	return false;
}

bool UpperStateSopor_LP::IfLowerCanSetYaw()
{
	return false;
}

void UpperStateSopor_LP::OnExit()
{
	m_pFSM->ReplayLowerAni();
}