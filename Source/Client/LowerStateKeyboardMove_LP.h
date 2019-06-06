#pragma once
#include "lowerstate_lp.h"
#include "ActionKeyboardMove_LP.h"
#include "CombatEvent.h"

/** \class LowerStateKeyboardMove_LP
	\brief 本地玩家下身键盘移动状态
*/
class LowerStateKeyboardMove_LP : public LowerState_LP
{
public:
	LowerStateKeyboardMove_LP(void);
	virtual ~LowerStateKeyboardMove_LP(void);

	//--LowerState_LP
	virtual void Active(tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole,tagPreState* pPreState=NULL);	
	virtual void Update(NavMap* pNav,LocalPlayer* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void ReplayAni(LocalPlayer* pRole);
	virtual bool IfCanChange2UpperState(FSM_LP::EUpperState state,tagGameEvent* pEvent);
	virtual bool IfCanChange2LowerState(FSM_LP::ELowerState state,tagGameEvent* pEvent);

private:
	ActionKeyboardMove_LP	m_actKeyboardMove;
	tagKeyboardMoveEvent	m_event;
};
