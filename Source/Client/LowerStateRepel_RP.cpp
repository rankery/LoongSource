#include "StdAfx.h"
#include "LowerStateRepel_RP.h"
#include "CombatEvent.h"
#include "Player.h"
#include "FSM_RP.h"
#include "CombatSysUtil.h"
#include "WorldFrame.h"
#include "..\WorldDefine\action.h"
#include "EffectMgr.h"

LowerStateRepel_RP::LowerStateRepel_RP(void) :
m_effectID( GT_INVALID )
{}

LowerStateRepel_RP::~LowerStateRepel_RP(void)
{}

void LowerStateRepel_RP::Active(tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole )
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

void LowerStateRepel_RP::Update(NavMap* pNav,Player* pRole)
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

void LowerStateRepel_RP::OnGameEvent( tagGameEvent* pEvent )
{}

void LowerStateRepel_RP::OnNetCmd( tagNetCmd* pNetCmd )
{
	m_pFSM->Change2LowerState(pNetCmd);
}

bool LowerStateRepel_RP::IfCanChange2UpperState( FSM_RP::EUpperState state )
{
	switch(state)
	{
	case FSM_RP::EUpperState_BeAttack:
		return false;
	}

	return true;
}

void LowerStateRepel_RP::OnExit()
{
	EffectMgr::Inst()->StopEffect( m_effectID );
	m_effectID = GT_INVALID;
}