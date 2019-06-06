#pragma once
#include "state.h"
#include "GameEvent.h"

/** ÔØ¾ßÉÏÒÆ¶¯×´Ì¬
*/
class StateMoveOnCarrier :
	public State
{
public:
	StateMoveOnCarrier(void);
	virtual ~StateMoveOnCarrier(void);

	//--State
	virtual void Active(tagGameEvent* pEvent);
	virtual void Update();
	virtual void OnGameEvent(tagGameEvent* pEvent);

private:
	tagMoveEvent				m_event;
	NavCollider_MoveOnCarrier	m_collider;
	Vector3						m_dropDir;//test
};
