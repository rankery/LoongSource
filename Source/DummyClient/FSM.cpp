#include "StdAfx.h"
#include "FSM.h"
#include "StateIdle.h"
#include "StateMove.h"
#include "StateDrop.h"
#include "StateVDrop.h"
#include "StateJump.h"
#include "StateSlide.h"
#include "StateSwim.h"
#include "StateFloat.h"
#include "StateStandOnCarrier.h"
#include "StateMoveOnCarrier.h"
#include "GameEvent.h"
#include "Role.h"

FSM::FSM(void)
{
	memset(m_states,0,sizeof(m_states));
	m_pRole=NULL;
	m_activeState=EState_Idle;
}

FSM::~FSM(void)
{
	for(int i=0;i<EState_Num;i++)
		SAFE_DELETE(m_states[i]);
}

void FSM::Init()
{
	m_states[EState_Idle]=new StateIdle;
	m_states[EState_Move]=new StateMove;
	m_states[EState_Drop]=new StateDrop;
	m_states[EState_VDrop]=new StateVDrop;
	m_states[EState_Jump]=new StateJump;
	m_states[EState_Slide]=new StateSlide;
	m_states[EState_Swim]=new StateSwim;
	m_states[EState_Float]=new StateFloat;
	m_states[EState_StandOnCarrier]=new StateStandOnCarrier;
	m_states[EState_MoveOnCarrier]=new StateMoveOnCarrier;

	for(int i=0;i<EState_Num;i++)
		m_states[i]->SetFSM(this);

	Change2IdleState();
}

void FSM::Update()
{
	m_states[m_activeState]->Update();
}

void FSM::Change2State( tagGameEvent* pEvent)
{
	EState state;
	switch(pEvent->event)
	{
	case EGE_Move:			state=EState_Move;			break;
	case EGE_Drop:			state=EState_Drop;			break;
	case EGE_VDrop:			state=EState_VDrop;			break;
	case EGE_Jump:			state=EState_Jump;			break;
	case EGE_Slide:			state=EState_Slide;			break;
	case EGE_Swim:			state=EState_Swim;			break;
	case EGE_StandOnCarrier:state=EState_StandOnCarrier;break;
	default: 
		return;
	}

	m_activeState=state;
	m_states[m_activeState]->Active(pEvent);
}

void FSM::Change2State( EState state,tagGameEvent* pEvent )
{
	m_activeState=state;
	m_states[m_activeState]->Active(pEvent);
}

void FSM::Change2IdleState()
{
	if(m_activeState!=EState_Idle)
	{
		m_activeState=EState_Idle;
		m_states[EState_Idle]->Active(NULL);
	}
}

void FSM::OnGameEvent( tagGameEvent* pEvent )
{
	m_states[m_activeState]->OnGameEvent(pEvent);
}

void FSM::Change2FloatState()
{
	if(m_activeState!=EState_Float)
	{
		m_activeState=EState_Float;
		m_states[EState_Float]->Active(NULL);
	}
}