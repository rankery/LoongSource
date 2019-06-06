#pragma once
#include "CombatEvent.h"

class FSM_NPC;
/** \class GameEventHandler_NPC
	\brief NPC游戏事件处理器
*/
class GameEventHandler_NPC
{
public:
	GameEventHandler_NPC(void);
	virtual ~GameEventHandler_NPC(void);
	void SetFSM(FSM_NPC* pFSM){m_pFSM=pFSM;}

	virtual void OnGameEvent(tagGameEvent* pEvent) =0;
	virtual void OnNetCmd(tagNetCmd* pNetCmd) =0;

protected:
	FSM_NPC*					m_pFSM;
};





/** \class HitTargetEventHandler_NPC
	\brief NPC命中目标游戏事件处理器
*/
class HitTargetEventHandler_NPC : public GameEventHandler_NPC
{
public:
	HitTargetEventHandler_NPC(void);
	virtual ~HitTargetEventHandler_NPC(void);

	//--GameEventHandler_NPC
	virtual void OnGameEvent(tagGameEvent* pEvent);
	virtual void OnNetCmd(tagNetCmd* pNetCmd);

	//--
	tagHitTargetEvent* GetLastHitTargetEvent(DWORD roleID);
protected:
	map<DWORD,tagHitTargetEvent> m_eventMap;
};
