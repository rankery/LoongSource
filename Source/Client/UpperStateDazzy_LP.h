#pragma once
#include "upperstate_lp.h"

/**	\class UpperStateDazzy_LP
	\brief ±¾µØÍæ¼Ò»÷ÔÎ×´Ì¬
*/
class UpperStateDazzy_LP : public UpperState_LP
{
public:
	UpperStateDazzy_LP(void);
	virtual ~UpperStateDazzy_LP(void);

	//--UpperState_LP
	virtual void Active(tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole){}
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,LocalPlayer* pRole);
	virtual void Update(NavMap* pNav,LocalPlayer* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnExit();
	virtual bool IfCanChange2UpperState(FSM_LP::EUpperState state,tagGameEvent* pEvent);
	virtual bool IfCanChange2LowerState(FSM_LP::ELowerState state,tagGameEvent* pEvent);
	virtual bool IfCanPlayLowerAni();
	virtual bool IfLowerCanSetYaw();

	//--
};
