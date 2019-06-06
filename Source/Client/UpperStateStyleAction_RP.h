#pragma once
#include "upperstate_rp.h"
#include "PersonalityActData.h"

/**	\class UpperStateStyleAction_RP
	\brief 远程玩家空闲状态
*/
class UpperStateStyleAction_RP : public UpperState_RP
{
	enum E3ActStep
	{
		E3ActStep_None,
		E3ActStep_In,				// 进入
		E3ActStep_Holding,			// 保持
		E3ActStep_Out,				// 离开
	};
public:
	UpperStateStyleAction_RP(void);
	virtual ~UpperStateStyleAction_RP(void);

	//--UpperState_RP
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole);		
	virtual void Update(NavMap* pNav,Player* pRole);							
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent);
	virtual void OnExit();
	virtual bool IfCanPlayLowerAni();
	virtual bool IfLowerCanSetYaw();
	virtual void OnChangeLowerState(FSM_RP::ELowerState state);

private:
	tagPersonAct m_sPersonActData;
	int          m_step;
	DWORD        m_targetID;
	bool         m_bSentEventOnExit;
	bool         m_bDestRole;
};
