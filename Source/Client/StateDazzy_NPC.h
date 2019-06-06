#pragma once
#include "state_npc.h"

/**	\class StateDazzy_NPC
	\brief NPC»÷ÔÎ×´Ì¬
*/
class StateDazzy_NPC : public State_NPC
{
public:
	StateDazzy_NPC(void);
	virtual ~StateDazzy_NPC(void);

	//--State_RP
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,NPC* pRole);		
	virtual void Update(NavMap* pNav,NPC* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);

	//--
private:
};
