#pragma once
#include "upperstate_rp.h"

/**	\class UpperStateSopor_RP
	\brief Ô¶³ÌÍæ¼Ò»èË¯×´Ì¬
*/
class UpperStateSopor_RP : public UpperState_RP
{
public:
	UpperStateSopor_RP(void);
	virtual ~UpperStateSopor_RP(void);

	//--UpperState_RP
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole);		
	virtual void Update(NavMap* pNav,Player* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnExit();
	virtual bool IfCanPlayLowerAni();
	virtual bool IfLowerCanSetYaw();

	//--
private:
};
