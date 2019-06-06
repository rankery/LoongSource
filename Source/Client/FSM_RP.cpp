#include "StdAfx.h"
#include "FSM_RP.h"
#include "Player.h"
#include "MapMgr.h"
#include "CombatSysUtil.h"

#include "LowerStateMove_RP.h"
#include "LowerStateSwim_RP.h"
#include "LowerStateWaterWalk_RP.h"
#include "LowerStateJump_RP.h"
#include "LowerStateDrop_RP.h"
#include "LowerStateVDrop_RP.h"
#include "LowerStateSlide_RP.h"
#include "LowerStateStand_RP.h"
#include "LowerStateFloat_RP.h"
#include "LowerStateWaterStand_RP.h"
#include "LowerStateHitFly_RP.h"
#include "LowerStateRepel_RP.h"
#include "LowerStateAssault_RP.h"
#include "LowerStatePull_RP.h"

#include "UpperStateIdle_RP.h"
#include "UpperStateDead_RP.h"
#include "UpperStateSkill_RP.h"
#include "UpperStateUseItem_RP.h"
#include "UpperStateStyleAction_RP.h"
#include "UpperStateBeAttack_RP.h"
#include "UpperStateDazzy_RP.h"
#include "UpperStateTie_RP.h"
#include "UpperStateSopor_RP.h"

#include "NetCmdHandler_RP.h"
#include "GameEventHandler_RP.h"

#include "GlobalStateCombat_RP.h"

#include "ActionStateNormal_RP.h"
#include "ActionStateRide_RP.h"
#include "ActionStateBoard_RP.h"



FSM_RP::FSM_RP(Player* pPlayer)
: m_pPlayer(pPlayer)
, m_activeLowerState(ELowerState_None)
, m_activeUpperState(EUpperState_None)
, m_activeActionState(EActionState_None)
{
	memset(m_upperStates,0,sizeof(m_upperStates));
	memset(m_lowerStates,0,sizeof(m_lowerStates));
	memset(m_actionStates,0,sizeof(m_actionStates));
	memset(m_globalStates,0,sizeof(m_globalStates));
	memset(m_gameEventHandlers,0,sizeof(m_gameEventHandlers));
}

FSM_RP::~FSM_RP(void)
{
	Destroy();
}

inline NavMap* GetNav()
{
	return MapMgr::Inst()->GetNavMap();
}

