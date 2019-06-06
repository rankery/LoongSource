#include "StdAfx.h"
#include "MarriageMgr.h"
#include "..\WorldDefine\msg_marriage.h"
#include "CombatSysUtil.h"
#include "RoleMgr.h"
#include "Player.h"
#include "LocalPlayer.h"

MarriageMgr::MarriageMgr(void) :
    m_Trunk(this)
{
}

MarriageMgr::~MarriageMgr(void)
{
}

MarriageMgr g_MarriageMgr;
MarriageMgr& MarriageMgr::Inst()
{
    return g_MarriageMgr;
}

void MarriageMgr::Init()
{
    // 获取工具
    m_pCmdMgr       =   TObjRef<NetCmdMgr>();
    m_pUtil         =   TObjRef<Util>();
    m_pFrameMgr     =   TObjRef<GameFrameMgr>();
    m_pSession      =   TObjRef<NetSession>();

    // 注册网络事件
    m_pCmdMgr->Register("NS_RemoteChangeSpouse",(NETMSGPROC)m_Trunk.sfp2(&MarriageMgr::OnNS_RemoteChangeSpouse),    _T("NS_RemoteChangeSpouse"));

    // 注册游戏事件
    m_pFrameMgr->RegisterEventHandle(_T("OpenMarriageNote"), (FRAMEEVENTPROC)m_Trunk.sfp1(&MarriageMgr::ShowMarriageManule));
}

void MarriageMgr::Destroy()
{
    if (P_VALID(GetObj("MarriageManual")))
        KillObj("MarriageManual");

    // 注销网络事件
    m_pCmdMgr->UnRegister("NS_RemoteChangeSpouse",(NETMSGPROC)m_Trunk.sfp2(&MarriageMgr::OnNS_RemoteChangeSpouse));

    // 注销游戏事件
    m_pFrameMgr->UnRegisterEventHandler(_T("OpenMarriageNote"), (FRAMEEVENTPROC)m_Trunk.sfp1(&MarriageMgr::ShowMarriageManule));

}

void MarriageMgr::Update()
{

}

bool MarriageMgr::ShowErrMsg( DWORD dwErrCode )
{
    if (dwErrCode > E_Marriage_Accept &&
        dwErrCode < E_Marriage_ErrEnd)
    {
        tstringstream tss;
        tss << _T("MarriageErrorMsg_") << dwErrCode;
        CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[tss.str().c_str()]);

        return true;
    }

    return false;
}

DWORD MarriageMgr::OnNS_RemoteChangeSpouse( tagNS_RemoteChangeSpouse* pMsg, DWORD )
{
    LocalPlayer* pLp = RoleMgr::Inst()->GetLocalPlayer();
    if (P_VALID(pLp) && pLp->GetID() == pMsg->dwRoleID)
    {
        pLp->SetPlayerLoverID(pMsg->dwLoverID);
        pLp->SetPlayerIsHasWedding(pMsg->bHaveWedding);
        return 0;
    }

    Role* pRole = RoleMgr::Inst()->FindRole(pMsg->dwRoleID);
    if (P_VALID(pRole) && pRole->IS_KIND_OF(Player))
    {
        Player* pPlayer = static_cast<Player*>(pRole);
        pPlayer->SetPlayerLoverID(pMsg->dwLoverID);
        pPlayer->SetPlayerIsHasWedding(pMsg->bHaveWedding);
    }

    return 0;
}

DWORD MarriageMgr::ShowMarriageManule( tagGameEvent *pEvent )
{
    if (P_VALID(GetObj("MarriageManual")))
        KillObj("MarriageManual");

    CreateObj("MarriageManual", "MsgBox");
    TObjRef<MsgBox>("MarriageManual")->Init(_T(""), g_StrTable[_T("MarriageManual")], _T(""), MBF_OK, TRUE);

    return 0;
}