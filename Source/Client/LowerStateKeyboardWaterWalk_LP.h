#pragma once
#include "lowerstate_lp.h"
#include "ActionKeyboardWaterWalk_LP.h"
#include "CombatEvent.h"

/** \class LowerStateKeyboardWaterWalk_LP
	\brief 本地玩家下身键盘水上行走状态
*/
class LowerStateKeyboardWaterWalk_LP : public LowerState_LP
{
public:
	LowerStateKeyboardWaterWalk_LP(void);
	virtual ~LowerStateKeyboardWaterWalk_LP(void);

	//--LowerState_LP
	virtual void Active(tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole,tagPreState* pPreState=NULL);	
	virtual void Update(NavMap* pNav,LocalPlayer* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnExit();
	virtual bool IfCanChange2UpperState(FSM_LP::EUpperState state,tagGameEvent* pEvent);
	virtual bool IfCanChange2LowerState(FSM_LP::ELowerState state,tagGameEvent* pEvent);

private:
	ActionKeyboardWaterWalk_LP	m_actKeyboardWaterWalk;
	tagKeyboardMoveEvent		m_event;
};
