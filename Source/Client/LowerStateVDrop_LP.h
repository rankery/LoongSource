#pragma once
#include "lowerstate_lp.h"
#include "CombatEvent.h"
#include "ActionVDrop_LP.h"
#include "PreState.h"

/** \class LowerStateVDrop_LP
	\brief 本地玩家下身垂直掉落状态
*/
class LowerStateVDrop_LP :	public LowerState_LP
{
public:
	LowerStateVDrop_LP(void);
	virtual ~LowerStateVDrop_LP(void);

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
	ActionVDrop_LP	m_actVDrop;
};
