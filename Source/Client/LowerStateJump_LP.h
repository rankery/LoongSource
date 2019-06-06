#pragma once
#include "lowerstate_lp.h"
#include "CombatEvent.h"
#include "ActionJump_LP.h"
#include "PreState.h"

/** \class LowerStateJump_LP
	\brief 本地玩家下身跳跃状态
*/
class LowerStateJump_LP : public LowerState_LP
{
public:
	LowerStateJump_LP(void);
	virtual ~LowerStateJump_LP(void);

	//--LowerState_LP
	virtual void Active(tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole,tagPreState* pPreState=NULL);	
	virtual void Update(NavMap* pNav,LocalPlayer* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd){}
	virtual bool IfCanChange2UpperState(FSM_LP::EUpperState state,tagGameEvent* pEvent);
	virtual bool IfCanChange2LowerState(FSM_LP::ELowerState state,tagGameEvent* pEvent);

private:
	tagPreState		m_prestate;
	bool			m_bOriginJump;
	ActionJump_LP	m_actJump;
};
