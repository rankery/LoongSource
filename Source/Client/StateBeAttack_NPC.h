#pragma once
#include "state_npc.h"

/**	\class StateBeAttack_NPC
	\brief NPC±»¹¥»÷×´Ì¬
*/
class StateBeAttack_NPC : public State_NPC
{
public:
	StateBeAttack_NPC(void);
	virtual ~StateBeAttack_NPC(void);

	//--State_NPC
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,NPC* pRole);		
	virtual void Update(NavMap* pNav,NPC* pRole);							
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent);
};
