#pragma once
#include "lowerstate_lp.h"
#include "CombatEvent.h"

/** \class LowerStatePull_LP
	\brief 本地玩家下身拖拽状态
*/
class LowerStatePull_LP : public LowerState_LP
{
public:
	LowerStatePull_LP(void);
	virtual ~LowerStatePull_LP(void);

	//--LowerState_LP
	virtual void Active(tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole,tagPreState* pPreState=NULL){}
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,LocalPlayer* pRole,tagPreState* pPreState);
	virtual void Update(NavMap* pNav,LocalPlayer* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd){}
	virtual bool IfCanChange2UpperState(FSM_LP::EUpperState state,tagGameEvent* pEvent);
	virtual bool IfCanChange2LowerState(FSM_LP::ELowerState state,tagGameEvent* pEvent);

private:
	NavCollider_Move				m_collider;
};
