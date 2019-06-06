#include "stdafx.h"
#include "ActionStateBoard_LP.h"
#include "FSM_LP.h"
#include "LocalPlayer.h"
#include "RoleEvent.h"
#include "PetEvent.h"
#include "Pet.h"
#include "CombatEvent.h"
#include "SkillMgr.h"
#include "ItemProtoData.h"

ActionStateBoard_LP::ActionStateBoard_LP( void )
{}

ActionStateBoard_LP::~ActionStateBoard_LP( void )
{}

void ActionStateBoard_LP::Active( NavMap* pNav,LocalPlayer* pRole )
{
	pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Stand,AvatarAniMixer::EAP_All,_T(""),true);
}

void ActionStateBoard_LP::OnExit()
{
	m_pFSM->GetLocalPlayer()->SetMount(NULL);

	// 重新播放下身状态动作
	m_pFSM->ReplayLowerAni();
}

void ActionStateBoard_LP::Update( NavMap* pNav,LocalPlayer* pRole )
{
}

void ActionStateBoard_LP::OnGameEvent( tagGameEvent* pEvent )
{
	if( pEvent->strEventName == _T("tagLPRoleStateChangeEvent") )//本地人物状态改变
	{
		tagLPRoleStateChangeEvent *pState = static_cast<tagLPRoleStateChangeEvent*>(pEvent);
		if( !m_pFSM->GetLocalPlayer()->GetRoleState(ERS_Mount2)
			&& (pState->dwOldState&ERS_Mount2) )
		{
			m_pFSM->Change2ActionState(EActionState_Normal);
		}
	}
}

void ActionStateBoard_LP::OnNetCmd( tagNetCmd* pNetCmd )
{
}

bool ActionStateBoard_LP::IfCanChange2UpperState( FSM_LP::EUpperState state,tagGameEvent* pEvent )
{
	switch( state )
	{
	case FSM_LP::EUpperState_Idle:
		return true;

	case FSM_LP::EUpperState_Skill:
		{
			tagUseSkillEvent *pSkillEvent = static_cast<tagUseSkillEvent*>(pEvent);
			const tagSkillData *pSkillData = SkillMgr::Inst()->GetSkillData(pSkillEvent->dwSkillID);
			if( P_VALID(pSkillData) && (pSkillData->pProto->dwSelfStateLimit & ESF_Mount2) )
				return true;

		}
		break;

	case FSM_LP::EUpperState_UseItem:
		{
			tagUseItemEvent *pItemEvent = static_cast<tagUseItemEvent*>(pEvent);
			const tagItemProto *pItemData = ItemProtoData::Inst()->FindItemProto(pItemEvent->dwItemTypeID);
			if( P_VALID(pItemData) && (pItemData->dwStateLimit & ESF_Mount2) )
				return true;
		}
		break;
	}

	return false;
}

bool ActionStateBoard_LP::IfCanChange2LowerState( FSM_LP::ELowerState state,tagGameEvent* pEvent )
{
	return false;
}