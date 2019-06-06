#pragma once
#include "lowerstate_rp.h"
#include "CombatEvent.h"

/** \class LowerStateRepel_RP
	\brief Ô¶³ÌÍæ¼Ò»÷ÍË×´Ì¬
*/
class LowerStateRepel_RP : public LowerState_RP
{
public:
	LowerStateRepel_RP(void);
	virtual ~LowerStateRepel_RP(void);

	//--LowerState_RP
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole);		
	virtual void Update(NavMap* pNav,Player* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual bool IfCanChange2UpperState(FSM_RP::EUpperState state);
	virtual void OnExit();
private:
	NavCollider_Move			m_collider;
	DWORD                       m_effectID;
};
