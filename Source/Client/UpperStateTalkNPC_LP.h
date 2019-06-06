#pragma once
#include "upperstate_lp.h"

class LocalPlayer;
class Role;
class UpperStateTalkNPC_LP :
	public UpperState_LP
{
public:
	UpperStateTalkNPC_LP(void);
	virtual ~UpperStateTalkNPC_LP(void);

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
	void SendTalkNPCEvent();
	void SendStopPursueEvent();
	void FaceToTarget();

private:
	DWORD					m_npcID;
	DWORD					m_npcTypeID;
	float					m_lastUpdatetime;
};	
