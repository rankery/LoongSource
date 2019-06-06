#pragma once
#include "upperstate_lp.h"

class Role;
/** \class UpperStateFollow_LP
	\brief 本地玩家鼠标移动状态
*/
class UpperStateFollow_LP : public UpperState_LP
{
public:
	UpperStateFollow_LP(void);
	virtual ~UpperStateFollow_LP(void);

	//--UpperState_LP
	virtual void Active(tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole);	
	virtual void Update(NavMap* pNav,LocalPlayer* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual bool IfCanChange2UpperState(FSM_LP::EUpperState state,tagGameEvent* pEvent);
	virtual bool IfCanChange2LowerState(FSM_LP::ELowerState state,tagGameEvent* pEvent);
	virtual void OnChangeLowerState(FSM_LP::ELowerState newState,tagGameEvent* pEvent);
	virtual bool IfCanPlayLowerAni();
	virtual bool IfLowerCanSetYaw();

private:
	void SendMouseMoveEvent(LocalPlayer* pSelf,Role* pTarget);

private:
	DWORD					m_targetRoleID;
	float					m_lastMoveTime;
};
