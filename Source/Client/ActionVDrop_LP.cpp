#include "StdAfx.h"
#include "ActionVDrop_LP.h"
#include "LocalPlayer.h"
#include "..\WorldDefine\action.h"
#include "EffectMgr.h"
#include "CombatSysUtil.h"
#include "FSM_LP.h"
#include "Pet.h"

ActionVDrop_LP::ActionVDrop_LP(void)
: m_step(EStep_Closed)
, m_result(ER_Unknow)
{}

ActionVDrop_LP::~ActionVDrop_LP(void)
{}

void ActionVDrop_LP::Active( const Vector3& start,NavMap* pNav,LocalPlayer* pRole )
{
	m_step=EStep_Drop;
	m_result=ER_Unknow;

	m_collider.Init(start,MathConst::Zero3,pRole->GetRoleSize(),0.0f,0.0f);

	//--发送网络消息
	tagNC_VDrop cmd;
	cmd.srcPos=start;
	TObjRef<NetSession>()->Send(&cmd);

	//--播放动作
	pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_VDrop,AvatarAniMixer::EAP_Lower,_T("jmp2"),true);

	//--特效相关
	m_bPlayInEffect=false;
}

void ActionVDrop_LP::Update( bool bPlayLandAni,NavMap* pNav,LocalPlayer* pRole )
{
	if(m_step==EStep_Closed)
		return;

	//--掉落状态--------------------------------------------------------------------------------
	if(m_step==EStep_Drop)
	{
		//--碰撞检测
		float deltaTime=Kernel::Inst()->GetDeltaTime();
		float startTime=Kernel::Inst()->GetAccumTime()-deltaTime;

		Vector3 nowPos;
		float endTime;
		DWORD dwCarrierObjID;
		float cdCarrierTime;
		ENavResult result=m_collider.Update(pNav->GetCollider(),startTime,deltaTime,NULL,nowPos,endTime,dwCarrierObjID,cdCarrierTime,pRole->CanWaterWalk());

		pRole->SetPos(nowPos);

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
			if(result==ENR_Landed
				&&bPlayLandAni)//判断是否播放落地动作
			{
				//--播放脚步音效
				Pet* pMount = pRole->GetMount();
				if( NULL == pMount )
					CombatSysUtil::Inst()->PlayFootSound(nowPos);
				else
					CombatSysUtil::Inst()->PlayNPCFootSound(pMount);

				//播放落地动作
				pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Land,AvatarAniMixer::EAP_Lower,_T("jmp3"),false);

				m_step=EStep_Land;
			}
			else
			{
				//--播放脚步音效
				Pet* pMount = pRole->GetMount();
				if( NULL == pMount )
					CombatSysUtil::Inst()->PlayFootSound(nowPos);
				else
					CombatSysUtil::Inst()->PlayNPCFootSound(pMount);

				m_step=EStep_Closed;

				if(result==ENR_WillSlide)
					m_result=ER_Slide;
				else if(result==ENR_WillSwim)
					m_result=ER_Swim;
				else if(result==ENR_WillOnWater)
					m_result=ER_OnWater;
				else
					m_result=ER_Land;
				return;
			}
		}

	}

	//--落地状态----------------------------------------------------------------------------------
	if(m_step==EStep_Land)
	{
		if( pRole->GetAniPlayer()->IsTrackEnd( AvatarAniPlayer::EAT_Land, _T("jmp3") ) )
		{
			m_step=EStep_Closed;			
			m_result=ER_Land;
			return;
		}
	}
}

void ActionVDrop_LP::ReplayAni( LocalPlayer* pRole )
{
	if( m_step==EStep_Closed )
		return;

	if(m_step==EStep_Drop)
	{
		pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_VDrop,AvatarAniMixer::EAP_Lower,_T("jmp2"),true);
	}
	else if(m_step==EStep_Land)
	{
		pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Land,AvatarAniMixer::EAP_Lower,_T("jmp3"),false);
	}
}