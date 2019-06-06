#pragma once
#include "state_npc.h"

/**	\class StateSopor_NPC
	\brief NPC»èË¯×´Ì¬
*/
class StateSopor_NPC : public State_NPC
{
public:
	StateSopor_NPC(void);
	virtual ~StateSopor_NPC(void);

	//--State_NPC
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,NPC* pRole);		
	virtual void Update(NavMap* pNav,NPC* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);

private:
};
