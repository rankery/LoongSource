#include "StdAfx.h"
#include "Pet.h"
#include "FSM_PET.h"
#include "NetCmdHandler_PET.h"
#include "..\WorldDefine\msg_combat.h"
#include "..\WorldDefine\action.h"

NetCmdHandler_PET::NetCmdHandler_PET(void) : m_pFSM(NULL)
{
}

NetCmdHandler_PET::~NetCmdHandler_PET(void)
{
}

void NS_SyncStandHandler_PET::OnNetCmd( tagNetCmd* pNetCmd )
{
    if(pNetCmd->dwID==m_pUtil->Crc32("NS_SyncStand"))
    {
        tagNS_SyncStand* pCmd=(tagNS_SyncStand*)pNetCmd;

        m_pFSM->GetPet()->SetPos(pCmd->curPos);
        m_pFSM->GetPet()->SetYaw(CalcYaw(pCmd->faceTo));
        m_pFSM->Change2IdleState();
    }
}

void NS_StopActionHandler_PET::OnNetCmd( tagNetCmd* pNetCmd )
{
    if(pNetCmd->dwID==m_pUtil->Crc32("NS_StopAction"))
    {
        tagNS_StopAction* pCmd=(tagNS_StopAction*)pNetCmd;

        m_pFSM->GetPet()->SetPos(pCmd->curPos);
        m_pFSM->GetPet()->SetYaw(CalcYaw(pCmd->faceTo));
        m_pFSM->Change2IdleState();
    }
}