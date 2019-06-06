#include "StdAfx.h"
#include "UpperStateSopor_RP.h"
#include "FSM_RP.h"
#include "Player.h"
#include "CombatEvent.h"
#include "..\WorldDefine\role_att.h"
#include "SkillProtoData.h"
#include "MapMgr.h"

UpperStateSopor_RP::UpperStateSopor_RP(void)
{}

UpperStateSopor_RP::~UpperStateSopor_RP(void)
{}

void UpperStateSopor_RP::Active( tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole )
{
	pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Other,AvatarAniMixer::EAP_All,_T("ze13"),true,true);
}

void UpperStateSopor_RP::Update(NavMap* pNav,Player* pRole)
{}

void UpperStateSopor_RP::OnGameEvent( tagGameEvent* pEvent )
{}

void UpperStateSopor_RP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==pNetCmd->Crc32("NS_UnSetState"))
	{
		tagNS_UnSetState* pCmd=(tagNS_UnSetState*)pNetCmd;
		if(pCmd->eState==ES_Spor)
		{
			Role* pRole = m_pFSM->GetPlayer();
			tagNS_SetState setStateCmd;
			setStateCmd.dwRoleID=pRole->GetID();
			if( pRole->IsInState(ES_Dizzy) ) 
			{
				setStateCmd.eState=ES_Dizzy;
				m_pFSM->Change2UpperState( FSM_RP::EUpperState_Dazzy, &setStateCmd );
			}
			else if( pRole->IsInState(ES_Tie) ) 
			{
				setStateCmd.eState=ES_Tie;
				m_pFSM->Change2UpperState( FSM_RP::EUpperState_Tie, &setStateCmd );
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

void UpperStateSopor_RP::OnExit()
{
	m_pFSM->ReplayLowerAni();
}

bool UpperStateSopor_RP::IfCanPlayLowerAni()
{
	return false;
}

bool UpperStateSopor_RP::IfLowerCanSetYaw()
{
	return false;
}