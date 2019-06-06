#include "StdAfx.h"
#include "LowerStateFloat_RP.h"
#include "FSM_RP.h"
#include "Player.h"
#include "EffectMgr.h"

LowerStateFloat_RP::LowerStateFloat_RP(void)
: m_effectID(GT_INVALID)
{}

LowerStateFloat_RP::~LowerStateFloat_RP(void)
{}

void LowerStateFloat_RP::Active( tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole )
{
	//--播放动作
	pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Other,
		AvatarAniMixer::EAP_Lower,
		_T("swm1"),
		true,
		false);

	//--播放特效
	Vector3 effectPos=pRole->GetPos();
	effectPos.y=pNav->GetCollider()->GetWaterHeightEx(effectPos,effectPos.y);
	m_effectID=EffectMgr::Inst()->PlayEffect(effectPos,pRole->GetDestYaw(),_T("txqt05"));
}

void LowerStateFloat_RP::Update(NavMap* pNav,Player* pRole)
{}

void LowerStateFloat_RP::OnNetCmd( tagNetCmd* pNetCmd )
{
	m_pFSM->Change2LowerState(pNetCmd);
}

void LowerStateFloat_RP::OnGameEvent( tagGameEvent* pEvent )
{}

void LowerStateFloat_RP::OnExit()
{
	if(m_effectID!=GT_INVALID)
	{
		EffectMgr::Inst()->StopEffect(m_effectID);
		m_effectID=GT_INVALID;
	}
}

bool LowerStateFloat_RP::IfCanChange2UpperState( FSM_RP::EUpperState state )
{
	switch(state)
	{
	case FSM_RP::EUpperState_BeAttack:
		return false;
	}

	return true;
}