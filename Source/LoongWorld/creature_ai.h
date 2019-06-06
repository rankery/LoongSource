//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: creature_ai.h
// author: Aslan
// actor:
// data: 2008-09-05
// last:
// brief: 游戏内生物AI类，一个有限状态机，包括多种AI状态
//-----------------------------------------------------------------------------
#pragma once

#include "ai_transition.h"
#include "ai_trigger.h"
#include "script_mgr.h"
#include "creature.h"
#include "creature_order.h"

class Unit;
class Creature;
class AIState;
class AITransition;
class AITriggerMgr;
class CVector2D;
struct tagCreatureAI;
struct tagTriggerProto;
enum  EFLEETIME;

const FLOAT CREATURE_RETURN_HEIGH = 100.0;

//-----------------------------------------------------------------------------
// AI状态类型
//-----------------------------------------------------------------------------
enum AIStateType
{
	AIST_Idle,				// 空闲状态
	AIST_Pursue,			// 追击状态
	AIST_Attack,			// 攻击状态
	AIST_Flee,				// 逃跑状态
	AIST_SOS,				// 呼救状态
	AIST_Return,			// 返回状态
	AIST_Follow,			// 跟随状态
	AIST_SpaceOut,			// 拉开距离
};

//-----------------------------------------------------------------------------
// AI状态抽象类
//-----------------------------------------------------------------------------
class AIState
{
public:
	virtual ~AIState() {}

	// 进入状态触发
	virtual VOID OnEnter(AIController* pAI) = 0;
	// 离开状态触发
	virtual VOID OnExit(AIController* pAI) = 0;
	// 更新函数
	virtual VOID Update(AIController* pAI) = 0;
	// 事件响应
	virtual BOOL OnEvent(AIController* pAI, Unit* pSrc, INT nEventType, DWORD dwEventMisc1=0, DWORD dwEventMisc2=0) = 0;
};

//-----------------------------------------------------------------------------
// 空闲状态
//-----------------------------------------------------------------------------
class AIStateIdle : public AIState
{
public:
	virtual ~AIStateIdle() {}

public:
	AIStateIdle() {}
	static AIStateIdle* Instance();

	virtual VOID OnEnter(AIController* pAI);
	virtual VOID OnExit(AIController* pAI);
	virtual VOID Update(AIController* pAI);
	virtual BOOL OnEvent(AIController* pAI, Unit* pSrc, INT nEventType, DWORD dwEventMisc1/* =0 */, DWORD dwEventMisc2/* =0 */);	// 参数会不会太少了

private:
	AIStateIdle(const AIStateIdle&);
	AIStateIdle& operator=(const AIStateIdle&);
};

//-----------------------------------------------------------------------------
// 追击状态
//-----------------------------------------------------------------------------
class AIStatePursue: public AIState
{
public:
	AIStatePursue() {}
	virtual ~AIStatePursue() {}

public:
	static AIStatePursue* Instance();

	virtual VOID OnEnter(AIController* pAI);
	virtual VOID OnExit(AIController* pAI);
	virtual VOID Update(AIController* pAI);
	virtual BOOL OnEvent(AIController* pAI, Unit* pSrc, INT nEventType, DWORD dwEventMisc1/* =0 */, DWORD dwEventMisc2/* =0 */);

private:
	AIStatePursue(const AIStatePursue&);
	AIStatePursue& operator=(const AIStatePursue&);
};

//-----------------------------------------------------------------------------
// 攻击状态
//-----------------------------------------------------------------------------
class AIStateAttack: public AIState
{
public:
	AIStateAttack() {}
	virtual ~AIStateAttack() {}

public:
	static AIStateAttack* Instance();

	virtual VOID OnEnter(AIController* pAI);
	virtual VOID OnExit(AIController* pAI);
	virtual VOID Update(AIController* pAI);
	virtual BOOL OnEvent(AIController* pAI, Unit* pSrc, INT nEventType, DWORD dwEventMisc1/* =0 */, DWORD dwEventMisc2/* =0 */);

private:
	AIStateAttack(const AIStateAttack&);
	AIStateAttack& operator=(const AIStateAttack&);
};

//-----------------------------------------------------------------------------
// 逃跑状态
//-----------------------------------------------------------------------------
class AIStateFlee: public AIState
{
public:
	AIStateFlee() {}
	virtual ~AIStateFlee() {}

public:
	static AIStateFlee* Instance();

