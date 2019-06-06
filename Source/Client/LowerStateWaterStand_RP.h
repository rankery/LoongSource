#pragma once
#include "lowerstate_rp.h"

/**	\class LowerStateWaterStand_RP
	\brief 远程玩家水上站立状态
*/
class LowerStateWaterStand_RP : public LowerState_RP
{
public:
	LowerStateWaterStand_RP(void);
	virtual ~LowerStateWaterStand_RP(void);

	//--LowerState_RP
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole);		
	virtual void Update(NavMap* pNav,Player* pRole);							
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent);
	virtual void OnExit();
	virtual bool IfCanChange2UpperState(FSM_RP::EUpperState state);

private:
	void PlayReadyAni(Player* pRole);
	void PlayStandAni(Player* pRole);

private:
	DWORD	m_effectID;
};
