#include "StdAfx.h"
#include "ActionKeyboardMove_LP.h"
#include "ClientCamera.h"
#include "LocalPlayer.h"
#include "WorldFrame.h"
#include "CombatSysUtil.h"
#include "..\WorldDefine\action.h"
#include "EffectMgr.h"
#include "FSM_LP.h"
#include "Pet.h"

ActionKeyboardMove_LP::ActionKeyboardMove_LP(void)
: m_bClosed(true)
, m_result(ER_Unknow)
{
	m_lastPlayeEffectTime=Kernel::Inst()->GetAccumTime();
}

ActionKeyboardMove_LP::~ActionKeyboardMove_LP(void)
{}

void ActionKeyboardMove_LP::Active( const Vector3& start,EWalkDir dir,NavMap* pNav,LocalPlayer* pRole,bool bCanPlayAni,bool bCanSetYaw )
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
	m_collider.Init(start,end,pRole->GetRoleSize(),pRole->GetMoveSpeed());

	//--发送网络消息
	SendCmd(start,end);

	//--播放动作
	if(bCanPlayAni)
	{
		const TCHAR* szTrackName=CombatSysUtil::Inst()->GetMoveTrackName(pRole->GetRWeaponType(),pRole->IsReadyState(),pRole->GetMoveSpeed());
		pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Move,AvatarAniMixer::EAP_Lower,szTrackName,true);
	}

	//--设置朝向
	float yaw=CalcYaw(m_moveVec);
	pRole->SetMoveYaw(yaw);
	if(bCanSetYaw)
		pRole->SetYaw(yaw);

	//--播放水花特效
	PlaySprayEffect(pNav,pRole);
}

void ActionKeyboardMove_LP::Update( NavMap* pNav,LocalPlayer* pRole,bool bCanPlayAni,bool bCanSetYaw )
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
	DWORD dwCarrierObjID;
	ENavResult result=m_collider.Update(pNav->GetCollider(),startTime,deltaTime,nowPos,endTime,dwCarrierObjID,pRole->CanWaterWalk());
	pRole->SetPos(nowPos);	

	//--播放水花特效
	PlaySprayEffect(pNav,pRole);

	//--播放脚步音效
	const DWORD dwFootMsg = _MakeFourCC(_T("foot"));
	Pet* pMount = pRole->GetMount();
	if( NULL == pMount )
	{
		const vector<DWORD>& msgCodes = pRole->GetMsgCodes();
		for( size_t nMsgCode = 0; nMsgCode < msgCodes.size(); nMsgCode++ )
		{
			if( dwFootMsg == msgCodes[nMsgCode] )
				CombatSysUtil::Inst()->PlayFootSound(nowPos);
		}
	}
	else
	{
		const vector<DWORD>& msgCodes = pMount->GetMsgCodes();
		for( size_t nMsgCode = 0; nMsgCode < msgCodes.size(); nMsgCode++ )
		{
			if( dwFootMsg == msgCodes[nMsgCode] )
				CombatSysUtil::Inst()->PlayNPCFootSound(pMount);
		}
	}


	//--
	if(result!=ENR_ToBeContinued)
	{
		IMSG(_T("keymove=%d\n"),result);
		if(result==ENR_WillDrop)
		{
			m_bClosed=true;
			m_result=ER_Drop;
		}
		else if(result==ENR_WillSlide)
		{
			m_bClosed=true;
			m_result=ER_Slide;
		}
		else if(result==ENR_WillSwim)
		{
			m_bClosed=true;
			m_result=ER_Swim;
		}
		else if(result==ENR_WillOnWater)
		{
			m_bClosed=true;
			m_result=ER_OnWater;
		}
		else if(result==ENR_Blocking)
		{
			m_bBlocking=true;
		}
		else//arrive
		{
			Active(nowPos,m_moveDir,pNav,pRole,bCanPlayAni,bCanSetYaw);
		}
	}
}

void ActionKeyboardMove_LP::Stop(NavMap* pNav,LocalPlayer* pRole)
{
	if(m_bClosed)
		return;

	m_bClosed=true;
	m_result=ER_Stand;

	if(!m_bBlocking)
	{
		//--发送停止消息
		tagNC_StopWalk cmd;
		cmd.curPos=pRole->GetPos();
		cmd.bSwim=false;
		TObjRef<NetSession>()->Send(&cmd);
	}
}

Vector3 ActionKeyboardMove_LP::CalcMoveEnd( EWalkDir dir,float cameraYaw,const Vector3& start )
{
	Vector3 moveDir=CombatSysUtil::Inst()->CalcKeyMoveDir(dir,cameraYaw);
	Vector3 moveDest=start+moveDir*1000.0f;
	return moveDest;
}

void ActionKeyboardMove_LP::SendCmd( const Vector3& srcPos,const Vector3& dstPos )
{
	tagNC_KeyboardWalk cmd;
	cmd.srcPos=srcPos;
	cmd.dstPos=dstPos;
	cmd.bSwim=false;
	TObjRef<NetSession>()->Send(&cmd);
}

void ActionKeyboardMove_LP::ReplayAni(LocalPlayer* pRole)
{
	if(m_bClosed)
		return;

	const TCHAR* szTrackName=CombatSysUtil::Inst()->GetMoveTrackName(pRole->GetRWeaponType(),pRole->IsReadyState(),pRole->GetMoveSpeed());
	pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Move,AvatarAniMixer::EAP_Lower,szTrackName,true);
}

void ActionKeyboardMove_LP::PlaySprayEffect( NavMap* pNav,LocalPlayer* pRole )
{
	if(Kernel::Inst()->GetAccumTime()-m_lastPlayeEffectTime>0.1f)
	{
		m_lastPlayeEffectTime=Kernel::Inst()->GetAccumTime();

		float waterH;
		if(pNav->GetCollider()->GetWaterHeight(pRole->GetPos(),waterH)
			&& pRole->GetPos().y<waterH)
		{
			Vector3 effectPos=pRole->GetPos();
			effectPos.y=waterH;
			EffectMgr::Inst()->PlayEffect(effectPos,pRole->GetDestYaw(),_T("txqt06"));
		}	
	}
}
