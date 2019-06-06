#pragma once

class Pet;
class FSM_PET;

class State_PET
{
public:
    State_PET(void);
    virtual ~State_PET(void);
    void SetFSM(FSM_PET* pFSM){ m_pFSM = pFSM;}

    //! 激活一个状态
    virtual void Active(tagNetCmd* pNetCmd, NavMap* pNav, Pet* pRole) = 0;

    //! 退出一上状态
    virtual void OnExit(){}
    
    //! 更新状态逻辑
    virtual void Update(NavMap* pNav, Pet* pRole) = 0;

    //! 网络消息处理
    virtual void OnNetCmd(tagNetCmd* pNetCmd) = 0;

    //! 游戏事件处理
    virtual void OnGameEvent(tagGameEvent* pEvent) = 0;

protected:
    FSM_PET*            m_pFSM;     //!< 所属状态机
    TObjRef<Util>       m_pUtil;
};