void FSM_RP::Init()
{
	Destroy();

	//--初始化全局状态
	m_globalStates[EGlobalState_Combat]=new GlobalStateCombat_RP;
	m_globalStates[EGlobalState_Combat]->SetFSM(this);

	//--初始化下身状态
	m_lowerStates[ELowerState_Move]=new LowerStateMove_RP;
	m_lowerStates[ELowerState_Move]->SetFSM(this);
	
	m_lowerStates[ELowerState_Swim]=new LowerStateSwim_RP;
	m_lowerStates[ELowerState_Swim]->SetFSM(this);

	m_lowerStates[ELowerState_WaterWalk]=new LowerStateWaterWalk_RP;
	m_lowerStates[ELowerState_WaterWalk]->SetFSM(this);

	m_lowerStates[ELowerState_Jump]=new LowerStateJump_RP;
	m_lowerStates[ELowerState_Jump]->SetFSM(this);

	m_lowerStates[ELowerState_Drop]=new LowerStateDrop_RP;
	m_lowerStates[ELowerState_Drop]->SetFSM(this);

	m_lowerStates[ELowerState_VDrop]=new LowerStateVDrop_RP;
	m_lowerStates[ELowerState_VDrop]->SetFSM(this);

	m_lowerStates[ELowerState_Slide]=new LowerStateSlide_RP;
	m_lowerStates[ELowerState_Slide]->SetFSM(this);

	m_lowerStates[ELowerState_Stand]=new LowerStateStand_RP;
	m_lowerStates[ELowerState_Stand]->SetFSM(this);

	m_lowerStates[ELowerState_Float]=new LowerStateFloat_RP;
	m_lowerStates[ELowerState_Float]->SetFSM(this);

	m_lowerStates[ELowerState_WaterStand]=new LowerStateWaterStand_RP;
	m_lowerStates[ELowerState_WaterStand]->SetFSM(this);

	m_lowerStates[ELowerState_HitFly]=new LowerStateHitFly_RP;
	m_lowerStates[ELowerState_HitFly]->SetFSM(this);

	m_lowerStates[ELowerState_Repel]=new LowerStateRepel_RP;
	m_lowerStates[ELowerState_Repel]->SetFSM(this);

	m_lowerStates[ELowerState_Assault]=new LowerStateAssault_RP;
	m_lowerStates[ELowerState_Assault]->SetFSM(this);

	m_lowerStates[ELowerState_Pull]=new LowerStatePull_RP;
	m_lowerStates[ELowerState_Pull]->SetFSM(this);

	m_activeLowerState=ELowerState_None;

	//--初始化上身状态
	m_upperStates[EUpperState_Idle]=new UpperStateIdle_RP;
	m_upperStates[EUpperState_Idle]->SetFSM(this);

	m_upperStates[EUpperState_Dead]=new UpperStateDead_RP;
	m_upperStates[EUpperState_Dead]->SetFSM(this);

	m_upperStates[EUpperState_Skill]=new UpperStateSkill_RP;
	m_upperStates[EUpperState_Skill]->SetFSM(this);

	m_upperStates[EUpperState_UseItem]=new UpperStateUseItem_RP;
	m_upperStates[EUpperState_UseItem]->SetFSM(this);

	m_upperStates[EUpperState_StyleAction]=new UpperStateStyleAction_RP;
	m_upperStates[EUpperState_StyleAction]->SetFSM(this);

	m_upperStates[EUpperState_BeAttack]=new UpperStateBeAttack_RP;
	m_upperStates[EUpperState_BeAttack]->SetFSM(this);

	m_upperStates[EUpperState_Dazzy]=new UpperStateDazzy_RP;
	m_upperStates[EUpperState_Dazzy]->SetFSM(this);

	m_upperStates[EUpperState_Tie]=new UpperStateTie_RP;
	m_upperStates[EUpperState_Tie]->SetFSM(this);

	m_upperStates[EUpperState_Sopor]=new UpperStateSopor_RP;
	m_upperStates[EUpperState_Sopor]->SetFSM(this);

	m_activeUpperState=EUpperState_None;

	//--初始化动作状态
	m_actionStates[EActionState_Normal]=new ActionStateNormal_RP;
	m_actionStates[EActionState_Normal]->SetFSM(this);

	m_actionStates[EActionState_Ride]=new ActionStateRide_RP;
	m_actionStates[EActionState_Ride]->SetFSM(this);

	m_actionStates[EActionState_Board]=new ActionStateBoard_RP;
	m_actionStates[EActionState_Board]->SetFSM(this);

	m_activeActionState=EActionState_Normal;
	m_actionStates[EActionState_Normal]->Active(GetNav(),m_pPlayer);

	//--初始化网络消息处理器
	NetCmdHandler_RP* pNetCmdHandler=new NS_SyncStandHandler_RP;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler=new NS_StopActionHandler_RP;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler=new NS_HPChangeHandler_RP;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler=new NS_RoleDeadHandler_RP;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler=new NS_HitFlyHandler_RP;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler=new NS_SpecialMove_RP;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler=new NS_AddRoleBuffHandler_RP;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler=new NS_SetStateHandler_RP;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler=new NS_HitTargetHandler_RP;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler=new NS_SyncGroundItemHandler_RP;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler=new NS_RoleLevelChangeHandler_RP;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler=new NS_UseItemHandler_RP;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler=new NS_SetRoleStateHandler_RP;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	//--初始化游戏事件处理器
	m_gameEventHandlers[EGameEventHandler_HitTarget]=new HitTargetEventHandler_RP;
	m_gameEventHandlers[EGameEventHandler_HitTarget]->SetFSM(this);
}

void FSM_RP::Destroy()
{
	int i;

	for(i=0;i<EUpperState_Num;i++)
		SAFE_DELETE(m_upperStates[i]);

	for(i=0;i<ELowerState_Num;i++)
		SAFE_DELETE(m_lowerStates[i]);

	for(i=0;i<EActionState_Num;i++)
		SAFE_DELETE(m_actionStates[i]);

	for(i=0;i<EGlobalState_Num;i++)
		SAFE_DELETE(m_globalStates[i]);

	for(i=0;i<(int)m_netCmdHandlers.size();i++)
		SAFE_DELETE(m_netCmdHandlers[i]);
	m_netCmdHandlers.clear();

	for(i=0;i<EGameEventHandler_Num;i++)
		SAFE_DELETE(m_gameEventHandlers[i]);
}

void FSM_RP::Update()
{
	//--update action state
	m_actionStates[m_activeActionState]->Update(NULL,m_pPlayer);

	//--update net cmd handlers
	int i;
	for(i=0;i<(int)m_netCmdHandlers.size();i++)
		m_netCmdHandlers[i]->Update();

	//--update global states
	for(i=0;i<EGlobalState_Num;i++)
	{
		if(!m_globalStates[i]->IsClosed())
			m_globalStates[i]->Update(GetNav(),m_pPlayer);
	}

	//--update lower state
	if(m_activeLowerState!=ELowerState_None)
		m_lowerStates[m_activeLowerState]->Update(GetNav(),m_pPlayer);

	//--update upper state
	if(m_activeUpperState!=EUpperState_None)
		m_upperStates[m_activeUpperState]->Update(GetNav(),m_pPlayer);
}

