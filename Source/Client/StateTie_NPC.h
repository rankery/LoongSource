#pragma once
#include "state_npc.h"

/**	\class StateTie_NPC
	\brief NPC¶¨Éí×´Ì¬
*/
class StateTie_NPC : public State_NPC
{
public:
	StateTie_NPC(void);
	virtual ~StateTie_NPC(void);

	//--State_RP
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,NPC* pRole);		
	virtual void Update(NavMap* pNav,NPC* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);

	//--
private:
};
