#pragma once
#include "lowerstate_rp.h"

/** \class LowerStateMove_RP
	\brief 远程玩家下身移动状态
*/
class LowerStateMove_RP : public LowerState_RP
{
public:
	LowerStateMove_RP(void);
	virtual ~LowerStateMove_RP(void);

	//--LowerState_RP
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole);		
	virtual void Update(NavMap* pNav,Player* pRole);							
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent);
	virtual void ReplayAni(Player* pRole);
	virtual bool IfCanChange2UpperState(FSM_RP::EUpperState state);

private:
	void PlaySprayEffect(NavMap* pNav,Player* pRole);

private:
	NavCollider_Move	m_collider;
	float				m_lastCDTime;
	float				m_lastPlayeEffectTime;
};
