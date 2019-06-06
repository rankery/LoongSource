#include "StdAfx.h"
#include "ProfessionMgr.h"
#include "..\WorldDefine\RoleDefine.h"
#include "..\WorldDefine\role_att.h"
#include "ProfessionFrame.h"
#include "CombatSysUtil.h"
#include "QuestMgr.h"
#include "Player.h"
#include "LocalPlayer.h"
#include "RoleMgr.h"

ProfessionMgr::ProfessionMgr(void) :
    m_Trunk(this)
{
}

ProfessionMgr::~ProfessionMgr(void)
{
}

void ProfessionMgr::Init()
{
    // 获取工具
    m_pCmdMgr       =   TObjRef<NetCmdMgr>();
    m_pUtil         =   TObjRef<Util>();
    m_pFrameMgr     =   TObjRef<GameFrameMgr>();
    m_pSession      =   TObjRef<NetSession>();

    // 注册网络事件
    m_pCmdMgr->Register("NS_SetClass",                  (NETMSGPROC)m_Trunk.sfp2(&ProfessionMgr::OnNS_SetClass),                   _T("NS_SetClass"));
    m_pCmdMgr->Register("NS_ChangeClass",               (NETMSGPROC)m_Trunk.sfp2(&ProfessionMgr::OnNS_ChangeClass),                _T("NS_ChangeClass"));
    m_pCmdMgr->Register("NS_RoleClassChangeBroadcast",  (NETMSGPROC)m_Trunk.sfp2(&ProfessionMgr::OnNS_RoleClassChangeBroadcast),   _T("NS_RoleClassChangeBroadcast"));

    // 注册游戏事件
    m_pFrameMgr->RegisterEventHandle(_T("OpenSetClass"),        (FRAMEEVENTPROC)m_Trunk.sfp1(&ProfessionMgr::OnSetClass));
    m_pFrameMgr->RegisterEventHandle(_T("OpenChangeClass"),     (FRAMEEVENTPROC)m_Trunk.sfp1(&ProfessionMgr::OnChangeClass));
    m_pFrameMgr->RegisterEventHandle(_T("OpenChangeClassEx"),   (FRAMEEVENTPROC)m_Trunk.sfp1(&ProfessionMgr::OnChangeClassEx));

}

void ProfessionMgr::Destroy()
{
    // 注销网络事件
    m_pCmdMgr->UnRegister("NS_SetClass",                  (NETMSGPROC)m_Trunk.sfp2(&ProfessionMgr::OnNS_SetClass));
    m_pCmdMgr->UnRegister("NS_ChangeClass",               (NETMSGPROC)m_Trunk.sfp2(&ProfessionMgr::OnNS_ChangeClass));
    m_pCmdMgr->UnRegister("NS_RoleClassChangeBroadcast",  (NETMSGPROC)m_Trunk.sfp2(&ProfessionMgr::OnNS_RoleClassChangeBroadcast));

    // 注销游戏事件
    m_pFrameMgr->UnRegisterEventHandler(_T("OpenSetClass"),     (FRAMEEVENTPROC)m_Trunk.sfp1(&ProfessionMgr::OnSetClass));
    m_pFrameMgr->UnRegisterEventHandler(_T("OpenChangeClass"),  (FRAMEEVENTPROC)m_Trunk.sfp1(&ProfessionMgr::OnChangeClass));
    m_pFrameMgr->UnRegisterEventHandler(_T("OpenChangeClassEx"),(FRAMEEVENTPROC)m_Trunk.sfp1(&ProfessionMgr::OnChangeClassEx));

}

void ProfessionMgr::Update()
{

}

ProfessionMgr g_ProfessionMgr;
ProfessionMgr& ProfessionMgr::Inst()
{
    return g_ProfessionMgr;
}

void ProfessionMgr::ShowErrorMsg( DWORD dwErrCode )
{
    if (dwErrCode > E_Success && dwErrCode < ECEC_End)
    {
        tstringstream tss;
        tss << _T("ClassErrorMsg_") << dwErrCode;
        CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[tss.str().c_str()]);
    }
}

LPCTSTR ProfessionMgr::GetClassName( EClassType eClass )
{
    if (eClass > EV_Begin && eClass < EV_End)
    {
        tstringstream tss;
        tss << _T("ClassName_") << eClass;
        return g_StrTable[tss.str().c_str()];
    }

    return _T("");
}

