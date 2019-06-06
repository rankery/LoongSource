#pragma once
#include "lowerstate_rp.h"

/**	\class LowerStateFloat_RP
	\brief Ô¶³ÌÍæ¼ÒÆ¯¸¡×´Ì¬
*/
class LowerStateFloat_RP : public LowerState_RP
{
public:
	LowerStateFloat_RP(void);
	virtual ~LowerStateFloat_RP(void);

	//--LowerState_RP
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole);		
	virtual void Update(NavMap* pNav,Player* pRole);							
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent);
	virtual void OnExit();
	virtual bool IfCanChange2UpperState(FSM_RP::EUpperState state);

private:
	DWORD	m_effectID;
};