	virtual VOID OnEnter(AIController* pAI);
	virtual VOID OnExit(AIController* pAI);
	virtual VOID Update(AIController* pAI);
	virtual BOOL OnEvent(AIController* pAI, Unit* pSrc, INT nEventType, DWORD dwEventMisc1/* =0 */, DWORD dwEventMisc2/* =0 */);

private:
	AIStateFlee(const AIStateFlee&);
	AIStateFlee& operator=(const AIStateFlee&);
};

//-----------------------------------------------------------------------------
// 呼救状态
//-----------------------------------------------------------------------------
class AIStateCallHelp : public AIState
{
public:
	AIStateCallHelp() {}
	virtual ~AIStateCallHelp() {}

public:
	static AIStateCallHelp* Instance();

	virtual VOID OnEnter(AIController* pAI);
	virtual VOID OnExit(AIController* pAI);
	virtual VOID Update(AIController* pAI);
	virtual BOOL OnEvent(AIController* pAI, Unit* pSrc, INT nEventType, DWORD dwEventMisc1/* =0 */, DWORD dwEventMisc2/* =0 */);

private:
	AIStateCallHelp(const AIStateCallHelp&);
	AIStateCallHelp& operator=(const AIStateCallHelp&);
};

//-----------------------------------------------------------------------------
// 返回状态
//-----------------------------------------------------------------------------
class AIStateReturn : public AIState
{
public:
	AIStateReturn() {}
	virtual ~AIStateReturn() {}

public:
	static AIStateReturn* Instance();

	virtual VOID OnEnter(AIController* pAI);
	virtual VOID OnExit(AIController* pAI);
	virtual VOID Update(AIController* pAI);
	virtual BOOL OnEvent(AIController* pAI, Unit* pSrc, INT nEventType, DWORD dwEventMisc1/* =0 */, DWORD dwEventMisc2/* =0 */);

private:
	AIStateReturn(const AIStateReturn&);
	AIStateReturn& operator=(const AIStateReturn&);
};

//-----------------------------------------------------------------------------
// 拉开距离
//-----------------------------------------------------------------------------
class AIStateSpaceOut : public AIState
{
public:
	AIStateSpaceOut() {}
	virtual ~AIStateSpaceOut() {}

public:
	static AIStateSpaceOut* Instance();

	virtual VOID OnEnter(AIController* pAI);
	virtual VOID OnExit(AIController* pAI);
	virtual VOID Update(AIController* pAI);
	virtual BOOL OnEvent(AIController* pAI, Unit* pSrc, INT nEventType, DWORD dwEventMisc/* =0 */, DWORD dwEventMisc2/* = 0 */);

private:
	AIStateSpaceOut(const AIStateSpaceOut&);
	AIStateSpaceOut& operator=(const AIStateSpaceOut&);
};

//-----------------------------------------------------------------------------
// 跟随状态，一般作为全局状态
//-----------------------------------------------------------------------------
class AIStateFollow: public AIState
{
public:
	AIStateFollow() {}
	virtual ~AIStateFollow() {}

public:
	static AIStateFollow* Instance();

	virtual VOID OnEnter(AIController* pAI);
	virtual VOID OnExit(AIController* pAI);
	virtual VOID Update(AIController* pAI);
	virtual BOOL OnEvent(AIController* pAI, Unit* pSrc, INT nEventType, DWORD dwEventMisc1/* =0 */, DWORD dwEventMisc2/* =0 */);

private:
	AIStateFollow(const AIStateFollow&);
	AIStateFollow& operator=(const AIStateFollow&);
};

//-----------------------------------------------------------------------------
// 脚本状态，用作所有脚本
//-----------------------------------------------------------------------------
class AIStateScript: public AIState
{
public:
	AIStateScript() {}
	virtual ~AIStateScript() {}

public:
	static AIStateScript* Instance();

	virtual VOID OnEnter(AIController* pAI);
	virtual VOID OnExit(AIController* pAI);
	virtual VOID Update(AIController* pAI);
	virtual BOOL OnEvent(AIController* pAI, Unit* pSrc, INT nEventType, DWORD dwEventMisc1/* =0 */, DWORD dwEventMisc2/* =0 */);

private:
	AIStateScript(const AIStateFollow&);
	AIStateScript& operator=(const AIStateScript&);
};

