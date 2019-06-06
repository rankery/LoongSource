#pragma once
#include "state_pet.h"

class StateMoveByTile_PET :
    public State_PET
{
public:
    StateMoveByTile_PET(void);
    virtual ~StateMoveByTile_PET(void);

    virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,Pet* pRole);		
    virtual void Update(NavMap* pNav,Pet* pRole);							
    virtual void OnNetCmd(tagNetCmd* pNetCmd);
    virtual void OnGameEvent(tagGameEvent* pEvent){}

private:
    Vector2						m_moveStart;		
    Vector2						m_moveDest;		
    Vector2						m_moveDir;
    float						m_moveStartTime;
    float						m_moveDeltaTime;
    float						m_moveTime;

    TObjRef<Util>				m_pUtil;

    list<NavCollider::tagMovePoint>	m_movePath;
};
