#pragma once
#include "state_npc.h"

/** \class StateMoveByCollide_NPC
	\brief NPCÒÆ¶¯×´Ì¬£¬Ê¹ÓÃÅö×²
*/
class StateMoveByCollide_NPC : public State_NPC
{
public:
	StateMoveByCollide_NPC(void);
	virtual ~StateMoveByCollide_NPC(void);

	//--State_NPC
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,NPC* pRole);		
	virtual void Update(NavMap* pNav,NPC* pRole);							
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent){}

private:
	NavCollider_NPCMove			m_collider;
	float						m_lastCDTime;
};
