#pragma once
#include "state_pet.h"

class StateShutdown_PET :
    public State_PET
{
public:
    StateShutdown_PET(void);
    virtual ~StateShutdown_PET(void);

    virtual void Active(tagNetCmd* pNetCmd, NavMap* pNav, Pet* pRole);		
    virtual void Update(NavMap* pNav, Pet* pRole);							
    virtual void OnNetCmd(tagNetCmd* pNetCmd);
    virtual void OnGameEvent(tagGameEvent* pEvent);
};
