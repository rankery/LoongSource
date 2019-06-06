#include "StdAfx.h"
#include "UpperStateStyleAction_LP.h"
#include "FSM_LP.h"
#include "LocalPlayer.h"
#include "CombatEvent.h"
#include "..\WorldDefine\action.h"
#include "..\WorldDefine\msg_motion.h"
#include "RoleMgr.h"

UpperStateStyleAction_LP::UpperStateStyleAction_LP(void)
{}

UpperStateStyleAction_LP::~UpperStateStyleAction_LP(void)
{}

void UpperStateStyleAction_LP::Active( tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole )
{
	ASSERT( pEvent->strEventName == _T("tagPlayPersonalityActEvent") );
	tagPlayPersonalityActEvent* pStyleEvent = (tagPlayPersonalityActEvent*)pEvent;

	m_step = E3ActStep_None;
	m_sPersonActData = *pStyleEvent->pData;
	m_targetID = pStyleEvent->dwTargetID;
	m_bDestRole = pStyleEvent->bDestRole;
	m_bSentEventOnExit = true;

	if( EMT1_Interactive==m_sPersonActData.eMotionType )
	{
		Role* pTarget= RoleMgr::Inst()->FindRole( m_targetID );
		if( !P_VALID( pTarget ) )
		{
			m_pFSM->Change2UpperState_Idle();
			m_pFSM->ReplayLowerAni();
			return;
		}
		if( !m_bDestRole && P_VALID(pTarget))
			pRole->SetYaw( pTarget->GetDestYaw() );
	}
	
	if (m_pFSM->GetLocalPlayer()->GetRWeaponType() != EITE_Faqi)
	{
		m_pFSM->GetLocalPlayer()->HideWeapon(true);
		m_pFSM->GetLocalPlayer()->HideWeaponPertain(true);
	}
	m_pFSM->GetLocalPlayer()->PlaySuitEffect(false);



	AvatarAniMixer::EAniPart aniPart = tagPersonAct::T_Upper == m_sPersonActData.eType ? AvatarAniMixer::EAP_Upper : AvatarAniMixer::EAP_All;
	pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Other,aniPart,m_sPersonActData.strAct1.c_str(),false,true);

	if( tagPersonAct::T_3Act == m_sPersonActData.eType )
		m_step = E3ActStep_In;
}

void UpperStateStyleAction_LP::Update(NavMap* pNav,LocalPlayer* pRole)
{
	// 交互动作需要检查目标是否存在
	if( EMT1_Interactive==m_sPersonActData.eMotionType )
	{
		Role* pRole = RoleMgr::Inst()->FindRole( m_targetID );
		if( !P_VALID( pRole ) )
		{
			m_pFSM->Change2UpperState_Idle();
			m_pFSM->ReplayLowerAni();
			return;
		}
	}

	if( tagPersonAct::T_3Act == m_sPersonActData.eType )
	{
		if( E3ActStep_In == m_step && pRole->GetAniPlayer()->IsTrackEnd(AvatarAniPlayer::EAT_Other,m_sPersonActData.strAct1.c_str()) )
		{
			pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Other,AvatarAniMixer::EAP_All,m_sPersonActData.strAct2.c_str(),true);
			m_step = E3ActStep_Holding;
		}
		else if( E3ActStep_Out == m_step && pRole->GetAniPlayer()->IsTrackEnd(AvatarAniPlayer::EAT_Other,m_sPersonActData.strAct3.c_str()) )
		{
			m_pFSM->Change2UpperState_Idle();
			m_pFSM->ReplayLowerAni();
		}
	}
	else if( pRole->GetAniPlayer()->IsTrackEnd(AvatarAniPlayer::EAT_Other,m_sPersonActData.strAct1.c_str()) )
	{
		m_pFSM->Change2UpperState_Idle();
		m_pFSM->ReplayLowerAni();
	}
}

void UpperStateStyleAction_LP::OnGameEvent( tagGameEvent* pEvent )
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
		if( tagPersonAct::T_3Act == m_sPersonActData.eType && E3ActStep_Out != m_step )
		{
			m_pFSM->GetLocalPlayer()->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Other,AvatarAniMixer::EAP_All,m_sPersonActData.strAct3.c_str(),false);
			m_step = E3ActStep_Out;
		}
		else
		{
			m_pFSM->TryChange2UpperState(FSM_LP::EUpperState_BeAttack,pEvent);
		}
	}
	else if(pEvent->strEventName==_T("tagPersonalityActExitEvent"))
	{
		// 目标已经离开该状态
		tagPersonalityActExitEvent* pActExitEvent = (tagPersonalityActExitEvent*)pEvent;
		if( pActExitEvent->dwActID == m_sPersonActData.dwID && 
			pActExitEvent->dwTargetID == m_pFSM->GetLocalPlayer()->GetID() &&
			pActExitEvent->dwRoleID == m_targetID )
		{
			m_bSentEventOnExit = false;
			m_pFSM->Change2UpperState_Idle();
			m_pFSM->ReplayLowerAni();
		}
	}
}

void UpperStateStyleAction_LP::OnNetCmd( tagNetCmd* pNetCmd )
{}

bool UpperStateStyleAction_LP::IfCanChange2UpperState( FSM_LP::EUpperState state,tagGameEvent* pEvent )
{
	return true;
}

bool UpperStateStyleAction_LP::IfCanChange2LowerState( FSM_LP::ELowerState state,tagGameEvent* pEvent )
{
	return true;
}

void UpperStateStyleAction_LP::OnChangeLowerState( FSM_LP::ELowerState newState,tagGameEvent* pEvent )
{
	switch(newState)
	{
	case FSM_LP::ELowerState_Stand:
	case FSM_LP::ELowerState_Float:
	case FSM_LP::ELowerState_WaterStand:
		break;
	default:
		{
			if(tagPersonAct::T_Upper!=m_sPersonActData.eType || EMT1_Interactive==m_sPersonActData.eMotionType)
			{
				m_pFSM->Change2UpperState_Idle();
			}
		}
		break;
	}
}

bool UpperStateStyleAction_LP::IfCanPlayLowerAni()
{
	return true;
}

bool UpperStateStyleAction_LP::IfLowerCanSetYaw()
{
	return true;
}

void UpperStateStyleAction_LP::OnExit()
{
	if( GT_INVALID != m_targetID )
	{
		// 通知目标角色
		if( m_bSentEventOnExit )
		{
			Role* pRole = RoleMgr::Inst()->FindRole( m_targetID );
			if( P_VALID( pRole ) && pRole->IS_KIND_OF(Player) )
			{
				Player* pTarget = (Player*)pRole;
				if( pTarget->IsStyleActionState() )
				{
					tagPersonalityActExitEvent evt;
					evt.dwRoleID = m_pFSM->GetLocalPlayer()->GetID();
					evt.dwTargetID = m_targetID;
					evt.dwActID = m_sPersonActData.dwID;
					pTarget->OnGameEvent( &evt );
				}
			}
		}
		// 恢复朝向
		if( !m_bDestRole )
			m_pFSM->GetLocalPlayer()->SetYaw( m_pFSM->GetLocalPlayer()->GetMoveYaw() );
	}
	if (m_pFSM->GetLocalPlayer()->GetRWeaponType() != EITE_Faqi)
	{
		m_pFSM->GetLocalPlayer()->HideWeapon(false);
		m_pFSM->GetLocalPlayer()->HideWeaponPertain(false);
	}
	m_pFSM->GetLocalPlayer()->PlaySuitEffect(true);
}