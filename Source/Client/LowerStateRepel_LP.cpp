#include "StdAfx.h"
#include "LowerStateRepel_LP.h"
#include "LocalPlayer.h"
#include "FSM_LP.h"
#include "..\WorldDefine\action.h"
#include "CombatSysUtil.h"
#include "EffectMgr.h"

LowerStateRepel_LP::LowerStateRepel_LP(void) :
m_effectID( GT_INVALID )
{}

LowerStateRepel_LP::~LowerStateRepel_LP(void)
{}

void LowerStateRepel_LP::Active( tagNetCmd* pNetCmd,NavMap* pNav,LocalPlayer* pRole,tagPreState* pPreState )
{
	ASSERT( pNetCmd->dwID==pNetCmd->Crc32("NS_SpecialMove") );
	tagNS_SpecialMove* pSpecialMove = (tagNS_SpecialMove*)pNetCmd;
	ASSERT( ESMT_Repel == pSpecialMove->eType );

	m_collider.Init( pRole->GetPos(), pSpecialMove->vDestPos, pRole->GetRoleSize(), X_DEF_REPEL_SPEED, false, true );

	//--播放动作
	if( !pRole->IsDead() )
	{
		const TCHAR* szTrackName=CombatSysUtil::Inst()->GetBeattackTrackName();
		pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Other,AvatarAniMixer::EAP_All,szTrackName,false,true);
	}

	//--播放特效
	m_effectID = EffectMgr::Inst()->PlayRoleEffect( pRole->GetID(), _T("Txzt28") );
}

void LowerStateRepel_LP::Update(NavMap* pNav,LocalPlayer* pRole)
{
	//--碰撞检测
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

void LowerStateRepel_LP::OnGameEvent( tagGameEvent* pEvent )
{}

bool LowerStateRepel_LP::IfCanChange2UpperState( FSM_LP::EUpperState state,tagGameEvent* pEvent )
{
	return false;
}

bool LowerStateRepel_LP::IfCanChange2LowerState( FSM_LP::ELowerState state,tagGameEvent* pEvent )
{
	return false;
}

void LowerStateRepel_LP::OnExit()
{
	EffectMgr::Inst()->StopEffect( m_effectID );
	m_effectID = GT_INVALID;
}