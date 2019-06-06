#pragma once
#include "state.h"
#include "GameEvent.h"

/** ÒÆ¶¯×´Ì¬
*/
class StateMove :
	public State
{
public:
	StateMove(void);
	virtual ~StateMove(void);

	//--State
	virtual void Active(tagGameEvent* pEvent);
	virtual void Update();
	virtual void OnGameEvent(tagGameEvent* pEvent);

private:
	tagMoveEvent		m_event;
	NavCollider_Move	m_collider;
};
