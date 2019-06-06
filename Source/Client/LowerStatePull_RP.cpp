#include "StdAfx.h"
#include "LowerStatePull_RP.h"
#include "CombatEvent.h"
#include "Player.h"
#include "FSM_RP.h"
#include "CombatSysUtil.h"
#include "WorldFrame.h"
#include "..\WorldDefine\action.h"


LowerStatePull_RP::LowerStatePull_RP(void)
{}

LowerStatePull_RP::~LowerStatePull_RP(void)
{}

void LowerStatePull_RP::Active(tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole )
{
	ASSERT( pNetCmd->dwID==pNetCmd->Crc32("NS_SpecialMove") );
	tagNS_SpecialMove* pSpecialMove = (tagNS_SpecialMove*)pNetCmd;
	ASSERT( ESMT_Pull == pSpecialMove->eType );

	m_collider.Init( pRole->GetPos(), pSpecialMove->vDestPos, pRole->GetRoleSize(), X_DEF_PULL_SPEED, false, true );

	//--²¥·Å¶¯×÷
	if( !pRole->IsDead() )
	{
		const TCHAR* szTrackName=CombatSysUtil::Inst()->GetBeattackTrackName();
		pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Other,AvatarAniMixer::EAP_All,szTrackName,false,true);
	}
}

void LowerStatePull_RP::Update(NavMap* pNav,Player* pRole)
{
	//--Åö×²¼ì²â
	float deltaTime=Kernel::Inst()->GetDeltaTime();
	float startTime=Kernel::Inst()->GetAccumTime()-deltaTime;

	Vector3 nowPos;
	float endTime;
	DWORD dwCarrierObjID;
	ENavResult result=m_collider.Update(pNav->GetCollider(),startTime,deltaTime,nowPos,endTime,dwCarrierObjID,pRole->CanWaterWalk());

	pRole->SetPos(nowPos);	

	if(result!=ENR_ToBeContinued)
	{
		m_pFSM->Change2LowerState_AutoStand();
	}
}

void LowerStatePull_RP::OnGameEvent( tagGameEvent* pEvent )
{}

void LowerStatePull_RP::OnNetCmd( tagNetCmd* pNetCmd )
{
	m_pFSM->Change2LowerState(pNetCmd);
}

bool LowerStatePull_RP::IfCanChange2UpperState( FSM_RP::EUpperState state )
{
	switch(state)
	{
	case FSM_RP::EUpperState_BeAttack:
		return false;
	}

	return true;
}