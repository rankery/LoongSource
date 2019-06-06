#pragma once
#include "state_npc.h"

/** \class StateRepel_NPC
	\brief NPC»÷ÍË×´Ì¬
*/
class StateRepel_NPC : public State_NPC
{
public:
	StateRepel_NPC(void);
	virtual ~StateRepel_NPC(void);

	//--State_NPC
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,NPC* pRole);		
	virtual void Update(NavMap* pNav,NPC* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnExit();
private:
	NavCollider_Move			m_collider;
	DWORD                       m_effectID;
};
