#pragma once
#include "..\WorldDefine\msg_combat.h"
#include "ActionState.h"

class LocalPlayer;
class LowerState_LP;
class UpperState_LP;
class GlobalState_LP;
class ActionState_LP;
class NetCmdHandler_LP;
class GameEventHandler_LP;
class UpperStateUseItem_LP;

struct tagPreState;
struct tagHitTargetEvent;

/** \class FSM_LP
	\brief 本地玩家状态机
*/
class FSM_LP
{
public:
	enum ELowerState					//下身状态
	{
		ELowerState_None=-1,
		ELowerState_MouseMove=0,		//鼠标移动
		ELowerState_MouseSwim,			//鼠标游泳
		ELowerState_MouseWaterWalk,		//鼠标水上行走
		ELowerState_KeyboardMove,		//键盘移动
		ELowerState_KeyboardSwim,		//键盘游泳
		ELowerState_KeyboardWaterWalk,	//键盘水上行走
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
		ELowerState_Pursue,				//追踪
		ELowerState_Num
	};

	enum EUpperState					//上身状态
	{
		EUpperState_None=-1,
		EUpperState_Idle=0,				//空闲
		EUpperState_Skill,				//使用技能
		EUpperState_UseItem,			//使用物品
		EUpperState_Dead,				//死亡
		EUpperState_TalkNPC,			//与NPC对话
		EUpperState_Follow,				//跟随
		EUpperState_Dazzy,				//击晕
		EUpperState_Tie,				//定身
		EUpperState_Sopor,				//昏睡
		EUpperState_StyleAction,		//个性动作
		EUpperState_PickupItem,			//拾取
		EUpperState_BeAttack,			//被攻击
		EUpperState_Num
	};

private:
	enum EGlobalState					//全局状态
	{
		EGlobalState_Combat,			//战斗状态
		EGlobalState_Num
	};

	enum EGameEventHandler				//游戏事件处理器
	{
		EGameEventHandler_HitTarget,	//命中目标
		EGameEventHandler_UseItem,		//使用物品
		EGameEventHandler_Num,
	};

public:
	FSM_LP(LocalPlayer* pLocalPlayer);
	virtual ~FSM_LP(void);

	//--基础功能------------------------------------------------------------------------------
	void Init();
	void Destroy();
	void Update();
	void OnGameEvent(tagGameEvent* pEvent);
	void OnNetCmd(tagNetCmd* pNetCmd);

	//--下身状态切换---------------------------------------------------------------------------
	void Change2LowerState(ELowerState state,tagGameEvent* pEvent,tagPreState* pPreState=NULL);
	void Change2LowerState(ELowerState state,tagNetCmd* pNetCmd);
	void Change2LowerState_Stand();
	void Change2LowerState_Float();
	void Change2LowerState_WaterStand();
	void Change2LowerState_AutoStand();
	bool TryChange2LowerState(ELowerState state,tagGameEvent* pEvent,tagPreState* pPreState=NULL);

	//--上身状态切换--------------------------------------------------------------------------
	void Change2UpperState(EUpperState state,tagGameEvent* pEvent);
	void Change2UpperState(EUpperState state,tagNetCmd* pNetCmd);
	void Change2UpperState_Idle();
	void Change2UpperState_Dead(bool bPlayAni,ERoleReviveType reviveType=ERRT_None);
	bool TryChange2UpperState(EUpperState state,tagGameEvent* pEvent);

	//--动作状态切换---------------------------------------------------------------------------
	void Change2ActionState(EActionState state);
	EActionState GetActionState(){ return m_activeActionState;	}

	//--下身动作相关---------------------------------------------------------------------------
	bool IfCanPlayLowerAni();
	void ReplayLowerAni();
	bool IfLowerCanSetYaw();

	//--获得当前状态---------------------------------------------------------------------------
	bool IsReadyState();
	bool IsCombatState();
	bool IsSkillState()		{ return m_activeUpperState==EUpperState_Skill;	}
	bool IsUpperState_UseItem(){ return m_activeUpperState == EUpperState_UseItem; }
	bool IsDeadState()		{ return m_activeUpperState==EUpperState_Dead;	}
	bool IsSwimState();
	bool IsOnWaterState();	
	bool IsStandState()		{ return m_activeLowerState==ELowerState_Stand;	}
	bool IsFloatState()		{ return m_activeLowerState==ELowerState_Float;	}
	bool IsWaterStandState(){ return m_activeLowerState==ELowerState_WaterStand;}
	bool IsUpperIdleState()	{ return m_activeUpperState==EUpperState_Idle;	}
	bool IsMoveState()      { return m_activeLowerState==ELowerState_MouseMove || 
		                             m_activeLowerState==ELowerState_KeyboardMove || 
									 m_activeLowerState==ELowerState_KeyboardWaterWalk || 
									 m_activeLowerState==ELowerState_MouseWaterWalk || 
									 m_activeLowerState==ELowerState_KeyboardSwim || 
									 m_activeLowerState==ELowerState_MouseSwim; }
	bool IsSpecailMoveState() { return   m_activeLowerState==ELowerState_Assault ||
										 m_activeLowerState==ELowerState_Repel ||
									     m_activeLowerState==ELowerState_Pull; }
	bool IsStyleActionState() { return m_activeUpperState==EUpperState_StyleAction; }
	//--
	tagHitTargetEvent* GetLastHitTargetEvent(DWORD roleID);
	LocalPlayer* GetLocalPlayer(){ return m_pLocalPlayer;}
	
	/**不播放悠闲动作
	*/
	void DontPlayEaseSkeleton();

	/** 如果当前处于使用物品状态则返回使用物品的type id
		否则返回0，注)钓鱼专用，其他人用要谨慎
	*/
	DWORD GetUsingItemTypeID();

private:
	ELowerState					m_activeLowerState;							//当前下身状态
	EUpperState					m_activeUpperState;							//当前上身状态
	EActionState				m_activeActionState;						//当前动作状态

	LowerState_LP*				m_lowerStates[ELowerState_Num];				//所有下身状态
	UpperState_LP*				m_upperStates[EUpperState_Num];				//所有上身状态
	ActionState_LP*				m_actionStates[EActionState_Num];			//所有动作状态
	GlobalState_LP*				m_globalStates[EGlobalState_Num];			//所有全局状态

	vector<NetCmdHandler_LP*>	m_netCmdHandlers;							//网络消息处理器
	GameEventHandler_LP*		m_gameEventHandlers[EGameEventHandler_Num];	//游戏事件处理器

	LocalPlayer*				m_pLocalPlayer;								//状态机所属角色
	TObjRef<Util>				m_pUtil;
};
