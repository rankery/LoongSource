//--------------------------------------------------------------------------------
//!\file script_mgr.h
//!\author Aslan
//!
//!\date 2008-09-01
//! last 2008-09-01
//!
//!\brief Lua脚本管理器，主要负责各种脚本的组织、生成及调用
//!
//!	Copyright (c) 2004 CTCRST Entertainment All rights reserved.
//--------------------------------------------------------------------------------
#pragma once

extern "C"
{
#include "../lua/src/lua.h"
#include "../lua/src/lauxlib.h"
#include "../lua/src/lualib.h"
};
#include "mutex.h"
#include "event_mgr.h"
#include "../ServerDefine/activity_define.h"
#include "../WorldDefine/penalty_define.h"

class Unit;
class Creature;
class Role;
class CActivity;
class ScriptMgr;
class Skill;

struct tagMapTriggerInfo;
struct tagMapAreaInfo;

enum ERoleReviveType;

//-------------------------------------------------------------------------------
// 任务脚本事件枚举
//-------------------------------------------------------------------------------
enum EScriptQuestEvent
{
	ESQE_On_Accept			=	0,			// 任务接取
	ESQE_On_Complete		=	1,			// 任务完成
	ESQE_On_Cancel			=	2,			// 任务取消
	ESQE_On_Creature_Kill	=	3,			// 杀死一只怪物
	ESQE_On_CheckAccept		=	4,			// 检测接取
	ESQE_On_CheckComplete	=	5,			// 检测交
	ESQE_On_NPC_Talk		=	6,			// 和NPC对话
	ESQE_On_Init			=	7,			// 任务初始化
	ESQE_On_Dlg_Default		=	8,			// 服务器可控对话框缺省事件
	ESQE_On_Invest			=	9,			// 调查地物		
	ESQE_End				=	10,
};

//---------------------------------------------------------------------------------
// 怪物脚本事件枚举
//---------------------------------------------------------------------------------
enum EScriptCreatureEvent
{
	// 日常事件
	ESCAE_On_Load			=	0,			// 载入
	ESCAE_On_Respawn		=	1,			// 重生
	ESCAE_On_Enter_Combat	=	2,			// 进入战斗
	ESCAE_On_Leave_Combat	=	3,			// 离开战斗
	ESCAE_On_Die			=	4,			// 死亡
	ESCAE_On_Timer			=	5,			// 定时触发
	ESCAE_On_Invest			=	6,			// 被调查
	ESCAE_On_Talk			=	7,			// 被说话

	// AI事件
	ESCAE_On_UpdateAI		=	8,			// 更新AI状态机
	ESCAE_On_UpdateCurAI	=	9,			// 更新当前AI状态
	ESCAE_On_EnterCurAI		=	10,			// 进入当前AI状态
	ESCAE_On_LeaveCurAI		=	11,			// 离开当前AI状态
	ESCAE_On_EventCurAI		=	12,			// 当前AI状态事件触发
	ESCAE_On_Disappear		=	13,			// 怪物消失出发

	ESCAE_End				=	14,
};

//---------------------------------------------------------------------------------
// 玩家脚本
//---------------------------------------------------------------------------------
enum EScriptRoleEvent
{
	ESRE_On_Online				=	0,			// 上线
	ESRE_On_FirstOnline			=	1,			// 第一次上线
	ESRE_On_IntoWorld			=	2,			// 进入游戏世界
	ESRE_On_FirstIntoWorld		=	3,			// 第一次进入游戏世界
	ESRE_On_EnterMap			=	4,			// 进入地图
	ESRE_IsDeadPenalty			=   5,			// 是否对玩家进行死亡惩罚
	ESRE_On_LevelChange			=	6,			// 等级提升
	ESRE_On_OpenChest			=	7,			// 开启宝箱
	ESRE_On_StopChest			=	8,			// 停止开启，产生随机物品
	ESRE_On_AgainChest			=	9,			// 再开一次
	ESRE_On_GetItem				=	10,			// 得到宝箱物品
	ESRE_On_GetMarriage			=	11,			// 结婚
	ESRE_On_GetMarriageCheck	=	12,			// 结婚检查
	ESRE_On_BreakMarriage		=	13,			// 离婚
	ESRE_On_BreakMarriageCheck	=	14,			// 离婚检查
	ESRE_IsEquipLootPenalty		=   15,			// 装备掉落惩罚
	ESRE_IsExpPenalty			=	16,			// 经验惩罚
	ESRE_IsInjuryPenalty		=	17,			// 内伤惩罚
	ESRE_IsMoralityPenalty		=	18,			// 道德惩罚
	ESRE_IsHostilityPenalty		=	19,			// 戾气惩罚
	ESRE_IsCommercePenalty		=	20,			// 跑商商银惩罚
	ESRE_GetRoleFishingExp		=	21,			// 钓鱼经验（离线用）
	ESRE_CanCancelPenalty		=	22,			// 是否能免除死亡惩罚
	ESRE_On_CompleteCommerce	=	23,			// 完成跑商

