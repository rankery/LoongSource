#pragma once
#include "upperstate_lp.h"
#include "..\WorldDefine\msg_combat.h"

/**	\class UpperStateDead_LP
	\brief 本地玩家死亡状态
*/
class UpperStateDead_LP : public UpperState_LP
{
	enum EStep
	{
		EStep_FallDown,	//倒下
		EStep_Lie,		//躺着
	};
public:
	UpperStateDead_LP(void);
	virtual ~UpperStateDead_LP(void);

	//--UpperState_LP
	virtual void Active(tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole);		
	virtual void Update(NavMap* pNav,LocalPlayer* pRole);							
	virtual void OnGameEvent(tagGameEvent* pEvent);	
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual bool IfCanChange2UpperState(FSM_LP::EUpperState state,tagGameEvent* pEvent);
	virtual bool IfCanChange2LowerState(FSM_LP::ELowerState state,tagGameEvent* pEvent);
	virtual bool IfCanPlayLowerAni();
	virtual bool IfLowerCanSetYaw();

	//--
	virtual void Active(bool bPlayAni,LocalPlayer* pRole,ERoleReviveType reviveType);		

private:
	void Revive(void);

private:
	EStep					m_step;
	ERoleReviveType			m_reviveType;
};
