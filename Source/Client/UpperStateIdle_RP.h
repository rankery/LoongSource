#pragma once
#include "upperstate_rp.h"

/**	\class UpperStateIdle_RP
	\brief 本地玩家上身空闲状态
*/
class UpperStateIdle_RP : public UpperState_RP
{
public:
	UpperStateIdle_RP(void);
	virtual ~UpperStateIdle_RP(void);

	//--UpperState_RP
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole);		
	virtual void Update(NavMap* pNav,Player* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual bool IfCanPlayLowerAni();
	virtual bool IfLowerCanSetYaw();
};