	ESRE_End					=	24,
};

//---------------------------------------------------------------------------------
// 地图脚本
//---------------------------------------------------------------------------------
enum EScriptMapEvent
{
	ESME_OnInit					=	0,			// 初始化时
	ESME_OnTimer				=	1,			// 定时更新
	ESME_OnPlayerEnter			=	2,			// 玩家进入
	ESME_OnPlayerLeave			=	3,			// 玩家离开
	ESME_OnCreatureDie			=	4,			// 生物死亡
	ESME_OnRoleDie				=	5,			// 玩家死亡
	ESME_OnEnterTrigger			=	6,			// 进入触发器
	ESME_OnEnterArea			=	7,			// 进入区域
	ESME_CanInviteJoinTeam		=	8,			// 是否允许邀请组队
	ESME_CanLeaveTeam			=	9,			// 是否允许离开队伍
	ESME_CanChangeLeader		=	10,			// 是否能移交队长
	ESME_On_Revive				=	11,			// 玩家复活	
	ESME_CanEnterWhenOnline		=   12,			// 玩家上线时是否能计入地图
	ESME_GetExportMapAndCoord	=	13,			// 得到玩家离开当前地图后的地图ＩＤ和坐标
	ESME_GetOnePerfectMap		=   14,			// 找到最佳的副本实例
	ESME_CanEnter				=	15,			// 玩家是否能进入该地图
	ESME_FriendEnemy			=	16,			// 两个对象间的敌我关系
	ESME_CanKickMember			=	17,			// 是否允许踢掉队友
	ESME_OnCreatureDisappear	=	18,			// 生物消失
	ESME_Safeguard				=	19,			// 是否允许玩家开启保护模式
	ESME_CanUseItem				=	20,			// 是否允许使用物品
	ESME_CanUseSkill			=	21,			// 是否允许使用技能
	ESME_End					=	22,
};

//---------------------------------------------------------------------------------
// 固定活动脚本
//---------------------------------------------------------------------------------
enum EScriptActEvent
{
	ESAE_OnInit				=	0,			// 初始化时
	ESAE_OnTimer			=	1,			// 定时更新
	ESAE_OnStart			=	2,			// 活动开始
	ESAE_OnEnd				=	3,			// 活动结束
	ESAE_OnTimerMin			=	4,			// 活动每分钟更新
	ESAE_OnDefaultRequest	=	5,			// 客户端触发服务器脚本的缺省消息 
	ESAE_Broad				=	6,			// 活动广播
	ESAE_End				=	7,
};

//---------------------------------------------------------------------------------
// 游戏世界事件脚本
//---------------------------------------------------------------------------------
enum EScriptWorldEvent
{
	ESWE_Adventure			=	0,			// 奇遇产生
	ESWE_End				=	1,
};

//---------------------------------------------------------------------------------
// 物品事件脚本
//---------------------------------------------------------------------------------
enum EScriptItemEvent
{
	ESIE_CanUse				=	0,			// 物品是否可用
	ESIE_Use				=	1,			// 物品使用
	ESIE_End
};

//---------------------------------------------------------------------------------
// 技能事件脚本
//---------------------------------------------------------------------------------
enum EScriptSkillEvent
{
	ESSE_CanCast			=	0,			// 技能是否可用
	ESSE_Cast				=	1,			// 技能使用	
	ESSE_End
};

//---------------------------------------------------------------------------------
// 通用脚本类
//---------------------------------------------------------------------------------
template<INT nSize>
class Script
{
public:
	VOID RegisterFunc(INT nIndex, const CHAR* szFunc);
	VOID Destroy();

protected:
	Script();
	~Script();

protected:
	CHAR*	m_szFunc[nSize];		// 脚本函数字符串
};

template<INT nSize>
inline Script<nSize>::Script()
{
	ZeroMemory(m_szFunc, sizeof(m_szFunc));
}

template<INT nSize>
inline Script<nSize>::~Script()
{
	Destroy();
}