void FSM_RP::OnNetCmd( tagNetCmd* pNetCmd )
{
	int i;

	//--action state
	m_actionStates[m_activeActionState]->OnNetCmd(pNetCmd);

	//--net cmd handlers
	for(i=0;i<(int)m_netCmdHandlers.size();i++)
	{
		if(!m_netCmdHandlers[i]->OnNetCmd(pNetCmd))
			return;
	}

	//--game event handlers
	for(i=0;i<EGameEventHandler_Num;i++)
		m_gameEventHandlers[i]->OnNetCmd(pNetCmd);

	//--global states
	for(i=0;i<EGlobalState_Num;i++)
	{
		if(!m_globalStates[i]->IsClosed())
			m_globalStates[i]->OnNetCmd(pNetCmd);
	}

	//--upper state
	if(m_activeUpperState==EUpperState_None)
		Change2LowerState(pNetCmd);
	else
		m_upperStates[m_activeUpperState]->OnNetCmd(pNetCmd);

	//--lower state
	if(m_activeLowerState==ELowerState_None)
		Change2UpperState(pNetCmd);
	else
		m_lowerStates[m_activeLowerState]->OnNetCmd(pNetCmd);

}

void FSM_RP::OnGameEvent( tagGameEvent* pEvent )
{
	int i;

	//--action state
	m_actionStates[m_activeActionState]->OnGameEvent(pEvent);

	//--game event handlers
	for(i=0;i<EGameEventHandler_Num;i++)
	{
		m_gameEventHandlers[i]->OnGameEvent(pEvent);
	}

	//--net cmd handlers
	for(i=0;i<(int)m_netCmdHandlers.size();i++)
		m_netCmdHandlers[i]->OnGameEvent(pEvent);

	//--global states
	for(i=0;i<EGlobalState_Num;i++)
	{
		if(!m_globalStates[i]->IsClosed())
			m_globalStates[i]->OnGameEvent(pEvent);
	}

	//--upper state
	if(m_activeUpperState!=EUpperState_None)
		m_upperStates[m_activeUpperState]->OnGameEvent(pEvent);

	//--lower state
	if(m_activeLowerState!=ELowerState_None)
		m_lowerStates[m_activeLowerState]->OnGameEvent(pEvent);
}

void FSM_RP::Change2LowerState( ELowerState state,tagNetCmd* pNetCmd )
{
	//--通知原状态
	if(m_activeLowerState!=ELowerState_None)
		m_lowerStates[m_activeLowerState]->OnExit();

	//--切换到新状态
	m_activeLowerState=state;
	m_lowerStates[m_activeLowerState]->Active(pNetCmd,GetNav(),m_pPlayer);

	//--通知上身状态
	if(m_activeUpperState!=EUpperState_None)
		m_upperStates[m_activeUpperState]->OnChangeLowerState(state);
}

void FSM_RP::Change2LowerState( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_SyncWalk"))
	{
		tagNS_SyncWalk* pCmd=(tagNS_SyncWalk*)pNetCmd;
		if(pCmd->bSwim)
		{
			if(GetPlayer()->CanWaterWalk())
				Change2LowerState(ELowerState_WaterWalk,pNetCmd);
			else
				Change2LowerState(ELowerState_Swim,pNetCmd);
		}
		else
			Change2LowerState(ELowerState_Move,pNetCmd);
	}
	else if(pNetCmd->dwID==m_pUtil->Crc32("NS_SyncJump"))
	{
		Change2LowerState(ELowerState_Jump,pNetCmd);
	}
	else if(pNetCmd->dwID==m_pUtil->Crc32("NS_SyncDrop"))
	{
		Change2LowerState(ELowerState_Drop,pNetCmd);
	}
	else if(pNetCmd->dwID==m_pUtil->Crc32("NS_SyncVDrop"))
	{
		Change2LowerState(ELowerState_VDrop,pNetCmd);
	}
	else if(pNetCmd->dwID==m_pUtil->Crc32("NS_SyncSlide"))
	{
		Change2LowerState(ELowerState_Slide,pNetCmd);
	}
}

void FSM_RP::Change2LowerState_Stand()
{
	Change2LowerState(ELowerState_Stand,NULL);
}

void FSM_RP::Change2LowerState_Float()
{
	Change2LowerState(ELowerState_Float,NULL);
}

