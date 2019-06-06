#pragma once
#include "State_PET.h"

class StateIdle_PET :
    public State_PET
{
private:
    //! 空闲状态有两种资势
    enum EStep
    {
        EStep_Stand,    //!< 普通站立
        EStep_Ease      //!< 抓耳挠腮
    };
public:
    StateIdle_PET(void);
    virtual ~StateIdle_PET(void);

    virtual void Active(tagNetCmd* pNetCmd, NavMap* pNav, Pet* pRole);		
    virtual void Update(NavMap* pNav, Pet* pRole);							
    virtual void OnNetCmd(tagNetCmd* pNetCmd);
    virtual void OnGameEvent(tagGameEvent* pEvent);

private:
    DWORD m_lastPlayEaseTime;       //!< 上次播放悠闲动作时间
    EStep m_step;
};
