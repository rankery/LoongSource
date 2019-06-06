#pragma once
#include "lowerstate_rp.h"

/** \class LowerStateJump_RP
	\brief 远程玩家跳跃状态
*/
class LowerStateJump_RP : public LowerState_RP
{
	enum EStep
	{
		EStep_StartJump,
		EStep_Jumpping,
		EStep_Land,
	};

public:
	LowerStateJump_RP(void);
	virtual ~LowerStateJump_RP(void);

	//--LowerState_RP
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole);		
	virtual void Update(NavMap* pNav,Player* pRole);							
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent){}
	virtual bool IfCanChange2UpperState(FSM_RP::EUpperState state);

private:
	NavCollider_Jump		m_collider;
	float					m_lastCDTime;
	EStep					m_step;
	bool					m_bPlayInEffect;//是否已播放入水特效
};
