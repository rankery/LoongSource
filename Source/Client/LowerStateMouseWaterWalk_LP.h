#pragma once
#include "lowerstate_lp.h"
#include "CombatEvent.h"
#include "ActionMouseWaterWalk_LP.h"

/** \class LowerStateMouseWaterWalk_LP
	\brief 本地玩家下身鼠标水上行走状态
*/
class LowerStateMouseWaterWalk_LP : public LowerState_LP
{
public:
	LowerStateMouseWaterWalk_LP(void);
	virtual ~LowerStateMouseWaterWalk_LP(void);

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
	tagMouseMoveEvent			m_event;
	ActionMouseWaterWalk_LP		m_actMouseWaterWalk;
	bool						m_finishedOnExit;  // 状态退出时是否移动结束
};
