#include "StdAfx.h"
#include "LowerStateMouseMove_LP.h"
#include "LocalPlayer.h"
#include "FSM_LP.h"
#include "WorldPickerFrame.h"
#include "PreState.h"
#include "CombatSysUtil.h"
#include "RoleEvent.h"
#include "PersonalityActData.h"

LowerStateMouseMove_LP::LowerStateMouseMove_LP(void) :
m_finishedOnExit(false)
{}

LowerStateMouseMove_LP::~LowerStateMouseMove_LP(void)
{}

void LowerStateMouseMove_LP::Active( tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole,tagPreState* pPreState )
{
	ASSERT(pEvent->strEventName==_T("tagMouseMoveEvent"));
	tagMouseMoveEvent* pMoveEvent=(tagMouseMoveEvent*)pEvent;

	//--保存event
	m_event=*pMoveEvent;
	m_finishedOnExit=true;
	//--
	m_actMouseMove.Active(pMoveEvent->start,pMoveEvent->end,pMoveEvent->validDist,pNav,pRole,
		m_pFSM->IfCanPlayLowerAni(),m_pFSM->IfLowerCanSetYaw());
}

void LowerStateMouseMove_LP::Update(NavMap* pNav,LocalPlayer* pRole)
{
	m_actMouseMove.Update(pNav,pRole,m_pFSM->IfCanPlayLowerAni(),m_pFSM->IfLowerCanSetYaw());
	if(m_actMouseMove.IsClosed())
	{
		if(m_actMouseMove.GetResult()==ActionMouseMove_LP::ER_Drop)
		{
			tagDropEvent event(_T("tagDropEvent"),NULL);
			event.start=pRole->GetPos();
			Yaw2Dir(pRole->GetYaw(),event.dir);

			tagPreState prestate;
			prestate.state=tagPreState::EState_MouseMove;
			prestate.mouseMove=m_event;

			m_finishedOnExit=false;
			m_pFSM->Change2LowerState(FSM_LP::ELowerState_Drop,&event,&prestate);
		}
		else if(m_actMouseMove.GetResult()==ActionMouseMove_LP::ER_Slide)
		{
			tagSlideEvent event(_T("tagSlideEvent"),NULL);
			event.start=pRole->GetPos();

			tagPreState prestate;
			prestate.state=tagPreState::EState_MouseMove;
			prestate.mouseMove=m_event;

			m_finishedOnExit=false;
			m_pFSM->Change2LowerState(FSM_LP::ELowerState_Slide,&event,&prestate);
		}
		else if(m_actMouseMove.GetResult()==ActionMouseMove_LP::ER_Swim)
		{
			tagMouseMoveEvent event;
			event.start=pRole->GetPos();
			event.end=m_event.end;
			event.bUserCtrl=false;

			m_finishedOnExit=false;
			m_pFSM->Change2LowerState(FSM_LP::ELowerState_MouseSwim,&event);
		}
		else if(m_actMouseMove.GetResult()==ActionMouseMove_LP::ER_OnWater)
		{
			tagMouseMoveEvent event;
			event.start=pRole->GetPos();
			event.end=m_event.end;
			event.bUserCtrl=false;

			m_finishedOnExit=false;
			m_pFSM->Change2LowerState(FSM_LP::ELowerState_MouseWaterWalk,&event);
		}
		else
		{
			m_pFSM->Change2LowerState_Stand();
		}
	}
}

