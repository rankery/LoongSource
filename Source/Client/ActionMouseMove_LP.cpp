#include "StdAfx.h"
#include "ActionMouseMove_LP.h"
#include "LocalPlayer.h"
#include "FSM_LP.h"
#include "..\WorldDefine\action.h"
#include "EffectMgr.h"
#include "CombatSysUtil.h"
#include "Pet.h"

ActionMouseMove_LP::ActionMouseMove_LP(void)
: m_bClosed(true)
, m_result(ER_Unknow)
{
	m_lastPlayeEffectTime=Kernel::Inst()->GetAccumTime();
}

ActionMouseMove_LP::~ActionMouseMove_LP(void)
{}

void ActionMouseMove_LP::Active( const Vector3& start,const Vector3& end,float validDist,NavMap* pNav,LocalPlayer* pRole,bool bCanPlayAni,bool bCanSetYaw,bool bNeedSendStopCmd )
{
	m_bClosed=false;
	m_result=ER_Unknow;
	m_validDistSq=validDist*validDist;
	m_moveEnd=end;
	m_bNeedSendStopCmd=bNeedSendStopCmd&&validDist>0.0f;

	//--判断是否进入有效距离
	if(Vec3DistSq(pRole->GetPos(),m_moveEnd)<=m_validDistSq)
	{
		m_bClosed=true;
		m_result=ER_Arrive;
		return;
	}

	//--尝试看是否可以直线到达
	if(m_collider.IfCanGo(pNav->GetCollider(),start,end,pRole->GetRoleSize(),NULL,100)==ENR_Arrive)
	{
		//--
		m_moveMode=EMM_DirectGo;
		//--
		m_collider.Init(start,end,pRole->GetRoleSize(),pRole->GetMoveSpeed());
		//--
		pRole->SetMoveYaw(CalcYaw(end-start));
		//--
		SendCmd(start,end);
	}
	//--尝试使用寻路
	else if(m_pathFinder.Init(start,end,pNav,pRole,true))
	{
		m_moveMode=EMM_FindPath;

		Vector3 point;
		if(m_pathFinder.GetPathPoint(pNav,pRole,point))
		{
			//--
			m_collider.Init(start,point,pRole->GetRoleSize(),pRole->GetMoveSpeed());
			//--
			pRole->SetMoveYaw(CalcYaw(point-start));
			//--
			SendCmd(start,point);
		}
		else
		{
			m_bClosed=true;
			m_result=ER_CannotArrive;
			return;
		}
	}
	//--直线行走
	else
	{
		//--
		m_moveMode=EMM_DirectGo;
		//--
		m_collider.Init(start,end,pRole->GetRoleSize(),pRole->GetMoveSpeed());
		//--
		pRole->SetMoveYaw(CalcYaw(end-start));
		//--
		SendCmd(start,end);
	}

	//--播放动作
	if(bCanPlayAni)
	{
		const TCHAR* szTrackName=CombatSysUtil::Inst()->GetMoveTrackName(pRole->GetRWeaponType(),pRole->IsReadyState(),pRole->GetMoveSpeed());
		pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Move,AvatarAniMixer::EAP_Lower,szTrackName,true);
	}

	//--设置朝向
	if(bCanSetYaw)
	{
		pRole->SetYaw(pRole->GetMoveYaw());
	}

	//--播放水花特效
	PlaySprayEffect(pNav,pRole);
}

