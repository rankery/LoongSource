#pragma once

class FSM_PET;

class GameEventHandler_PET
{
public:
    GameEventHandler_PET(void);
    virtual ~GameEventHandler_PET(void);
    void SetFSM(FSM_PET* pFSM){ m_pFSM = pFSM; }

    virtual void OnGameEvent(tagGameEvent* pEvent) = 0;
    virtual void OnNetCmd(tagNetCmd* pNetCmd) = 0;

protected:
    FSM_PET*            m_pFSM;
};
