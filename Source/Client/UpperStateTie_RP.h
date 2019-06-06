#pragma once
#include "upperstate_rp.h"

/**	\class UpperStateTie_RP
	\brief 远程玩家定身状态
*/
class UpperStateTie_RP : public UpperState_RP
{
public:
	UpperStateTie_RP(void);
	virtual ~UpperStateTie_RP(void);

	//--UpperState_LP
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole);		
	virtual void Update(NavMap* pNav,Player* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnExit();
	virtual bool IfCanPlayLowerAni();
	virtual bool IfLowerCanSetYaw();

private:
};
