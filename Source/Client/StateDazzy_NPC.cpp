#include "StdAfx.h"
#include "StateDazzy_NPC.h"
#include "FSM_NPC.h"
#include "NPC.h"
#include "CombatEvent.h"
#include "..\WorldDefine\role_att.h"
#include "SkillProtoData.h"

StateDazzy_NPC::StateDazzy_NPC(void)
{}

StateDazzy_NPC::~StateDazzy_NPC(void)
{}

void StateDazzy_NPC::Active( tagNetCmd* pNetCmd,NavMap* pNav,NPC* pRole )
{
	pRole->PlayTrack(_T("coma"),true);
}

void StateDazzy_NPC::Update(NavMap* pNav,NPC* pRole)
{}

void StateDazzy_NPC::OnGameEvent( tagGameEvent* pEvent )
{}

void StateDazzy_NPC::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==pNetCmd->Crc32("NS_UnSetState"))
	{
		tagNS_UnSetState* pCmd=(tagNS_UnSetState*)pNetCmd;
		if(pCmd->eState==ES_Dizzy)
		{
			Role* pRole = m_pFSM->GetNPC();
			tagNS_SetState setStateCmd;
			setStateCmd.dwRoleID=pRole->GetID();
			if( pRole->IsInState(ES_Tie) ) 
			{
				setStateCmd.eState=ES_Tie;
				m_pFSM->Change2State( FSM_NPC::EState_Tie, &setStateCmd );
			}
			else if( pRole->IsInState(ES_Spor) ) 
			{
				setStateCmd.eState=ES_Spor;
				m_pFSM->Change2State( FSM_NPC::EState_Sopor, &setStateCmd );
			}
			else
				m_pFSM->Change2IdleState();
		}
	}
	else
		m_pFSM->Change2State(pNetCmd);
}