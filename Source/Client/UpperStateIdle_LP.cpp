#include "StdAfx.h"
#include "UpperStateIdle_LP.h"
#include "FSM_LP.h"
#include "LocalPlayer.h"
#include "CombatSys.h"
#include "RoleMgr.h"
#include "NPC.h"

UpperStateIdle_LP::UpperStateIdle_LP(void)
: m_lastUpdatetime(0.0f)
{}

UpperStateIdle_LP::~UpperStateIdle_LP(void)
{}

void UpperStateIdle_LP::Active( tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole )
{
	pRole->GetAniPlayer()->Sync2Lower();

	m_lastUpdatetime = Kernel::Inst()->GetAccumTime();

	FaceToTarget();
}

void UpperStateIdle_LP::Update(NavMap* pNav,LocalPlayer* pRole)
{
	float curTime = Kernel::Inst()->GetAccumTime();
	if( curTime-m_lastUpdatetime > 0.1f )//每隔0.1秒更新
	{
		FaceToTarget();
		m_lastUpdatetime = curTime;
	}
}

void UpperStateIdle_LP::OnGameEvent( tagGameEvent* pEvent )
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

void UpperStateIdle_LP::OnNetCmd( tagNetCmd* pNetCmd )
{
}

bool UpperStateIdle_LP::IfCanChange2UpperState( FSM_LP::EUpperState state,tagGameEvent* pEvent )
{
	return true;
}

bool UpperStateIdle_LP::IfCanChange2LowerState( FSM_LP::ELowerState state,tagGameEvent* pEvent )
{
	return true;
}

bool UpperStateIdle_LP::IfCanPlayLowerAni()
{
	return true;
}

bool UpperStateIdle_LP::IfLowerCanSetYaw()
{
	return true;
}

void UpperStateIdle_LP::FaceToTarget( void )
{
	LocalPlayer* pRole = m_pFSM->GetLocalPlayer();

	CombatSys* pCombatSys = (CombatSys*)TObjRef<GameFrameMgr>()->GetFrame(_T("CombatSys"));
	if( P_VALID( pCombatSys ) && GT_INVALID != pCombatSys->GetCurTargetID() )
	{
		Role* pTarget = RoleMgr::Inst()->FindRole( pCombatSys->GetCurTargetID() );
		if( P_VALID(pTarget)
			&& pTarget != pRole
			&& pTarget->IsAttributeInited()
			&& !RoleMgr::Inst()->IsOutFaceDist( pTarget->GetID() ) )
		{
			if( pTarget->IS_KIND_OF( NPC ) )
			{
				NPC* pNpc = (NPC*)pTarget;
				if( pNpc->IsNPC() || pNpc->IsMonster() )
				{
					// 朝向目标
					Vector3 targetPos;
					pTarget->GetHeadPos(targetPos);
					pRole->FaceTo(targetPos,FALSE);

					return;
				}
			}
			else if( pTarget->IS_KIND_OF( Player ) )//玩家
			{
				// 朝向目标
				Vector3 targetPos;
				pTarget->GetHeadPos(targetPos);
				pRole->FaceTo(targetPos,FALSE);

				return;
			}
		}
	}

	pRole->FaceBack();
}

void UpperStateIdle_LP::OnExit()
{
	m_pFSM->GetLocalPlayer()->FaceBack();
}