#include "StdAfx.h"
#include "StatePull_NPC.h"
#include "CombatEvent.h"
#include "NPC.h"
#include "FSM_NPC.h"
#include "CombatSysUtil.h"
#include "WorldFrame.h"
#include "..\WorldDefine\action.h"
#include "..\WorldDefine\role_att.h"

StatePull_NPC::StatePull_NPC(void)
{}

StatePull_NPC::~StatePull_NPC(void)
{}

void StatePull_NPC::Active( tagNetCmd* pNetCmd,NavMap* pNav,NPC* pRole )
{
	ASSERT( pNetCmd->dwID==pNetCmd->Crc32("NS_SpecialMove") );
	tagNS_SpecialMove* pSpecialMove = (tagNS_SpecialMove*)pNetCmd;
	ASSERT( ESMT_Pull == pSpecialMove->eType );

	//--³õÊ¼»¯Åö×²¼ì²âÆ÷
	m_collider.Init( pRole->GetPos(), pSpecialMove->vDestPos, pRole->GetRoleSize(), X_DEF_PULL_SPEED, false, true );

	//--²¥·Å¶¯×÷
	pRole->PlayTrack(_T("batk"),true);
}

void StatePull_NPC::Update(NavMap* pNav,NPC* pRole)
{
	//--Åö×²¼ì²â
	float deltaTime=Kernel::Inst()->GetDeltaTime();
	float startTime=Kernel::Inst()->GetAccumTime()-deltaTime;

	Vector3 nowPos;
	float endTime;
	DWORD dwCarrierObjID;
	ENavResult result=m_collider.Update(pNav->GetCollider(),startTime,deltaTime,nowPos,endTime,dwCarrierObjID,false);

	pRole->SetPos(nowPos);	

	//--
	if(result!=ENR_ToBeContinued)
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
		else if( pRole->IsInState(ES_Spor) ) 
		{
			setStateCmd.eState=ES_Spor;
			m_pFSM->Change2State( FSM_NPC::EState_Sopor, &setStateCmd );
		}
		else
			m_pFSM->Change2IdleState();
	}
}

void StatePull_NPC::OnGameEvent( tagGameEvent* pEvent )
{}

void StatePull_NPC::OnNetCmd( tagNetCmd* pNetCmd )
{
	m_pFSM->Change2State(pNetCmd);
}