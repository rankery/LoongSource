#include "StdAfx.h"
#include "LowerStateAssault_LP.h"
#include "LocalPlayer.h"
#include "FSM_LP.h"
#include "..\WorldDefine\action.h"
#include "CombatSysUtil.h"
#include "EffectMgr.h"

LowerStateAssault_LP::LowerStateAssault_LP(void) :
m_effectID( GT_INVALID )
{}

LowerStateAssault_LP::~LowerStateAssault_LP(void)
{}

void LowerStateAssault_LP::Active( tagNetCmd* pNetCmd,NavMap* pNav,LocalPlayer* pRole,tagPreState* pPreState )
{
	ASSERT( pNetCmd->dwID==pNetCmd->Crc32("NS_SpecialMove") );
	tagNS_SpecialMove* pSpecialMove = (tagNS_SpecialMove*)pNetCmd;
	ASSERT( ESMT_Assault == pSpecialMove->eType );

	m_collider.Init( pRole->GetPos(), pSpecialMove->vDestPos, pRole->GetRoleSize(), X_DEF_ASSAULT_SPEED, false, true );

	//--播放动作
	const TCHAR* szTrackName=CombatSysUtil::Inst()->GetMoveTrackName(pRole->GetRWeaponType(),pRole->IsReadyState(),X_DEF_ASSAULT_SPEED);
	pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Other,AvatarAniMixer::EAP_All,szTrackName,true);

	pRole->SetMoveYaw(CalcYaw(pSpecialMove->vDestPos-pRole->GetPos()));
	pRole->SetYaw(pRole->GetMoveYaw());

	//--播放特效
	m_effectID = EffectMgr::Inst()->PlayRoleEffect( pRole->GetID(), _T("Txzt27") );
}

void LowerStateAssault_LP::Update(NavMap* pNav,LocalPlayer* pRole)
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

void LowerStateAssault_LP::OnGameEvent( tagGameEvent* pEvent )
{}

bool LowerStateAssault_LP::IfCanChange2UpperState( FSM_LP::EUpperState state,tagGameEvent* pEvent )
{
	return false;
}

bool LowerStateAssault_LP::IfCanChange2LowerState( FSM_LP::ELowerState state,tagGameEvent* pEvent )
{
	return false;
}

void LowerStateAssault_LP::OnExit()
{
	EffectMgr::Inst()->StopEffect( m_effectID );
	m_effectID = GT_INVALID;
}