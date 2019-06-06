#pragma once
#include "upperstate_lp.h"
#include "PersonalityActData.h"

/**	\class UpperStateStyleAction_LP
	\brief 本地玩家个性动作
*/
class UpperStateStyleAction_LP : public UpperState_LP
{
	enum E3ActStep
	{
		E3ActStep_None,
		E3ActStep_In,				// 进入
		E3ActStep_Holding,			// 保持
		E3ActStep_Out,				// 离开
	};
public:
	UpperStateStyleAction_LP(void);
	virtual ~UpperStateStyleAction_LP(void);

	//--UpperState_LP
	virtual void Active(tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole);
	virtual void Update(NavMap* pNav,LocalPlayer* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual bool IfCanChange2UpperState(FSM_LP::EUpperState state,tagGameEvent* pEvent);
	virtual bool IfCanChange2LowerState(FSM_LP::ELowerState state,tagGameEvent* pEvent);
	virtual void OnChangeLowerState(FSM_LP::ELowerState newState,tagGameEvent* pEvent);
	virtual bool IfCanPlayLowerAni();
	virtual bool IfLowerCanSetYaw();
	virtual void OnExit();
private:
	tagPersonAct m_sPersonActData;
	int          m_step;
	DWORD        m_targetID;
	bool         m_bSentEventOnExit;
	bool         m_bDestRole;
};
