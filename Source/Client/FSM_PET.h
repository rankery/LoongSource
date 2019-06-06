#pragma once

class Pet;
class State_PET;
class NetCmdHandler_PET;
class GameEventHandler_PET;

//! \class  FSM_PET
//! \brief  宠物状态机
//! \author hyu
//! \date   2990-4-7
class FSM_PET
{
public:
    enum EState
    {
        EState_Idle,            //!< 空闲
        EState_MoveByCollide,	//!< 基于碰撞的移动
        EState_MoveByTile,		//!< 基于格子的移动
        EState_Shutdown,        //!< 状态机停止状态,控制权交给角色,如骑乘,飞行
        EState_Num
    };
public:
    FSM_PET(Pet* pPet);
    virtual ~FSM_PET(void);

    void Init();
    void Destroy();
    void Update();
    void OnNetCmd(tagNetCmd* pNetCmd);
    void OnGameEvent(tagGameEvent* pEvent);

    void Change2State(tagNetCmd* pNetCmd);
    void Change2State(tagGameEvent* pEvent);
    void Change2IdleState();
    void Change2State(EState state,tagNetCmd* pNetCmd);

    Pet* GetPet() { return m_pPet; }

private:
    EState							m_activeState;						        //!< 当前状态
    State_PET*						m_states[EState_Num];				        //!< 所有状态
    vector<NetCmdHandler_PET*>		m_netCmdHandlers;					        //!< 网络消息处理器
    vector<GameEventHandler_PET*>	m_gameEventHandlers;                        //!< 游戏事件处理器
    Pet*                            m_pPet;                                     //!< 状态机所属宠物
    TObjRef<Util>					m_pUtil;
};
