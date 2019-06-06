#include "StdAfx.h"
#include "LowerStatePursue_LP.h"
#include "LocalPlayer.h"
#include "FSM_LP.h"
#include "WorldPickerFrame.h"
#include "PreState.h"
#include "CombatSysUtil.h"
#include "RoleMgr.h"
#include "MapMgr.h"
#include "..\WorldDefine\action.h"
#include "RoleEvent.h"
#include "PersonalityActData.h"

LowerStatePursue_LP::LowerStatePursue_LP(void)
{}

LowerStatePursue_LP::~LowerStatePursue_LP(void)
{}

void LowerStatePursue_LP::Active( tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole,tagPreState* pPreState )
{
	ASSERT(pEvent->strEventName==_T("tagPursueEvent"));
	tagPursueEvent* pPursueEvent=(tagPursueEvent*)pEvent;

	//--保存event
	m_event=*pPursueEvent;
	m_validDistSq=m_event.validDist*m_event.validDist;

	//--
	if(m_event.targetType==tagPursueEvent::ETargetType_Pos)//移动到指定位置
	{
		m_actMouseMove.Active(pRole->GetPos(),m_event.targetPos,m_event.validDist*0.9f,pNav,pRole,true,true,m_event.bNeedSendStopCmd);
	}
	else//追踪角色
	{
		Role* pTarget=RoleMgr::Inst()->FindRole(m_event.targetRoleID);
		if(pTarget==NULL)
		{
			m_pFSM->Change2LowerState_AutoStand();
			SendEvent(false);
			return;
		}

		m_targetPos=pTarget->GetPos();
		m_actMouseMove.Active(pRole->GetPos(),m_targetPos,m_event.validDist*0.9f,pNav,pRole,true,true,m_event.bNeedSendStopCmd);
		
		m_lastCheckPosTime=Kernel::Inst()->GetAccumTime();
	}
}

void LowerStatePursue_LP::Update(NavMap* pNav,LocalPlayer* pRole)
{
	//--
	m_actMouseMove.Update(pNav,pRole,true,true);

	//--
	if(m_event.targetType==tagPursueEvent::ETargetType_Role)//追踪角色
	{
		//--查找角色
		Role* pTarget=RoleMgr::Inst()->FindRole(m_event.targetRoleID);
		if(pTarget==NULL)
		{
			SendEvent(false);
			m_pFSM->Change2LowerState_AutoStand();
			return;
		}

		//--检查是否接近目标
		if(Vec3DistSq(pRole->GetPos(),pTarget->GetPos())<=m_validDistSq)
		{
			if(m_event.bNeedSendStopCmd)
				SendStopCmd(pRole->GetPos());

			SendEvent(true);
			m_pFSM->Change2LowerState_AutoStand();
			return;
		}

		//--如果目标正在移动，需要重新追踪
		if(Kernel::Inst()->GetAccumTime()-m_lastCheckPosTime>0.2f)
		{
			m_lastCheckPosTime=Kernel::Inst()->GetAccumTime();

			if(Vec3DistSq(m_targetPos,pTarget->GetPos())>1.0f)
			{
				m_targetPos=pTarget->GetPos();
				m_actMouseMove.Active(pRole->GetPos(),m_targetPos,m_event.validDist*0.9f,pNav,pRole,true,true,m_event.bNeedSendStopCmd);
				return;
			}
		}
	}

	//--
	if(m_actMouseMove.IsClosed())
	{
		//--掉落
		if(m_actMouseMove.GetResult()==ActionMouseMove_LP::ER_Drop)
		{
			tagDropEvent event(_T("tagDropEvent"),NULL);
			event.start=pRole->GetPos();
			Yaw2Dir(pRole->GetYaw(),event.dir);

			tagPreState prestate;
			prestate.state=tagPreState::EState_Pursue;
			prestate.pursue=m_event;

			m_pFSM->Change2LowerState(FSM_LP::ELowerState_Drop,&event,&prestate);
		}
		//--滑落
		else if(m_actMouseMove.GetResult()==ActionMouseMove_LP::ER_Slide)
		{
			tagSlideEvent event(_T("tagSlideEvent"),NULL);
			event.start=pRole->GetPos();

			tagPreState prestate;
			prestate.state=tagPreState::EState_Pursue;
			prestate.pursue=m_event;

			m_pFSM->Change2LowerState(FSM_LP::ELowerState_Slide,&event,&prestate);
		}
		//--游泳
		else if(m_actMouseMove.GetResult()==ActionMouseMove_LP::ER_Swim)
		{
			SendEvent(false);
			m_pFSM->Change2LowerState_Float();
		}
		//--水上行走
		else if(m_actMouseMove.GetResult()==ActionMouseMove_LP::ER_OnWater)
		{
			SendEvent(false);
			m_pFSM->Change2LowerState_WaterStand();
		}
		//--到达
		else if(m_actMouseMove.GetResult()==ActionMouseMove_LP::ER_Arrive)
		{
			if(m_event.targetType==tagPursueEvent::ETargetType_Pos)
			{
				SendEvent(true);
				m_pFSM->Change2LowerState_AutoStand();
			}
		}
		//--无法到达
		else
		{
			SendEvent(false);
			m_pFSM->Change2LowerState_AutoStand();
		}
	}
}

