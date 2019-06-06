#include "StdAfx.h"
#include "ActionJump_LP.h"
#include "LocalPlayer.h"
#include "..\WorldDefine\action.h"
#include "EffectMgr.h"
#include "CombatSysUtil.h"
#include "Pet.h"

ActionJump_LP::ActionJump_LP(void)
: m_step(EStep_Closed)
, m_result(ER_Unknow)
{}

ActionJump_LP::~ActionJump_LP(void)
{}

void ActionJump_LP::Active( const Vector3& start,const Vector3& dir,bool bOrginJump,NavMap* pNav,LocalPlayer* pRole )
{
	m_result=ER_Unknow;
	m_step=EStep_StartJump;

	//--初始化碰撞检测器
	Vector3 xzDir = bOrginJump ?  MathConst::Zero3 : dir;
	float xzSpeed;
	//if(pNav->GetCollider()->IfWillSwim(start,pRole->GetRoleSize().y))
	//	xzSpeed = pRole->GetSwimSpeed();
	//else
	xzSpeed = pRole->GetMoveSpeed();

	m_collider.Init(start,xzDir,pRole->GetRoleSize(),xzSpeed,pRole->GetJumpYSpeed());

	//--发送网络消息
	tagNC_Jump cmd;
	cmd.srcPos=start;
	cmd.dir=xzDir;
	TObjRef<NetSession>()->Send(&cmd);

	//--设置朝向
	if(!bOrginJump)
		pRole->SetYaw(CalcYaw(xzDir));

	//--播放动作
	pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_StartJump,AvatarAniMixer::EAP_Lower,_T("jmp1"),false);

	//--播放出水特效 
	float waterH;
	if(pNav->GetCollider()->GetWaterHeight(pRole->GetPos(),waterH)
		&& pRole->GetPos().y<=waterH)
	{
		Vector3 effectPos=pRole->GetPos();
		effectPos.y=waterH;
		EffectMgr::Inst()->PlayEffect(effectPos,pRole->GetDestYaw(),_T("txqt04"));
	}

	//--初始化入水特效标志
	m_bPlayInEffect=false;
}

void ActionJump_LP::Update( bool bPlayLandAni, NavMap* pNav,LocalPlayer* pRole )
{
	if(m_step==EStep_Closed)
		return;

	//--碰撞检测
	float deltaTime=Kernel::Inst()->GetDeltaTime();
	float startTime=Kernel::Inst()->GetAccumTime()-deltaTime;

	Vector3 nowPos;
	float endTime;
	DWORD dwCarrierObjID;
	float cdCarrierTime;
	ENavResult result=m_collider.Update(pNav->GetCollider(),startTime,deltaTime,NULL,nowPos,endTime,dwCarrierObjID,cdCarrierTime,pRole->CanWaterWalk());

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
			if(result==ENR_Landed
				&&bPlayLandAni)//判断是否播放落地动作
			{
				//--播放脚步音效
				Pet* pMount = pRole->GetMount();
				if( NULL == pMount )
					CombatSysUtil::Inst()->PlayFootSound(nowPos);
				else
					CombatSysUtil::Inst()->PlayNPCFootSound(pMount);

				//--播放落地动作
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

				if(result==ENR_WillVDrop)
					m_result=ER_VDrop;
				else if(result==ENR_WillSlide)
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
	//--落地---------------------------------------------------------------------------------
	if(m_step==EStep_Land)
	{
		if( pRole->GetAniPlayer()->IsTrackEnd( AvatarAniPlayer::EAT_Land, _T("jmp3") ) )
		{
			m_step=EStep_Closed;			
			m_result=ER_Land;
		}
	}
}