//-----------------------------------------------------------------------------
// AI控制器，为一有限状态机
//-----------------------------------------------------------------------------
class AIController
{
public:
	explicit AIController(Creature* pCreature, const tagMapWayPointInfoList* patrolList);
	~AIController() { Destroy(); }

	//--------------------------------------------------------------------------
	// 初始化和更新
	//--------------------------------------------------------------------------
	VOID			InitPatrol(const tagMapWayPointInfoList* patrolList);
	VOID			InitAIProto();
	VOID			InitTriggerMgr();
	VOID			InitAITransition();
	VOID			InitPathFinder();

	VOID			Update();
	VOID			Destroy();
	VOID			Refresh();

	//--------------------------------------------------------------------------
	// 事件触发
	//--------------------------------------------------------------------------
	VOID			OnEvent(Unit* pSrc, INT nEventType, DWORD dwEventMisc1=0, DWORD dwEventMisc2=0);

	//--------------------------------------------------------------------------
	// 得到主人
	//--------------------------------------------------------------------------
	Creature*		GetOwner()								{ return m_pOwner; }

	//--------------------------------------------------------------------------
	// 状态相关
	//--------------------------------------------------------------------------
	AIStateType		GetCurrentStateType()					{ return m_eCurAIState; }
	AIState*		GetCurrentState()						{ return m_pCurrentState; }
	AIState*		GetPreviousState()						{ return m_pPreState; }
	AIState*		GetGlobalState()						{ return m_pGlobalState; }

	VOID			SetCurrentStateType(AIStateType eType)	{ m_eCurAIState = eType; }
	VOID			SetCurrentState(AIState* pState)		{ m_pCurrentState = pState; }
	VOID			SetPreviousState(AIState* pState)		{ m_pPreState = pState; }
	VOID			SetGlobalState(AIState* pState)			{ m_pGlobalState = pState; }

	VOID			ChangeState(AIStateType eState);

	//---------------------------------------------------------------------------
	// 战斗逻辑相关
	//---------------------------------------------------------------------------

	// 通用
	DWORD			GetEnterCombatTick() const			{ return m_dwEnterCombatTick; }
	const Vector3&	GetEnterCombatPos() const			{ return m_vPosEnterCombat; }
	VOID			SetEnterCombatPos(Vector3& vPos)	{ m_vPosEnterCombat = vPos; }
	VOID			SetEnterCombatTick(DWORD dwTick)	{ m_dwEnterCombatTick = dwTick; }
	BOOL			IsTargetValid(DWORD dwID);
	DWORD			GetTargetIDByType(ECreatureTargetFriendEnemy eTargetType);

	// 巡逻
	BOOL			IsReversePatrol() const				{ return m_bReversePatrol; }
	BOOL			IsPatroling() const					{ return m_bIsPatroling; }
	VOID			SetIsPatroling(BOOL bPatrol)		{ m_bIsPatroling = bPatrol; }
	VOID			SetIsReversePatrol(BOOL bReverse)	{ m_bReversePatrol = bReverse; }
	VOID			ReSetPatrolRestTick()				{ m_nPatrolRestTick = IUTIL->Rand() % CREATURE_PATROL_REST_TICK_INTERVAL + CREATURE_PATROL_REST_TICK_MIN; }
	VOID			UpdatePatrol();
	VOID			StartPatrol();

	// 索敌
	VOID			ReSetLookForTargetTick()			{ m_nLookForTargetTick = IUTIL->Rand() % CREATURE_LOOK_FOR_ENEMY_REST_TICK_INTERVAL + CREATURE_LOOK_FOR_ENEMY_REST_TICK_MIN; }
	DWORD			UpdateLookForEnemy();
	DWORD			StartLookForEnemy();
	BOOL			UpdateLockTarget();

	// 追击
	const Vector3&	GetPursueTargetPos() const			{ return m_vPursueTargetPos; }
	DWORD			GetTargetUnitID() const				{ return m_dwTargeUnitID; }
	Unit*			GetPursueTarget();
	BOOL			GetPosNearPursueTarget(Unit* pTarget, Vector3& vPos);
	VOID			StartPursue(Unit* pTarget);
	VOID			UpdatePursue(Unit* pTarget);
	VOID			SetPursueTargetPos(Vector3& vPos)	{ m_vPursueTargetPos = vPos; }
	VOID			SetTargetUnitID(DWORD dwID)			{ m_dwTargeUnitID = dwID; }
	VOID			ReSetLockTargetTime()				{ m_nLockTargetTick = IUTIL->Rand() % CREATURE_LOCK_TARGET_TICK_INTERVAL + CREATURE_LOCK_TARGET_MIN_TICK; }

