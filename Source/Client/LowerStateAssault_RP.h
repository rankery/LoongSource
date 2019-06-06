#pragma once
#include "lowerstate_rp.h"
#include "CombatEvent.h"

/** \class LowerStateAssault_RP
	\brief 远程玩家下身冲锋状态
*/
class LowerStateAssault_RP : public LowerState_RP
{
public:
	LowerStateAssault_RP(void);
	virtual ~LowerStateAssault_RP(void);

	//--LowerState_RP
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole);		
	virtual void Update(NavMap* pNav,Player* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual bool IfCanChange2UpperState(FSM_RP::EUpperState state);
	virtual void OnExit();

private:
	NavCollider_Move				m_collider;
	DWORD                           m_effectID;
};
