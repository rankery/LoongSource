#pragma once
#include "ActionState_RP.h"

/** \class ActionStateRide_RP
	\brief 远程玩家骑乘动作状态
*/
class ActionStateRide_RP : public ActionState_RP
{
public:
	ActionStateRide_RP(void);
	virtual ~ActionStateRide_RP(void);

	//--ActionState_RP
	virtual void Active(NavMap* pNav,Player* pRole);	
	virtual void Update(NavMap* pNav,Player* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnExit();
	virtual bool IfCanChange2UpperState(FSM_RP::EUpperState state);
};