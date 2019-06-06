#include "StdAfx.h"
#include "DivorceFrame.h"
#include "CombatSysUtil.h"
#include "CurrencyMgr.h"
#include "..\WorldDefine\currency_define.h"
#include "RoleMgr.h"
#include "PlayerNameTab.h"
#include "Player.h"
#include "LocalPlayer.h"
#include "..\WorldDefine\msg_marriage.h"
#include "CurrencyMgr.h"
#include "MsgBox.h"
#include "QuestMgr.h"
#include "MarriageFrame.h"
#include "MarriageMgr.h"

DivorceFrame::DivorceFrame(void)
:m_Trunk(this),
m_dwReqTime(0),
m_dwTargetID(GT_INVALID)
{
}

DivorceFrame::~DivorceFrame(void)
{
}

BOOL DivorceFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
    BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);

    // 注册网络事件
    m_pCmdMgr->Register("NS_BreakMarriageReq",      (NETMSGPROC)m_Trunk.sfp2(&DivorceFrame::OnNS_BreakMarriageReq),    _T("NS_BreakMarriageReq"));
    m_pCmdMgr->Register("NS_BreakMarriageReqRes",   (NETMSGPROC)m_Trunk.sfp2(&DivorceFrame::OnNS_BreakMarriageReqRes), _T("NS_BreakMarriageReqRes"));

    // 注册游戏事件
    m_pFrameMgr->RegisterEventHandle(_T("OpenDivorceApply"),            (FRAMEEVENTPROC)m_Trunk.sfp1(&DivorceFrame::OnOpenDivorceApply));
    m_pFrameMgr->RegisterEventHandle(_T("DivorceConfirmCheckEvent"),    (FRAMEEVENTPROC)m_Trunk.sfp1(&DivorceFrame::OnDivorceConfirmCheckEvent));
    m_pFrameMgr->RegisterEventHandle(_T("DivorceReqCheckEvent"),        (FRAMEEVENTPROC)m_Trunk.sfp1(&DivorceFrame::OnDivorceReqCheckEvent));

    return bRet;
}

BOOL DivorceFrame::Destroy()
{
    BOOL bRet = GameFrame::Destroy();

    // 销毁对话框
    if(P_VALID(GetObj("DivorceConfirmCheck")))
        KillObj("DivorceConfirmCheck");

    if(P_VALID(GetObj("DivorceReq")))
        KillObj("DivorceReq");

    if (P_VALID(GetObj("BoxDivorceSuccess")))
        KillObj("BoxDivorceSuccess");

    // 注销网络事件
    m_pCmdMgr->UnRegister("NS_BreakMarriageReq",      (NETMSGPROC)m_Trunk.sfp2(&DivorceFrame::OnNS_BreakMarriageReq));
    m_pCmdMgr->UnRegister("NS_BreakMarriageReqRes",   (NETMSGPROC)m_Trunk.sfp2(&DivorceFrame::OnNS_BreakMarriageReqRes));

    // 注销游戏事件
    m_pFrameMgr->UnRegisterEventHandler(_T("OpenDivorceApply"),             (FRAMEEVENTPROC)m_Trunk.sfp1(&DivorceFrame::OnOpenDivorceApply));
    m_pFrameMgr->UnRegisterEventHandler(_T("DivorceConfirmCheckEvent"),     (FRAMEEVENTPROC)m_Trunk.sfp1(&DivorceFrame::OnDivorceConfirmCheckEvent));
    m_pFrameMgr->UnRegisterEventHandler(_T("DivorceReqCheckEvent"),         (FRAMEEVENTPROC)m_Trunk.sfp1(&DivorceFrame::OnDivorceReqCheckEvent));

    return bRet;
}

VOID DivorceFrame::Update()
{
    GameFrame::Update();
}

DWORD DivorceFrame::OnNS_BreakMarriageReq( tagNS_BreakMarriageReq* pMsg, DWORD )
{
    // 服务器转发给目标玩家

    // 如果玩家尚未初始化，则直接拒绝
    LocalPlayer* pLp = RoleMgr::Inst()->GetLocalPlayer();
    if (!P_VALID(pLp) || !pLp->IsAttributeInited())
    {
        tagNC_BreakMarriageReqRes m;
        m.dwDstID       = pMsg->dwSrcID;
        m.dwErrorCode   = E_Marriage_Refuse;
        if (m_pSession->IsConnect())
            m_pSession->Send(&m);

        return 0;
    }

    if (pMsg->dwSrcID != RoleMgr::Inst()->GetLocalPlayerID())
    {
        // 保存目标id
        m_dwTargetID = pMsg->dwSrcID;
        // 请求时间
        m_dwReqTime	 = MAX_MARRIAGE_REQTIME;

        // 弹出"同意/拒绝"对话框
        if(P_VALID(GetObj("DivorceReq")))
            KillObj("DivorceReq");

        TCHAR szTemp[X_LONG_NAME] = {0};
        _sntprintf(szTemp, sizeof(szTemp)/sizeof(TCHAR), g_StrTable[_T("DivorceReqCheck")], PlayerNameTab::Inst()->FindNameByID(pMsg->dwSrcID));
        CreateObj("DivorceReq", "MsgBox");
        TObjRef<MsgBox>("DivorceReq")->Init(_T(""), szTemp, _T("DivorceReqCheckEvent"));
    }

    return 0;
}

