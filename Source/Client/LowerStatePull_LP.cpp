#include "StdAfx.h"
#include "LowerStatePull_LP.h"
#include "LocalPlayer.h"
#include "FSM_LP.h"
#include "..\WorldDefine\action.h"
#include "CombatSysUtil.h"

LowerStatePull_LP::LowerStatePull_LP(void)
{}

LowerStatePull_LP::~LowerStatePull_LP(void)
{}

void LowerStatePull_LP::Active( tagNetCmd* pNetCmd,NavMap* pNav,LocalPlayer* pRole,tagPreState* pPreState )
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

void LowerStatePull_LP::Update(NavMap* pNav,LocalPlayer* pRole)
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

void LowerStatePull_LP::OnGameEvent( tagGameEvent* pEvent )
{}

bool LowerStatePull_LP::IfCanChange2UpperState( FSM_LP::EUpperState state,tagGameEvent* pEvent )
{
	return false;
}

bool LowerStatePull_LP::IfCanChange2LowerState( FSM_LP::ELowerState state,tagGameEvent* pEvent )
{
	return false;
}