#include "StdAfx.h"
#include "UpperStateDazzy_RP.h"
#include "FSM_RP.h"
#include "Player.h"
#include "CombatEvent.h"
#include "..\WorldDefine\role_att.h"
#include "SkillProtoData.h"
#include "MapMgr.h"

UpperStateDazzy_RP::UpperStateDazzy_RP(void)
{}

UpperStateDazzy_RP::~UpperStateDazzy_RP(void)
{}

void UpperStateDazzy_RP::Active( tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole )
{
	pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Other,AvatarAniMixer::EAP_All,_T("ze12"),true,true);
}

void UpperStateDazzy_RP::Update(NavMap* pNav,Player* pRole)
{}

void UpperStateDazzy_RP::OnGameEvent( tagGameEvent* pEvent )
{}

void UpperStateDazzy_RP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==pNetCmd->Crc32("NS_UnSetState"))
	{
		tagNS_UnSetState* pCmd=(tagNS_UnSetState*)pNetCmd;
		if(pCmd->eState==ES_Dizzy)
		{
			Role* pRole = m_pFSM->GetPlayer();
			tagNS_SetState setStateCmd;
			setStateCmd.dwRoleID=pRole->GetID();
			if( pRole->IsInState(ES_Tie) ) 
			{
				setStateCmd.eState=ES_Tie;
				m_pFSM->Change2UpperState( FSM_RP::EUpperState_Tie, &setStateCmd );
			}
			else if( pRole->IsInState(ES_Spor) ) 
			{
				setStateCmd.eState=ES_Spor;
				m_pFSM->Change2UpperState( FSM_RP::EUpperState_Sopor, &setStateCmd );
			}
			else
				m_pFSM->Change2UpperState_Idle();
		}
	}
	else
	{
		m_pFSM->Change2UpperState(pNetCmd);
	}
}

void UpperStateDazzy_RP::OnExit()
{
	m_pFSM->ReplayLowerAni();
}

bool UpperStateDazzy_RP::IfCanPlayLowerAni()
{
	return false;
}

bool UpperStateDazzy_RP::IfLowerCanSetYaw()
{
	return false;
}