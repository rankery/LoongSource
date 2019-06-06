#pragma once
#include "CombatEvent.h"


class FSM_LP;
/** \class GameEventHandler_LP
	\brief 本地玩家游戏事件处理器
*/
class GameEventHandler_LP
{
public:
	GameEventHandler_LP(void);
	virtual ~GameEventHandler_LP(void);
	void SetFSM(FSM_LP* pFSM){m_pFSM=pFSM;}

	/** 游戏事件处理
		\remark 返回false表示游戏事件不需要再交给状态去处理了
	*/
	virtual bool OnGameEvent(tagGameEvent* pEvent) =0;
	virtual void OnNetCmd(tagNetCmd* pNetCmd) =0;
protected:
	FSM_LP*					m_pFSM;
};



/** \class HitTargetEventHandler_LP
	\brief 本地玩家命中目标游戏事件处理器
*/
class HitTargetEventHandler_LP : public GameEventHandler_LP
{
public:
	HitTargetEventHandler_LP(void);
	virtual ~HitTargetEventHandler_LP(void);

	//--GameEventHandler_LP
	virtual bool OnGameEvent(tagGameEvent* pEvent);
	virtual void OnNetCmd(tagNetCmd* pNetCmd);

	//--
	tagHitTargetEvent* GetLastHitTargetEvent(DWORD roleID);
protected:
	map<DWORD,tagHitTargetEvent> m_eventMap;
};





/** \class UseItemEventHandler_LP
	\brief 本地使用物品游戏事件处理器
*/
class UseItemEventHandler_LP : public GameEventHandler_LP
{
public:
	UseItemEventHandler_LP(void);
	virtual ~UseItemEventHandler_LP(void);

	//--GameEventHandler_LP
	virtual bool OnGameEvent(tagGameEvent* pEvent);
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
};
