#pragma once
#include "ActionState_RP.h"

/** \class ActionStateNormal_RP
	\brief 远程玩家正常动作状态
*/
class ActionStateNormal_RP : public ActionState_RP
{
public:
	ActionStateNormal_RP(void);
	virtual ~ActionStateNormal_RP(void);

	//--ActionState_RP
	virtual void Active(NavMap* pNav,Player* pRole);	
	virtual void Update(NavMap* pNav,Player* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual bool IfCanChange2UpperState(FSM_RP::EUpperState state);

private:
	DWORD m_mountID;
};