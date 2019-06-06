#include "StdAfx.h"
#include "FSM_NPC.h"
#include "NPC.h"
#include "StateIdle_NPC.h"
#include "StateMoveByCollide_NPC.h"
#include "StateMoveByTile_NPC.h"
#include "StateSkill_NPC.h"
#include "StateDead_NPC.h"
#include "StateBeAttack_NPC.h"
#include "StateHitFly_NPC.h"
#include "StateRepel_NPC.h"
#include "StateAssault_NPC.h"
#include "StatePull_NPC.h"
#include "StateDazzy_NPC.h"
#include "StateTie_NPC.h"
#include "StateSopor_NPC.h"
#include "MapMgr.h"
#include "..\WorldDefine\action.h"
#include "NetCmdHandler_NPC.h"
#include "GameEventHandler_NPC.h"

FSM_NPC::FSM_NPC(NPC* pNPC)
: m_pNPC(pNPC)
, m_activeState(EState_Idle)
{
	memset(m_states,0,sizeof(m_states));
	memset(m_gameEventHandlers,0,sizeof(m_gameEventHandlers));
}

FSM_NPC::~FSM_NPC(void)
{
	Destroy();
}

inline NavMap* GetNav()
{
	return MapMgr::Inst()->GetNavMap();
}

void FSM_NPC::Init()
{
	Destroy();

	//--初始化状态
	m_states[EState_Idle]=new StateIdle_NPC;
	m_states[EState_Idle]->SetFSM(this);

	m_states[EState_MoveByCollide]=new StateMoveByCollide_NPC;
	m_states[EState_MoveByCollide]->SetFSM(this);

	m_states[EState_MoveByTile]=new StateMoveByTile_NPC;
	m_states[EState_MoveByTile]->SetFSM(this);

	m_states[EState_Skill]=new StateSkill_NPC;
	m_states[EState_Skill]->SetFSM(this);

	m_states[EState_Dead]=new StateDead_NPC;
	m_states[EState_Dead]->SetFSM(this);

	m_states[EState_BeAttack]=new StateBeAttack_NPC;
	m_states[EState_BeAttack]->SetFSM(this);

	m_states[EState_HitFly]=new StateHitFly_NPC;
	m_states[EState_HitFly]->SetFSM(this);

	m_states[EState_Repel]=new StateRepel_NPC;
	m_states[EState_Repel]->SetFSM(this);

	m_states[EState_Assault]=new StateAssault_NPC;
	m_states[EState_Assault]->SetFSM(this);

	m_states[EState_Pull]=new StatePull_NPC;
	m_states[EState_Pull]->SetFSM(this);

	m_states[EState_Dazzy]=new StateDazzy_NPC;
	m_states[EState_Dazzy]->SetFSM(this);
	
	m_states[EState_Tie]=new StateTie_NPC;
	m_states[EState_Tie]->SetFSM(this);

	m_states[EState_Sopor]=new StateSopor_NPC;
	m_states[EState_Sopor]->SetFSM(this);	

	m_activeState=EState_Idle;
	m_states[m_activeState]->Active(NULL,GetNav(),m_pNPC);

	//--初始化网络消息处理器
	NetCmdHandler_NPC* pNetCmdHandler=new NS_HPChangeHandler_NPC;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler=new NS_SyncStandHandler_NPC;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler=new NS_StopActionHandler_NPC;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler=new NS_RoleDeadHandler_NPC;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler=new NS_HitFlyHandler_NPC;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler=new NS_SpecialMove_NPC;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler=new NS_AddRoleBuffHandler_NPC;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler=new NS_SetStateHandler_NPC;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler=new NS_HitTargetHandler_NPC;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler=new NS_SyncGroundItemHandler_NPC;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler=new NS_MonsterEnterCombatHandler_NPC;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler = new NS_MonsterPlayActionHandler_NPC;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	pNetCmdHandler = new NS_MonsterSaHandler_NPC;
	pNetCmdHandler->SetFSM(this);
	m_netCmdHandlers.push_back(pNetCmdHandler);

	//--初始化游戏事件处理器
	m_gameEventHandlers[EGameEventHandler_HitTarget]=new HitTargetEventHandler_NPC;
	m_gameEventHandlers[EGameEventHandler_HitTarget]->SetFSM(this);
}

