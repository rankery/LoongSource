#pragma once
#include "ActionState.h"

class LowerState_RP;
class UpperState_RP;
class ActionState_RP;
class NetCmdHandler_RP;
class GameEventHandler_RP;
class GlobalState_RP;
class Player;
struct tagHitTargetEvent;

/** \class FSM_RP
	\brief 远程玩家状态机
*/
class FSM_RP
{
public:
	enum ELowerState					//下身状态
	{
		ELowerState_None=-1,
		ELowerState_Move=0,				//移动
		ELowerState_Swim,				//游泳
		ELowerState_WaterWalk,			//水上行走
		ELowerState_Stand,				//陆上站立
		ELowerState_Float,				//水中漂浮
		ELowerState_WaterStand,			//水上站立
		ELowerState_Jump,				//跳跃 
		ELowerState_Drop,				//掉落
		ELowerState_VDrop,				//垂直掉落	
		ELowerState_Slide,				//滑落
		ELowerState_HitFly,				//击飞
		ELowerState_Repel,				//击退
		ELowerState_Assault,			//冲锋
		ELowerState_Pull,				//拖拽
		ELowerState_Num
	};

	enum EUpperState					//上身状态
	{
		EUpperState_None=-1,
		EUpperState_Idle=0,				//空闲
		EUpperState_Skill,				//使用技能
		EUpperState_UseItem,			//使用物品
		EUpperState_Dead,				//死亡
		EUpperState_Dazzy,				//击晕
		EUpperState_Tie,				//定身
		EUpperState_Sopor,				//昏睡
		EUpperState_StyleAction,		//个性动作
		EUpperState_BeAttack,			//被攻击
		EUpperState_Num
	};

	enum EGlobalState					//全局状态
	{
		EGlobalState_Combat,			//战斗状态
		EGlobalState_Num
	};

private:
	enum EGameEventHandler				//游戏事件处理器
	{
		EGameEventHandler_HitTarget,	//命中目标
		EGameEventHandler_Num,
	};

public:
	FSM_RP(Player* pPlayer);
	virtual ~FSM_RP(void);

	//--基础功能------------------------------------------------------------------------------
	void Init();
	void Destroy();
	void Update();
	void OnNetCmd(tagNetCmd* pNetCmd);
	void OnGameEvent(tagGameEvent* pEvent);

	//--下身状态切换---------------------------------------------------------------------------
	void Change2LowerState(ELowerState state,tagNetCmd* pNetCmd);
	void Change2LowerState(tagNetCmd* pNetCmd);
	void Change2LowerState_Stand();
	void Change2LowerState_Float();
	void Change2LowerState_WaterStand();
	void Change2LowerState_AutoStand();

	//--上身状态切换--------------------------------------------------------------------------
	void Change2UpperState(EUpperState state,tagNetCmd* pNetCmd);
	void Change2UpperState(tagNetCmd* pNetCmd);
	void Change2UpperState_Idle();
	void Change2UpperState_Dead(bool bPlayAni);
	bool TryChange2UpperState(EUpperState state,tagGameEvent* pEvent);

	//--动作状态切换---------------------------------------------------------------------------
	void Change2ActionState(EActionState state);
	EActionState GetActionState(){ return m_activeActionState;	}

	//--下身动作相关---------------------------------------------------------------------------
	bool IfCanPlayLowerAni();
	void ReplayLowerAni();
	bool IfLowerCanSetYaw();

	//--全局状态切换---------------------------------------------------------------------------
	void ActiveGlobalState(EGlobalState state,tagNetCmd* pNetCmd,DWORD dwParam);

	//--获得状态-------------------------------------------------------------------------------
	bool IsReadyState();
	bool IsStandState()		{ return m_activeLowerState==ELowerState_Stand;	}
	bool IsFloatState()		{ return m_activeLowerState==ELowerState_Float;	}
	bool IsWaterStandState(){ return m_activeLowerState==ELowerState_WaterStand;}
	bool IsDeadState()		{ return m_activeUpperState==EUpperState_Dead;	}
	bool IsUpperIdleState()	{ return m_activeUpperState==EUpperState_Idle;	}
	bool IsSpecailMoveState() { return  m_activeLowerState==ELowerState_Assault ||
										m_activeLowerState==ELowerState_Repel ||
										m_activeLowerState==ELowerState_Pull; }
	bool IsStyleActionState() { return m_activeUpperState==EUpperState_StyleAction; }
	//--
	tagHitTargetEvent* GetLastHitTargetEvent(DWORD roleID);
	Player* GetPlayer(){ return m_pPlayer;}

private:
	ELowerState					m_activeLowerState;							//当前下身状态
	EUpperState					m_activeUpperState;							//当前上身状态
	EActionState				m_activeActionState;						//当前动作状态

	LowerState_RP*				m_lowerStates[ELowerState_Num];				//所有下身状态
	UpperState_RP*				m_upperStates[EUpperState_Num];				//所有上身状态
	ActionState_RP*				m_actionStates[EActionState_Num];			//所有动作状态
	GlobalState_RP*				m_globalStates[EGlobalState_Num];			//所有全局状态

	vector<NetCmdHandler_RP*>	m_netCmdHandlers;							//网络消息处理器
	GameEventHandler_RP*		m_gameEventHandlers[EGameEventHandler_Num];	//游戏事件处理器

	Player*						m_pPlayer;									//状态机所属角色
	TObjRef<Util>				m_pUtil;
};