void FSM_RP::Change2LowerState_WaterStand()
{
	Change2LowerState(ELowerState_WaterStand,NULL);
}

void FSM_RP::Change2LowerState_AutoStand()
{
	if(GetNav()->GetCollider()->IfWillOnWater(m_pPlayer->GetPos()) && m_pPlayer->CanWaterWalk())
	{
		Change2LowerState(ELowerState_WaterStand,NULL);
	}
	else if(GetNav()->GetCollider()->IfWillSwim(m_pPlayer->GetPos(),m_pPlayer->GetRoleSize().y))
	{
		Change2LowerState(ELowerState_Float,NULL);
	}
	else
	{
		Change2LowerState(ELowerState_Stand,NULL);	
	}
}

void FSM_RP::Change2UpperState( EUpperState state,tagNetCmd* pNetCmd )
{
	//--通知原状态
	if(m_activeUpperState!=EUpperState_None)
		m_upperStates[m_activeUpperState]->OnExit();

	//--切换到新状态
	m_activeUpperState=state;
	m_upperStates[m_activeUpperState]->Active(pNetCmd,GetNav(),m_pPlayer);
}

void FSM_RP::Change2UpperState( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_Skill"))
	{
		Change2UpperState(EUpperState_Skill,pNetCmd);
	}
	else if(pNetCmd->dwID==m_pUtil->Crc32("NS_UseItem"))
	{
		Change2UpperState(EUpperState_UseItem,pNetCmd);
	}
	else if(pNetCmd->dwID==m_pUtil->Crc32("NS_RoleStyleAction"))
	{
		Change2UpperState(EUpperState_StyleAction,pNetCmd);
	}
}

void FSM_RP::Change2UpperState_Idle()
{
	Change2UpperState(EUpperState_Idle,NULL);
}

void FSM_RP::Change2UpperState_Dead( bool bPlayAni )
{
	//--通知原状态
	if(m_activeUpperState!=EUpperState_None)
		m_upperStates[m_activeUpperState]->OnExit();

	//--切换到新状态
	m_activeUpperState=EUpperState_Dead;
	((UpperStateDead_RP*)m_upperStates[m_activeUpperState])->Active(bPlayAni,m_pPlayer);
}

bool FSM_RP::TryChange2UpperState( EUpperState state,tagGameEvent* pEvent )
{
	//--询问下身状态是否可以切换
	if(m_activeLowerState!=ELowerState_None
		&&!m_lowerStates[m_activeLowerState]->IfCanChange2UpperState(state))
		return false;

	//--询问动作状态是否可以切换
	if(!m_actionStates[m_activeActionState]->IfCanChange2UpperState(state))
		return false;

	//--通知原状态
	if(m_activeUpperState!=ELowerState_None)
		m_upperStates[m_activeUpperState]->OnExit();

	//--切换到新状态
	m_activeUpperState=state;
	m_upperStates[m_activeUpperState]->Active(pEvent,GetNav(),m_pPlayer);

	return true;
}

void FSM_RP::Change2ActionState( EActionState state )
{
	//--通知原状态
	if(m_activeActionState!=EActionState_None)
		m_actionStates[m_activeActionState]->OnExit();

	//--切换到新状态
	m_activeActionState=state;
	m_actionStates[m_activeActionState]->Active(GetNav(),m_pPlayer);
}

bool FSM_RP::IfCanPlayLowerAni()
{
	if(m_activeUpperState==EUpperState_None)
		return true;

	return m_upperStates[m_activeUpperState]->IfCanPlayLowerAni();
}

void FSM_RP::ReplayLowerAni()
{
	if(m_activeLowerState!=ELowerState_None)
		m_lowerStates[m_activeLowerState]->ReplayAni(m_pPlayer);
}

bool FSM_RP::IfLowerCanSetYaw()
{
	if(m_activeUpperState==EUpperState_None)
		return true;

	return m_upperStates[m_activeUpperState]->IfLowerCanSetYaw();
}

void FSM_RP::ActiveGlobalState( EGlobalState state,tagNetCmd* pNetCmd,DWORD dwParam )
{
	if(m_globalStates[state]->IsClosed())
		m_globalStates[state]->Active(pNetCmd,GetNav(),m_pPlayer,dwParam);
}

bool FSM_RP::IsReadyState()
{
	return !m_globalStates[EGlobalState_Combat]->IsClosed();
}

tagHitTargetEvent* FSM_RP::GetLastHitTargetEvent( DWORD roleID )
{
	HitTargetEventHandler_RP* pHandler=(HitTargetEventHandler_RP*)m_gameEventHandlers[EGameEventHandler_HitTarget];
	return pHandler->GetLastHitTargetEvent(roleID);
}