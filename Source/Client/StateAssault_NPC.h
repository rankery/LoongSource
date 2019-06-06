#pragma once
#include "state_npc.h"

/** \class StateAssault_NPC
	\brief NPC³å·æ×´Ì¬
*/
class StateAssault_NPC : public State_NPC
{
public:
	StateAssault_NPC(void);
	virtual ~StateAssault_NPC(void);

	//--State_NPC
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,NPC* pRole);		
	virtual void Update(NavMap* pNav,NPC* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);

private:
	NavCollider_Move			m_collider;
};
