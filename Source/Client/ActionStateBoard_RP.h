#pragma once
#include "ActionState_RP.h"

/** \class ActionStateBoard_RP
	\brief 远程玩家搭乘动作状态
*/
class ActionStateBoard_RP : public ActionState_RP
{
public:
	ActionStateBoard_RP(void);
	virtual ~ActionStateBoard_RP(void);

	//--ActionState_RP
	virtual void Active(NavMap* pNav,Player* pRole);	
	virtual void Update(NavMap* pNav,Player* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnExit();
	virtual bool IfCanChange2UpperState(FSM_RP::EUpperState state);
};