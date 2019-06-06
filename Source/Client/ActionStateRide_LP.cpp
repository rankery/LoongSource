#include "stdafx.h"
#include "ActionStateRide_LP.h"
#include "FSM_LP.h"
#include "LocalPlayer.h"
#include "RoleEvent.h"
#include "PetEvent.h"
#include "Pet.h"
#include "CombatEvent.h"
#include "SkillMgr.h"
#include "ItemProtoData.h"
#include "RoleMgr.h"
#include "ChatFrame.h"

ActionStateRide_LP::ActionStateRide_LP( void )
{}

ActionStateRide_LP::~ActionStateRide_LP( void )
{}

void ActionStateRide_LP::Active( NavMap* pNav,LocalPlayer* pRole )
{
	pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Stand,AvatarAniMixer::EAP_All,_T(""),true);
}

void ActionStateRide_LP::OnExit()
{
	// 向宠物系统发送离开骑乘事件	
	Pet *pPet = m_pFSM->GetLocalPlayer()->GetMount();
	if( P_VALID(pPet) )
	{
		if( pPet->GetOwnerID() == m_pFSM->GetLocalPlayer()->GetID() )//主骑乘状态
		{
			tagLeaveMount event(_T("tagLeaveMount"), NULL);
			event.dwPetID = pPet->GetID();
			TObjRef<GameFrameMgr>()->SendEvent(&event);

			m_pFSM->GetLocalPlayer()->SetMount(NULL);
		}
	}

	// 重新播放下身状态动作
	m_pFSM->ReplayLowerAni();
}

void ActionStateRide_LP::Update( NavMap* pNav,LocalPlayer* pRole )
{
}

void ActionStateRide_LP::OnGameEvent( tagGameEvent* pEvent )
{
	if( pEvent->strEventName == _T("tagLPRoleStateChangeEvent") )//本地人物状态改变
	{
		tagLPRoleStateChangeEvent *pState = static_cast<tagLPRoleStateChangeEvent*>(pEvent);
		if( !m_pFSM->GetLocalPlayer()->GetRoleState(ERS_Mount) && (pState->dwOldState&ERS_Mount) )
		{
			m_pFSM->Change2ActionState(EActionState_Normal);
		}
	}
}

void ActionStateRide_LP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if( pNetCmd->dwID == TObjRef<Util>()->Crc32("NS_MountInvite") )//邀请搭乘
	{
		tagNS_MountInvite *pMountCmd = static_cast<tagNS_MountInvite*>(pNetCmd);

		switch( pMountCmd->dwErrCode )
		{
		case E_Pets_Success:
			{
				// 是否需要把搭乘ID放入一个列表进行管理，多人骑乘需要
			}
			break;
		case E_Pets_Mount_DstRoleRefuse://拒绝邀请
			{
				ChatFrame* pFrame = (ChatFrame*)TObjRef<GameFrameMgr>()->GetFrame(_T("Chat"));

				Role *pRole = RoleMgr::Inst()->FindRole(pMountCmd->dwDstRoleID);
				if( P_VALID(pRole) )
				{
					tstringstream strInfo;
					strInfo << pRole->GetRoleName() << g_StrTable[_T("Team_InviteDisAgree")];
					pFrame->PushInfo( strInfo.str().c_str(), ESCC_Current );
				}
			}
			break;
		default:
			{
				ChatFrame* pFrame = (ChatFrame*)TObjRef<GameFrameMgr>()->GetFrame(_T("Chat"));
				pFrame->PushInfo( g_StrTable[_T("Mount_Cannot")], ESCC_Current );
			}
			break;
		}
	}
	else if( pNetCmd->dwID == TObjRef<Util>()->Crc32("NS_Mount2") )//搭乘消息
	{
		tagNS_Mount2 *pMountCmd = static_cast<tagNS_Mount2*>(pNetCmd);

		map<DWORD,Role*>::const_iterator finder = RoleMgr::Inst()->GetRoleMap().find(pMountCmd->dwPetID);
		if( finder != RoleMgr::Inst()->GetRoleMap().end() )
		{
			Role *pRole = finder->second;
			if( pRole->IS_STATIC_CLASS(Pet) )
			{
				Pet *pPet = static_cast<Pet*>(pRole);
				m_pFSM->GetLocalPlayer()->SetMount(pPet);

				m_pFSM->Change2ActionState(EActionState_Board);
			}
		}
	}
}

bool ActionStateRide_LP::IfCanChange2UpperState( FSM_LP::EUpperState state,tagGameEvent* pEvent )
{
	switch( state )
	{
	case FSM_LP::EUpperState_Idle:
	case FSM_LP::EUpperState_TalkNPC:
	case FSM_LP::EUpperState_Follow:
	case FSM_LP::EUpperState_PickupItem:
		return true;

	case FSM_LP::EUpperState_Skill:
		{
			tagUseSkillEvent *pSkillEvent = static_cast<tagUseSkillEvent*>(pEvent);
			const tagSkillData *pSkillData = SkillMgr::Inst()->GetSkillData(pSkillEvent->dwSkillID);
			if( P_VALID(pSkillData) && (pSkillData->pProto->dwSelfStateLimit & ESF_Mount) )
				return true;
			
		}
		break;

	case FSM_LP::EUpperState_UseItem:
		{
			tagUseItemEvent *pItemEvent = static_cast<tagUseItemEvent*>(pEvent);
			const tagItemProto *pItemData = ItemProtoData::Inst()->FindItemProto(pItemEvent->dwItemTypeID);
			if( P_VALID(pItemData) && (pItemData->dwStateLimit & ESF_Mount) )
				return true;
		}
		break;
	}

	return false;
}

bool ActionStateRide_LP::IfCanChange2LowerState( FSM_LP::ELowerState state,tagGameEvent* pEvent )
{
	switch( state )
	{
	case FSM_LP::ELowerState_MouseMove:
	case FSM_LP::ELowerState_MouseWaterWalk:
	case FSM_LP::ELowerState_KeyboardMove:
	case FSM_LP::ELowerState_KeyboardWaterWalk:
	case FSM_LP::ELowerState_Stand:
	case FSM_LP::ELowerState_WaterStand:
	case FSM_LP::ELowerState_Pursue:
	case FSM_LP::ELowerState_Drop:
	case FSM_LP::ELowerState_VDrop:
	case FSM_LP::ELowerState_Slide:
	case FSM_LP::ELowerState_Jump:
		return true;
	}

	return false;
}