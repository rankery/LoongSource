#pragma once
#include "lowerstate_lp.h"
#include "ActionKeyboardSwim_LP.h"
#include "CombatEvent.h"

/** \class LowerStateKeyboardSwim_LP
	\brief 本地玩家下身键盘游泳状态
*/
class LowerStateKeyboardSwim_LP : public LowerState_LP
{
public:
	LowerStateKeyboardSwim_LP(void);
	virtual ~LowerStateKeyboardSwim_LP(void);

	//--LowerState_LP
	virtual void Active(tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole,tagPreState* pPreState=NULL);	
	virtual void Update(NavMap* pNav,LocalPlayer* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void ReplayAni(LocalPlayer* pRole);
	virtual void OnExit();
	virtual bool IfCanChange2UpperState(FSM_LP::EUpperState state,tagGameEvent* pEvent);
	virtual bool IfCanChange2LowerState(FSM_LP::ELowerState state,tagGameEvent* pEvent);

private:
	ActionKeyboardSwim_LP	m_actKeyboardSwim;
	tagKeyboardMoveEvent	m_event;
};
