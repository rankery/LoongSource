#include "StdAfx.h"
#include "LowerStateKeyboardSwim_LP.h"
#include "LocalPlayer.h"
#include "FSM_LP.h"
#include "CombatSysUtil.h"
#include "WorldFrame.h"
#include "CombatEvent.h"
#include "..\WorldDefine\action.h"
#include "PreState.h"
#include "MapMgr.h"

LowerStateKeyboardSwim_LP::LowerStateKeyboardSwim_LP(void)
{}

LowerStateKeyboardSwim_LP::~LowerStateKeyboardSwim_LP(void)
{}

void LowerStateKeyboardSwim_LP::Active( tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole,tagPreState* pPreState )
{
	ASSERT(pEvent->strEventName==_T("tagKeyboardMoveEvent"));
	tagKeyboardMoveEvent* pMoveEvent=(tagKeyboardMoveEvent*)pEvent;

	//--保存event
	m_event=*pMoveEvent;

	m_actKeyboardSwim.Active(pMoveEvent->start,pMoveEvent->dir,pNav,pRole,true);
}

void LowerStateKeyboardSwim_LP::Update( NavMap* pNav,LocalPlayer* pRole )
{
	m_actKeyboardSwim.Update(pNav,pRole);
	if(m_actKeyboardSwim.IsClosed())
	{
		if(m_actKeyboardSwim.GetResult()==ActionKeyboardSwim_LP::ER_Ground)
		{
			tagKeyboardMoveEvent event(_T("tagKeyboardMoveEvent"),NULL);
			event.start=pRole->GetPos();
			event.dir=m_event.dir;

			m_pFSM->Change2LowerState(FSM_LP::ELowerState_KeyboardMove,&event);
		}
		else
		{
			m_pFSM->Change2LowerState_Float();
		}
	}
}

void LowerStateKeyboardSwim_LP::OnGameEvent( tagGameEvent* pEvent )
{
	if(pEvent->strEventName==_T("tagKeyboardMoveEvent"))//新的键盘移动事件
	{
		Role* pRole=m_pFSM->GetLocalPlayer();
		NavMap* pNav=MapMgr::Inst()->GetNavMap();

		if(pNav->GetCollider()->IfWillSwim(pRole->GetPos(),pRole->GetRoleSize().y))
		{
			m_pFSM->TryChange2LowerState(FSM_LP::ELowerState_KeyboardSwim,pEvent);
		}
	}
	else if(pEvent->strEventName==_T("tagStopKeyboardMoveEvent"))//停止键盘移动游戏事件
	{
		m_actKeyboardSwim.Stop(MapMgr::Inst()->GetNavMap(),m_pFSM->GetLocalPlayer());
		m_pFSM->Change2LowerState_AutoStand();
	}
	else if(pEvent->strEventName==_T("tagRotateCameraEvent"))//旋转角色游戏事件
	{
		Role* pRole=m_pFSM->GetLocalPlayer();
		NavMap* pNav=MapMgr::Inst()->GetNavMap();

		if(pNav->GetCollider()->IfWillSwim(pRole->GetPos(),pRole->GetRoleSize().y))
		{
			//重新激活键盘移动行为
			m_actKeyboardSwim.Active(pRole->GetPos(),m_actKeyboardSwim.GetMoveDir(),MapMgr::Inst()->GetNavMap(),m_pFSM->GetLocalPlayer(),false);
		}
	}
	else if(pEvent->strEventName==_T("tagJumpEvent"))//跳跃游戏事件
	{
		tagJumpEvent* pJumpEvent=(tagJumpEvent*)pEvent;
		pJumpEvent->dir=m_actKeyboardSwim.GetMoveVec();

		tagPreState prestate;
		prestate.state=tagPreState::EState_KeyboardMove;
		prestate.keyboardMove=m_event;

		m_pFSM->TryChange2LowerState(FSM_LP::ELowerState_Jump,pEvent,&prestate);
	}
	else if(pEvent->strEventName==_T("tagLPRoleStateChangeEvent"))
	{
		LocalPlayer* pSelf=m_pFSM->GetLocalPlayer();
		if(pSelf->CanWaterWalk())//水上行走
		{
			tagKeyboardMoveEvent evt(_T("tagKeyboardMoveEvent"),NULL);
			evt.start=pSelf->GetPos();
			evt.dir=m_event.dir;

			m_pFSM->Change2LowerState(FSM_LP::ELowerState_KeyboardWaterWalk,&evt);
		}
	}
}

void LowerStateKeyboardSwim_LP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==pNetCmd->Crc32("NS_MoveSpeedChange"))//速度变化消息
	{
		tagKeyboardMoveEvent event=m_event;
		event.start=m_pFSM->GetLocalPlayer()->GetPos();
		m_pFSM->TryChange2LowerState(FSM_LP::ELowerState_KeyboardSwim,&event);
	}
	//--瞬移消息
	else if(pNetCmd->dwID==pNetCmd->Crc32("NS_SpecialMove"))
	{
		tagNS_SpecialMove* pCmd=(tagNS_SpecialMove*)pNetCmd;
		if( ESMT_Teleport == pCmd->eType )
		{
			tagKeyboardMoveEvent event = m_event;
			event.start = pCmd->vDestPos;
			OnGameEvent( &event );
		}
	}
}

void LowerStateKeyboardSwim_LP::OnExit()
{
	//--停止音效
	m_actKeyboardSwim.StopSound();
}

bool LowerStateKeyboardSwim_LP::IfCanChange2UpperState( FSM_LP::EUpperState state,tagGameEvent* pEvent )
{
	switch(state)
	{
	case FSM_LP::EUpperState_Idle:		//空闲
	case FSM_LP::EUpperState_Dead:		//死亡
	case FSM_LP::EUpperState_Dazzy:		//击晕
	case FSM_LP::EUpperState_Tie:		//定身
	case FSM_LP::EUpperState_Sopor:		//昏睡
	case FSM_LP::EUpperState_Follow:	//跟随
	case FSM_LP::EUpperState_PickupItem://拾取
	case FSM_LP::EUpperState_TalkNPC:	//与NPC对话
	case FSM_LP::EUpperState_UseItem:	//使用物品
		{
			return true;
		}
		break;
	}

	return false;
}

bool LowerStateKeyboardSwim_LP::IfCanChange2LowerState( FSM_LP::ELowerState state,tagGameEvent* pEvent )
{
	return true;
}

void LowerStateKeyboardSwim_LP::ReplayAni( LocalPlayer* pRole )
{
	m_actKeyboardSwim.ReplayAni(pRole);
}