#include "StdAfx.h"
#include "StateBeAttack_NPC.h"
#include "FSM_NPC.h"
#include "NPC.h"
#include "CombatSysUtil.h"

StateBeAttack_NPC::StateBeAttack_NPC(void)
{}

StateBeAttack_NPC::~StateBeAttack_NPC(void)
{}

void StateBeAttack_NPC::Active( tagNetCmd* pNetCmd,NavMap* pNav,NPC* pRole )
{
	pRole->PlayTrack(_T("batk"),false,true);

	// 播放被攻击喊叫声(%5几率)
	if( 0 == rand() % 20 )
		CombatSysUtil::Inst()->PlayMonsterHowlSound(pRole);
}

void StateBeAttack_NPC::Update(NavMap* pNav,NPC* pRole)
{
	if(pRole->IsTrackEnd(_T("batk")))
		m_pFSM->Change2IdleState();
}

void StateBeAttack_NPC::OnNetCmd( tagNetCmd* pNetCmd )
{
	m_pFSM->Change2State(pNetCmd);
}

void StateBeAttack_NPC::OnGameEvent( tagGameEvent* pEvent )
{
	m_pFSM->Change2State(pEvent);
}