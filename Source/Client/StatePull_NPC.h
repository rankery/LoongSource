#pragma once
#include "state_npc.h"

/** \class StatePull_NPC
	\brief NPCÍÏ×§×´Ì¬
*/
class StatePull_NPC : public State_NPC
{
public:
	StatePull_NPC(void);
	virtual ~StatePull_NPC(void);

	//--State_NPC
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,NPC* pRole);		
	virtual void Update(NavMap* pNav,NPC* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);

private:
	NavCollider_Move			m_collider;
};