template<INT nSize>
inline VOID Script<nSize>::RegisterFunc(INT nIndex, const CHAR* szFunc)
{
	if( nIndex < 0  || nIndex >= nSize ) return;
	if( !P_VALID(szFunc) ) return;

	if( P_VALID(m_szFunc[nIndex]) )
	{
		free(m_szFunc[nIndex]);
	}

	m_szFunc[nIndex] = _strdup(szFunc);
}

template<INT nSize>
inline VOID Script<nSize>::Destroy()
{
	for(INT n = 0; n < nSize; ++n)
	{
		if( P_VALID(m_szFunc[n]) )
		{
			free(m_szFunc[n]);
			m_szFunc[n] = NULL;
		}
	}
}

//---------------------------------------------------------------------------------
// 任务脚本
//---------------------------------------------------------------------------------
class QuestScript : public Script<ESQE_End>
{
	friend class ScriptMgr;
private:
	~QuestScript() {}
public:
	INT  CheckAccept(UINT16 u16QuestID, Role* pRole, Creature* pNPC) const;
	INT  CheckComplete(UINT16 u16QuestID, Role* pRole, Creature* pNPC) const;

	VOID OnInit(UINT16 u16QuestID, Role* pOwner) const;
	VOID OnAccept(UINT16 u16QuestID, Role* pOwner, Creature* pNPC) const;
	VOID OnComplete(UINT16 u16QuestID, Role* pOwner, Creature* pNPC) const;
	VOID OnCancel(UINT16 u16QuestID, Role* pOwner) const;
	VOID OnCreatureKill(UINT16 u16QuestID, Role* pOwner, DWORD dwCreatureTypeID) const;
	VOID OnNPCTalk(UINT16 u16QuestID, Role* pOwner, DWORD dwNPCID, DWORD dwNPCTypeID) const;
	VOID OnDlgDefault(UINT16 u16QuestID, Role* pOwner, DWORD dwDlgOption) const;
	VOID OnInvest(UINT16 u16QuestID, Role* pOwner, DWORD dwCreatureTypeID) const;
};

//----------------------------------------------------------------------------------
// 怪物AI脚本
//----------------------------------------------------------------------------------
class CreatureScript : public Script<ESCAE_End>
{
	friend class ScriptMgr;
private:
	~CreatureScript() {}
public:
	VOID OnLoad(Creature* pCreature) const;
	VOID OnTalk(Creature* pCreature, Role* pRole, INT nIndex=-1) const;
	VOID OnRespawn(Creature* pCreature) const;
	VOID OnEnterCombat(Creature* pCreature) const;
	VOID OnLeaveCombat(Creature* pCreature) const;
	VOID OnDie(Creature* pCreature, Unit* pKiller, BOOL& bRemove) const;
	VOID OnInvest(Creature* pCreature, Role* pScr) const;

	VOID OnUpdateAI(Creature* pCreature) const;
	VOID OnUpdateCurAI(Creature* pCreature) const;
	VOID OnEnterCurAI(Creature* pCreature) const;
	VOID OnLeaveCurAI(Creature* pCreature) const;
	VOID OnDisappear(Creature* pCreature) const;
	BOOL OnEventCurAI(Creature* pCreature) const;
};

//-------------------------------------------------------------------------------------
// 玩家脚本
//-------------------------------------------------------------------------------------
class RoleScript : public Script<ESRE_End>
{
	friend class ScriptMgr;
private:
	~RoleScript() {}
public:
	VOID	OnRoleOnline(Role* pRole) const;
	VOID	OnRoleFirstOnline(Role* pRole) const;
	VOID	OnRoleIntoWorld(Role* pRole) const;
	VOID	OnRoleFirstIntoWorld(Role* pRole) const;
	VOID	OnRoleEnterMap(Role* pRole) const;
	BOOL	IsDeadPenalty(Role* pRole, EDeadPenaltyMode eMode) const;
	VOID	OnRoleLevelChange(Role* pRole) const;
	VOID	OnOpenChest(Role* pRole, DWORD dwChestID, DWORD dwKeyID) const;
	VOID	OnStopChest(Role* pRole, DWORD dwChestID, DWORD dwKeyID) const;
	VOID	OnAgainChest(Role* pRole) const;
	VOID	OnGetItem(Role* pRole, DWORD dwChestID, DWORD dwItemID, INT nNum) const;
	VOID	OnGetMarriage(Role* pRole, DWORD dwApplicantRoleID,DWORD dwTargetRoleID) const;
	DWORD	OnGetMarriageCheck(Role* pRole, DWORD dwApplicantRoleID,DWORD dwTargetRoleID) const;
	VOID	OnBreakMarriage(Role* pRole, DWORD dwApplicantRoleID,DWORD dwTargetRoleID) const;
	DWORD	OnBreakMarriageCheck(Role* pRole, DWORD dwApplicantRoleID,DWORD dwTargetRoleID) const;
	INT		OnGetRoleFishingExp(Role* pRole) const;
	DWORD	CanCancelPenalty(Role* pRole, Unit* pKiller, BOOL& bEquip, BOOL& bExp, BOOL& bInjury, BOOL& bHostility, BOOL& bCommerce) const;
	VOID	OnCompleteCommerce(Role* pRole,DWORD dwTaelProgress) const;
};

