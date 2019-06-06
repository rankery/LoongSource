#pragma once
#include "state.h"

/** 漂浮状态
*/
class StateFloat :
	public State
{
	enum EStep
	{
		EStep_GoDown,	//下沉
		EStep_FloatUp,	//上浮
		EStep_Floating,	//保持在水面
	};
public:
	StateFloat(void);
	virtual ~StateFloat(void);

	//--State
	virtual void Active(tagGameEvent* pEvent);
	virtual void Update();
	virtual void OnGameEvent(tagGameEvent* pEvent);

private:
	float m_startH;
	float m_endH;
	float m_startTime;
	float m_totalTime;
	EStep m_step;
};
