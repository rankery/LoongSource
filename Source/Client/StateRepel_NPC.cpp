#include "StdAfx.h"
#include "StateRepel_NPC.h"
#include "CombatEvent.h"
#include "NPC.h"
#include "FSM_NPC.h"
#include "CombatSysUtil.h"
#include "WorldFrame.h"
#include "..\WorldDefine\action.h"
#include "EffectMgr.h"
#include "..\WorldDefine\role_att.h"

StateRepel_NPC::StateRepel_NPC(void) :
m_effectID( GT_INVALID )
{}

StateRepel_NPC::~StateRepel_NPC(void)
{}

void StateRepel_NPC::Active( tagNetCmd* pNetCmd,NavMap* pNav,NPC* pRole )
{
	ASSERT( pNetCmd->dwID==pNetCmd->Crc32("NS_SpecialMove") );
	tagNS_SpecialMove* pSpecialMove = (tagNS_SpecialMove*)pNetCmd;
	ASSERT( ESMT_Repel == pSpecialMove->eType );

	//--初始化碰撞检测器
	m_collider.Init( pRole->GetPos(), pSpecialMove->vDestPos, pRole->GetRoleSize(), X_DEF_REPEL_SPEED, false, true );

	//--播放动作
	pRole->PlayTrack(_T("batk"),true);

	//--播放特效
	m_effectID = EffectMgr::Inst()->PlayRoleEffect( pRole->GetID(), _T("Txzt28") );
}

void StateRepel_NPC::Update(NavMap* pNav,NPC* pRole)
{
	//--碰撞检测
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

void StateRepel_NPC::OnGameEvent( tagGameEvent* pEvent )
{}

void StateRepel_NPC::OnNetCmd( tagNetCmd* pNetCmd )
{
	m_pFSM->Change2State(pNetCmd);
}

void StateRepel_NPC::OnExit()
{
	EffectMgr::Inst()->StopEffect( m_effectID );
	m_effectID = GT_INVALID;
}