//-------------------------------------------------------------------------------------
// 地图脚本
//-------------------------------------------------------------------------------------
class MapScript : public Script<ESME_End>
{
	friend class ScriptMgr;
private:
	~MapScript() {}
public:
	VOID	OnInit(Map* pMap) const;
	VOID	OnTimer(Map* pMap, INT nMilliSec) const;
	VOID	OnPlayerEnter(Role* pRole, Map* pMap) const;
	VOID	OnPlayerLeave(Role* pRole, Map* pMap) const;
	VOID	OnCreatureDie(Creature* pCreature, Unit* pKiller, Map* pMap) const;
	VOID	OnRoleDie(Role* pRole, Unit* pKiller, Map* pMap) const;
	VOID	Revive(Role* pRole, ERoleReviveType eType, INT &nReviveHP, INT &nReviveMP, FLOAT &fx, FLOAT &fy, FLOAT &fz, DWORD &dwRebornMapID) const;
	VOID	OnEnterTrigger(Role* pRole, tagMapTriggerInfo* pTrigger, Map* pMap) const;
	VOID	OnEnterArea(Role* pRole, tagMapAreaInfo* pArea, Map* pMap) const;
	INT		CanInviteJoinTeam(Map* pMap) const;
	INT		CanLeaveTeam(Map* pMap) const;
	INT		CanChangeLeader(Map* pMap) const;
	INT		CanKickMember(Map* pMap) const;
	DWORD	FriendEnemy(Map* pMap, Unit* pSrc, Unit* pTarget, BOOL& bIgnore) const;
	VOID	OnCreatureDisappear(Creature* pCreature, Map* pMap) const;
	INT		CanSetSafeguard(Map* pMap) const;
	BOOL	CanUseSkill(Map* pMap, DWORD dwTypeID) const;
	BOOL	CanUseItem(Map* pMap, DWORD dwTypeID) const;

	// 脚本创建的副本独有接口
	INT		GetExportMapAndCoord(Role* pRole, DWORD& dwOutMapID, Vector3& vOut) const;
	VOID	CanTakeOverWhenOnline(Role* pRole, DWORD& dwOutMapID, Vector3& vOut) const;
	VOID	GetOnePerfectMap(Role* pRole, DWORD &dwInstanceID) const;
	INT		CanEnter(Role* pRole) const;
};

//-------------------------------------------------------------------------------------
// 活动脚本
//-------------------------------------------------------------------------------------
class ActScript : public Script<ESAE_End>
{
	friend class ScriptMgr;
private:
	~ActScript() {}
public:
	VOID OnInit(DWORD dwActID) const;
	VOID OnTimer(DWORD dwActID, INT nSec) const;
	VOID OnTimerMin(DWORD dwActID) const;
	VOID OnActStart(DWORD dwActID) const;
	VOID OnActEnd(DWORD dwActID) const;
	VOID OnDefaultRequest(DWORD dwActID, Role* pRole, DWORD	dwEventType) const;
	VOID BroadActivityState(EActBroadMode eState) const;
};

//-------------------------------------------------------------------------------------
// 物品脚本
//-------------------------------------------------------------------------------------
class ItemScript : public Script<ESIE_End>
{
	friend class ScriptMgr;
private:
	~ItemScript() {}
public:
	BOOL CanUseItem(Map* pMap, DWORD dwTypeID, DWORD dwTargetID, BOOL &bIgnore) const;
	VOID UseItem(Map* pMap, DWORD dwTypeID, DWORD dwTargetID) const;
};

