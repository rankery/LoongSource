#include "StdAfx.h"
#include "LowerStateSwim_RP.h"
#include "Player.h"
#include "FSM_RP.h"
#include "CombatSysUtil.h"
#include "..\WorldDefine\action.h"
#include "EffectMgr.h"

LowerStateSwim_RP::LowerStateSwim_RP(void)
{
	m_lastPlayeEffectTime=Kernel::Inst()->GetAccumTime();
}

LowerStateSwim_RP::~LowerStateSwim_RP(void)
{}

void LowerStateSwim_RP::Active( tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole )
{
	ASSERT(pNetCmd->dwID==pNetCmd->Crc32("NS_SyncWalk"));
	tagNS_SyncWalk* pCmd=(tagNS_SyncWalk*)pNetCmd;

	//--初始化碰撞检测器
	m_collider.Init(pCmd->srcPos,pCmd->dstPos,pRole->GetRoleSize(),pCmd->fXZSpeed,false);
	m_lastCDTime=Kernel::Inst()->GetAccumTime()-pCmd->curTime;

	//--设置朝向
	Vector3 dir=pCmd->dstPos-pCmd->srcPos;
	float yaw=CalcYaw(dir);
	pRole->SetYaw(yaw);
	pRole->SetMoveYaw(yaw);

	//--播放动作
	pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Other,AvatarAniMixer::EAP_Lower,_T("swm2"),true);

	//--播放水花特效
	if(Kernel::Inst()->GetAccumTime()-m_lastPlayeEffectTime>0.1f)
	{
		m_lastPlayeEffectTime=Kernel::Inst()->GetAccumTime();
		PlaySprayEffect(pNav,pRole);
	}
}

void LowerStateSwim_RP::Update( NavMap* pNav,Player* pRole )
{
	//--碰撞检测
	Vector3 nowPos;
	float endTime;

	ENavResult result=m_collider.Update(pNav->GetCollider(),m_lastCDTime,Kernel::Inst()->GetAccumTime()-m_lastCDTime,nowPos,endTime,false);
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
		m_pFSM->Change2LowerState_Float();
}

void LowerStateSwim_RP::OnNetCmd( tagNetCmd* pNetCmd )
{
	m_pFSM->Change2LowerState(pNetCmd);
}

void LowerStateSwim_RP::OnGameEvent( tagGameEvent* pEvent )
{}

void LowerStateSwim_RP::PlaySprayEffect( NavMap* pNav,Player* pRole )
{
	float waterH;
	if(pNav->GetCollider()->GetWaterHeight(pRole->GetPos(),waterH)
		&& pRole->GetPos().y<waterH)
	{
		Vector3 effectPos=pRole->GetPos();
		effectPos.y=waterH;
		EffectMgr::Inst()->PlayEffect(effectPos,pRole->GetDestYaw(),_T("txqt06"));
	}
}

bool LowerStateSwim_RP::IfCanChange2UpperState( FSM_RP::EUpperState state )
{
	switch(state)
	{
	case FSM_RP::EUpperState_BeAttack:
		return false;
	}

	return true;
}