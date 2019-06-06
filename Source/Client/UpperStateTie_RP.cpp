#include "StdAfx.h"
#include "UpperStateTie_RP.h"
#include "FSM_RP.h"
#include "Player.h"
#include "CombatEvent.h"
#include "..\WorldDefine\role_att.h"
#include "SkillProtoData.h"
#include "MapMgr.h"

UpperStateTie_RP::UpperStateTie_RP(void)
{}

UpperStateTie_RP::~UpperStateTie_RP(void)
{}

void UpperStateTie_RP::Active( tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole )
{
}

void UpperStateTie_RP::Update(NavMap* pNav,Player* pRole)
{}

void UpperStateTie_RP::OnGameEvent( tagGameEvent* pEvent )
{}

void UpperStateTie_RP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==pNetCmd->Crc32("NS_UnSetState"))
	{
		tagNS_UnSetState* pCmd=(tagNS_UnSetState*)pNetCmd;
		if(pCmd->eState==ES_Tie)
		{
			Role* pRole = m_pFSM->GetPlayer();
			tagNS_SetState setStateCmd;
			setStateCmd.dwRoleID=pRole->GetID();
			if( pRole->IsInState(ES_Dizzy) ) 
			{
				setStateCmd.eState=ES_Dizzy;
				m_pFSM->Change2UpperState( FSM_RP::EUpperState_Dazzy, &setStateCmd );
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

void UpperStateTie_RP::OnExit()
{
	m_pFSM->ReplayLowerAni();
}

bool UpperStateTie_RP::IfCanPlayLowerAni()
{
	return false;
}

bool UpperStateTie_RP::IfLowerCanSetYaw()
{
	return false;
}