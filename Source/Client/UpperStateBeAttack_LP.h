#pragma once
#include "upperstate_lp.h"

/**	\class UpperStateBeAttack_LP
	\brief 本地玩家被攻击状态
*/
class UpperStateBeAttack_LP : public UpperState_LP
{
public:
	UpperStateBeAttack_LP(void);
	virtual ~UpperStateBeAttack_LP(void);

	//--UpperState_LP
	virtual void Active(tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole);		
	virtual void Update(NavMap* pNav,LocalPlayer* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnExit();
	virtual bool IfCanChange2UpperState(FSM_LP::EUpperState state,tagGameEvent* pEvent);
	virtual bool IfCanChange2LowerState(FSM_LP::ELowerState state,tagGameEvent* pEvent);
	virtual bool IfCanPlayLowerAni();
	virtual bool IfLowerCanSetYaw();
	virtual void OnChangeLowerState(FSM_LP::ELowerState newState);

private:
	tstring m_szTrackName;
};
