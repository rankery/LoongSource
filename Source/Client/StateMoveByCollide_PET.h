#pragma once
#include "state_pet.h"

class StateMoveByCollide_PET :
    public State_PET
{
public:
    StateMoveByCollide_PET(void);
    virtual ~StateMoveByCollide_PET(void);

    virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,Pet* pRole);		
    virtual void Update(NavMap* pNav,Pet* pRole);							
    virtual void OnNetCmd(tagNetCmd* pNetCmd);
    virtual void OnGameEvent(tagGameEvent* pEvent){}

private:
	NavCollider_NPCMove			m_collider;
	float						m_lastCDTime;
};
