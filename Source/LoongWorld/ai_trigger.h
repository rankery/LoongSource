//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: ai_trigger.h
// author: wjiang
// actor:
// data: 2009-6-25
// last:
// brief: 怪物AI触发器
//-----------------------------------------------------------------------------
#pragma once

class Unit;
class Creature;
class AITriggerScript;
struct tagTriggerProto;
enum ECreatureTargetFriendEnemy;


class AITrigger
{
public:
	AITrigger();
	~AITrigger() {}

public:
	BOOL		Init(DWORD dwCreatureTypeID, DWORD dwTypeID, INT dwTargetType, DWORD dwSkillID=GT_INVALID);
	VOID		Refresh();
	VOID		SetTriggerActive(INT eEventType);
	VOID		OnEvent(INT eEventType, Creature* pOwner);
	VOID		Update(Creature* pOwner);

private:
	VOID		SetActive(BOOL bActive) { m_bActive = bActive; }
	BOOL		TestTimeIntervalTrigger();
	BOOL		TestEventTrigger(Creature* pOwner);
	BOOL		TestStateTrigger(Creature* pOwner, DWORD& dwTargetID);
	VOID		OnTrigger(Creature* pOwner, DWORD dwTargetID);

	BOOL						m_bActive;			// 触发器是否被激活
	INT							m_nActiveTick;		// 激活时间
	DWORD						m_dwTriggeredTimes;	// 已触发的次数
	DWORD						m_dwSkillID;		// 技能ID
	ECreatureTargetFriendEnemy	m_eTargeType;		// 目标类型
	const tagTriggerProto*		m_pProto;
};

class AITriggerMgr
{
public:
	AITriggerMgr():m_pOwner(NULL), m_bPaused(FALSE) {}
	~AITriggerMgr();

public:
	BOOL		Init(Creature* pOwner, const tagCreatureAI* m_pAIProto);
	VOID		SetTriggerActive(INT eEventType);
	VOID		Refresh();
	VOID		Update();
	VOID		OnEvent(INT eEventType);

	VOID		Pause()			{ m_bPaused = TRUE; }
	VOID		UnPause()		{ m_bPaused = FALSE; }
	BOOL		IsPaused()		{ return m_bPaused; }

public:
	Creature*					m_pOwner;			 
	BOOL						m_bPaused;
	std::list<AITrigger*>		m_listTrigger;
};