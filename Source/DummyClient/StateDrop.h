#pragma once
#include "state.h"

/** µôÂä×´Ì¬
*/
class StateDrop :
	public State
{
public:
	StateDrop(void);
	virtual ~StateDrop(void);

	//--State
	virtual void Active(tagGameEvent* pEvent);
	virtual void Update();
	virtual void OnGameEvent(tagGameEvent* pEvent);

private:
	NavCollider_Jump	m_collider;
	float				m_startTime;
};
