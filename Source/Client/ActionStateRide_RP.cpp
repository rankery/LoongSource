#include "stdafx.h"
#include "ActionStateRide_RP.h"
#include "FSM_RP.h"
#include "RoleMgr.h"
#include "RoleEvent.h"
#include "Player.h"
#include "Pet.h"

ActionStateRide_RP::ActionStateRide_RP( void )
{}

ActionStateRide_RP::~ActionStateRide_RP( void )
{}

void ActionStateRide_RP::Active( NavMap* pNav,Player* pRole )
{
	pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Stand,AvatarAniMixer::EAP_All,_T(""),true);
}

void ActionStateRide_RP::Update( NavMap* pNav,Player* pRole )
{

}

void ActionStateRide_RP::OnGameEvent( tagGameEvent* pEvent )
{
	if( pEvent->strEventName == _T("tagRPRoleStateChangeEvent") )//远程人物状态改变
	{
		tagRPRoleStateChangeEvent *pState = static_cast<tagRPRoleStateChangeEvent*>(pEvent);
		Role *pRole = RoleMgr::Inst()->FindRole(pState->dwRoleID);
		if( P_VALID(pRole) && pRole->IS_STATIC_CLASS(Player) )
		{
			Player *pPlayer = static_cast<Player*>(pRole);
			if( (!pPlayer->GetRoleState(ERS_Mount) && (pState->dwOldState&ERS_Mount))
				|| (!pPlayer->GetRoleState(ERS_Mount2) && (pState->dwOldState&ERS_Mount2)) )
			{
				m_pFSM->Change2ActionState(EActionState_Normal);
			}	
		}
	}
}

void ActionStateRide_RP::OnNetCmd( tagNetCmd* pNetCmd )
{

}

void ActionStateRide_RP::OnExit()
{
	// 向宠物系统发送离开骑乘事件	
	Pet *pPet = m_pFSM->GetPlayer()->GetMount();
	if( P_VALID(pPet) )
	{
		if( pPet->GetOwnerID() == m_pFSM->GetPlayer()->GetID() )//主骑乘状态
		{
			tagLeaveMount event(_T("tagLeaveMount"), NULL);
			event.dwPetID = pPet->GetID();
			TObjRef<GameFrameMgr>()->SendEvent(&event);
		}

		m_pFSM->GetPlayer()->SetMount(NULL);
	}

	// 重新播放下身状态动作
	m_pFSM->ReplayLowerAni();
}

bool ActionStateRide_RP::IfCanChange2UpperState( FSM_RP::EUpperState state )
{
	switch( state )
	{
	case FSM_RP::EUpperState_Idle:
	case FSM_RP::EUpperState_Skill:
	case FSM_RP::EUpperState_UseItem:
		return true;
	}

	return false;
}