#include "StdAfx.h"
#include "StateAssault_NPC.h"
#include "CombatEvent.h"
#include "NPC.h"
#include "FSM_NPC.h"
#include "CombatSysUtil.h"
#include "WorldFrame.h"
#include "..\WorldDefine\action.h"
#include "..\WorldDefine\role_att.h"

StateAssault_NPC::StateAssault_NPC(void)
{}

StateAssault_NPC::~StateAssault_NPC(void)
{}

void StateAssault_NPC::Active( tagNetCmd* pNetCmd,NavMap* pNav,NPC* pRole )
{
	ASSERT( pNetCmd->dwID==pNetCmd->Crc32("NS_SpecialMove") );
	tagNS_SpecialMove* pSpecialMove = (tagNS_SpecialMove*)pNetCmd;
	ASSERT( ESMT_Assault == pSpecialMove->eType );

	m_collider.Init( pRole->GetPos(), pSpecialMove->vDestPos, pRole->GetRoleSize(), X_DEF_ASSAULT_SPEED, false, true );

	//--²¥·Å¶¯×÷
	const TCHAR* szTrackName=CombatSysUtil::Inst()->GetNPCMoveTrackName(X_DEF_ASSAULT_SPEED);
	pRole->PlayTrack(szTrackName,true);

	pRole->SetYaw(CalcYaw(pSpecialMove->vDestPos-pRole->GetPos()));
}

void StateAssault_NPC::Update(NavMap* pNav,NPC* pRole)
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

void StateAssault_NPC::OnGameEvent( tagGameEvent* pEvent )
{}

void StateAssault_NPC::OnNetCmd( tagNetCmd* pNetCmd )
{
	m_pFSM->Change2State(pNetCmd);
}