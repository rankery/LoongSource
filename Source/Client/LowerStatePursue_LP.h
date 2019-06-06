#pragma once
#include "lowerstate_lp.h"
#include "CombatEvent.h"
#include "ActionMouseMove_LP.h"

/** \class LowerStatePursue_LP
	\brief ±¾µØÍæ¼Ò×·×Ù×´Ì¬
*/
class LowerStatePursue_LP : public LowerState_LP
{
public:
	LowerStatePursue_LP(void);
	virtual ~LowerStatePursue_LP(void);

	//--LowerState_LP
	virtual void Active(tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole,tagPreState* pPreState=NULL);
	virtual void Update(NavMap* pNav,LocalPlayer* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void ReplayAni(LocalPlayer* pRole);
	virtual bool IfCanChange2UpperState(FSM_LP::EUpperState state,tagGameEvent* pEvent);
	virtual bool IfCanChange2LowerState(FSM_LP::ELowerState state,tagGameEvent* pEvent);

private:
	void SendEvent(bool bSuccess);
	void SendStopCmd(const Vector3& pos);

private:
	tagPursueEvent			m_event;
	float					m_validDistSq;
	float					m_lastCheckPosTime;
	Vector3					m_targetPos;
	ActionMouseMove_LP		m_actMouseMove;
};
