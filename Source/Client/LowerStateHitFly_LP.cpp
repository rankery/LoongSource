#include "StdAfx.h"
#include "LowerStateHitFly_LP.h"
#include "CombatEvent.h"
#include "LocalPlayer.h"
#include "FSM_LP.h"
#include "CombatSysUtil.h"
#include "WorldFrame.h"
#include "..\WorldDefine\action.h"


LowerStateHitFly_LP::LowerStateHitFly_LP(void)
{}

LowerStateHitFly_LP::~LowerStateHitFly_LP(void)
{}

void LowerStateHitFly_LP::Active( tagNetCmd* pNetCmd,NavMap* pNav,LocalPlayer* pRole,tagPreState* pPreState )
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

void LowerStateHitFly_LP::Update(NavMap* pNav,LocalPlayer* pRole)
{
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

		//--播放摔倒音效
		const DWORD dwFallMsg = _MakeFourCC(_T("fall"));
		const vector<DWORD>& msgCodes = pRole->GetMsgCodes();
		for( size_t nMsgCode = 0; nMsgCode < msgCodes.size(); nMsgCode++ )
		{
			if( dwFallMsg == msgCodes[nMsgCode] )
			{
				CombatSysUtil::Inst()->PlayFallSound(nowPos);
			}
		}

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
				m_pFSM->Change2LowerState_Float();
				return;
			}
		}
	}
	//--爬起中-------------------------------------------------------------------
	if(m_step==EStep_Craw)
	{
		if(pRole->GetAniPlayer()->IsTrackEnd(AvatarAniPlayer::EAT_Other,_T("craw")))
		{
			m_pFSM->Change2LowerState_Stand();
		}
	}
}

void LowerStateHitFly_LP::OnGameEvent( tagGameEvent* pEvent )
{}

bool LowerStateHitFly_LP::IfCanChange2UpperState( FSM_LP::EUpperState state,tagGameEvent* pEvent )
{
	return false;
}

bool LowerStateHitFly_LP::IfCanChange2LowerState( FSM_LP::ELowerState state,tagGameEvent* pEvent )
{
	return false;
}