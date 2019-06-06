#pragma once
#include "lowerstate_lp.h"

/**	\class LowerStateWaterStand_LP
	\brief 本地玩家下身水上站立状态
*/
class LowerStateWaterStand_LP : public LowerState_LP
{
public:
	LowerStateWaterStand_LP(void);
	virtual ~LowerStateWaterStand_LP(void);

	//--LowerState_LP
	virtual void Active(tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole,tagPreState* pPreState=NULL);	
	virtual void Update(NavMap* pNav,LocalPlayer* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnExit();
	virtual bool IfCanChange2UpperState(FSM_LP::EUpperState state,tagGameEvent* pEvent);
	virtual bool IfCanChange2LowerState(FSM_LP::ELowerState state,tagGameEvent* pEvent);

private:
	DWORD m_effectID;
};