//-------------------------------------------------------------------------------------
// 技能脚本
//-------------------------------------------------------------------------------------
class SkillScript : public Script<ESSE_End>
{
	friend class ScriptMgr;
private:
	~SkillScript () {}
public:
	DWORD CanCastSkill(Map* pMap, DWORD dwSkillID, DWORD dwOwnerID, DWORD dwDstUnitID) const;
	VOID CastSkill(Map* pMap, DWORD dwSkillID, DWORD dwOwnerID, BOOL &bIgnore) const;
};


//-------------------------------------------------------------------------------------
// 游戏世界事件脚本
//-------------------------------------------------------------------------------------
class WorldScript : public Script<ESWE_End>
{
	friend class ScriptMgr;
private:
	~WorldScript() {}
public:
	VOID OnAdventure(Role* pRole) const;
};

//-------------------------------------------------------------------------------------
// 脚本管理器
//-------------------------------------------------------------------------------------
class ScriptMgr : public EventMgr<ScriptMgr>
{
public:
	//---------------------------------------------------------------------------------
	// 初始化和销毁
	//---------------------------------------------------------------------------------
	BOOL Init();
	VOID Update();
	VOID Destroy();
	
	//---------------------------------------------------------------------------------
	// 注册脚本函数
	//---------------------------------------------------------------------------------
	VOID RegisterCreatureEvent(DWORD dwID, EScriptCreatureEvent eEvent, const CHAR* szFunction);
	VOID RegisterQuestEvent(UINT16 u16QuestID, EScriptQuestEvent eEvent, const CHAR* szFunction);
	VOID RegisterRoleEvent(EScriptRoleEvent eEvent, const CHAR* szFunction);
	VOID RegisterMapEvent(const CHAR* szMapName, EScriptMapEvent eEvent, const CHAR* szFunction);
	VOID RegisterActEvent(DWORD dwActID, EScriptActEvent eEvent, const CHAR* szFunction);
	VOID RegisterWorldEvent(EScriptWorldEvent eEvent, const CHAR* szFunction);
	VOID RegisterItemEvent(DWORD dwTypeID, EScriptItemEvent eEvent, const CHAR* szFunction);
	VOID RegisterSkillEvent(DWORD dwTypeID, EScriptSkillEvent eEvent, const CHAR* szFunction);	
	//---------------------------------------------------------------------------------
	// 生成脚本对象
	//---------------------------------------------------------------------------------
	const CreatureScript*	GetCreatureScript(DWORD dwCreatureID)	{ return m_mapCreatureScript.Peek(dwCreatureID); }
	const QuestScript*		GetQuestScript(UINT16 u16QuestID)		{ return m_mapQusetScript.Peek(u16QuestID); }
	const MapScript*		GetMapScript(DWORD dwMapID)				{ return m_mapMapScript.Peek(dwMapID); }
	const RoleScript*		GetRoleScript()							{ return m_pRoleScript; }
	const ActScript*		GetActScript(DWORD dwActID)				{ return m_mapActScript.Peek(dwActID); }
	const WorldScript*		GetWorldScript()						{ return m_pWorldScript; }
	const ItemScript*		GetItemScript(DWORD dwTypeID)			{ return m_mapItemScript.Peek(dwTypeID); }
	const SkillScript*		GetSkillScript(DWORD dwTypeID)			{ return m_mapSkillScript.Peek(dwTypeID); }

	//---------------------------------------------------------------------------------
	// 调用脚本
	//---------------------------------------------------------------------------------
	VOID	CallScriptFunction(CHAR* szFunction, CHAR* szFormat, ...);

	//---------------------------------------------------------------------------------
	// 生成脚本锁
	//---------------------------------------------------------------------------------
	DWORD	CreateScriptMutex();

	//---------------------------------------------------------------------------------
	// 锁住解除某个脚本锁
	//---------------------------------------------------------------------------------
	VOID	LockScriptMutex(DWORD dwMutexID);
	VOID	UnLockScriptMutex(DWORD dwMutexID);

	//---------------------------------------------------------------------------------
	// 获取全局变量
	//---------------------------------------------------------------------------------
	template<typename T>
	VOID	GetGlobal(LPCSTR strVarName, OUT T &res);

private:
	//---------------------------------------------------------------------------------
	// 注册异步事件函数
	//---------------------------------------------------------------------------------
	static VOID	RegisterScriptEventFunc();

	//---------------------------------------------------------------------------------
	// 异步事件处理函数
	//---------------------------------------------------------------------------------
	VOID	OnReload(DWORD dwSenderID, LPVOID pEventMessage);

