#include "StdAfx.h"
#include "LowerStateAssault_RP.h"
#include "CombatEvent.h"
#include "Player.h"
#include "FSM_RP.h"
#include "CombatSysUtil.h"
#include "WorldFrame.h"
#include "..\WorldDefine\action.h"
#include "EffectMgr.h"

LowerStateAssault_RP::LowerStateAssault_RP(void) :
m_effectID( GT_INVALID )
{}

LowerStateAssault_RP::~LowerStateAssault_RP(void)
{}

void LowerStateAssault_RP::Active(tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole )
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

void LowerStateAssault_RP::Update(NavMap* pNav,Player* pRole)
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

void LowerStateAssault_RP::OnGameEvent( tagGameEvent* pEvent )
{}

void LowerStateAssault_RP::OnNetCmd( tagNetCmd* pNetCmd )
{
	m_pFSM->Change2LowerState(pNetCmd);
}

bool LowerStateAssault_RP::IfCanChange2UpperState( FSM_RP::EUpperState state )
{
	return true;
}

void LowerStateAssault_RP::OnExit()
{
	EffectMgr::Inst()->StopEffect( m_effectID );
	m_effectID = GT_INVALID;
}