	// 攻击
	BOOL			IsInAttackDist(DWORD dwTargetUnitID);
	INT				GetNextAttackWaitTick()	const		{ return m_nNextAttackWaitTick; }
	VOID			SetNextAttackWaitTick(INT nTick)	{ m_nNextAttackWaitTick = nTick; }
	VOID			CountDownNextAttackWaitTick()		{ --m_nNextAttackWaitTick; }
	INT				GetCheckAIEvnetTick() const			{ return m_nCheckAIEventTick; }
	VOID			SetCheckAIEventTick(INT nTick)		{ m_nCheckAIEventTick = nTick; }
	INT				AIUseSkill(DWORD dwSkillID, DWORD dwTargetUnitID);
	DWORD			GetAIUseSkillID()					{ return m_dwUseSkillID; }

	// 逃跑
	const Vector3&	GetFleePos() const					{ return m_vFleePos; }
	VOID			SetFleePos(Vector3 vPos)			{ m_vFleePos = vPos; }
	BOOL			IsArrivedFleePos()					{ return m_bArrivedFleePos; }
	VOID			SetIsArrivedFleePos(BOOL bArrived)	{ m_bArrivedFleePos = bArrived; }
	VOID			SetFleeTime(EFLEETIME eFleeTime)	{ m_eFleeTime = eFleeTime; }
	EFLEETIME		GetFleeTime()						{ return m_eFleeTime; }
	VOID			CountDownNextFleeTick()				{ --m_dwFleeTick; }
	DWORD			GetFleeTick()						{ return m_dwFleeTick; }
	VOID			SetFleeTick(DWORD dwFleeTick)		{ m_dwFleeTick = dwFleeTick; }                                                                     
	VOID			CalFleePos();
	VOID			StartFlee(Vector3 vFleePos);
	VOID			UpdateFlee();

	// 呼救
	BOOL			CalHelpPos();
	DWORD			GetHelperID()						{ return m_dwHelperID; }
	VOID			SetHelperID(DWORD dwHelperID)		{ m_dwHelperID = dwHelperID; }
	BOOL			NeedReCalHelpPos();

	// 保持距离
	BOOL			CalSpaceOutPos();
	VOID			StartSpaceOut();
	VOID			UpdateSpaceOut();
	DWORD			GetSpaceOutTick()					{ return m_dwSpaceOutTick; }
	BOOL			IsArrivedPos()						{ return m_bArrivedPos; }
	VOID			SetIfArrivedPos(BOOL bArrived)		{ m_bArrivedPos = bArrived; }

	// 返回
	VOID			StartReturn();
	VOID			UpdateReturn();
	BOOL			IsArrivedReturnPos()				{ return m_bArrivedReturnPos; }

	//---------------------------------------------------------------------------
	// AI转换器相关
	//---------------------------------------------------------------------------
	AITransition*	GetTransition()						{ return m_pTransition; }

	//----------------------------------------------------------------------------
	// 触发器管理器相关
	//----------------------------------------------------------------------------
	AITriggerMgr*	GetAITriggerMgr()					{ return m_pAITrigger; }

	//----------------------------------------------------------------------------
	// 仇恨相关
	//----------------------------------------------------------------------------
	DWORD			RandRoleInEnmityList();
	DWORD			GetMinEnmityInEnmityList();
	VOID			AddEnmity(Unit *pUnit, INT nValue, BOOL bSyncTeam=TRUE);
	VOID			AddEnmityMod(Unit *pUnit, INT nValue, BOOL bSyncTeam=TRUE);
	VOID			ClearAllEnmity();
	VOID			ClearEnmity(DWORD dwRoleID);
	VOID			SetEnmityActive(DWORD dwRoleID, BOOL bActive);
	VOID			CalMaxEnmity();
	DWORD			GetMaxEnmityUnitID()				{ return m_dwMaxEnmityUnitID; }
	INT				GetEnmityValue(DWORD dwID);
	INT				GetBaseEnmityValue(DWORD dwID);
	VOID			ClearEnmityModValue(DWORD dwID);
	VOID			SetMaxEnmityUnitID(DWORD dwID)	{ m_dwMaxEnmityUnitID = dwID; }
	BOOL			IsEnmityListAllInvincible();

