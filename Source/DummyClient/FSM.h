#pragma once

class State;
class Role;
struct tagGameEvent;
/** 状态机
*/
class FSM
{
public:
	enum EState
	{
		EState_Idle=0,
		EState_Move,
		EState_Drop,
		EState_VDrop,
		EState_Jump,
		EState_Slide,
		EState_Swim,
		EState_Float,
		EState_StandOnCarrier,
		EState_MoveOnCarrier,
		EState_Num
	};
public:
	FSM(void);
	virtual ~FSM(void);

	void Init();
	void Update();
	void OnGameEvent(tagGameEvent* pEvent);
	void Change2State(tagGameEvent* pEvent);
	void Change2State(EState state,tagGameEvent* pEvent);
	void Change2IdleState();
	void Change2FloatState();
	void SetRole(Role* pRole){m_pRole=pRole;}
	Role* GetRole() { return m_pRole;}

private:
	EState	m_activeState;//当前状态
	State*	m_states[EState_Num];//所有状态
	Role*	m_pRole;
};
