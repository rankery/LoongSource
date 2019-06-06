#pragma once
#include "lowerstate_rp.h"
#include "CombatEvent.h"

/** \class LowerStatePull_RP
	\brief 远程玩家下身拖拽状态
*/
class LowerStatePull_RP : public LowerState_RP
{
public:
	LowerStatePull_RP(void);
	virtual ~LowerStatePull_RP(void);

	//--LowerState_RP
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole);		
	virtual void Update(NavMap* pNav,Player* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual bool IfCanChange2UpperState(FSM_RP::EUpperState state);

private:
	NavCollider_Move				m_collider;
};
