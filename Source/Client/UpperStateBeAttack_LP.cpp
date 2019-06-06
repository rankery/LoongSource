#include "StdAfx.h"
#include "UpperStateBeAttack_LP.h"
#include "FSM_LP.h"
#include "LocalPlayer.h"
#include "CombatEvent.h"
#include "CombatSysUtil.h"

UpperStateBeAttack_LP::UpperStateBeAttack_LP(void)
{}

UpperStateBeAttack_LP::~UpperStateBeAttack_LP(void)
{}

void UpperStateBeAttack_LP::Active( tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole )
{
	ASSERT(pEvent->strEventName==_T("tagBeAttackEvent"));
	tagBeAttackEvent* pBeAttackEvent=(tagBeAttackEvent*)pEvent;

	if(pBeAttackEvent->bMiss)
		m_szTrackName=_T("duck");
	else if(pBeAttackEvent->bBlock)
		m_szTrackName=CombatSysUtil::Inst()->GetBlockTrackName(pRole->GetRWeaponType());
	else
	{
		m_szTrackName=CombatSysUtil::Inst()->GetBeattackTrackName();

		// ²¥·Å±»¹¥»÷º°½ÐÉù
		CombatSysUtil::Inst()->PlayPlayerHowlSound(pRole);
	}

	if( !pRole->IsDead() )
	{
		pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Other,AvatarAniMixer::EAP_All,m_szTrackName.c_str(),false,true);
	}
}

void UpperStateBeAttack_LP::Update(NavMap* pNav,LocalPlayer* pRole)
{
	if(pRole->GetAniPlayer()->IsTrackEnd(AvatarAniPlayer::EAT_Other,m_szTrackName.c_str()))
	{
		m_pFSM->Change2UpperState_Idle();
	}
}

void UpperStateBeAttack_LP::OnGameEvent( tagGameEvent* pEvent )
{
	if(pEvent->strEventName==_T("tagUseSkillEvent"))
	{
		m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_Skill,pEvent);
	}
	else if(pEvent->strEventName==_T("tagUseItemEvent"))
	{
		m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_UseItem,pEvent);
	}
	else if(pEvent->strEventName==_T("tagFollowRoleEvent"))
	{
		m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_Follow,pEvent);
	}
	else if(pEvent->strEventName==_T("tagTalkNPCEvent"))
	{
		m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_TalkNPC,pEvent);
	}
	else if(pEvent->strEventName==_T("tagPickupItemEvent"))
	{
		m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_PickupItem,pEvent);
	}
	else if(pEvent->strEventName==_T("tagPlayPersonalityActEvent"))
	{
		m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_StyleAction,pEvent);
	}
	else if(pEvent->strEventName==_T("tagBeAttackEvent"))
	{
		m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_BeAttack,pEvent);
	}
}

void UpperStateBeAttack_LP::OnNetCmd( tagNetCmd* pNetCmd )
{}

bool UpperStateBeAttack_LP::IfCanChange2UpperState( FSM_LP::EUpperState state,tagGameEvent* pEvent )
{
	return true;
}

bool UpperStateBeAttack_LP::IfCanChange2LowerState( FSM_LP::ELowerState state,tagGameEvent* pEvent )
{
	return true;
}

bool UpperStateBeAttack_LP::IfCanPlayLowerAni()
{
	return true;
}

bool UpperStateBeAttack_LP::IfLowerCanSetYaw()
{
	return true;
}

void UpperStateBeAttack_LP::OnChangeLowerState( FSM_LP::ELowerState newState )
{
	m_pFSM->Change2UpperState_Idle();
}

void UpperStateBeAttack_LP::OnExit()
{
	m_pFSM->ReplayLowerAni();
}