#include "StdAfx.h"
#include "FSM_PET.h"
#include "MapMgr.h"
#include "State_PET.h"
#include "StateIdle_PET.h"
#include "StateMoveByCollide_PET.h"
#include "StateMoveByTile_PET.h"
#include "StateShutdown_PET.h"
#include "NetCmdHandler_PET.h"
#include "GameEventHandler_PET.h"
#include "..\WorldDefine\action.h"

FSM_PET::FSM_PET(Pet* pPet) :
    m_pPet(pPet),
    m_activeState(EState_Idle)
{
    memset(m_states, 0, sizeof(m_states));
}

FSM_PET::~FSM_PET(void)
{
    Destroy();
}

inline NavMap* GetNav()
{
    return MapMgr::Inst()->GetNavMap();
}

void FSM_PET::Init()
{
    Destroy();

    // 初始化状态
    m_states[EState_Idle] = new StateIdle_PET;
    m_states[EState_Idle]->SetFSM(this);

    m_states[EState_MoveByCollide] = new StateMoveByCollide_PET;
    m_states[EState_MoveByCollide]->SetFSM(this);

    m_states[EState_MoveByTile] = new StateMoveByTile_PET;
    m_states[EState_MoveByTile]->SetFSM(this);

    m_states[EState_Shutdown] = new StateShutdown_PET;
    m_states[EState_Shutdown]->SetFSM(this);

    m_activeState = EState_Idle;
    m_states[EState_Idle]->Active(NULL, GetNav(), m_pPet);

    // 初始化网络消息处理器
    NetCmdHandler_PET* pNetCmdHandler = new NS_SyncStandHandler_PET;
    pNetCmdHandler->SetFSM(this);
    m_netCmdHandlers.push_back(pNetCmdHandler);

    pNetCmdHandler = new NS_StopActionHandler_PET;
    pNetCmdHandler->SetFSM(this);
    m_netCmdHandlers.push_back(pNetCmdHandler);

    // 初始化游戏事件处理器
}

void FSM_PET::Destroy()
{
    if( NULL != m_states[m_activeState] )
        m_states[m_activeState]->OnExit();

    int i;
    for(int i=0;i<EState_Num;i++)
        SAFE_DELETE(m_states[i]);

    for(i=0;i<(int)m_netCmdHandlers.size();i++)
        SAFE_DELETE(m_netCmdHandlers[i]);
    m_netCmdHandlers.clear();

    for(i=0;i<(int)m_gameEventHandlers.size();i++)
        SAFE_DELETE(m_gameEventHandlers[i]);
    m_gameEventHandlers.clear();
}

void FSM_PET::Update()
{
    // update net cmd handlers
    int i;
    for(i=0;i<(int)m_netCmdHandlers.size();i++)
        m_netCmdHandlers[i]->Update();

    // update states
    m_states[m_activeState]->Update(GetNav(), m_pPet);
}

void FSM_PET::OnNetCmd( tagNetCmd* pNetCmd )
{
    // net cmd handlers
    int i;
    for(i=0;i<(int)m_netCmdHandlers.size();i++)
        m_netCmdHandlers[i]->OnNetCmd(pNetCmd);

    // game event handlers
    for(i=0;i<(int)m_gameEventHandlers.size();i++)
        m_gameEventHandlers[i]->OnNetCmd(pNetCmd);

    // states
    m_states[m_activeState]->OnNetCmd(pNetCmd);
}

void FSM_PET::OnGameEvent( tagGameEvent* pEvent )
{
    // net cmd handlers
    int i;
    for(i=0;i<(int)m_netCmdHandlers.size();i++)
        m_netCmdHandlers[i]->OnGameEvent(pEvent);

    // game event handlers
    for(i=0;i<(int)m_gameEventHandlers.size();i++)
        m_gameEventHandlers[i]->OnGameEvent(pEvent);

    // states
    m_states[m_activeState]->OnGameEvent(pEvent);
}

void FSM_PET::Change2State( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==m_pUtil->Crc32("NS_SyncWalk"))
	{
		tagNS_SyncWalk* pCmd=(tagNS_SyncWalk*)pNetCmd;
		if(pCmd->bCollide)
			Change2State(EState_MoveByCollide,pNetCmd);
		else
			Change2State(EState_MoveByTile,pNetCmd);
	}
}

void FSM_PET::Change2State( tagGameEvent* pEvent )
{
}

void FSM_PET::Change2State( EState state,tagNetCmd* pNetCmd )
{
	m_states[m_activeState]->OnExit();

	m_activeState=state;
	m_states[state]->Active(pNetCmd,GetNav(),m_pPet);
}

void FSM_PET::Change2IdleState()
{
	Change2State(EState_Idle,NULL);
}