void FSM_NPC::Destroy()
{
	if( NULL != m_states[m_activeState] )
		m_states[m_activeState]->OnExit();

	int i;
	for(int i=0;i<EState_Num;i++)
		SAFE_DELETE(m_states[i]);

	for(i=0;i<(int)m_netCmdHandlers.size();i++)
		SAFE_DELETE(m_netCmdHandlers[i]);
	m_netCmdHandlers.clear();

	for(i=0;i<EGameEventHandler_Num;i++)
		SAFE_DELETE(m_gameEventHandlers[i]);
}

void FSM_NPC::Update()
{
	//--
	int i;
	for(i=0;i<(int)m_netCmdHandlers.size();i++)
		m_netCmdHandlers[i]->Update();

	//--
	m_states[m_activeState]->Update(GetNav(),m_pNPC);
}

void FSM_NPC::OnNetCmd( tagNetCmd* pNetCmd )
{
	//--
	int i;
	for(i=0;i<(int)m_netCmdHandlers.size();i++)
		m_netCmdHandlers[i]->OnNetCmd(pNetCmd);

	//--game event handlers
	for(i=0;i<EGameEventHandler_Num;i++)
		m_gameEventHandlers[i]->OnNetCmd(pNetCmd);

	//--
	m_states[m_activeState]->OnNetCmd(pNetCmd);
}

void FSM_NPC::OnGameEvent( tagGameEvent* pEvent )
{
	//--net cmd handlers
	int i;
	for(i=0;i<(int)m_netCmdHandlers.size();i++)
		m_netCmdHandlers[i]->OnGameEvent(pEvent);

	//--game event handlers
	for(i=0;i<EGameEventHandler_Num;i++)
		m_gameEventHandlers[i]->OnGameEvent(pEvent);

	//--
	m_states[m_activeState]->OnGameEvent(pEvent);
}

void FSM_NPC::Change2State( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_SyncWalk"))
	{
		tagNS_SyncWalk* pCmd=(tagNS_SyncWalk*)pNetCmd;
		if(pCmd->bCollide)
			Change2State(EState_MoveByCollide,pNetCmd);
		else
			Change2State(EState_MoveByTile,pNetCmd);
	}
	else if(pNetCmd->dwID==m_pUtil->Crc32("NS_Skill"))
	{
		Change2State(EState_Skill,pNetCmd);
	}
}

void FSM_NPC::Change2State( tagGameEvent* pEvent )
{
	EState state;
	if(pEvent->strEventName==_T("tagBeAttackEvent"))
		state=EState_BeAttack;
	else
		return;

	m_states[m_activeState]->OnExit();

	m_activeState=state;
	m_states[m_activeState]->Active(NULL,GetNav(),m_pNPC);
}

void FSM_NPC::Change2IdleState()
{
	Change2State(EState_Idle,NULL);
}

void FSM_NPC::Change2DeadState()
{
	Change2State(EState_Dead,NULL);
}

void FSM_NPC::Change2State( EState state,tagNetCmd* pNetCmd )
{
	m_states[m_activeState]->OnExit();

	m_activeState=state;
	m_states[state]->Active(pNetCmd,GetNav(),m_pNPC);
}

tagHitTargetEvent* FSM_NPC::GetLastHitTargetEvent( DWORD roleID )
{
	HitTargetEventHandler_NPC* pHandler=(HitTargetEventHandler_NPC*)m_gameEventHandlers[EGameEventHandler_HitTarget];
	return pHandler->GetLastHitTargetEvent(roleID);
}