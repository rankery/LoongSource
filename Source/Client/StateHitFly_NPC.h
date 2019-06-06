#pragma once
#include "state_npc.h"

/** \class StateHitFly_NPC
	\brief NPC»÷·É×´Ì¬
*/
class StateHitFly_NPC : public State_NPC
{
	enum EStep
	{
		EStep_Fly,		//»÷·É
		EStep_Craw,		//ÅÀÆð
	};
public:
	StateHitFly_NPC(void);
	virtual ~StateHitFly_NPC(void);

	//--State_NPC
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,NPC* pRole);		
	virtual void Update(NavMap* pNav,NPC* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);

private:
	NavCollider_HitFly		m_collider;
	EStep					m_step;
	bool					m_bCollide;
};
