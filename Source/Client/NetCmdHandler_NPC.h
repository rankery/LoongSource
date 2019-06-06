#pragma once
#include "..\WorldDefine\msg_combat.h"
#include "..\WorldDefine\action.h"
#include "..\WorldDefine\msg_loot.h"
#include "CombatEvent.h"



class FSM_NPC;
/** \class NetCmdHandler_NPC
	\brief NPC网络消息处理器
*/
class NetCmdHandler_NPC
{
public:
	NetCmdHandler_NPC(void);
	virtual ~NetCmdHandler_NPC(void);
	void SetFSM(FSM_NPC* pFSM){m_pFSM=pFSM;}

	virtual void OnNetCmd(tagNetCmd* pNetCmd) =0;
	virtual void OnGameEvent(tagGameEvent* pEvent) =0;
	virtual void Update() =0;
protected:
	FSM_NPC*				m_pFSM;
	TObjRef<Util>			m_pUtil;
	TObjRef<GameFrameMgr>	m_pGameFrameMgr;
};




/** \class NS_SyncStandHandler_NPC
	\brief 站立状态同步命令处理器
*/
class NS_SyncStandHandler_NPC : public NetCmdHandler_NPC
{
public:
	NS_SyncStandHandler_NPC();
	virtual ~NS_SyncStandHandler_NPC();

	//--NetCmdHandler_NPC
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent);
	virtual void Update();
};


/** \class NS_StopActionHandler_NPC
	\brief 停止所有行为命令处理器
*/
class NS_StopActionHandler_NPC : public NetCmdHandler_NPC
{
public:
	NS_StopActionHandler_NPC();
	virtual ~NS_StopActionHandler_NPC();

	//--NetCmdHandler_NPC
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent);
	virtual void Update();
};



/** \class NS_HPChangeHandler_NPC
	\brief 角色HP变化命令处理器
*/
class NS_HPChangeHandler_NPC : public NetCmdHandler_NPC
{
	struct tagData
	{
		DWORD recvTime;
		tagNS_RoleHPChange cmd;
	};
public:
	NS_HPChangeHandler_NPC();
	virtual ~NS_HPChangeHandler_NPC();

	//--NetCmdHandler_NPC
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent);
	virtual void Update();

private:
	void SendShowHPChangeEvent(tagNS_RoleHPChange* pCmd);
	void SendBeAttackEvent();
	void PlayCritSound(tagNS_RoleHPChange* pCmd);
	void PlayQuake();
	bool IfNeedCacheCmd(tagNS_RoleHPChange* pCmd);
	bool IfNeedClearCmd(tagNS_RoleHPChange* pCacheCmd,tagHitTargetEvent* pEvent);

private:
	list<tagData> m_cache;
};



/** \class NS_RoleDeadHandler_NPC
	\brief 角色死亡命令处理器
*/
class NS_RoleDeadHandler_NPC : public NetCmdHandler_NPC
{
	struct tagData
	{
		DWORD recvTime;
		tagNS_RoleDead cmd;
	};
public:
	NS_RoleDeadHandler_NPC();
	virtual ~NS_RoleDeadHandler_NPC();

	//--NetCmdHandler_NPC
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent);
	virtual void Update();

private:
	void SendRewardEvent(tagNS_RoleDead* pCmd);
	void SendCostEvent(tagNS_RoleDead* pCmd);
	bool IfNeedCacheCmd(tagNS_RoleDead* pCmd);
	bool IfNeedClearCmd(tagNS_RoleDead* pCacheCmd,tagHitTargetEvent* pEvent);

private:
	list<tagData> m_cache;
};



/** \class NS_HitFlyHandler_NPC
	\brief 击飞命令处理器
*/
class NS_HitFlyHandler_NPC : public NetCmdHandler_NPC
{
public:
	NS_HitFlyHandler_NPC();
	virtual ~NS_HitFlyHandler_NPC();

	//--NetCmdHandler_NPC
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent);
	virtual void Update();

