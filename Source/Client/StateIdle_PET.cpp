#include "StdAfx.h"
#include "StateIdle_PET.h"
#include "FSM_PET.h"
#include "Pet.h"
#include "CombatEvent.h"

StateIdle_PET::StateIdle_PET(void)
{
}

StateIdle_PET::~StateIdle_PET(void)
{
}

void StateIdle_PET::Active( tagNetCmd* pNetCmd, NavMap* pNav, Pet* pRole )
{
    pRole->PlayTrack(_T("std"),true);

    // 初始化悠闲动作播放时间
    m_lastPlayEaseTime=Kernel::Inst()->GetAccumTimeDW();
    m_step=EStep_Stand;
}

void StateIdle_PET::Update( NavMap* pNav, Pet* pRole )
{
    // 临时，有些模型还没有悠闲动作
    if( !pRole->IsAttributeInited() || !pRole->IsPet() )
        return;

    if(m_step==EStep_Stand)
    {
        DWORD intervalTime = 30000 + (rand() % 20000);
        if(Kernel::Inst()->GetAccumTimeDW()-m_lastPlayEaseTime>=intervalTime)
        {
            pRole->PlayTrack(_T("lei"),false);
            m_lastPlayEaseTime=Kernel::Inst()->GetAccumTimeDW();
            m_step=EStep_Ease;
            tagNPCEaseSpeechEvent evt(_T("tagNPCEaseSpeechEvent"), NULL);
            evt.dwNPCID = pRole->GetID();
            evt.dwTypeID = pRole->GetTypeID();
            TObjRef<GameFrameMgr>()->SendEvent(&evt);
        }
    }
    else if(m_step==EStep_Ease)
    {
        if(pRole->IsTrackEnd(_T("lei")))
        {
            pRole->PlayTrack(_T("std"),true);
            m_step=EStep_Stand;
        }
    }
}

void StateIdle_PET::OnNetCmd( tagNetCmd* pNetCmd )
{
	m_pFSM->Change2State(pNetCmd);
}

void StateIdle_PET::OnGameEvent( tagGameEvent* pEvent )
{
	m_pFSM->Change2State(pEvent);
}