#pragma once
#include "lowerstate_lp.h"
#include "CombatEvent.h"
#include "ActionMouseMove_LP.h"

/** \class LowerStateMouseMove_LP
	\brief 本地玩家下身鼠标移动状态
*/
class LowerStateMouseMove_LP : public LowerState_LP
{
public:
	LowerStateMouseMove_LP(void);
	virtual ~LowerStateMouseMove_LP(void);

	//--LowerState_LP
	virtual void Active(tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole,tagPreState* pPreState=NULL);
	virtual void Update(NavMap* pNav,LocalPlayer* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void ReplayAni(LocalPlayer* pRole);
	virtual bool IfCanChange2UpperState(FSM_LP::EUpperState state,tagGameEvent* pEvent);
	virtual bool IfCanChange2LowerState(FSM_LP::ELowerState state,tagGameEvent* pEvent);
	virtual void OnExit();

private:
	void ShowDecal(bool bShow);
	void SendFinishEvent(bool bSuccessed);
private:
	tagMouseMoveEvent		m_event;
	ActionMouseMove_LP		m_actMouseMove;
	bool                    m_finishedOnExit;  // 状态退出时是否移动结束
};
