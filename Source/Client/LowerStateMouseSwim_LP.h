#pragma once
#include "lowerstate_lp.h"
#include "CombatEvent.h"
#include "ActionMouseSwim_LP.h"

/** \class LowerStateMouseSwim_LP
	\brief 本地玩家下身鼠标游泳状态
*/
class LowerStateMouseSwim_LP : public LowerState_LP
{
public:
	LowerStateMouseSwim_LP(void);
	virtual ~LowerStateMouseSwim_LP(void);

	//--LowerState_LP
	virtual void Active(tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole,tagPreState* pPreState=NULL);
	virtual void Update(NavMap* pNav,LocalPlayer* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnExit();
	virtual bool IfCanChange2UpperState(FSM_LP::EUpperState state,tagGameEvent* pEvent);
	virtual bool IfCanChange2LowerState(FSM_LP::ELowerState state,tagGameEvent* pEvent);

private:
	void ShowDecal(bool bShow);
	void SendFinishEvent(bool bSuccessed);
private:
	tagMouseMoveEvent		m_event;
	ActionMouseSwim_LP		m_actMouseSwim;
	bool                    m_finishedOnExit;  // 状态退出时是否移动结束
};
