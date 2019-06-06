#pragma once
#include "state.h"
#include "GameEvent.h"

/** 游泳状态
*/
class StateSwim :
	public State
{
public:
	StateSwim(void);
	virtual ~StateSwim(void);

	//--State
	virtual void Active(tagGameEvent* pEvent);
	virtual void Update();
	virtual void OnGameEvent(tagGameEvent* pEvent);

private:
	tagSwimEvent	m_event;
	Vector3			m_moveStart;
	Vector3			m_moveDest;
	Vector3			m_moveDir;
	float			m_moveStartTime;
	float			m_moveTime;//移动到目标点需要多少时间
	NavCollider::EReturn m_ret;

	list<NavCollider::tagMovePoint>	m_movePath;
};