void LowerStateMouseMove_LP::OnGameEvent( tagGameEvent* pEvent )
{
	if(pEvent->strEventName==_T("tagKeyboardMoveEvent"))//键盘移动游戏事件
	{
		m_pFSM->TryChange2LowerState(FSM_LP::ELowerState_KeyboardMove,pEvent);
	}
	else if(pEvent->strEventName==_T("tagMouseMoveEvent"))//鼠标移动事件
	{
		tagMouseMoveEvent* pMouseMoveEvent=(tagMouseMoveEvent*)pEvent;
		m_finishedOnExit=pMouseMoveEvent->bUserCtrl;
		m_pFSM->TryChange2LowerState(FSM_LP::ELowerState_MouseMove,pEvent);
	}
	else if(pEvent->strEventName==_T("tagJumpEvent"))//跳跃事件
	{
		LocalPlayer* pSelf=m_pFSM->GetLocalPlayer();

		tagJumpEvent* pJumpEvent=(tagJumpEvent*)pEvent;
		pJumpEvent->start=pSelf->GetPos();
		Yaw2Dir(pSelf->GetYaw(),pJumpEvent->dir);

		tagPreState prestate;
		prestate.state=tagPreState::EState_MouseMove;
		prestate.mouseMove=m_event;

		m_finishedOnExit=false;
		m_pFSM->TryChange2LowerState(FSM_LP::ELowerState_Jump,pEvent,&prestate);
	}
	//--脱离战斗-------------------------------------------------------------------
	if(pEvent->strEventName==_T("tagLeaveCombatEvent"))
	{
		if(m_pFSM->IfCanPlayLowerAni())
			m_actMouseMove.ReplayAni(m_pFSM->GetLocalPlayer());
	}
	//--切换武器-------------------------------------------------------------------
	else if(pEvent->strEventName==_T("tagWeaponChangeEvent"))
	{
		tagWeaponChangeEvent* pWeaponChangeEvent = (tagWeaponChangeEvent*)pEvent;
		if( pWeaponChangeEvent->bRightWeapon && m_pFSM->IfCanPlayLowerAni() )
			m_actMouseMove.ReplayAni(m_pFSM->GetLocalPlayer());
	}
}

void LowerStateMouseMove_LP::ShowDecal( bool bShow )
{
	tagShowScenePickDecalEvent event(_T("tagShowScenePickDecalEvent"),NULL);
	event.bShow=bShow;
	TObjRef<GameFrameMgr>()->SendEvent(&event);
}

void LowerStateMouseMove_LP::SendFinishEvent( bool bSuccessed )
{
	tagMouseMoveFinishEvent event;
	event.bSuccessed=bSuccessed;
	TObjRef<GameFrameMgr>()->SendEvent(&event);
}

void LowerStateMouseMove_LP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==pNetCmd->Crc32("NS_MoveSpeedChange"))//速度变化消息
	{
		tagMouseMoveEvent event=m_event;
		event.start=m_pFSM->GetLocalPlayer()->GetPos();
		m_pFSM->TryChange2LowerState(FSM_LP::ELowerState_MouseMove,&event);
	}
	//--瞬移消息
	else if(pNetCmd->dwID==pNetCmd->Crc32("NS_SpecialMove"))
	{
		tagNS_SpecialMove* pCmd=(tagNS_SpecialMove*)pNetCmd;
		if( ESMT_Teleport == pCmd->eType )
		{
			tagMouseMoveEvent event=m_event;
			event.start=pCmd->vDestPos;
			event.bUserCtrl=false;
			OnGameEvent(&event);
		}
	}
}

void LowerStateMouseMove_LP::ReplayAni( LocalPlayer* pRole )
{
	m_actMouseMove.ReplayAni(pRole);
}

bool LowerStateMouseMove_LP::IfCanChange2UpperState( FSM_LP::EUpperState state,tagGameEvent* pEvent )
{
	switch(state)
	{
	case FSM_LP::EUpperState_BeAttack:		//被攻击
		{
			return false;
		}
		break;
	case FSM_LP::EUpperState_StyleAction:
		{
			tagPlayPersonalityActEvent* pStyleEvent = (tagPlayPersonalityActEvent*)pEvent;
			if( tagPersonAct::T_Upper != pStyleEvent->pData->eType )
				return false;
		}
	}

	return true;
}

bool LowerStateMouseMove_LP::IfCanChange2LowerState( FSM_LP::ELowerState state,tagGameEvent* pEvent )
{
	return true;
}

void LowerStateMouseMove_LP::OnExit()
{
	if(m_finishedOnExit)
	{
		ShowDecal(false);
		SendFinishEvent(m_actMouseMove.GetResult()==ActionMouseMove_LP::ER_Arrive);
	}
}
