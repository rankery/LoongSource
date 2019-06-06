#include "StdAfx.h"
#include "FSM_LP.h"
#include "LocalPlayer.h"
#include "MapMgr.h"
#include "CombatSysUtil.h"

#include "LowerStateMouseMove_LP.h"
#include "LowerStateMouseSwim_LP.h"
#include "LowerStateMouseWaterWalk_LP.h"
#include "LowerStateKeyboardMove_LP.h"
#include "LowerStateKeyboardSwim_LP.h"
#include "LowerStateKeyboardWaterWalk_LP.h"
#include "LowerStateStand_LP.h"
#include "LowerStateFloat_LP.h"
#include "LowerStateWaterStand_LP.h"
#include "LowerStateJump_LP.h"
#include "LowerStateDrop_LP.h"
#include "LowerStateVDrop_LP.h"
#include "LowerStateSlide_LP.h"
#include "LowerStatePursue_LP.h"
#include "LowerStateHitFly_LP.h"
#include "LowerStateRepel_LP.h"
#include "LowerStateAssault_LP.h"
#include "LowerStatePull_LP.h"

#include "UpperStateIdle_LP.h"
#include "UpperStateSkill_LP.h"
#include "UpperStateUseItem_LP.h"
#include "UpperStateTalkNPC_LP.h"
#include "UpperStateFollow_LP.h"
#include "UpperStatePickup_LP.h"
#include "UpperStateBeAttack_LP.h"
#include "UpperStateDazzy_LP.h"
#include "UpperStateTie_LP.h"
#include "UpperStateSopor_LP.h"
#include "UpperStateStyleAction_LP.h"
#include "UpperStateDead_LP.h"

#include "ActionStateNormal_LP.h"
#include "ActionStateRide_LP.h"
#include "ActionStateBoard_LP.h"

#include "GlobalStateCombat_LP.h"

#include "NetCmdHandler_LP.h"
#include "GameEventHandler_LP.h"


FSM_LP::FSM_LP(LocalPlayer* pLocalPlayer)
: m_pLocalPlayer(pLocalPlayer)
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

FSM_LP::~FSM_LP(void)
{
	Destroy();
}

inline NavMap* GetNav()
{
	return MapMgr::Inst()->GetNavMap();
}

