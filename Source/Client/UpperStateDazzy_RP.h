#pragma once
#include "upperstate_rp.h"

/**	\class UpperStateDazzy_RP
	\brief Ô¶³ÌÍæ¼Ò»÷ÔÎ×´Ì¬
*/
class UpperStateDazzy_RP : public UpperState_RP
{
public:
	UpperStateDazzy_RP(void);
	virtual ~UpperStateDazzy_RP(void);

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
