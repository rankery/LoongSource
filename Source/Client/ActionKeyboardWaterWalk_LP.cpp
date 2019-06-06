#include "StdAfx.h"
#include "ActionKeyboardWaterWalk_LP.h"
#include "ClientCamera.h"
#include "LocalPlayer.h"
#include "WorldFrame.h"
#include "CombatSysUtil.h"
#include "..\WorldDefine\action.h"
#include "FSM_LP.h"
#include "EffectMgr.h"
#include "AudioSys.h"

ActionKeyboardWaterWalk_LP::ActionKeyboardWaterWalk_LP(void)
: m_bClosed(true)
, m_result(ER_Unknow)
, m_nFoot(0)
{
}

ActionKeyboardWaterWalk_LP::~ActionKeyboardWaterWalk_LP(void)
{}

void ActionKeyboardWaterWalk_LP::Active( const Vector3& start,EWalkDir dir,NavMap* pNav,LocalPlayer* pRole )
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
	const TCHAR* szTrackName=CombatSysUtil::Inst()->GetMoveTrackName(pRole->GetRWeaponType(),pRole->IsReadyState(),pRole->GetSwimSpeed());
	pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Move,AvatarAniMixer::EAP_Lower,szTrackName,true);

	//--设置朝向
	pRole->SetYaw(CalcYaw(m_moveVec));
}

void ActionKeyboardWaterWalk_LP::Update( NavMap* pNav,LocalPlayer* pRole )
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
			Active(nowPos,m_moveDir,pNav,pRole);
		}
	}
}

void ActionKeyboardWaterWalk_LP::Stop(NavMap* pNav,LocalPlayer* pRole)
{
	if(m_bClosed)
		return;

	m_bClosed=true;
	m_result=ER_OnWater;
	m_nFoot=0;
	if(!m_bBlocking)
	{
		//--发送停止消息
		tagNC_StopWalk cmd;
		cmd.curPos=pRole->GetPos();
		cmd.bSwim=true;
		TObjRef<NetSession>()->Send(&cmd);
	}
}

Vector3 ActionKeyboardWaterWalk_LP::CalcMoveEnd( EWalkDir dir,float cameraYaw,const Vector3& start )
{
	Vector3 moveDir=CombatSysUtil::Inst()->CalcKeyMoveDir(dir,cameraYaw);
	Vector3 moveDest=start+moveDir*1000.0f;
	return moveDest;
}

void ActionKeyboardWaterWalk_LP::SendCmd( const Vector3& srcPos,const Vector3& dstPos )
{
	tagNC_KeyboardWalk cmd;
	cmd.srcPos=srcPos;
	cmd.dstPos=dstPos;
	cmd.bSwim=true;
	TObjRef<NetSession>()->Send(&cmd);
}

void ActionKeyboardWaterWalk_LP::PlaySprayEffect( NavMap* pNav,LocalPlayer* pRole )
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
