#include "StdAfx.h"
#include "LowerStateJump_RP.h"
#include "Player.h"
#include "FSM_RP.h"
#include "CombatSysUtil.h"
#include "..\WorldDefine\action.h"
#include "EffectMgr.h"


LowerStateJump_RP::LowerStateJump_RP(void)
{}

LowerStateJump_RP::~LowerStateJump_RP(void)
{}

void LowerStateJump_RP::Active( tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole )
{
    // 关掉灵兽俯仰
    pRole->SwitchMountPitch(false);

	ASSERT(pNetCmd->dwID==pNetCmd->Crc32("NS_SyncJump"));
	tagNS_SyncJump* pCmd=(tagNS_SyncJump*)pNetCmd;

	//--初始化碰撞检测器
	m_collider.Init(pCmd->srcPos,pCmd->dir,pRole->GetRoleSize(),pCmd->fXZSpeed,pCmd->fYSpeed);
	m_lastCDTime=Kernel::Inst()->GetAccumTime()-pCmd->curTime;

	//--播放动作
	pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_StartJump,AvatarAniMixer::EAP_Lower,_T("jmp1"),false);

	//--设置朝向
	if(pCmd->dir.x!=0.0f && pCmd->dir.z!=0.0f)
	{
		float yaw=CalcYaw(pCmd->dir);
		pRole->SetMoveYaw(yaw);
		pRole->SetYaw(yaw);
	}

	//--播放出水特效 
	float waterH;
	if(pNav->GetCollider()->GetWaterHeight(pRole->GetPos(),waterH)
		&& pRole->GetPos().y<=waterH)
	{

		Vector3 effectPos=pRole->GetPos();
		effectPos.y=waterH;
		EffectMgr::Inst()->PlayEffect(effectPos,pRole->GetDestYaw(),_T("txqt04"));
	}

	//--入水特效相关
	m_bPlayInEffect=false;

	m_step=EStep_StartJump;
}

void LowerStateJump_RP::Update(NavMap* pNav,Player* pRole)
{
	//--碰撞检测
	Vector3 nowPos;
	float endTime;
	DWORD dwCarrierObjID;
	float cdCarrierTime;

	ENavResult result=m_collider.Update(pNav->GetCollider(),m_lastCDTime,Kernel::Inst()->GetAccumTime()-m_lastCDTime,NULL,nowPos,endTime,dwCarrierObjID,cdCarrierTime,pRole->CanWaterWalk());
	m_lastCDTime=Kernel::Inst()->GetAccumTime();
	
	pRole->SetPos(nowPos);

	//--起跳------------------------------------------------------------------------------
	if(m_step==EStep_StartJump)
	{
		//--
		if(pRole->GetAniPlayer()->IsTrackEnd(AvatarAniPlayer::EAT_StartJump,_T("jmp1"))
			||result!=ENR_ToBeContinued)
		{
			pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Jumpping,AvatarAniMixer::EAP_Lower,_T("jmp2"),true);

			m_step=EStep_Jumpping;
		}
	}
	//--跳跃中------------------------------------------------------------------------------
	if(m_step==EStep_Jumpping)
	{
		//--播放入水特效
		if(!m_bPlayInEffect)
		{
			float waterH;
			if(pNav->GetCollider()->GetWaterHeight(nowPos,waterH)
				&& nowPos.y<=waterH)
			{
				m_bPlayInEffect=true;

				Vector3 effectPos=nowPos;
				effectPos.y=waterH;
				EffectMgr::Inst()->PlayEffect(effectPos,pRole->GetDestYaw(),_T("txqt04"));
			}
		}

		if(result!=ENR_ToBeContinued)
		{
			if(result==ENR_Landed)//判断是否播放落地动作
			{
				//--播放落地动作
				pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Land,AvatarAniMixer::EAP_Lower,_T("jmp3"),false);

				m_step=EStep_Land;
			}
			else
			{
                // 打开灵兽俯仰
                pRole->SwitchMountPitch(true);

				m_pFSM->Change2LowerState_Stand();

				return;
			}
		}
	}
	//--落地---------------------------------------------------------------------------------
	if(m_step==EStep_Land)
	{
		if( pRole->GetAniPlayer()->IsTrackEnd( AvatarAniPlayer::EAT_Land, _T("jmp3") ) )
		{
            // 打开灵兽俯仰
            pRole->SwitchMountPitch(true);

			m_pFSM->Change2LowerState_Stand();
		}
	}
}

void LowerStateJump_RP::OnNetCmd( tagNetCmd* pNetCmd )
{
	m_pFSM->Change2LowerState(pNetCmd);
}

bool LowerStateJump_RP::IfCanChange2UpperState( FSM_RP::EUpperState state )
{
	switch(state)
	{
	case FSM_RP::EUpperState_BeAttack:
		return false;
	}

	return true;
}