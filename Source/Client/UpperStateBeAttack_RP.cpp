#include "StdAfx.h"
#include "UpperStateBeAttack_RP.h"
#include "FSM_LP.h"
#include "LocalPlayer.h"
#include "CombatEvent.h"
#include "CombatSysUtil.h"

UpperStateBeAttack_RP::UpperStateBeAttack_RP(void)
{}

UpperStateBeAttack_RP::~UpperStateBeAttack_RP(void)
{}

void UpperStateBeAttack_RP::Active( tagGameEvent* pEvent,NavMap* pNav,Player* pRole )
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

void UpperStateBeAttack_RP::Update(NavMap* pNav,Player* pRole)
{
	if(pRole->GetAniPlayer()->IsTrackEnd(AvatarAniPlayer::EAT_Other,m_szTrackName.c_str()))
	{
		m_pFSM->Change2UpperState_Idle();
	}
}

void UpperStateBeAttack_RP::OnGameEvent( tagGameEvent* pEvent )
{
	if(pEvent->strEventName==_T("tagBeAttackEvent"))
	{
		m_pFSM->TryChange2UpperState(FSM_RP::EUpperState_BeAttack,pEvent);
	}
}

void UpperStateBeAttack_RP::OnNetCmd( tagNetCmd* pNetCmd )
{	
	m_pFSM->Change2UpperState(pNetCmd);
}

bool UpperStateBeAttack_RP::IfCanPlayLowerAni()
{
	return true;
}

bool UpperStateBeAttack_RP::IfLowerCanSetYaw()
{
	return true;
}

void UpperStateBeAttack_RP::OnExit()
{
	m_pFSM->ReplayLowerAni();
}

void UpperStateBeAttack_RP::OnChangeLowerState( FSM_RP::ELowerState state )
{
	m_pFSM->Change2UpperState_Idle();
}