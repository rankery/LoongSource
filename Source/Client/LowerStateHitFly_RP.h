#pragma once
#include "lowerstate_rp.h"
#include "CombatEvent.h"

/** \class LowerStateHitFly_RP
	\brief Ô¶³ÌÍæ¼Ò»÷·É×´Ì¬
*/
class LowerStateHitFly_RP : public LowerState_RP
{
	enum EStep
	{
		EStep_Fly,		//»÷·É
		EStep_Craw,		//ÅÀÆð
		EStep_Finish,	//Íê³É
	};
public:
	LowerStateHitFly_RP(void);
	virtual ~LowerStateHitFly_RP(void);

	//--LowerState_RP
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole);		
	virtual void Update(NavMap* pNav,Player* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual bool IfCanChange2UpperState(FSM_RP::EUpperState state);

private:
	NavCollider_HitFly		m_collider;	
	EStep					m_step;
};
