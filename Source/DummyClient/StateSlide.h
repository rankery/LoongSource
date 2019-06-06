#pragma once
#include "state.h"
#include "GameEvent.h"


/** »¬Âä×´Ì¬
*/
class StateSlide :
	public State
{
public:
	StateSlide(void);
	virtual ~StateSlide(void);

	//--State
	virtual void Active(tagGameEvent* pEvent);
	virtual void Update();
	virtual void OnGameEvent(tagGameEvent* pEvent);

private:
	NavCollider_Slide	m_collider;
	tagSlideEvent		m_event;
};
