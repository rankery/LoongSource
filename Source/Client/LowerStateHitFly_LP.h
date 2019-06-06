#pragma once
#include "lowerstate_lp.h"
#include "CombatEvent.h"

/** \class LowerStateHitFly_LP
	\brief 本地玩家下身击飞状态
*/
class LowerStateHitFly_LP : public LowerState_LP
{
	enum EStep
	{
		EStep_Fly,		//击飞
		EStep_Craw,		//爬起
	};
public:
	LowerStateHitFly_LP(void);
	virtual ~LowerStateHitFly_LP(void);

	//--LowerState_LP
	virtual void Active(tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole,tagPreState* pPreState=NULL){}
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,LocalPlayer* pRole,tagPreState* pPreState=NULL);
	virtual void Update(NavMap* pNav,LocalPlayer* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd){}
	virtual bool IfCanChange2UpperState(FSM_LP::EUpperState state,tagGameEvent* pEvent);
	virtual bool IfCanChange2LowerState(FSM_LP::ELowerState state,tagGameEvent* pEvent);

	//--

private:
	NavCollider_HitFly		m_collider;	
	EStep					m_step;
};
