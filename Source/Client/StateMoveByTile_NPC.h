#pragma once
#include "state_npc.h"

/** \class StateMoveByTile_NPC
	\brief NPC移动状态，基于格子
*/
class StateMoveByTile_NPC : public State_NPC
{
public:
	StateMoveByTile_NPC(void);
	virtual ~StateMoveByTile_NPC(void);

	//--State_NPC
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,NPC* pRole);		
	virtual void Update(NavMap* pNav,NPC* pRole);							
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
