#include "StdAfx.h"
#include "StateTie_NPC.h"
#include "FSM_NPC.h"
#include "NPC.h"
#include "CombatEvent.h"
#include "..\WorldDefine\role_att.h"
#include "SkillProtoData.h"

StateTie_NPC::StateTie_NPC(void)
{}

StateTie_NPC::~StateTie_NPC(void)
{}

void StateTie_NPC::Active( tagNetCmd* pNetCmd,NavMap* pNav,NPC* pRole )
{
}

void StateTie_NPC::Update(NavMap* pNav,NPC* pRole)
{}

void StateTie_NPC::OnGameEvent( tagGameEvent* pEvent )
{}

void StateTie_NPC::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==pNetCmd->Crc32("NS_UnSetState"))
	{
		tagNS_UnSetState* pCmd=(tagNS_UnSetState*)pNetCmd;
		if(pCmd->eState==ES_Tie)
		{
			Role* pRole = m_pFSM->GetNPC();
			tagNS_SetState setStateCmd;
			setStateCmd.dwRoleID=pRole->GetID();
			if( pRole->IsInState(ES_Dizzy) ) 
			{
				setStateCmd.eState=ES_Dizzy;
				m_pFSM->Change2State( FSM_NPC::EState_Dazzy, &setStateCmd );
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