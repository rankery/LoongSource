#include "StdAfx.h"
#include "LowerStateFloat_LP.h"
#include "FSM_LP.h"
#include "LocalPlayer.h"
#include "CombatEvent.h"
#include "..\WorldDefine\action.h"
#include "CombatSysUtil.h"
#include "RoleEvent.h"
#include "EffectMgr.h"

LowerStateFloat_LP::LowerStateFloat_LP(void)
: m_effectID(GT_INVALID)
{}

LowerStateFloat_LP::~LowerStateFloat_LP(void)
{}

void LowerStateFloat_LP::Active( tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole,tagPreState* pPreState )
{
	//--播放动作
	pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Other,
		AvatarAniMixer::EAP_Lower,
		_T("swm1"),
		true,
		false);

	//--发送游戏事件
	tagEnterStandStateEvent event(_T("tagEnterStandStateEvent"),NULL);
	TObjRef<GameFrameMgr>()->SendEvent(&event);

	//--播放特效
	Vector3 effectPos=pRole->GetPos();
	effectPos.y=pNav->GetCollider()->GetWaterHeightEx(effectPos,effectPos.y);
	m_effectID=EffectMgr::Inst()->PlayEffect(effectPos,pRole->GetDestYaw(),_T("txqt05"));
}

void LowerStateFloat_LP::Update(NavMap* pNav,LocalPlayer* pRole)
{}

void LowerStateFloat_LP::OnGameEvent( tagGameEvent* pEvent )
{
	if(pEvent->strEventName==_T("tagMouseMoveEvent"))
	{
		m_pFSM->TryChange2LowerState(FSM_LP::ELowerState_MouseSwim,pEvent);
	}
	else if(pEvent->strEventName==_T("tagKeyboardMoveEvent"))
	{
		m_pFSM->TryChange2LowerState(FSM_LP::ELowerState_KeyboardSwim,pEvent);
	}
	else if(pEvent->strEventName==_T("tagJumpEvent"))
	{
		m_pFSM->TryChange2LowerState(FSM_LP::ELowerState_Jump,pEvent);
	}
}

void LowerStateFloat_LP::OnNetCmd( tagNetCmd* pNetCmd )
{}

void LowerStateFloat_LP::OnExit()
{
	if(m_effectID!=GT_INVALID)
	{
		EffectMgr::Inst()->StopEffect(m_effectID);
		m_effectID=GT_INVALID;
	}
}

bool LowerStateFloat_LP::IfCanChange2UpperState( FSM_LP::EUpperState state,tagGameEvent* pEvent )
{
	switch(state)
	{
	case FSM_LP::EUpperState_Idle:		//空闲
	case FSM_LP::EUpperState_Dead:		//死亡
	case FSM_LP::EUpperState_Dazzy:		//击晕
	case FSM_LP::EUpperState_Tie:		//定身
	case FSM_LP::EUpperState_Sopor:		//昏睡
	case FSM_LP::EUpperState_Follow:	//跟随
	case FSM_LP::EUpperState_PickupItem://拾取
	case FSM_LP::EUpperState_UseItem:	//使用物品
		{
			return true;
		}
		break;
	}

	return false;
}

bool LowerStateFloat_LP::IfCanChange2LowerState( FSM_LP::ELowerState state,tagGameEvent* pEvent )
{
	return true;
}

void LowerStateFloat_LP::ReplayAni( LocalPlayer* pRole )
{
	pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Other,
		AvatarAniMixer::EAP_Lower,
		_T("swm1"),
		true,
		false);
}