	//----------------------------------------------------------------------------
	// 脚本相关
	//----------------------------------------------------------------------------
	VOID			SetScriptUpdateTimer(INT nTimer)	{ m_nScriptUpdateTimer = nTimer; }

private:
	//----------------------------------------------------------------------------
	// 更新相关
	//----------------------------------------------------------------------------
	VOID			UpdateAIController();
	VOID			UpdateTransition();
	VOID			UpdateCurrentState();
	VOID			UpdateTriggerMgr();

	//----------------------------------------------------------------------------
	// 状态相关
	//----------------------------------------------------------------------------
	AIState*		GetStateByType(AIStateType eStateType);

	//----------------------------------------------------------------------------
	// 状态转换器相关
	//----------------------------------------------------------------------------
	AITransition*	GetTransitionByType();

	//----------------------------------------------------------------------------
	// 辅助函数
	//----------------------------------------------------------------------------
	BOOL			IsInStateCantUpdateAI();

private:
	Creature*				m_pOwner;					// 对应的生物

	//----------------------------------------------------------------------------
	// 状态
	//----------------------------------------------------------------------------
	AIStateType				m_eCurAIState;				// 当前AI状态枚举
	AIState*				m_pCurrentState;			// 当前AI状态
	AIState*				m_pPreState;				// 上一次的状态
	AIState*				m_pGlobalState;				// 全局AI状态

	//----------------------------------------------------------------------------
	// 状态转换器
	//----------------------------------------------------------------------------
	AITransition*			m_pTransition;				// AI状态转换器

	//----------------------------------------------------------------------------
	// 触发器管理器
	//----------------------------------------------------------------------------
	AITriggerMgr*			m_pAITrigger;				// AI触发器管理类

	//----------------------------------------------------------------------------
	// 属性
	//----------------------------------------------------------------------------
	const tagCreatureProto*	m_pProto;					// 该生物对应的静态属性
	const tagCreatureAI*	m_pAIProto;					// 该生物对应的AI静态属性

	//----------------------------------------------------------------------------
	// 脚本
	//----------------------------------------------------------------------------
	const CreatureScript*	m_pScript;					// 怪物脚本
	INT						m_nScriptUpdateTimer;		// 怪物脚本更新AI的时间，初始化时为GT_INVALID，不更新，由脚本来设置这个时间
	INT						m_nScriptUpdateCountDown;	// 怪物脚本更新AI倒计时

	//-----------------------------------------------------------------------------
	// AI战斗逻辑属性
	//-----------------------------------------------------------------------------

	// 空闲状态
	Vector3*				m_pWPArray;					// 路径巡逻怪的巡逻导航点列表
	INT						m_nWPNum;					// 共有几个导航点
	INT						m_nWPIndex;					// 当前巡逻在哪个导航点
	BOOL					m_bReversePatrol;			// 路径巡逻怪是否正在沿着相反路径巡逻
	BOOL					m_bIsPatroling;				// 正在巡逻
	INT						m_nPatrolRestTick;			// 进行下一个路径点休息的Tick
	INT						m_nLookForTargetTick;		// 上一次索敌的Tick

	// 追击和攻击
	Vector3					m_vPursueTargetPos;			// 怪物追击当前的目标点
	BOOL					m_bPursueFailed;			// 追击失败
	BOOL					m_bCanNotPursue;			// 暂时不能追击
	BOOL					m_bPathFinding;				// 寻路中
	INT						m_nCheckPursueTick;			// 间隔查询移动的时间间隔

	DWORD					m_dwTargeUnitID;			// 目标对象ID
	INT						m_nCheckAIEventTick;		// 上一次检测AI事件的tick
	DWORD					m_dwEnterCombatTick;		// 进入战斗的tick

	DWORD					m_dwUseSkillID;				// 使用的技能ID
	INT						m_nNextAttackWaitTick;		// 本次攻击完成后，下次攻击需要等待的时间
	DWORD					m_dwSerial;					// 攻击序列号

	// 逃跑
	Vector3					m_vFleePos;					// 逃跑的目标点
	CVector2D				m_vFleeDir;					// 逃跑的方向
	DWORD					m_dwFleeTick;				// 逃跑状态持续时间
	BOOL					m_bArrivedFleePos;			// 是否到达逃跑的目标点
	EFLEETIME				m_eFleeTime;				// 计入逃跑状态的次数
	DWORD					m_dwHelperID;				// 求救对象ID

