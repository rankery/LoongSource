#include "StdAfx.h"
#include "UpperStateStyleAction_RP.h"
#include "FSM_RP.h"
#include "Player.h"
#include "..\WorldDefine\msg_motion.h"
#include "PersonalityActData.h"
#include "RoleMgr.h"
#include "CombatEvent.h"

UpperStateStyleAction_RP::UpperStateStyleAction_RP(void)
{}

UpperStateStyleAction_RP::~UpperStateStyleAction_RP(void)
{}

void UpperStateStyleAction_RP::Active( tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole )
{
	tagNS_RoleStyleAction* pRecv = (tagNS_RoleStyleAction*)pNetCmd;

	const tagPersonAct* pAct = PersonalityActData::Inst()->FindPersonAct(pRecv->dwActionID);
	THROW_FALSE(P_VALID(pAct),_T("cannot find personal act"));

	m_step = E3ActStep_None;
	m_sPersonActData = *pAct;
	m_bDestRole = pRole->GetID() == pRecv->dwDstRoleID;
	m_targetID = m_bDestRole ? pRecv->dwRoleID : pRecv->dwDstRoleID;
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
			if( P_VALID(pTarget) )
				pRole->SetYaw( pTarget->GetDestYaw() );
	}

	if (m_pFSM->GetPlayer()->GetRWeaponType() != EITE_Faqi)
	{
		m_pFSM->GetPlayer()->HideWeapon(true);
		m_pFSM->GetPlayer()->HideWeaponPertain(true);
	}
	m_pFSM->GetPlayer()->PlaySuitEffect(false);

	AvatarAniMixer::EAniPart aniPart = tagPersonAct::T_Upper == m_sPersonActData.eType ? AvatarAniMixer::EAP_Upper : AvatarAniMixer::EAP_All;
	pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Other,aniPart,m_sPersonActData.strAct1.c_str(),false,true);

	if( tagPersonAct::T_3Act == m_sPersonActData.eType )
		m_step = E3ActStep_In;

}

void UpperStateStyleAction_RP::Update(NavMap* pNav,Player* pRole)
{
	// 交互动作需要检查目标是否存在
	if( GT_INVALID != m_targetID )
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

void UpperStateStyleAction_RP::OnNetCmd( tagNetCmd* pNetCmd )
{
	m_pFSM->Change2UpperState(pNetCmd);
}

void UpperStateStyleAction_RP::OnGameEvent( tagGameEvent* pEvent )
{
	if(pEvent->strEventName==_T("tagBeAttackEvent"))
	{
		if( tagPersonAct::T_3Act == m_sPersonActData.eType && E3ActStep_Out != m_step )
		{
			m_pFSM->GetPlayer()->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Other,AvatarAniMixer::EAP_All,m_sPersonActData.strAct3.c_str(),false);
			m_step = E3ActStep_Out;
		}
		else
		{
			m_pFSM->TryChange2UpperState(FSM_RP::EUpperState_BeAttack,pEvent);
		}
	}
	else if(pEvent->strEventName==_T("tagPersonalityActExitEvent"))
	{
		// 目标已经离开该状态
		tagPersonalityActExitEvent* pActExitEvent = (tagPersonalityActExitEvent*)pEvent;
		if( pActExitEvent->dwActID == m_sPersonActData.dwID && 
			pActExitEvent->dwTargetID == m_pFSM->GetPlayer()->GetID() &&
			pActExitEvent->dwRoleID == m_targetID )
		{
			m_bSentEventOnExit = false;
			m_pFSM->Change2UpperState_Idle();
			m_pFSM->ReplayLowerAni();
		}
	}
}

bool UpperStateStyleAction_RP::IfCanPlayLowerAni()
{
	return true;
}

bool UpperStateStyleAction_RP::IfLowerCanSetYaw()
{
	return true;
}

void UpperStateStyleAction_RP::OnChangeLowerState( FSM_RP::ELowerState state )
{
	switch(state)
	{
	case FSM_RP::ELowerState_Stand:
	case FSM_RP::ELowerState_Float:
	case FSM_RP::ELowerState_WaterStand:
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

void UpperStateStyleAction_RP::OnExit()
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
					evt.dwRoleID = m_pFSM->GetPlayer()->GetID();
					evt.dwTargetID = m_targetID;
					evt.dwActID = m_sPersonActData.dwID;
					pTarget->OnGameEvent( &evt );
				}
			}
		}
		// 恢复朝向
		if( !m_bDestRole )
			m_pFSM->GetPlayer()->SetYaw( m_pFSM->GetPlayer()->GetMoveYaw() );
	}
	if (m_pFSM->GetPlayer()->GetRWeaponType() != EITE_Faqi)
	{
		m_pFSM->GetPlayer()->HideWeapon(false);
		m_pFSM->GetPlayer()->HideWeaponPertain(false);
	}
	m_pFSM->GetPlayer()->PlaySuitEffect(true);
}