DWORD DivorceFrame::OnNS_BreakMarriageReqRes( tagNS_BreakMarriageReqRes* pMsg, DWORD )
{
    if (E_Marriage_Accept != pMsg->dwErrorCode)
    {
        // 提示错误
        // 提示错误
        if (pMsg->dwErrorCode == E_Marriage_Refuse
            && RoleMgr::Inst()->GetLocalPlayerID() == pMsg->dwSrcID)
            return 0;

        MarriageMgr::Inst().ShowErrMsg(pMsg->dwErrorCode);

        return 0;
    }

    // 保存本地玩家配偶信息
    LocalPlayer* pLp = RoleMgr::Inst()->GetLocalPlayer();
    if (P_VALID(pLp))
    {
        if (pLp->GetID() == pMsg->dwSrcID)
            m_dwTargetID = pMsg->dwDstID;
        else
            m_dwTargetID = pMsg->dwSrcID;
    }
    pLp->SetPlayerLoverID(GT_INVALID);

    // 提示成功
    MsgBox* pMsgBox = static_cast<MsgBox*>(GetObj("BoxDivorceSuccess"));
    if (P_VALID(pMsgBox))
        KillObj("BoxDivorceSuccess");

    CreateObj("BoxDivorceSuccess", "MsgBox");
    TObjRef<MsgBox>("BoxDivorceSuccess")->Init(_T(""), g_StrTable[_T("DivorceSuccess")], _T(""), MBF_OK, TRUE);

    return 0;
}

DWORD DivorceFrame::OnOpenDivorceApply( tagGameEvent* pEvent )
{
    LocalPlayer* pLp = RoleMgr::Inst()->GetLocalPlayer();
    if (!P_VALID(pLp))
        return 0;

    // 先判断本地玩家是否已婚
    if (GT_INVALID == pLp->GetPlayerLoverID())
    {
        CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("DivorceHasNotGetMarried")]);
        return 0;
    }

    // 弹出确认提示框
    if(P_VALID(GetObj("DivorceConfirmCheck")))
        KillObj("DivorceConfirmCheck");

    TCHAR szTemp[X_LONG_NAME] = {0};
    _sntprintf(szTemp, sizeof(szTemp)/sizeof(TCHAR), g_StrTable[_T("DivorceConfirmCheck")], PlayerNameTab::Inst()->FindNameByID(pLp->GetPlayerLoverID()));
    CreateObj("DivorceConfirmCheck", "MsgBox");
    TObjRef<MsgBox>("DivorceConfirmCheck")->Init(_T(""), szTemp, _T("DivorceConfirmCheckEvent"));


    return 0;
}

DWORD DivorceFrame::OnDivorceConfirmCheckEvent( tagMsgBoxEvent* pEvent )
{
    LocalPlayer* pLp = RoleMgr::Inst()->GetLocalPlayer();
    if (!P_VALID(pLp))
        return 0;

    if (pEvent->eResult != MBF_OK)
        return 0;

    // 申请人是否拥有MIN_DIVORCE_GOLD金现金
    if (MSilver2DBGold(CurrencyMgr::Inst()->GetBagSilver()) < MIN_DIVORCE_GOLD)
    {
        CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("DivorceMoneyLimit")]);
        return 0;
    }

    // 给目标玩家发出离婚审请
    tagNC_BreakMarriageReq msg;
    msg.dwDstID = pLp->GetPlayerLoverID();
    m_pSession->Send(&msg);

    return 0;
}

DWORD DivorceFrame::OnDivorceReqCheckEvent( tagMsgBoxEvent* pEvent )
{
    // 拒绝
    if (pEvent->eResult != MBF_OK)
    {
        tagNC_BreakMarriageReqRes m;
        m.dwDstID       = m_dwTargetID;
        m.dwErrorCode   = E_Marriage_Refuse;
        if (m_pSession->IsConnect())
            m_pSession->Send(&m);

        return 0;
    }

    // 同意
    tagNC_BreakMarriageReqRes m;
    m.dwDstID       = m_dwTargetID;
    m.dwErrorCode   = E_Marriage_Accept;
    if (m_pSession->IsConnect())
        m_pSession->Send(&m);

    return 0;
}