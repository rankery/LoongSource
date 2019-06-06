#include "StdAfx.h"
#include "ActionKeyboardSwim_LP.h"
#include "ClientCamera.h"
#include "LocalPlayer.h"
#include "WorldFrame.h"
#include "CombatSysUtil.h"
#include "..\WorldDefine\action.h"
#include "FSM_LP.h"
#include "EffectMgr.h"
#include "AudioSys.h"

ActionKeyboardSwim_LP::ActionKeyboardSwim_LP(void)
: m_bClosed(true)
, m_result(ER_Unknow)
, m_dwSoundID(GT_INVALID)
{
	m_lastPlayeEffectTime=Kernel::Inst()->GetAccumTime();
}

ActionKeyboardSwim_LP::~ActionKeyboardSwim_LP(void)
{}

void ActionKeyboardSwim_LP::Active( const Vector3& start,EWalkDir dir,NavMap* pNav,LocalPlayer* pRole,bool bPlaySound )
{
	m_bClosed=false;
	m_result=ER_Unknow;
	m_bBlocking=false;

	//--计算移动终点
	ClientCamera* pCamera=GetWorldFrm()->GetCamera();
	Vector3 end=CalcMoveEnd(dir,pCamera->GetYaw(),start);

	//--保存方向
	m_moveDir		= dir;
	m_moveVec		= end-start;	
	Vec3Normalize(m_moveVec);

	//--初始化碰撞检测器
	m_collider.Init(start,end,pRole->GetRoleSize(),pRole->GetSwimSpeed());

	//--发送网络消息
	SendCmd(start,end);

	//--播放动作
	pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Move,AvatarAniMixer::EAP_Lower,_T("swm2"),true);

	//--设置朝向
	pRole->SetYaw(CalcYaw(m_moveVec));

	//--播放水花特效
	if(Kernel::Inst()->GetAccumTime()-m_lastPlayeEffectTime>0.1f)
	{
		m_lastPlayeEffectTime=Kernel::Inst()->GetAccumTime();
		PlaySprayEffect(pNav,pRole);
	}

	//--播放音效
	if(bPlaySound)
	{
		PlaySound(pRole);
	}
}

void ActionKeyboardSwim_LP::Update( NavMap* pNav,LocalPlayer* pRole )
{
	if(m_bClosed)
		return;

	if(m_bBlocking)
		return;

	//--碰撞检测
	float deltaTime=Kernel::Inst()->GetDeltaTime();
	float startTime=Kernel::Inst()->GetAccumTime()-deltaTime;

	Vector3 nowPos;
	float endTime;
	ENavResult result=m_collider.Update(pNav->GetCollider(),startTime,deltaTime,nowPos,endTime,false);

	pRole->SetPos(nowPos);	

	//--播放水花特效
	if(Kernel::Inst()->GetAccumTime()-m_lastPlayeEffectTime>0.1f)
	{
		m_lastPlayeEffectTime=Kernel::Inst()->GetAccumTime();
		PlaySprayEffect(pNav,pRole);
	}

	//--更新音效位置
	UpdateSound(nowPos);

	//--
	if(result!=ENR_ToBeContinued)
	{
		if(result==ENR_SwimLanded)
		{
			m_bClosed=true;
			m_result=ER_Ground;
		}
		else if(result==ENR_Blocking)
		{
			m_bBlocking=true;
		}
		else//arrive
		{
			Active(nowPos,m_moveDir,pNav,pRole,false);
		}
	}
}

void ActionKeyboardSwim_LP::Stop(NavMap* pNav,LocalPlayer* pRole)
{
	if(m_bClosed)
		return;

	m_bClosed=true;
	m_result=ER_Swim;

	if(!m_bBlocking)
	{
		//--发送停止消息
		tagNC_StopWalk cmd;
		cmd.curPos=pRole->GetPos();
		cmd.bSwim=true;
		TObjRef<NetSession>()->Send(&cmd);
	}
}

Vector3 ActionKeyboardSwim_LP::CalcMoveEnd( EWalkDir dir,float cameraYaw,const Vector3& start )
{
	Vector3 moveDir=CombatSysUtil::Inst()->CalcKeyMoveDir(dir,cameraYaw);
	Vector3 moveDest=start+moveDir*1000.0f;
	return moveDest;
}

void ActionKeyboardSwim_LP::SendCmd( const Vector3& srcPos,const Vector3& dstPos )
{
	tagNC_KeyboardWalk cmd;
	cmd.srcPos=srcPos;
	cmd.dstPos=dstPos;
	cmd.bSwim=true;
	TObjRef<NetSession>()->Send(&cmd);
}

void ActionKeyboardSwim_LP::PlaySprayEffect( NavMap* pNav,LocalPlayer* pRole )
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

void ActionKeyboardSwim_LP::PlaySound( LocalPlayer* pRole )
{
	m_dwSoundID=TObjRef<AudioSys>()->Create3DSound(_T("swim"),true,100.0f,100.0f*50.0f,SOUND_WISPER);
	if(GT_INVALID!=m_dwSoundID)
	{
		TObjRef<AudioSys>()->Set3DSoundAtt(m_dwSoundID,pRole->GetPos(),MathConst::Zero3);
		TObjRef<AudioSys>()->Play3DSound(m_dwSoundID);
	}
}

void ActionKeyboardSwim_LP::StopSound()
{
	if(GT_INVALID!=m_dwSoundID)
	{
		TObjRef<AudioSys>()->Stop3DSound(m_dwSoundID);
		TObjRef<AudioSys>()->Destroy3DSound(m_dwSoundID);
		m_dwSoundID=GT_INVALID;
	}
}

void ActionKeyboardSwim_LP::UpdateSound( const Vector3& rolePos )
{
	if(GT_INVALID!=m_dwSoundID)
	{
		TObjRef<AudioSys>()->Set3DSoundAtt(m_dwSoundID,rolePos,MathConst::Zero3);
	}
}

void ActionKeyboardSwim_LP::ReplayAni( LocalPlayer* pRole )
{
	pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Move,AvatarAniMixer::EAP_Lower,_T("swm2"),true);
}