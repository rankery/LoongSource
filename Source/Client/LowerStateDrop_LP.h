#pragma once
#include "lowerstate_lp.h"
#include "CombatEvent.h"
#include "ActionDrop_LP.h"
#include "PreState.h"

/** \class LowerStateDrop_LP
	\brief 本地玩家下身掉落状态
*/
class LowerStateDrop_LP : public LowerState_LP
{
public:
	LowerStateDrop_LP(void);
	virtual ~LowerStateDrop_LP(void);

	//--LowerState_LP
	virtual void Active(tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole,tagPreState* pPreState=NULL);
	virtual void Update(NavMap* pNav,LocalPlayer* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd){}
	virtual void ReplayAni(LocalPlayer* pRole);
	virtual bool IfCanChange2UpperState(FSM_LP::EUpperState state,tagGameEvent* pEvent);
	virtual bool IfCanChange2LowerState(FSM_LP::ELowerState state,tagGameEvent* pEvent);

private:
	tagPreState		m_prestate;
	ActionDrop_LP	m_actDrop;		
};