void FSM_LP::Init()
{
	Destroy();

	//--初始化上身状态
	m_upperStates[EUpperState_Dead]=new UpperStateDead_LP;
	m_upperStates[EUpperState_Dead]->SetFSM(this);

	m_upperStates[EUpperState_Skill]=new UpperStateSkill_LP;
	m_upperStates[EUpperState_Skill]->SetFSM(this);

	m_upperStates[EUpperState_UseItem]=new UpperStateUseItem_LP;
	m_upperStates[EUpperState_UseItem]->SetFSM(this);

	m_upperStates[EUpperState_TalkNPC]=new UpperStateTalkNPC_LP;
	m_upperStates[EUpperState_TalkNPC]->SetFSM(this);

	m_upperStates[EUpperState_PickupItem]=new UpperStatePickup_LP;
	m_upperStates[EUpperState_PickupItem]->SetFSM(this);

	m_upperStates[EUpperState_Follow]=new UpperStateFollow_LP;
	m_upperStates[EUpperState_Follow]->SetFSM(this);

	m_upperStates[EUpperState_BeAttack]=new UpperStateBeAttack_LP;
	m_upperStates[EUpperState_BeAttack]->SetFSM(this);

	m_upperStates[EUpperState_StyleAction]=new UpperStateStyleAction_LP;
	m_upperStates[EUpperState_StyleAction]->SetFSM(this);

	m_upperStates[EUpperState_Idle]=new UpperStateIdle_LP;
	m_upperStates[EUpperState_Idle]->SetFSM(this);

	m_upperStates[EUpperState_Dazzy]=new UpperStateDazzy_LP;
	m_upperStates[EUpperState_Dazzy]->SetFSM(this);

	m_upperStates[EUpperState_Tie]=new UpperStateTie_LP;
	m_upperStates[EUpperState_Tie]->SetFSM(this);

	m_upperStates[EUpperState_Sopor]=new UpperStateSopor_LP;
	m_upperStates[EUpperState_Sopor]->SetFSM(this);

	m_activeUpperState=EUpperState_None;

	//--初始化下身状态
	m_lowerStates[ELowerState_Stand]=new LowerStateStand_LP;
	m_lowerStates[ELowerState_Stand]->SetFSM(this);

	m_lowerStates[ELowerState_Float]=new LowerStateFloat_LP;
	m_lowerStates[ELowerState_Float]->SetFSM(this);

	m_lowerStates[ELowerState_WaterStand]=new LowerStateWaterStand_LP;
	m_lowerStates[ELowerState_WaterStand]->SetFSM(this);

	m_lowerStates[ELowerState_MouseMove]=new LowerStateMouseMove_LP;
	m_lowerStates[ELowerState_MouseMove]->SetFSM(this);

	m_lowerStates[ELowerState_MouseSwim]=new LowerStateMouseSwim_LP;
	m_lowerStates[ELowerState_MouseSwim]->SetFSM(this);

	m_lowerStates[ELowerState_MouseWaterWalk]=new LowerStateMouseWaterWalk_LP;
	m_lowerStates[ELowerState_MouseWaterWalk]->SetFSM(this);

	m_lowerStates[ELowerState_KeyboardMove]=new LowerStateKeyboardMove_LP;
	m_lowerStates[ELowerState_KeyboardMove]->SetFSM(this);

	m_lowerStates[ELowerState_KeyboardSwim]=new LowerStateKeyboardSwim_LP;
	m_lowerStates[ELowerState_KeyboardSwim]->SetFSM(this);

	m_lowerStates[ELowerState_KeyboardWaterWalk]=new LowerStateKeyboardWaterWalk_LP;
	m_lowerStates[ELowerState_KeyboardWaterWalk]->SetFSM(this);

	m_lowerStates[ELowerState_Jump]=new LowerStateJump_LP;
	m_lowerStates[ELowerState_Jump]->SetFSM(this);

	m_lowerStates[ELowerState_Drop]=new LowerStateDrop_LP;
	m_lowerStates[ELowerState_Drop]->SetFSM(this);

	m_lowerStates[ELowerState_VDrop]=new LowerStateVDrop_LP;
	m_lowerStates[ELowerState_VDrop]->SetFSM(this);

	m_lowerStates[ELowerState_Slide]=new LowerStateSlide_LP;
	m_lowerStates[ELowerState_Slide]->SetFSM(this);

	m_lowerStates[ELowerState_HitFly]=new LowerStateHitFly_LP;
	m_lowerStates[ELowerState_HitFly]->SetFSM(this);

	m_lowerStates[ELowerState_Repel]=new LowerStateRepel_LP;
	m_lowerStates[ELowerState_Repel]->SetFSM(this);

	m_lowerStates[ELowerState_Assault]=new LowerStateAssault_LP;
	m_lowerStates[ELowerState_Assault]->SetFSM(this);

	m_lowerStates[ELowerState_Pull]=new LowerStatePull_LP;
	m_lowerStates[ELowerState_Pull]->SetFSM(this);

	m_lowerStates[ELowerState_Pursue]=new LowerStatePursue_LP;
	m_lowerStates[ELowerState_Pursue]->SetFSM(this);

	m_activeLowerState=ELowerState_None;

	//--初始化全局状态
	m_globalStates[EGlobalState_Combat]=new GlobalStateCombat_LP;
	m_globalStates[EGlobalState_Combat]->SetFSM(this);

	//--初始化动作状态
	m_actionStates[EActionState_Normal]=new ActionStateNormal_LP;
	m_actionStates[EActionState_Normal]->SetFSM(this);

	m_actionStates[EActionState_Ride]=new ActionStateRide_LP;
	m_actionStates[EActionState_Ride]->SetFSM(this);

	m_actionStates[EActionState_Board]=new ActionStateBoard_LP;
	m_actionStates[EActionState_Board]->SetFSM(this);

	m_actionStates[EActionState_Normal]->Active(GetNav(),m_pLocalPlayer);
	m_activeActionState=EActionState_Normal;

	//--初始化网络消息处理器
	NetCmdHandler_LP* pNetCmdHandler=new NS_MoveFailedHandler_LP;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler=new NS_HPChangeHandler_LP;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler=new NS_RoleDeadHandler_LP;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler=new NS_HitFlyHandler_LP;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler=new NS_SpecialMove_LP;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler=new NS_StopActionHandler_LP;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler=new NS_AddRoleBuffHandler_LP;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler=new NS_SetStateHandler_LP;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler=new NS_HitTargetHandler_LP;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler=new NS_SyncGroundItemHandler_LP;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler=new NS_RoleExpChangeHandler_LP;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler=new NS_RoleLevelChangeHandler_LP;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler=new NS_CompleteFishHandler_LP;
	pNetCmdHandler->SetFSM( this );
	m_netCmdHandlers.push_back( pNetCmdHandler);

	//--初始化游戏事件处理器
	m_gameEventHandlers[EGameEventHandler_HitTarget]=new HitTargetEventHandler_LP;
	m_gameEventHandlers[EGameEventHandler_HitTarget]->SetFSM(this);

	m_gameEventHandlers[EGameEventHandler_UseItem]=new UseItemEventHandler_LP;
	m_gameEventHandlers[EGameEventHandler_UseItem]->SetFSM(this);
}

