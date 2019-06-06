#pragma once
#include "CombatEvent.h"

class FSM_RP;
/** \class GameEventHandler_RP
	\brief 远程玩家游戏事件处理器
*/
class GameEventHandler_RP
{
public:
	GameEventHandler_RP(void);
	virtual ~GameEventHandler_RP(void);
	void SetFSM(FSM_RP* pFSM){m_pFSM=pFSM;}

	virtual void OnGameEvent(tagGameEvent* pEvent) =0;
	virtual void OnNetCmd(tagNetCmd* pNetCmd) =0;

protected:
	FSM_RP*					m_pFSM;
};





/** \class HitTargetEventHandler_RP
	\brief 远程玩家命中目标游戏事件处理器
*/
class HitTargetEventHandler_RP : public GameEventHandler_RP
{
public:
	HitTargetEventHandler_RP(void);
	virtual ~HitTargetEventHandler_RP(void);

	//--GameEventHandler_RP
	virtual void OnGameEvent(tagGameEvent* pEvent);
	virtual void OnNetCmd(tagNetCmd* pNetCmd);

	//--
	tagHitTargetEvent* GetLastHitTargetEvent(DWORD roleID);
protected:
	map<DWORD,tagHitTargetEvent> m_eventMap;
};
