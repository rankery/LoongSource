#pragma once
#include "lowerstate_rp.h"
#include "CombatEvent.h"

/** \class LowerStateSlide_RP
	\brief Ô¶³ÌÍæ¼Ò»¬Âä×´Ì¬
*/
class LowerStateSlide_RP :	public LowerState_RP
{
public:
	LowerStateSlide_RP(void);
	virtual ~LowerStateSlide_RP(void);

	//--LowerState_RP
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole);		
	virtual void Update(NavMap* pNav,Player* pRole);							
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent){}
	virtual void ReplayAni(Player* pRole);
	virtual bool IfCanChange2UpperState(FSM_RP::EUpperState state);

private:
	NavCollider_Slide		m_collider;
	float					m_lastCDTime;
};
