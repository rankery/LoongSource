#include "StdAfx.h"
#include "LowerStateHitFly_RP.h"
#include "CombatEvent.h"
#include "Player.h"
#include "FSM_RP.h"
#include "CombatSysUtil.h"
#include "WorldFrame.h"
#include "..\WorldDefine\action.h"


LowerStateHitFly_RP::LowerStateHitFly_RP(void)
{}

LowerStateHitFly_RP::~LowerStateHitFly_RP(void)
{}

void LowerStateHitFly_RP::Active(tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole )
{
	ASSERT(pNetCmd->dwID==pNetCmd->Crc32("NS_HitFly"));
	tagNS_HitFly* pFly=(tagNS_HitFly*)pNetCmd;

	m_collider.Init(pFly->curPos,-1.0f*pFly->faceTo,pRole->GetRoleSize(),pRole->GetHitFlyXzSpeed(),pRole->GetHitFlyYSpeed());

	//--设置朝向
	pRole->SetYaw(CalcYaw(pFly->faceTo));

	//--播放动作
	pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Other,
		AvatarAniMixer::EAP_All,
		_T("sfly"),
		false);

	m_step=EStep_Fly;
}

void LowerStateHitFly_RP::Update(NavMap* pNav,Player* pRole)
{
	if(m_step==EStep_Finish)
		return;

	//--击飞中-------------------------------------------------------------------
	if(m_step==EStep_Fly)
	{
		//--碰撞检测
		float deltaTime=Kernel::Inst()->GetDeltaTime();
		float startTime=Kernel::Inst()->GetAccumTime()-deltaTime;

		Vector3 nowPos;
		float endTime;
		DWORD dwCarrierObjID;
		ENavResult result=m_collider.Update(pNav->GetCollider(),startTime,deltaTime,NULL,nowPos,endTime,dwCarrierObjID,pRole->CanWaterWalk());

		pRole->SetPos(nowPos);

		//--
		if(result!=ENR_ToBeContinued)
		{
			if(result==ENR_Landed)
			{
				m_step=EStep_Craw;

				pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Other,
					AvatarAniMixer::EAP_All,
					_T("craw"),
					false);
			}
			else
			{
				m_step=EStep_Finish;
				return;
			}
		}
	}
	//--爬起中-------------------------------------------------------------------
	if(m_step==EStep_Craw)
	{
		if(pRole->GetAniPlayer()->IsTrackEnd(AvatarAniPlayer::EAT_Other,_T("craw")))
		{
			m_step=EStep_Finish;
		}
	}
}

void LowerStateHitFly_RP::OnGameEvent( tagGameEvent* pEvent )
{}

void LowerStateHitFly_RP::OnNetCmd( tagNetCmd* pNetCmd )
{
	m_pFSM->Change2LowerState(pNetCmd);
}

bool LowerStateHitFly_RP::IfCanChange2UpperState( FSM_RP::EUpperState state )
{
	switch(state)
	{
	case FSM_RP::EUpperState_BeAttack:
		return false;
	}

	return true;
}