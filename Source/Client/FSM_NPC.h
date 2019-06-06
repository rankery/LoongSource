#pragma once

class State_NPC;
class NetCmdHandler_NPC;
class GameEventHandler_NPC;
class NPC;
struct tagHitTargetEvent;

/** \class FSM_NPC
	\brief NPC状态机
*/
class FSM_NPC
{
public:
	enum EState					//状态定义
	{
		EState_Idle,			//空闲
		EState_MoveByCollide,	//基于碰撞的移动
		EState_MoveByTile,		//基于格子的移动
		EState_Skill,			//使用技能
		EState_Dead,			//死亡
		EState_BeAttack,		//被攻击
		EState_HitFly,			//击飞
		EState_Repel,			//击退
		EState_Assault,			//冲锋
		EState_Pull,			//拖拽
		EState_Dazzy,			//击晕
		EState_Tie,				//定身
		EState_Sopor,			//昏睡
		EState_Num		
	};

private:
	enum EGameEventHandler		//游戏事件处理器
	{
		EGameEventHandler_HitTarget,//命中目标
		EGameEventHandler_Num,
	};

public:
	FSM_NPC(NPC* pNPC);
	virtual ~FSM_NPC(void);

	void Init();
	void Destroy();
	void Update();
	void OnNetCmd(tagNetCmd* pNetCmd);
	void OnGameEvent(tagGameEvent* pEvent);

	void Change2State(tagNetCmd* pNetCmd);
	void Change2State(tagGameEvent* pEvent);
	void Change2IdleState();
	void Change2DeadState();
	void Change2State(EState state,tagNetCmd* pNetCmd);

	bool IsDeadState(){ return m_activeState==EState_Dead;}
    bool IsHitFlyState(){ return m_activeState==EState_HitFly;}
	bool IsSpecailMoveState() { return  m_activeState==EState_Assault ||
										m_activeState==EState_Repel ||
										m_activeState==EState_Pull; }

	tagHitTargetEvent* GetLastHitTargetEvent(DWORD roleID);

	NPC* GetNPC(){ return m_pNPC;}

    void SetNSDead(bool bDead) { m_bNSDead = bDead; }
    bool bNSDead() { return m_bNSDead; }

private:
	EState							m_activeState;			//当前状态
	State_NPC*						m_states[EState_Num];	//所有状态
	vector<NetCmdHandler_NPC*>		m_netCmdHandlers;		//网络消息处理器
	GameEventHandler_NPC*			m_gameEventHandlers[EGameEventHandler_Num];	//游戏事件处理器
	NPC*							m_pNPC;					//状态机所属角色
	TObjRef<Util>					m_pUtil;

    bool                            m_bNSDead;              //收到服务器死亡消息
};
