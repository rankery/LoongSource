#include "StdAfx.h"
#include "StateSopor_NPC.h"
#include "FSM_NPC.h"
#include "NPC.h"
#include "CombatEvent.h"
#include "..\WorldDefine\role_att.h"
#include "SkillProtoData.h"

StateSopor_NPC::StateSopor_NPC(void)
{}

StateSopor_NPC::~StateSopor_NPC(void)
{}

void StateSopor_NPC::Active( tagNetCmd* pNetCmd,NavMap* pNav,NPC* pRole )
{
	pRole->PlayTrack(_T("coma"),true);
}

void StateSopor_NPC::Update(NavMap* pNav,NPC* pRole)
{}

void StateSopor_NPC::OnGameEvent( tagGameEvent* pEvent )
{}

void StateSopor_NPC::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==pNetCmd->Crc32("NS_UnSetState"))
	{
		tagNS_UnSetState* pCmd=(tagNS_UnSetState*)pNetCmd;
		if(pCmd->eState==ES_Spor)
		{
			Role* pRole = m_pFSM->GetNPC();
			tagNS_SetState setStateCmd;
			setStateCmd.dwRoleID=pRole->GetID();
			if( pRole->IsInState(ES_Dizzy) ) 
			{
				setStateCmd.eState=ES_Dizzy;
				m_pFSM->Change2State( FSM_NPC::EState_Dazzy, &setStateCmd );
			}
			else if( pRole->IsInState(ES_Tie) ) 
			{
				setStateCmd.eState=ES_Tie;
				m_pFSM->Change2State( FSM_NPC::EState_Tie, &setStateCmd );
			}
			else
				m_pFSM->Change2IdleState();
		}
	}
	else
		m_pFSM->Change2State(pNetCmd);
}