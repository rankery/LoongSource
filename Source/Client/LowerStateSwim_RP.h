#pragma once
#include "lowerstate_rp.h"

/** \class LowerStateSwim_RP
	\brief 远程玩家游泳状态
*/
class LowerStateSwim_RP : public LowerState_RP
{
public:
	LowerStateSwim_RP(void);
	virtual ~LowerStateSwim_RP(void);

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
	float						m_lastPlayeEffectTime;//上次播放水花特效时间
};
