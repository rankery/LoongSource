#include "StdAfx.h"
#include "LowerStateWaterStand_RP.h"
#include "FSM_RP.h"
#include "Player.h"
#include "EffectMgr.h"
#include "CombatSysUtil.h"
#include "RoleEvent.h"

LowerStateWaterStand_RP::LowerStateWaterStand_RP(void)
: m_effectID(GT_INVALID)
{}

LowerStateWaterStand_RP::~LowerStateWaterStand_RP(void)
{}

void LowerStateWaterStand_RP::Active( tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole )
{
	//--播放动作
	if(pRole->IsReadyState())
	{
		PlayReadyAni(pRole);
	}
	else
	{
		PlayStandAni(pRole);
	}

	//--播放水花特效
	Vector3 effectPos=pRole->GetPos();
	effectPos.y=pNav->GetCollider()->GetWaterHeightEx(effectPos,effectPos.y);
	m_effectID=EffectMgr::Inst()->PlayEffect(effectPos,pRole->GetDestYaw(),_T("txqt05"));
}

void LowerStateWaterStand_RP::Update(NavMap* pNav,Player* pRole)
{}

void LowerStateWaterStand_RP::OnNetCmd( tagNetCmd* pNetCmd )
{
	m_pFSM->Change2LowerState(pNetCmd);
}

void LowerStateWaterStand_RP::OnGameEvent( tagGameEvent* pEvent )
{
	if(pEvent->strEventName==_T("tagWeaponChangeEvent"))//切换武器
	{
		tagWeaponChangeEvent* pWeaponChangeEvent = (tagWeaponChangeEvent*)pEvent;
		if( pWeaponChangeEvent->bRightWeapon )
		{
			Player* pRole=m_pFSM->GetPlayer();
			if(pRole->IsReadyState())
			{
				PlayReadyAni(pRole);
			}
			else
			{
				PlayStandAni(pRole);
			}
		}
	}
}

void LowerStateWaterStand_RP::OnExit()
{
	//--删除水花特效
	if(m_effectID!=GT_INVALID)
	{
		EffectMgr::Inst()->StopEffect(m_effectID);
		m_effectID=GT_INVALID;
	}
}

void LowerStateWaterStand_RP::PlayReadyAni( Player* pRole )
{
	const TCHAR* szTrackName=CombatSysUtil::Inst()->GetStandTrackName(pRole->GetRWeaponType());
	pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Stand,AvatarAniMixer::EAP_Lower,szTrackName,true);
}

void LowerStateWaterStand_RP::PlayStandAni( Player* pRole )
{
	pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Ease,AvatarAniMixer::EAP_All,_T("std3"),false);
}

bool LowerStateWaterStand_RP::IfCanChange2UpperState( FSM_RP::EUpperState state )
{
	return true;
}