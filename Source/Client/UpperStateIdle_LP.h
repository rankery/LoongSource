#pragma once
#include "upperstate_lp.h"

/**	\class UpperStateIdle_LP
	\brief 本地玩家上身空闲状态
*/
class UpperStateIdle_LP : public UpperState_LP
{
public:
	UpperStateIdle_LP(void);
	virtual ~UpperStateIdle_LP(void);

	//--UpperState_LP
	virtual void Active(tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole);		
	virtual void Update(NavMap* pNav,LocalPlayer* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual bool IfCanChange2UpperState(FSM_LP::EUpperState state,tagGameEvent* pEvent);
	virtual bool IfCanChange2LowerState(FSM_LP::ELowerState state,tagGameEvent* pEvent);
	virtual bool IfCanPlayLowerAni();
	virtual bool IfLowerCanSetYaw();
	virtual void OnExit();

private:
	void FaceToTarget(void);

private:
	float	m_lastUpdatetime;
};
