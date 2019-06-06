#include "StdAfx.h"
#include "ActionMouseWaterWalk_LP.h"
#include "LocalPlayer.h"
#include "FSM_LP.h"
#include "..\WorldDefine\action.h"
#include "EffectMgr.h"
#include "CombatSysUtil.h"
#include "AudioSys.h"

ActionMouseWaterWalk_LP::ActionMouseWaterWalk_LP(void)
: m_bClosed(true)
, m_result(ER_Unknow)
, m_nFoot(0)
{
}

ActionMouseWaterWalk_LP::~ActionMouseWaterWalk_LP(void)
{}

void ActionMouseWaterWalk_LP::Active( const Vector3& start,const Vector3& end,float validDist,NavMap* pNav,LocalPlayer* pRole,bool bNeedSendStopCmd )
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
		m_collider.Init(start,end,pRole->GetRoleSize(),pRole->GetSwimSpeed());
		//--
		pRole->SetMoveYaw(CalcYaw(end-start));
		//--
		SendCmd(start,end);
	}
	//--尝试使用寻路
	else if(m_pathFinder.Init(start,end,pNav,pRole,false))
	{
		//--
		m_moveMode=EMM_FindPath;

		Vector3 point;
		if(m_pathFinder.GetPathPoint(pNav,pRole,point))
		{
			//--
			m_collider.Init(start,point,pRole->GetRoleSize(),pRole->GetSwimSpeed());
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
		m_collider.Init(start,end,pRole->GetRoleSize(),pRole->GetSwimSpeed());
		//--
		pRole->SetMoveYaw(CalcYaw(end-start));
		//--
		SendCmd(start,end);
	}

	//--播放动作
	const TCHAR* szTrackName=CombatSysUtil::Inst()->GetMoveTrackName(pRole->GetRWeaponType(),pRole->IsReadyState(),pRole->GetSwimSpeed());
	pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Other,AvatarAniMixer::EAP_Lower,szTrackName,true);

	//--设置朝向
	pRole->SetYaw(pRole->GetMoveYaw());
}

void ActionMouseWaterWalk_LP::Update( NavMap* pNav,LocalPlayer* pRole )
{
	if(m_bClosed)
		return;

	//--碰撞检测
	Vector3 nowPos;
	float endTime;
	float deltaTime=Kernel::Inst()->GetDeltaTime();
	float startTime=Kernel::Inst()->GetAccumTime()-deltaTime;
	ENavResult result=m_collider.Update(pNav->GetCollider(),startTime,deltaTime,nowPos,endTime,true);
	pRole->SetPos(nowPos);

	//--播放脚步音效，水花特效
	const DWORD dwFootMsg = _MakeFourCC(_T("foot"));
	const vector<DWORD>& msgCodes = pRole->GetMsgCodes();
	for( size_t nMsgCode = 0; nMsgCode < msgCodes.size(); nMsgCode++ )
	{
		if( dwFootMsg == msgCodes[nMsgCode] )
		{
			CombatSysUtil::Inst()->PlayFootSound(nowPos);
			PlaySprayEffect(pNav,pRole);
		}
	}

	//--判断是否进入有效距离
	if(Vec3DistSq(pRole->GetPos(),m_moveEnd)<=m_validDistSq)
	{
		if(m_bNeedSendStopCmd)
		{
			tagNC_StopWalk cmd;
			cmd.curPos=pRole->GetPos();
			cmd.bSwim=true;
			TObjRef<NetSession>()->Send(&cmd);
		}
		m_bClosed=true;
		m_result=ER_Arrive;
		return;
	}

	if(m_moveMode==EMM_DirectGo)//direct go mode
	{
		if(result!=ENR_ToBeContinued)
		{
			m_bClosed=true;
			if(result==ENR_SwimLanded)
				m_result=ER_Land;
			else if(result==ENR_WillSwim)
				m_result=ER_Swim;
			else
				m_result=ER_CannotArrive;
		}
	}
	else//find path mode
	{
		if(result==ENR_Arrive)
		{
			Vector3 point;
			if(m_pathFinder.GetPathPoint(pNav,pRole,point))
			{
				//--
				m_collider.Init(pRole->GetPos(),point,pRole->GetRoleSize(),pRole->GetSwimSpeed());

				//--
				SendCmd(pRole->GetPos(),point);

				//--播放动作
				const TCHAR* szTrackName=CombatSysUtil::Inst()->GetMoveTrackName(pRole->GetRWeaponType(),pRole->IsReadyState(),pRole->GetSwimSpeed());
				pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Move,AvatarAniMixer::EAP_Lower,szTrackName,true);

				//--设置朝向
				pRole->SetMoveYaw(CalcYaw(point-pRole->GetPos()));
				pRole->SetYaw(pRole->GetMoveYaw());
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
			if(result==ENR_SwimLanded)
				m_result=ER_Land;
			else if(result==ENR_WillSwim)
				m_result=ER_Swim;
			else
				m_result=ER_CannotArrive;
		}
	}
}

void ActionMouseWaterWalk_LP::Stop(LocalPlayer* pRole)
{
	if(m_bClosed)
		return;

	m_bClosed=true;
	m_nFoot=0;

	tagNC_StopWalk cmd;
	cmd.curPos = pRole->GetPos();
	cmd.bSwim=true;
	TObjRef<NetSession>()->Send(&cmd);
}

void ActionMouseWaterWalk_LP::PlaySprayEffect( NavMap* pNav,LocalPlayer* pRole )
{
	tstring strFootBoneName;
	if( m_nFoot % 2 == 0 )
		strFootBoneName = _T("Bip01 R Foot");
	else
		strFootBoneName = _T("Bip01 L Foot");

	float waterH;
	if(pNav->GetCollider()->GetWaterHeight(pRole->GetPos(),waterH)
		&& pRole->GetPos().y<=waterH)
	{
		Matrix4 mat;
		if( pRole->GetBoneSpace( strFootBoneName.c_str(), mat ) )
		{
			Vector3 effectPos=Vector3(mat._41,mat._42,mat._43);
			effectPos.y=waterH;
			EffectMgr::Inst()->PlayEffect(effectPos,pRole->GetDestYaw(),_T("txqt10"));
		}
	}
	m_nFoot++;
}

void ActionMouseWaterWalk_LP::SendCmd( const Vector3& start,const Vector3& end )
{
	tagNC_MouseWalk cmd;
	cmd.srcPos=start;
	cmd.dstPos=end;
	cmd.bSwim=true;
	TObjRef<NetSession>()->Send(&cmd);
}
