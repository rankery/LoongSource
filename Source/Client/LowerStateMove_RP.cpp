#include "StdAfx.h"
#include "LowerStateMove_RP.h"
#include "Player.h"
#include "FSM_RP.h"
#include "CombatSysUtil.h"
#include "..\WorldDefine\action.h"
#include "EffectMgr.h"
#include "RoleEvent.h"

LowerStateMove_RP::LowerStateMove_RP(void)
{
	m_lastPlayeEffectTime=Kernel::Inst()->GetAccumTime();
}

LowerStateMove_RP::~LowerStateMove_RP(void)
{}

void LowerStateMove_RP::Active( tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole )
{
	ASSERT(pNetCmd->dwID==pNetCmd->Crc32("NS_SyncWalk"));
	tagNS_SyncWalk* pCmd=(tagNS_SyncWalk*)pNetCmd;

	//--初始化碰撞检测器
	m_collider.Init(pCmd->srcPos,pCmd->dstPos,pRole->GetRoleSize(),pCmd->fXZSpeed,false);
	m_lastCDTime=Kernel::Inst()->GetAccumTime()-pCmd->curTime;

	//--播放动作
	if(m_pFSM->IfCanPlayLowerAni())
	{
		const TCHAR* szTrackName=CombatSysUtil::Inst()->GetMoveTrackName(pRole->GetRWeaponType(),pRole->IsReadyState(),pCmd->fXZSpeed);
		pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Move,AvatarAniMixer::EAP_Lower,szTrackName,true);
	}

	//--设置朝向
	Vector3 dir=pCmd->dstPos-pCmd->srcPos;
	pRole->SetMoveYaw(CalcYaw(dir));
	if(m_pFSM->IfLowerCanSetYaw())
	{
		pRole->SetYaw(pRole->GetMoveYaw());
	}

	//--播放水花特效
	if(Kernel::Inst()->GetAccumTime()-m_lastPlayeEffectTime>0.1f)
	{
		m_lastPlayeEffectTime=Kernel::Inst()->GetAccumTime();
		PlaySprayEffect(pNav,pRole);
	}
}

void LowerStateMove_RP::Update( NavMap* pNav,Player* pRole )
{
	//--碰撞检测
	Vector3 nowPos;
	DWORD dwCarrierObjID;
	float endTime;

	float startTime=m_lastCDTime;
	float deltaTime=Kernel::Inst()->GetAccumTime()-m_lastCDTime;

	ENavResult result=m_collider.Update(pNav->GetCollider(),startTime,deltaTime,nowPos,endTime,dwCarrierObjID,pRole->CanWaterWalk());
	m_lastCDTime=Kernel::Inst()->GetAccumTime();

	pRole->SetPos(nowPos);

	//--播放水花特效
	if(Kernel::Inst()->GetAccumTime()-m_lastPlayeEffectTime>0.1f)
	{
		m_lastPlayeEffectTime=Kernel::Inst()->GetAccumTime();
		PlaySprayEffect(pNav,pRole);
	}

	//--
	if(result!=ENR_ToBeContinued)
		m_pFSM->Change2LowerState_Stand();
}

void LowerStateMove_RP::OnNetCmd( tagNetCmd* pNetCmd )
{
	m_pFSM->Change2LowerState(pNetCmd);
}

void LowerStateMove_RP::OnGameEvent( tagGameEvent* pEvent )
{
	if(pEvent->strEventName==_T("tagWeaponChangeEvent"))//切换武器
	{
		tagWeaponChangeEvent* pWeaponChangeEvent = (tagWeaponChangeEvent*)pEvent;
		if( pWeaponChangeEvent->bRightWeapon )
			ReplayAni(m_pFSM->GetPlayer());
	}
}

void LowerStateMove_RP::ReplayAni( Player* pRole )
{
	const TCHAR* szTrackName=CombatSysUtil::Inst()->GetMoveTrackName(pRole->GetRWeaponType(),pRole->IsReadyState(),pRole->GetMoveSpeed());
	pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Move,AvatarAniMixer::EAP_Lower,szTrackName,true);
}

void LowerStateMove_RP::PlaySprayEffect( NavMap* pNav,Player* pRole )
{
	float waterH;
	if(pNav->GetCollider()->GetWaterHeight(pRole->GetPos(),waterH)
		&& pRole->GetPos().y<=waterH)
	{
		Vector3 effectPos=pRole->GetPos();
		effectPos.y=waterH;
		EffectMgr::Inst()->PlayEffect(effectPos,pRole->GetDestYaw(),_T("txqt06"));
	}
}

bool LowerStateMove_RP::IfCanChange2UpperState( FSM_RP::EUpperState state )
{
	switch(state)
	{
	case FSM_RP::EUpperState_BeAttack:
		return false;
	}

	return true;
}