void ActionMouseMove_LP::Update( NavMap* pNav,LocalPlayer* pRole,bool bCanPlayAni,bool bCanSetYaw )
{
	if(m_bClosed)
		return;

	//--移动碰撞检测
	float deltaTime=Kernel::Inst()->GetDeltaTime();
	float startTime=Kernel::Inst()->GetAccumTime()-deltaTime;

	Vector3 nowPos;
	float endTime;
	DWORD dwCarrierObjID;
	ENavResult result=m_collider.Update(pNav->GetCollider(),startTime,deltaTime,nowPos,endTime,dwCarrierObjID,pRole->CanWaterWalk());
	pRole->SetPos(nowPos);

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

	//--播放水花特效
	PlaySprayEffect(pNav,pRole);

	//--判断是否进入有效距离
	if(Vec3DistSq(pRole->GetPos(),m_moveEnd)<=m_validDistSq)
	{
		if(m_bNeedSendStopCmd)
		{
			tagNC_StopWalk cmd;
			cmd.curPos=pRole->GetPos();
			cmd.bSwim=false;
			TObjRef<NetSession>()->Send(&cmd);
		}
		m_bClosed=true;
		m_result=ER_Arrive;
		return;
	}

	//--直线行走模式
	if(m_moveMode==EMM_DirectGo)
	{
		if(result!=ENR_ToBeContinued)
		{
			m_bClosed=true;
			if(result==ENR_Arrive)
				m_result=ER_Arrive;
			else if(result==ENR_WillDrop)
				m_result=ER_Drop;
			else if(result==ENR_WillSlide)
				m_result=ER_Slide;
			else if(result==ENR_WillSwim)
				m_result=ER_Swim;
			else if(result==ENR_WillOnWater)
				m_result=ER_OnWater;
			else
				m_result=ER_CannotArrive;
		}
	}
	//--寻路模式
	else
	{
		if(result==ENR_Arrive)
		{
			Vector3 point;
			if(m_pathFinder.GetPathPoint(pNav,pRole,point))
			{
				//--
				m_collider.Init(pRole->GetPos(),point,pRole->GetRoleSize(),pRole->GetMoveSpeed());

				//--
				SendCmd(pRole->GetPos(),point);

				//--播放动作
				if(bCanPlayAni)
				{
					const TCHAR* szTrackName=CombatSysUtil::Inst()->GetMoveTrackName(pRole->GetRWeaponType(),pRole->IsReadyState(),pRole->GetMoveSpeed());
					pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Move,AvatarAniMixer::EAP_Lower,szTrackName,true);
				}

				//--设置朝向
				pRole->SetMoveYaw(CalcYaw(point-pRole->GetPos()));
				if(bCanSetYaw)
				{
					pRole->SetYaw(pRole->GetMoveYaw());
				}
			}
			else
			{
				m_bClosed=true;
				m_result=ER_CannotArrive;
				return;
			}
		}
		else if(result!=ENR_ToBeContinued)
		{
			m_bClosed=true;
			if(result==ENR_WillDrop)
				m_result=ER_Drop;
			else if(result==ENR_WillSlide)
				m_result=ER_Slide;
			else if(result==ENR_WillSwim)
				m_result=ER_Swim;
			else if(result==ENR_WillOnWater)
				m_result=ER_OnWater;
			else
				m_result=ER_CannotArrive;
		}
	}
}

void ActionMouseMove_LP::Stop(LocalPlayer* pRole)
{
	if(m_bClosed)
		return;

	m_bClosed=true;

	tagNC_StopWalk cmd;
	cmd.curPos = pRole->GetPos();
	cmd.bSwim=false;
	TObjRef<NetSession>()->Send(&cmd);
}

void ActionMouseMove_LP::ReplayAni( LocalPlayer* pRole )
{
	if(m_bClosed)
		return;

	const TCHAR* szTrackName=CombatSysUtil::Inst()->GetMoveTrackName(pRole->GetRWeaponType(),pRole->IsReadyState(),pRole->GetMoveSpeed());
	pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Move,AvatarAniMixer::EAP_Lower,szTrackName,true);
}

void ActionMouseMove_LP::PlaySprayEffect( NavMap* pNav,LocalPlayer* pRole )
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

void ActionMouseMove_LP::SendCmd( const Vector3& start,const Vector3& end )
{
	tagNC_MouseWalk cmd;
	cmd.srcPos=start;
	cmd.dstPos=end;
	cmd.bSwim=false;
	TObjRef<NetSession>()->Send(&cmd);
}