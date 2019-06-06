#pragma once
#include "lowerstate_lp.h"

/**	\class LowerStateFloat_LP
	\brief 本地玩家下身漂浮状态
*/
class LowerStateFloat_LP : public LowerState_LP
{
public:
	LowerStateFloat_LP(void);
	virtual ~LowerStateFloat_LP(void);

	//--LowerState_LP
	virtual void Active(tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole,tagPreState* pPreState=NULL);	
	virtual void Update(NavMap* pNav,LocalPlayer* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void ReplayAni(LocalPlayer* pRole);
	virtual void OnExit();
	virtual bool IfCanChange2UpperState(FSM_LP::EUpperState state,tagGameEvent* pEvent);
	virtual bool IfCanChange2LowerState(FSM_LP::ELowerState state,tagGameEvent* pEvent);

private:
	DWORD						m_effectID;
	TObjRef<NetSession>			m_pSession;
	TObjRef<GameFrameMgr>		m_pGameFrameMgr;
};