LPCTSTR ProfessionMgr::GetClassExName( EClassTypeEx eClassEx )
{
    if (eClassEx > EHV_Base && eClassEx < EHV_End)
    {
        tstringstream tss;
        tss << _T("ClassExName_") << eClassEx;
        return g_StrTable[tss.str().c_str()];
    }

    return _T("");
}

DWORD ProfessionMgr::OnSetClass( tagGameEvent* pEvent )
{
    ProfessionFrame *pFrame = (ProfessionFrame*)m_pFrameMgr->GetFrame(_T("ProfessionFrame"));
    if (!P_VALID(pFrame))
    {
        pFrame = (ProfessionFrame*)m_pFrameMgr->CreateFrame(_T("World"), _T("ProfessionFrame"), _T("ProfessionFrame"), 0);
    }

    DWORD npcId = QuestMgr::Inst()->GetTalkNPCGlobalID();
    pFrame->SetOperaType(ESClassType_NEW);
    pFrame->SetNPCID(npcId);
    pFrame->ShowWnd(TRUE);

    return 0;
}

DWORD ProfessionMgr::OnChangeClass( tagGameEvent* pEvent )
{
    ProfessionFrame *pFrame = (ProfessionFrame*)m_pFrameMgr->GetFrame(_T("ProfessionFrame"));
    if (!P_VALID(pFrame))
    {
        pFrame = (ProfessionFrame*)m_pFrameMgr->CreateFrame(_T("World"), _T("ProfessionFrame"), _T("ProfessionFrame"), 0);
    }

    DWORD npcId = QuestMgr::Inst()->GetTalkNPCGlobalID();
    pFrame->SetOperaType(ESClassType_Change);
    pFrame->SetNPCID(npcId);
    pFrame->ShowWnd(TRUE);

    return 0;
}

DWORD ProfessionMgr::OnChangeClassEx( tagGameEvent* pEvent )
{
    CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("ClassExNotOpen")]);

    return 0;
}

void ProfessionMgr::BeginNC_SetClass( DWORD dwNpcId, EClassType classType )
{
    tagNC_SetClass msg;
    msg.dwNpcId     = dwNpcId;
    msg.eClass      = classType;
    if (m_pSession->IsConnect())
        m_pSession->Send(&msg);
}

DWORD ProfessionMgr::OnNS_SetClass( tagNS_SetClass* pMsg, DWORD )
{
    if (E_Success == pMsg->dwErrCode)
    {
        CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ClassErrorMsg_0")]);
    }
    else
    {
        ShowErrorMsg(pMsg->dwErrCode);
    }

    return 0;
}

void ProfessionMgr::BeginNC_ChangeClass( DWORD dwNpcId, EClassType classType )
{
    tagNC_ChangeClass msg;
    msg.dwNpcId     = dwNpcId;
    msg.eClass      = classType;
    if (m_pSession->IsConnect())
        m_pSession->Send(&msg);
}

DWORD ProfessionMgr::OnNS_ChangeClass( tagNS_ChangeClass* pMsg, DWORD )
{
    if (E_Success == pMsg->dwErrCode)
    {
        CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ClassErrorMsg_0")]);
    }
    else
    {
        ShowErrorMsg(pMsg->dwErrCode);
    }

    return 0;
}

DWORD ProfessionMgr::OnNS_RoleClassChangeBroadcast( tagNS_RoleClassChangeBroadcast* pMsg, DWORD )
{
    if (RoleMgr::Inst()->GetLocalPlayerID() == pMsg->dwRoleID)
    {
        LocalPlayer* pLp = RoleMgr::Inst()->GetLocalPlayer();
        if (P_VALID(pLp))
        {
            pLp->SetPlayerClass(pMsg->eClass);

            // 发送游戏事件
            tagGameEvent e(_T("LocalPlayerSetClass"), NULL);
            m_pFrameMgr->SendEvent(&e);
        }
    }
    else
    {
        Role* pRole = RoleMgr::Inst()->FindRole(pMsg->dwRoleID);
        if (pRole->IS_KIND_OF(Player))
        {
            Player* pPlayer = static_cast<Player*>(pRole);
            pPlayer->SetPlayerClass(pMsg->eClass);

            // 发送游戏事件
            tagGameEvent e(_T("RemotePlayerSetClass"), NULL);
            m_pFrameMgr->SendEvent(&e);
        }
    }

    return 0;
}