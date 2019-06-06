#pragma once
#include "state.h"
#include "GameEvent.h"

/** ´¹Ö±µôÂä×´Ì¬
*/
class StateVDrop :
	public State
{
public:
	StateVDrop(void);
	virtual ~StateVDrop(void);

	//--State
	virtual void Active(tagGameEvent* pEvent);
	virtual void Update();
	virtual void OnGameEvent(tagGameEvent* pEvent);

private:
	NavCollider_Jump	m_collider;
	float				m_startTime;
};
