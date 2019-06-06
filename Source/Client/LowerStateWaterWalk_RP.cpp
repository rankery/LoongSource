#include "StdAfx.h"
#include "LowerStateWaterWalk_RP.h"
#include "Player.h"
#include "FSM_RP.h"
#include "CombatSysUtil.h"
#include "..\WorldDefine\action.h"
#include "EffectMgr.h"

LowerStateWaterWalk_RP::LowerStateWaterWalk_RP(void) :
m_nFoot(0)
{
}

LowerStateWaterWalk_RP::~LowerStateWaterWalk_RP(void)
{}

void LowerStateWaterWalk_RP::Active( tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole )
{
	ASSERT(pNetCmd->dwID==pNetCmd->Crc32("NS_SyncWalk"));
	tagNS_SyncWalk* pCmd=(tagNS_SyncWalk*)pNetCmd;

	//--初始化碰撞检测器
	m_collider.Init(pCmd->srcPos,pCmd->dstPos,pRole->GetRoleSize(),pCmd->fXZSpeed,false);
	m_lastCDTime=Kernel::Inst()->GetAccumTime()-pCmd->curTime;

	//--播放动作
	const TCHAR* szTrackName=CombatSysUtil::Inst()->GetMoveTrackName(pRole->GetRWeaponType(),pRole->IsReadyState(),pCmd->fXZSpeed);
	pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Other,AvatarAniMixer::EAP_Lower,szTrackName,true);

	//--设置朝向
	Vector3 dir=pCmd->dstPos-pCmd->srcPos;
	float yaw=CalcYaw(dir);
	pRole->SetMoveYaw(yaw);
	pRole->SetYaw(yaw);

	m_nFoot=0;
}

void LowerStateWaterWalk_RP::Update( NavMap* pNav,Player* pRole )
{
	//--碰撞检测
	Vector3 nowPos;
	float endTime;
	ENavResult result=m_collider.Update(pNav->GetCollider(),m_lastCDTime,Kernel::Inst()->GetAccumTime()-m_lastCDTime,nowPos,endTime,true);
	m_lastCDTime=Kernel::Inst()->GetAccumTime();

	pRole->SetPos(nowPos);

	//--播放脚步音效，水花特效
	const DWORD dwFootMsg = _MakeFourCC(_T("foot"));
	const vector<DWORD>& msgCodes = pRole->GetMsgCodes();
	for( size_t nMsgCode = 0; nMsgCode < msgCodes.size(); nMsgCode++ )
	{
		if( dwFootMsg == msgCodes[nMsgCode] )
		{
			PlaySprayEffect(pNav,pRole);
		}
	}

	//--
	if(result!=ENR_ToBeContinued)
		m_pFSM->Change2LowerState_WaterStand();
}

void LowerStateWaterWalk_RP::OnNetCmd( tagNetCmd* pNetCmd )
{
	m_pFSM->Change2LowerState(pNetCmd);
}

void LowerStateWaterWalk_RP::OnGameEvent( tagGameEvent* pEvent )
{}

void LowerStateWaterWalk_RP::PlaySprayEffect( NavMap* pNav,Player* pRole )
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

bool LowerStateWaterWalk_RP::IfCanChange2UpperState( FSM_RP::EUpperState state )
{
	switch(state)
	{
	case FSM_RP::EUpperState_BeAttack:
		return false;
	}

	return true;
}