#pragma once
#include "lowerstate_lp.h"
#include "CombatEvent.h"

/** \class LowerStateRepel_LP
	\brief 本地玩家下身击退状态
*/
class LowerStateRepel_LP : public LowerState_LP
{
public:
	LowerStateRepel_LP(void);
	virtual ~LowerStateRepel_LP(void);

	//--LowerState_LP
	virtual void Active(tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole,tagPreState* pPreState=NULL){}
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,LocalPlayer* pRole,tagPreState* pPreState);
	virtual void Update(NavMap* pNav,LocalPlayer* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd){}
	virtual bool IfCanChange2UpperState(FSM_LP::EUpperState state,tagGameEvent* pEvent);
	virtual bool IfCanChange2LowerState(FSM_LP::ELowerState state,tagGameEvent* pEvent);
	virtual void OnExit();
private:
	NavCollider_Move				m_collider;
	DWORD                           m_effectID;
};