private:
	bool			m_bRecvedCmd;
	tagNS_HitFly	m_cmd;
	DWORD			m_recvTime;
};



/** \class NS_SpecialMove_NPC
	\brief 特殊移动命令处理器
*/
class NS_SpecialMove_NPC : public NetCmdHandler_NPC
{
public:
	NS_SpecialMove_NPC();
	virtual ~NS_SpecialMove_NPC();

	//--NetCmdHandler_LP
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent);
	virtual void Update();
};



/** \class NS_AddRoleBuffHandler_NPC
	\brief 添加Buff命令处理器
*/
class NS_AddRoleBuffHandler_NPC : public NetCmdHandler_NPC
{
public:
	NS_AddRoleBuffHandler_NPC();
	virtual ~NS_AddRoleBuffHandler_NPC();

	//--NetCmdHandler_NPC
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent);
	virtual void Update();

private:
};







/** \class NS_SetStateHandler_NPC
	\brief 设置状态命令处理器
*/
class NS_SetStateHandler_NPC : public NetCmdHandler_NPC
{
public:
	NS_SetStateHandler_NPC();
	virtual ~NS_SetStateHandler_NPC();

	//--NetCmdHandler_NPC
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent);
	virtual void Update();

private:
};




/** \class NS_HitTargetHandler_NPC
	\brief 命中目标命令处理器
*/
class NS_HitTargetHandler_NPC : public NetCmdHandler_NPC
{
	struct tagData
	{
		DWORD recvTime;
		DWORD maxChannelIndex;
		tagNS_HitTarget cmd;
	};
public:
	NS_HitTargetHandler_NPC();
	virtual ~NS_HitTargetHandler_NPC();

	//--NetCmdHandler_NPC
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent);
	virtual void Update();

private:
	void PlayBeAttackEffect(tagNS_HitTarget* pCmd);
	void CacheCmd(tagNS_HitTarget* pCmd);
	bool IfNeedClearCmd(tagNS_HitTarget* pCacheCmd,tagHitTargetEvent* pEvent);
	bool IfNeedHandleCmd(tagNS_HitTarget* pCacheCmd,tagHitTargetEvent* pEvent);

private:
	list<tagData> m_cache;
};











/** \class NS_SyncGroundItemHandler_NPC
	\brief 角色掉落命令处理器
*/
class NS_SyncGroundItemHandler_NPC : public NetCmdHandler_NPC
{
	struct tagData
	{
		DWORD recvTime;
		tagNS_SyncGroundItem cmd;
	};
public:
	NS_SyncGroundItemHandler_NPC();
	virtual ~NS_SyncGroundItemHandler_NPC();

	//--NetCmdHandler_NPC
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent);
	virtual void Update();

private:
	list<tagData> m_cache;
};









/** \class NS_MonsterEnterCombatHandler_NPC
	\brief 怪物进入战斗命令处理器
*/
class NS_MonsterEnterCombatHandler_NPC : public NetCmdHandler_NPC
{
public:
	NS_MonsterEnterCombatHandler_NPC();
	virtual ~NS_MonsterEnterCombatHandler_NPC();

	//--NetCmdHandler_NPC
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent);
	virtual void Update();
};

/** \class NS_MonsterPlayActionHandler_NPC
\brief 怪物播放一个动作处理器
*/
class NS_MonsterPlayActionHandler_NPC : public NetCmdHandler_NPC
{
public:
	NS_MonsterPlayActionHandler_NPC(){};
	virtual ~NS_MonsterPlayActionHandler_NPC(){};

	virtual void OnNetCmd(tagNetCmd *pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent);
	virtual void Update();
};

/** \class NS_MonsterSaHandler_NPC
\brief 怪物说话
*/

class MonsterSayTable;

class NS_MonsterSaHandler_NPC : public NetCmdHandler_NPC
{
public:
	NS_MonsterSaHandler_NPC();
	virtual ~NS_MonsterSaHandler_NPC();
	
	virtual void OnNetCmd(tagNetCmd *pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent);
	virtual void Update();
};