void LowerStatePursue_LP::OnGameEvent( tagGameEvent* pEvent )
{
	if(pEvent->strEventName==_T("tagKeyboardMoveEvent"))//键盘移动
	{
		m_pFSM->TryChange2LowerState(FSM_LP::ELowerState_KeyboardMove,pEvent);
	}
	else if(pEvent->strEventName==_T("tagMouseMoveEvent"))//鼠标移动
	{
		m_pFSM->TryChange2LowerState(FSM_LP::ELowerState_MouseMove,pEvent);
	}
	else if(pEvent->strEventName==_T("tagJumpEvent"))//跳跃
	{
		LocalPlayer* pSelf=m_pFSM->GetLocalPlayer();

		tagJumpEvent* pJumpEvent=(tagJumpEvent*)pEvent;
		pJumpEvent->start=pSelf->GetPos();
		Yaw2Dir(pSelf->GetYaw(),pJumpEvent->dir);

		tagPreState prestate;
		prestate.state=tagPreState::EState_Pursue;
		prestate.pursue=m_event;

		m_pFSM->TryChange2LowerState(FSM_LP::ELowerState_Jump,pEvent,&prestate);
	}
	//--脱离战斗-------------------------------------------------------------------
	if(pEvent->strEventName==_T("tagLeaveCombatEvent"))
	{
		m_actMouseMove.ReplayAni(m_pFSM->GetLocalPlayer());
	}
	//--切换武器-------------------------------------------------------------------
	else if(pEvent->strEventName==_T("tagWeaponChangeEvent"))
	{
		tagWeaponChangeEvent* pWeaponChangeEvent = (tagWeaponChangeEvent*)pEvent;
		if( pWeaponChangeEvent->bRightWeapon )
			m_actMouseMove.ReplayAni(m_pFSM->GetLocalPlayer());
	}
	else if(pEvent->strEventName==_T("tagStopPursueEvent"))//停止追踪
	{
		m_actMouseMove.Stop(m_pFSM->GetLocalPlayer());
		m_pFSM->Change2LowerState_AutoStand();
	}
}

void LowerStatePursue_LP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==pNetCmd->Crc32("NS_MoveSpeedChange"))//速度变化
	{
		Active(&m_event,MapMgr::Inst()->GetNavMap(),RoleMgr::Inst()->GetLocalPlayer());
	}
	//--瞬移消息
	else if(pNetCmd->dwID==pNetCmd->Crc32("NS_SpecialMove"))
	{
		tagNS_SpecialMove* pCmd=(tagNS_SpecialMove*)pNetCmd;
		if( ESMT_Teleport == pCmd->eType )
		{
			tagPursueEvent event=m_event;
			event.start=pCmd->vDestPos;
			Active(&event,MapMgr::Inst()->GetNavMap(),RoleMgr::Inst()->GetLocalPlayer());
		}
	}
}

bool LowerStatePursue_LP::IfCanChange2UpperState( FSM_LP::EUpperState state,tagGameEvent* pEvent )
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

bool LowerStatePursue_LP::IfCanChange2LowerState( FSM_LP::ELowerState state,tagGameEvent* pEvent )
{
	return true;
}

void LowerStatePursue_LP::ReplayAni( LocalPlayer* pRole )
{
	m_actMouseMove.ReplayAni(pRole);
}

void LowerStatePursue_LP::SendEvent( bool bSuccess )
{
	tagPursueFinishEvent event;
	event.bSuccessed=bSuccess;
	m_pFSM->OnGameEvent(&event);
}

void LowerStatePursue_LP::SendStopCmd(const Vector3& pos)
{
	tagNC_StopWalk cmd;
	cmd.curPos=pos;
	cmd.bSwim=false;
	TObjRef<NetSession>()->Send(&cmd);
}