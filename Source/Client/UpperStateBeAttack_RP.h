#pragma once
#include "upperstate_rp.h"

/**	\class UpperStateBeAttack_RP
	\brief Ô¶³ÌÍæ¼Ò±»¹¥»÷×´Ì¬
*/
class UpperStateBeAttack_RP : public UpperState_RP
{
public:
	UpperStateBeAttack_RP(void);
	virtual ~UpperStateBeAttack_RP(void);

	//--UpperState_RP
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole){}	
	virtual void Active(tagGameEvent* pEvent,NavMap* pNav,Player* pRole);		
	virtual void Update(NavMap* pNav,Player* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnExit();
	virtual bool IfCanPlayLowerAni();
	virtual bool IfLowerCanSetYaw();
	virtual void OnChangeLowerState(FSM_RP::ELowerState state);

private:
	tstring m_szTrackName;
};
