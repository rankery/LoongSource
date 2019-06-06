#pragma once
#include "lowerstate_rp.h"

/** \class LowerStateWaterWalk_RP
	\brief 远程玩家水上行走状态
*/
class LowerStateWaterWalk_RP : public LowerState_RP
{
public:
	LowerStateWaterWalk_RP(void);
	virtual ~LowerStateWaterWalk_RP(void);

	//--LowerState_RP
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole);		
	virtual void Update(NavMap* pNav,Player* pRole);							
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent);
	virtual bool IfCanChange2UpperState(FSM_RP::EUpperState state);

private:
	void PlaySprayEffect(NavMap* pNav,Player* pRole);

private:
	NavCollider_Swim			m_collider;
	float						m_lastCDTime;
	int							m_nFoot;
};