	// 返回
	Vector3					m_vPosEnterCombat;			// 进入战斗时的坐标
	BOOL					m_bArrivedReturnPos;		// 是否返回到进入战斗时的坐标

	// 保持距离
	DWORD					m_dwSpaceOutTick;			// 保持距离触发间隔
	BOOL					m_bArrivedPos;				// 是否到达目标点

	//-----------------------------------------------------------------------
	// 仇恨系统
	//-----------------------------------------------------------------------
	TMap<DWORD, tagEnmity*>	m_mapEnmity;				// 仇恨列表
	DWORD					m_dwMaxEnmityUnitID;		// 最大仇恨ID
	INT						m_nLockTargetTick;			// 锁定目标倒计时
	INT						m_nNoAnmityTick;			// 没有增加仇恨的心跳数

	//-----------------------------------------------------------------------
	// 寻路相关
	//-----------------------------------------------------------------------
	PathFinder*				m_pPathFinder;
};

//---------------------------------------------------------------------------
// 改变当前AI状态
//----------------------------------------------------------------------------
inline VOID AIController::ChangeState(AIStateType eState)
{
	if( P_VALID(m_pCurrentState) )
		m_pCurrentState->OnExit(this);

	m_eCurAIState = eState;
	m_pCurrentState = GetStateByType(eState);

	if( P_VALID(m_pCurrentState) )
		m_pCurrentState->OnEnter(this);
}

//------------------------------------------------------------------------------------------
// 根据状态类型得到状态对象
//------------------------------------------------------------------------------------------
inline AIState* AIController::GetStateByType(AIStateType eStateType)
{
	switch (eStateType)
	{
	case AIST_Idle:
		return AIStateIdle::Instance();

	case AIST_Pursue:
		return AIStatePursue::Instance();

	case AIST_Attack:
		return AIStateAttack::Instance();

	case AIST_Return:
		return AIStateReturn::Instance();

	case AIST_Flee:
		return AIStateFlee::Instance();

	case AIST_SOS:
		return AIStateCallHelp::Instance();

	case AIST_SpaceOut:
		return AIStateSpaceOut::Instance();

	default:
		return AIStateScript::Instance();
	}
}

//------------------------------------------------------------------------------------------
// 更新状态机
//------------------------------------------------------------------------------------------
inline VOID AIController::UpdateAIController()
{
	// 如果无脚本，不更新
	if( !P_VALID(m_pScript) ) return;

	// 检测现在是否已经暂停
	if( GT_INVALID == m_nScriptUpdateTimer ) return;

	// 倒计时
	if( --m_nScriptUpdateCountDown > 0 ) return;

	// 调用脚本
	m_nScriptUpdateCountDown = m_nScriptUpdateTimer;
	m_pScript->OnUpdateAI(m_pOwner);
}

//------------------------------------------------------------------------------------------
// 更新状态切换
//------------------------------------------------------------------------------------------
inline VOID AIController::UpdateTransition()
{
	if( !P_VALID(m_pTransition) ) return;

	// 检测状态是否转换
	AIStateType eNewState = m_pTransition->Transition(this, m_eCurAIState);

	if( m_eCurAIState != eNewState )
	{
		ChangeState(eNewState);
	}
}

//------------------------------------------------------------------------------------------
// 更新当前状态
//------------------------------------------------------------------------------------------
inline VOID AIController::UpdateCurrentState()
{
	// 更新当前AI状态
	if( P_VALID(m_pCurrentState) )
		m_pCurrentState->Update(this);

	// 更新全局AI状态
	if( P_VALID(m_pGlobalState) )
		m_pGlobalState->Update(this);
}

//------------------------------------------------------------------------------------------
// 更新触发器
//------------------------------------------------------------------------------------------
inline VOID AIController::UpdateTriggerMgr()
{
	// 更新触发器
	if( P_VALID(m_pAITrigger) && !m_pAITrigger->IsPaused() )
	{
		m_pAITrigger->Update();
	}
}

//------------------------------------------------------------------------------------------
// 处在不能更新AI的状态
//------------------------------------------------------------------------------------------
inline BOOL AIController::IsInStateCantUpdateAI()
{
	return m_pOwner->IsInState(ES_Dead) || m_pOwner->IsInState(ES_Dizzy); // || 击飞
}