	//---------------------------------------------------------------------------------
	// 初始化
	//---------------------------------------------------------------------------------
	VOID	LoadScripts();
	VOID	LoadScriptsFromDir(LPCTSTR szDir, std::vector<tstring>& luaFiles, BOOL bFirst=FALSE);
	VOID	RegisterCoreFunctions();

	//-----------------------------------------------------------------------------------
	// 销毁
	//-----------------------------------------------------------------------------------
	VOID	DestroyScripts();
	VOID	DestroyAllStates();
	VOID	UnloadScripts();

	//-----------------------------------------------------------------------------------
	// 重新加载脚本
	//-----------------------------------------------------------------------------------
	VOID	Reload();
	
	//-----------------------------------------------------------------------------------
	// 得到一个线程状态
	//-----------------------------------------------------------------------------------
	lua_State* GetThreadState();
	lua_State* CreateThreadState(DWORD dwThreadID);

	//-----------------------------------------------------------------------------------
	// 错误显示
	//-----------------------------------------------------------------------------------
	VOID ErrMsg(lua_State* pState);
	
private:
	typedef TMap<UINT16, QuestScript*>			QuestScriptMap;
	typedef TMap<DWORD, CreatureScript*>		CreatureScriptMap;
	typedef TMap<DWORD, MapScript*>				MapScriptMap;
	typedef TMap<DWORD, ActScript*>				ActScriptMap;
	typedef TMap<DWORD, ItemScript*>			ItemScriptMap;
	typedef TMap<DWORD, SkillScript*>			SkillScriptMap;

private:
	TObjRef<Util>					m_pUtil;
	Mutex							m_Lock;					// 用于生成线程状态的锁

	lua_State*						m_pMasterState;			// 主状态
	TSafeMap<DWORD, lua_State*>		m_mapThreadState;		// 各个线程状态

	TMap<DWORD, Mutex*>				m_mapScriptMutex;		// 脚本锁，用于需要锁定运行的脚本函数
	DWORD							m_dwScriptMutexIDGen;	// 脚本锁ID生成器

	QuestScriptMap					m_mapQusetScript;		// 任务脚本表（任务ID---脚本）
	CreatureScriptMap				m_mapCreatureScript;	// 怪物脚本表（怪物ID——脚本）
	MapScriptMap					m_mapMapScript;			// 地图脚本表（地图属性ID——脚本）
	ActScriptMap					m_mapActScript;			// 活动脚本表（活动ID－－脚本）
	ItemScriptMap					m_mapItemScript;		// 物品脚本表（物品TypeID－－脚本)
	SkillScriptMap					m_mapSkillScript;		// 技能脚本

	WorldScript*					m_pWorldScript;			// 游戏世界脚本
	RoleScript*						m_pRoleScript;			// 玩家脚本
	DWORD							m_dwMaxPcallTime;		// 脚本调用的最长时间
};


//-----------------------------------------------------------------------------------
// 生成脚本锁
//-----------------------------------------------------------------------------------
inline DWORD ScriptMgr::CreateScriptMutex()
{
	Mutex* pMutex = new Mutex;

	++m_dwScriptMutexIDGen;
	m_mapScriptMutex.Add(m_dwScriptMutexIDGen, pMutex);

	return m_dwScriptMutexIDGen;
}

//------------------------------------------------------------------------------------
// 锁住某个脚本锁
//------------------------------------------------------------------------------------
inline VOID ScriptMgr::LockScriptMutex(DWORD dwMutexID)
{
	Mutex* pMutex = m_mapScriptMutex.Peek(dwMutexID);

	if( P_VALID(pMutex) )
	{
		pMutex->Acquire();
	}
}

//-------------------------------------------------------------------------------------
// 解除某个脚本锁
//-------------------------------------------------------------------------------------
inline VOID ScriptMgr::UnLockScriptMutex(DWORD dwMutexID)
{
	Mutex* pMutex = m_mapScriptMutex.Peek(dwMutexID);

	if( P_VALID(pMutex) )
	{
		pMutex->Release();
	}
}

//---------------------------------------------------------------------------------
// 获取全局变量
//---------------------------------------------------------------------------------
template<typename INT>
VOID ScriptMgr::GetGlobal(LPCSTR strVarName, OUT INT &res)
{
	lua_State* pThreadState = GetThreadState();
	if( !P_VALID(pThreadState) ) return;

	m_Lock.Acquire();

	lua_getglobal(pThreadState, strVarName);
	res = luaL_checkint(pThreadState, 1);

	lua_pop(pThreadState, 1);

	m_Lock.Release();
}

extern ScriptMgr g_ScriptMgr;