void FSM_LP::Destroy()
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

void FSM_LP::Update()
{
	//--update action state
	m_actionStates[m_activeActionState]->Update(NULL,m_pLocalPlayer);

	//--update net cmd handlers
	int i;
	for(i=0;i<(int)m_netCmdHandlers.size();i++)
		m_netCmdHandlers[i]->Update();

	//--update global states
	for(i=0;i<EGlobalState_Num;i++)
	{
		if(!m_globalStates[i]->IsClosed())
			m_globalStates[i]->Update(GetNav(),m_pLocalPlayer);
	}

	//--update lower state
	if(m_activeLowerState!=ELowerState_None)
		m_lowerStates[m_activeLowerState]->Update(GetNav(),m_pLocalPlayer);

	//--update upper state
	if(m_activeUpperState!=EUpperState_None)
		m_upperStates[m_activeUpperState]->Update(GetNav(),m_pLocalPlayer);
}

void FSM_LP::OnGameEvent( tagGameEvent* pEvent )
{
	int i;

	//--action state
	m_actionStates[m_activeActionState]->OnGameEvent(pEvent);

	//--game event handlers
	for(i=0;i<EGameEventHandler_Num;i++)
	{
		if(!m_gameEventHandlers[i]->OnGameEvent(pEvent))
			return;
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

	//--active global state
	if(pEvent->strEventName==_T("tagEnterCombatEvent"))
	{
		tagEnterCombatEvent event(_T("tagEnterCombatEvent"),NULL);
		TObjRef<GameFrameMgr>()->SendEvent(&event);

		if(m_globalStates[EGlobalState_Combat]->IsClosed())
			m_globalStates[EGlobalState_Combat]->Active(pEvent,GetNav(),m_pLocalPlayer);
	}

	//--upper state
	if(m_activeUpperState!=EUpperState_None)
		m_upperStates[m_activeUpperState]->OnGameEvent(pEvent);

	//--lower state
	if(m_activeLowerState!=ELowerState_None)
		m_lowerStates[m_activeLowerState]->OnGameEvent(pEvent);
}

void FSM_LP::OnNetCmd( tagNetCmd* pNetCmd )
{
	int i;

	//--action state
	m_actionStates[m_activeActionState]->OnNetCmd(pNetCmd);

	//--net cmd handlers
	for(i=0;i<(int)m_netCmdHandlers.size();i++)
		m_netCmdHandlers[i]->OnNetCmd(pNetCmd);

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
	if(m_activeUpperState!=EUpperState_None)
		m_upperStates[m_activeUpperState]->OnNetCmd(pNetCmd);

	//--lower state
	if(m_activeLowerState!=ELowerState_None)
		m_lowerStates[m_activeLowerState]->OnNetCmd(pNetCmd);
}

void FSM_LP::Change2LowerState( ELowerState state,tagGameEvent* pEvent,tagPreState* pPreState/*=NULL*/ )
{
	//--通知原状态
	if(m_activeLowerState!=ELowerState_None)
		m_lowerStates[m_activeLowerState]->OnExit();

	//--切换到新状态
	m_activeLowerState=state;
	m_lowerStates[m_activeLowerState]->Active(pEvent,GetNav(),m_pLocalPlayer,pPreState);

	//--通知上身状态
	if(m_activeUpperState!=EUpperState_None)
		m_upperStates[m_activeUpperState]->OnChangeLowerState(state,pEvent);
}

void FSM_LP::Change2LowerState( ELowerState state,tagNetCmd* pNetCmd )
{
	//--通知原状态
	if(m_activeLowerState!=ELowerState_None)
		m_lowerStates[m_activeLowerState]->OnExit();

	//--切换到新状态
	m_activeLowerState=state;
	m_lowerStates[m_activeLowerState]->Active(pNetCmd,GetNav(),m_pLocalPlayer,NULL);

	//--通知上身状态
	if(m_activeUpperState!=EUpperState_None)
		m_upperStates[m_activeUpperState]->OnChangeLowerState(state,NULL);
}

void FSM_LP::Change2LowerState_Stand()
{
	Change2LowerState(ELowerState_Stand,NULL,NULL);	
}

void FSM_LP::Change2LowerState_Float()
{
	Change2LowerState(ELowerState_Float,NULL,NULL);	
}

void FSM_LP::Change2LowerState_WaterStand()
{
	Change2LowerState(ELowerState_WaterStand,NULL,NULL);	
}

void FSM_LP::Change2LowerState_AutoStand()
{
	if(GetNav()->GetCollider()->IfWillOnWater(m_pLocalPlayer->GetPos()) && m_pLocalPlayer->CanWaterWalk())
	{
		Change2LowerState(ELowerState_WaterStand,NULL,NULL);
	}
	else if(GetNav()->GetCollider()->IfWillSwim(m_pLocalPlayer->GetPos(),m_pLocalPlayer->GetRoleSize().y))
	{
		Change2LowerState(ELowerState_Float,NULL,NULL);
	}
	else
	{
		Change2LowerState(ELowerState_Stand,NULL,NULL);	
	}
}

bool FSM_LP::TryChange2LowerState( ELowerState state,tagGameEvent* pEvent,tagPreState* pPreState/*=NULL*/ )
{
	//--询问动作状态是否可以切换
	if(!m_actionStates[m_activeActionState]->IfCanChange2LowerState(state,pEvent))
		return false;

	//--询问下身状态是否可以切换
	if(m_activeLowerState!=ELowerState_None
		&&!m_lowerStates[m_activeLowerState]->IfCanChange2LowerState(state,pEvent))
		return false;

	//--询问上身状态是否可以切换
	if(m_activeUpperState!=EUpperState_None
		&&!m_upperStates[m_activeUpperState]->IfCanChange2LowerState(state,pEvent))
		return false;

	//--通知原状态
	if(m_activeLowerState!=ELowerState_None)
		m_lowerStates[m_activeLowerState]->OnExit();

	//--切换到新状态
	m_activeLowerState=state;
	m_lowerStates[m_activeLowerState]->Active(pEvent,GetNav(),m_pLocalPlayer,pPreState);

	//--通知上身状态
	if(m_activeUpperState!=EUpperState_None)
		m_upperStates[m_activeUpperState]->OnChangeLowerState(state,pEvent);

	return true;
}

void FSM_LP::Change2UpperState( EUpperState state,tagGameEvent* pEvent )
{
	//--通知原状态
	if(m_activeUpperState!=EUpperState_None)
		m_upperStates[m_activeUpperState]->OnExit();

	//--切换到新状态
	m_activeUpperState=state;
	m_upperStates[m_activeUpperState]->Active(pEvent,GetNav(),m_pLocalPlayer);
}

void FSM_LP::Change2UpperState( EUpperState state,tagNetCmd* pNetCmd )
{
	//--通知原状态
	if(m_activeUpperState!=EUpperState_None)
		m_upperStates[m_activeUpperState]->OnExit();

	//--切换到新状态
	m_activeUpperState=state;
	m_upperStates[m_activeUpperState]->Active(pNetCmd,GetNav(),m_pLocalPlayer);
}

void FSM_LP::Change2UpperState_Idle()
{
	Change2UpperState(EUpperState_Idle,(tagGameEvent*)NULL);
}

void FSM_LP::Change2UpperState_Dead( bool bPlayAni,ERoleReviveType reviveType/*=ERRT_None*/ )
{
	//--通知原状态
	if(m_activeUpperState!=EUpperState_None)
		m_upperStates[m_activeUpperState]->OnExit();

	//--切换到新状态
	m_activeUpperState=EUpperState_Dead;
	((UpperStateDead_LP*)m_upperStates[m_activeUpperState])->Active(bPlayAni,m_pLocalPlayer,reviveType);
}

bool FSM_LP::TryChange2UpperState( EUpperState state,tagGameEvent* pEvent )
{
	//--询问下身状态是否可以切换
	if(m_activeLowerState!=ELowerState_None
		&&!m_lowerStates[m_activeLowerState]->IfCanChange2UpperState(state,pEvent))
		return false;

	//--询问上身状态是否可以切换
	if(m_activeUpperState!=EUpperState_None
		&&!m_upperStates[m_activeUpperState]->IfCanChange2UpperState(state,pEvent))
		return false;

	//--询问动作状态是否可以切换
	if(!m_actionStates[m_activeActionState]->IfCanChange2UpperState(state,pEvent))
		return false;

	//--通知原状态
	if(m_activeUpperState!=ELowerState_None)
		m_upperStates[m_activeUpperState]->OnExit();

	//--切换到新状态
	m_activeUpperState=state;
	m_upperStates[m_activeUpperState]->Active(pEvent,GetNav(),m_pLocalPlayer);

	return true;
}

void FSM_LP::Change2ActionState( EActionState state )
{
	//--通知原状态
	if(m_activeActionState!=EActionState_None)
		m_actionStates[m_activeActionState]->OnExit();

	//--切换到新状态
	m_activeActionState=state;
	m_actionStates[m_activeActionState]->Active(GetNav(),m_pLocalPlayer);
}

bool FSM_LP::IfCanPlayLowerAni()
{
	if(m_activeUpperState==EUpperState_None)
		return true;

	return m_upperStates[m_activeUpperState]->IfCanPlayLowerAni();
}

void FSM_LP::ReplayLowerAni()
{
	if(m_activeLowerState!=ELowerState_None)
		m_lowerStates[m_activeLowerState]->ReplayAni(m_pLocalPlayer);
}

bool FSM_LP::IfLowerCanSetYaw()
{
	if(m_activeUpperState==EUpperState_None)
		return true;

	return m_upperStates[m_activeUpperState]->IfLowerCanSetYaw();
}

bool FSM_LP::IsReadyState()
{
	return !m_globalStates[EGlobalState_Combat]->IsClosed();
}

bool FSM_LP::IsCombatState()
{
	return m_activeUpperState==EUpperState_Skill
		|| m_activeUpperState==EUpperState_UseItem;
}

bool FSM_LP::IsSwimState()
{
	return m_activeLowerState==ELowerState_Float 
		|| m_activeLowerState==ELowerState_MouseSwim
		|| m_activeLowerState==ELowerState_KeyboardSwim;
}

bool FSM_LP::IsOnWaterState()
{
	return m_activeLowerState==ELowerState_WaterStand 
		|| m_activeLowerState==ELowerState_MouseWaterWalk
		|| m_activeLowerState==ELowerState_KeyboardWaterWalk;
}

tagHitTargetEvent* FSM_LP::GetLastHitTargetEvent( DWORD roleID )
{
	HitTargetEventHandler_LP* pHandler=(HitTargetEventHandler_LP*)m_gameEventHandlers[EGameEventHandler_HitTarget];
	return pHandler->GetLastHitTargetEvent(roleID);
}


void FSM_LP::DontPlayEaseSkeleton()
{
	LowerStateStand_LP* pLowerStandLP = (LowerStateStand_LP*)m_lowerStates[ELowerState_Stand];
	if( P_VALID(pLowerStandLP ) )
	{
		pLowerStandLP->ResetLastEaseTime();
	}
}

DWORD FSM_LP::GetUsingItemTypeID()
{ 
	if( m_activeUpperState == EUpperState_UseItem )
	{
		UpperStateUseItem_LP* pUseItemLP = (UpperStateUseItem_LP*)(m_upperStates[EUpperState_UseItem]);
		if( P_VALID(pUseItemLP) )
		{
			return pUseItemLP->GetUsingItemTypeID();
		}
		else
			return 0;
	}

	return 0;
}