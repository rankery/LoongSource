#pragma once
#include "lowerstate_lp.h"
#include "CombatEvent.h"
#include "ActionSlide_LP.h"
#include "PreState.h"

/** \class LowerStateSlide_LP
	\brief 本地玩家下身滑落状态
*/
class LowerStateSlide_LP :	public LowerState_LP
{
public:
	LowerStateSlide_LP(void);
	virtual ~LowerStateSlide_LP(void);

	//--LowerState_LP
	virtual void Active(tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole,tagPreState* pPreState=NULL);	
	virtual void Update(NavMap* pNav,LocalPlayer* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd){}
	virtual void ReplayAni(LocalPlayer* pRole);
	virtual bool IfCanChange2UpperState(FSM_LP::EUpperState state,tagGameEvent* pEvent);
	virtual bool IfCanChange2LowerState(FSM_LP::ELowerState state,tagGameEvent* pEvent);

private:
	tagPreState					m_prestate;
	ActionSlide_LP				m_actSlide;
};
