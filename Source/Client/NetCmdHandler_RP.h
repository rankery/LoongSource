#pragma once
#include "..\WorldDefine\msg_combat.h"
#include "..\WorldDefine\action.h"
#include "..\WorldDefine\msg_loot.h"
#include "..\WorldDefine\role_att.h"
#include "CombatEvent.h"

class FSM_RP;
/** \class NetCmdHandler_RP
	\brief 远程玩家网络消息处理器
*/
class NetCmdHandler_RP
{
public:
	NetCmdHandler_RP(void);
	virtual ~NetCmdHandler_RP(void);
	void SetFSM(FSM_RP* pFSM){m_pFSM=pFSM;}

	virtual bool OnNetCmd(tagNetCmd* pNetCmd) =0;
	virtual void OnGameEvent(tagGameEvent* pEvent) =0;
	virtual void Update() =0;
protected:
	FSM_RP*					m_pFSM;
	TObjRef<Util>			m_pUtil;
	TObjRef<GameFrameMgr>	m_pGameFrameMgr;
};


/** \class NS_SyncStandHandler_RP
	\brief 站立状态同步命令处理器
*/
class NS_SyncStandHandler_RP : public NetCmdHandler_RP
{
public:
	NS_SyncStandHandler_RP();
	virtual ~NS_SyncStandHandler_RP();

	//--NetCmdHandler_RP
	virtual bool OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent);
	virtual void Update();
};


/** \class NS_StopActionHandler_RP
	\brief 停止所有行为命令处理器
*/
class NS_StopActionHandler_RP : public NetCmdHandler_RP
{
public:
	NS_StopActionHandler_RP();
	virtual ~NS_StopActionHandler_RP();

	//--NetCmdHandler_RP
	virtual bool OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent);
	virtual void Update();
};



/** \class NS_HPChangeHandler_RP
	\brief 角色HP变化命令处理器
*/
class NS_HPChangeHandler_RP : public NetCmdHandler_RP
{
	struct tagData
	{
		DWORD recvTime;
		tagNS_RoleHPChange cmd;
	};
public:
	NS_HPChangeHandler_RP();
	virtual ~NS_HPChangeHandler_RP();

	//--NetCmdHandler_RP
	virtual bool OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent);
	virtual void Update();

private:
	void SendShowHPChangeEvent(tagNS_RoleHPChange* pCmd);
	void SendBeAttackEvent(tagNS_RoleHPChange* pCmd);
	void PlayCritSound(tagNS_RoleHPChange* pCmd);
	void PlayQuake();
	bool IfNeedCacheCmd(tagNS_RoleHPChange* pCmd);
	bool IfNeedClearCmd(tagNS_RoleHPChange* pCacheCmd,tagHitTargetEvent* pEvent);

private:
	list<tagData> m_cache;
};

/** \class NS_RoleDeadHandler_RP
	\brief 角色死亡命令处理器
*/
class NS_RoleDeadHandler_RP : public NetCmdHandler_RP
{
	struct tagData
	{
		DWORD recvTime;
		tagNS_RoleDead cmd;
	};
public:
	NS_RoleDeadHandler_RP();
	virtual ~NS_RoleDeadHandler_RP();

	//--NetCmdHandler_RP
	virtual bool OnNetCmd(tagNetCmd* pNetCmd);
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


/** \class NS_HitFlyHandler_RP
	\brief 击飞命令处理器
*/
class NS_HitFlyHandler_RP : public NetCmdHandler_RP
{
public:
	NS_HitFlyHandler_RP();
	virtual ~NS_HitFlyHandler_RP();

	//--NetCmdHandler_RP
	virtual bool OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent);
	virtual void Update();

private:
	bool			m_bRecvedCmd;
	tagNS_HitFly	m_cmd;
	DWORD			m_recvTime;
};


/** \class NS_SpecialMove_RP
	\brief 特殊移动命令处理器
*/
class NS_SpecialMove_RP : public NetCmdHandler_RP
{
public:
	NS_SpecialMove_RP();
	virtual ~NS_SpecialMove_RP();

	//--NetCmdHandler_LP
	virtual bool OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent);
	virtual void Update();
};


/** \class NS_AddRoleBuffHandler_RP
	\brief 添加Buff命令处理器
*/
class NS_AddRoleBuffHandler_RP : public NetCmdHandler_RP
{
public:
	NS_AddRoleBuffHandler_RP();
	virtual ~NS_AddRoleBuffHandler_RP();

	//--NetCmdHandler_RP
	virtual bool OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent);
	virtual void Update();

private:
};





/** \class NS_SetStateHandler_RP
	\brief 设置状态命令处理器
*/
class NS_SetStateHandler_RP : public NetCmdHandler_RP
{
public:
	NS_SetStateHandler_RP();
	virtual ~NS_SetStateHandler_RP();

	//--NetCmdHandler_RP
	virtual bool OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent);
	virtual void Update();

private:
};





/** \class NS_HitTargetHandler_RP
	\brief 命中目标命令处理器
*/
class NS_HitTargetHandler_RP : public NetCmdHandler_RP
{
	struct tagData
	{
		DWORD recvTime;
		DWORD maxChannelIndex;
		tagNS_HitTarget cmd;
	};
public:
	NS_HitTargetHandler_RP();
	virtual ~NS_HitTargetHandler_RP();

	//--NetCmdHandler_RP
	virtual bool OnNetCmd(tagNetCmd* pNetCmd);
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





/** \class NS_SyncGroundItemHandler_RP
	\brief 角色掉落命令处理器
*/
class NS_SyncGroundItemHandler_RP : public NetCmdHandler_RP
{
	struct tagData
	{
		DWORD recvTime;
		tagNS_SyncGroundItem cmd;
	};
public:
	NS_SyncGroundItemHandler_RP();
	virtual ~NS_SyncGroundItemHandler_RP();

	//--NetCmdHandler_RP
	virtual bool OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent);
	virtual void Update();

private:
	list<tagData> m_cache;
};






/** \class NS_RoleLevelChangeHandler_RP
	\brief 角色等级变化消息处理器
*/
class NS_RoleLevelChangeHandler_RP : public NetCmdHandler_RP
{
	struct tagData
	{
		DWORD recvTime;
		tagNS_RoleLevelChange cmd;
	};
public:
	NS_RoleLevelChangeHandler_RP();
	virtual ~NS_RoleLevelChangeHandler_RP();

	//--NetCmdHandler_RP
	virtual bool OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent);
	virtual void Update();

private:
	void SendShowLevelChangeEvent(tagNS_RoleLevelChange* pCmd);

private:
	list<tagData> m_cache;
};








/** \class NS_UseItemHandler_RP
	\brief 使用物品消息处理器
*/
class NS_UseItemHandler_RP : public NetCmdHandler_RP
{
public:
	NS_UseItemHandler_RP();
	virtual ~NS_UseItemHandler_RP();

	//--NetCmdHandler_RP
	virtual bool OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent);
	virtual void Update();

};








/** \class NS_SetRoleStateHandler_RP
	\brief 设置角色状态命令处理器
*/
class NS_SetRoleStateHandler_RP : public NetCmdHandler_RP
{
public:
	NS_SetRoleStateHandler_RP();
	virtual ~NS_SetRoleStateHandler_RP();

	//--NetCmdHandler_RP
	virtual bool OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pEvent);
	virtual void Update();

private:
};



