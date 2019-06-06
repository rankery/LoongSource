#include "StdAfx.h"
#include "MarriageFrame.h"
#include "MarriageMgr.h"
#include "..\WorldDefine\msg_marriage.h"
#include "RoleMgr.h"
#include "PlayerNameTab.h"
#include "Player.h"
#include "LocalPlayer.h"
#include "..\WorldDefine\currency_define.h"
#include "CurrencyMgr.h"
#include "MsgBox.h"
#include "QuestMgr.h"

MarriageFrame::MarriageFrame(void) :
    m_Trunk(this),
    m_dwReqTime(0),
    m_dwTargetID(GT_INVALID),
    m_dwNpcId(GT_INVALID)
{
}

MarriageFrame::~MarriageFrame(void)
{
}

BOOL MarriageFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
    BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);

    // 注册网络事件
    m_pCmdMgr->Register("NS_GetMarriageReq",        (NETMSGPROC)m_Trunk.sfp2(&MarriageFrame::OnNS_GetMarriageReq),      _T("NS_GetMarriageReq"));
    m_pCmdMgr->Register("NS_GetMarriageReqRes",     (NETMSGPROC)m_Trunk.sfp2(&MarriageFrame::OnNS_GetMarriageReqRes),   _T("NS_GetMarriageReqRes"));

    // 注册游戏事件
    m_pFrameMgr->RegisterEventHandle(_T("OpenMarriageApply"),       (FRAMEEVENTPROC)m_Trunk.sfp1(&MarriageFrame::OnOpenMarriageApply));
    m_pFrameMgr->RegisterEventHandle(_T("MarriageInputNameCommit"), (FRAMEEVENTPROC)m_Trunk.sfp1(&MarriageFrame::OnMarriageInputNameCommit));
    m_pFrameMgr->RegisterEventHandle(_T("MarriageReqCheckEvent"),   (FRAMEEVENTPROC)m_Trunk.sfp1(&MarriageFrame::OnMarriageReqCheckEvent));

    return bRet;
}

BOOL MarriageFrame::Destroy()
{
    BOOL bRet = GameFrame::Destroy();

    // 销毁对话框
    MsgBox* pMsgBox = static_cast<MsgBox*>(GetObj("BoxCanNotMarry"));
    if (P_VALID(pMsgBox))
        KillObj("BoxCanNotMarry");

    MsgInputBox* pInputBox = static_cast<MsgInputBox*>(GetObj("BoxMarriageInputName"));
    if (P_VALID(pInputBox))
        KillObj("BoxMarriageInputName");

    MsgBox* pObjMsgBox = static_cast<MsgBox*>(GetObj("BoxObjCanNotMarry"));
    if (P_VALID(pObjMsgBox))
        KillObj("BoxObjCanNotMarry");

    if(P_VALID(GetObj("MarriageReq")))
        KillObj("MarriageReq");

    if (P_VALID(GetObj("BoxMarrySuccess")))
        KillObj("BoxMarrySuccess");

    // 注销网络事件
    m_pCmdMgr->UnRegister("NS_GetMarriageReq",        (NETMSGPROC)m_Trunk.sfp2(&MarriageFrame::OnNS_GetMarriageReq));
    m_pCmdMgr->UnRegister("NS_GetMarriageReqRes",     (NETMSGPROC)m_Trunk.sfp2(&MarriageFrame::OnNS_GetMarriageReqRes));

    // 注销游戏事件
    m_pFrameMgr->UnRegisterEventHandler(_T("OpenMarriageApply"),        (FRAMEEVENTPROC)m_Trunk.sfp1(&MarriageFrame::OnOpenMarriageApply));
    m_pFrameMgr->UnRegisterEventHandler(_T("MarriageInputNameCommit"),  (FRAMEEVENTPROC)m_Trunk.sfp1(&MarriageFrame::OnMarriageInputNameCommit));
    m_pFrameMgr->UnRegisterEventHandler(_T("MarriageReqCheckEvent"),    (FRAMEEVENTPROC)m_Trunk.sfp1(&MarriageFrame::OnMarriageReqCheckEvent));

    return bRet;
}

VOID MarriageFrame::Update()
{
    GameFrame::Update();
}

DWORD MarriageFrame::OnNS_GetMarriageReq( tagNS_GetMarriageReq* pMsg, DWORD )
{
    // 服务器转发给目标玩家

    // 如果玩家尚未初始化，则直接拒绝
    LocalPlayer* pLp = RoleMgr::Inst()->GetLocalPlayer();
    if (!P_VALID(pLp) || !pLp->IsAttributeInited())
    {
        tagNC_GetMarriageReqRes m;
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
        if(P_VALID(GetObj("MarriageReq")))
            KillObj("MarriageReq");

        TCHAR szTemp[X_LONG_NAME] = {0};
        _sntprintf(szTemp, sizeof(szTemp)/sizeof(TCHAR), g_StrTable[_T("MarriageReqCheck")], PlayerNameTab::Inst()->FindNameByID(pMsg->dwSrcID));
        CreateObj("MarriageReq", "MsgBox");
        TObjRef<MsgBox>("MarriageReq")->Init(_T(""), szTemp, _T("MarriageReqCheckEvent"));
    }

    return 0;
}

DWORD MarriageFrame::OnNS_GetMarriageReqRes( tagNS_GetMarriageReqRes* pMsg, DWORD )
{
    if (E_Marriage_Accept != pMsg->dwErrorCode)
    {
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
    pLp->SetPlayerLoverID(m_dwTargetID);

    // 提示成功
    TCHAR szTemp[100] = {0};
    _sntprintf(szTemp, sizeof(szTemp) / sizeof(TCHAR),
        g_StrTable[_T("MarriageObjLimit")],
        PlayerNameTab::Inst()->FindNameByID(m_dwTargetID));

    MsgBox* pMsgBox = static_cast<MsgBox*>(GetObj("BoxMarrySuccess"));
    if (P_VALID(pMsgBox))
        KillObj("BoxMarrySuccess");

    _sntprintf(szTemp, sizeof(szTemp) / sizeof(TCHAR),
        g_StrTable[_T("MarriageSuccess")],
        PlayerNameTab::Inst()->FindNameByID(m_dwTargetID));
    CreateObj("BoxMarrySuccess", "MsgBox");
    TObjRef<MsgBox>("BoxMarrySuccess")->Init(_T(""), szTemp, _T(""), MBF_OK, TRUE);


    return 0;
}

DWORD MarriageFrame::OnOpenMarriageApply( tagGameEvent* pEvent )
{
    m_dwNpcId = QuestMgr::Inst()->GetTalkNPCGlobalID();

    // 先判断是否具备结婚条件
    if (!IfCanGetToMarry())
    {
        // 提示错误
        MsgBox* pMsgBox = static_cast<MsgBox*>(GetObj("BoxCanNotMarry"));
        if (P_VALID(pMsgBox))
            KillObj("BoxCanNotMarry");

        CreateObj("BoxCanNotMarry", "MsgBox");
        TObjRef<MsgBox>("BoxCanNotMarry")->Init(_T(""), g_StrTable[_T("MarriageLimit")], _T("eventCanNotMarry"), MBF_OK, TRUE);


        return 0;
    }

    // 弹出名称输入框，需要在输入框内输入结婚对象名称
    MsgInputBox* pMsgBox = static_cast<MsgInputBox*>(GetObj("BoxMarriageInputName"));
    if (P_VALID(pMsgBox))
        KillObj("BoxMarriageInputName");

    CreateObj("BoxMarriageInputName", "MsgInputBox");
    TObjRef<MsgInputBox>("BoxMarriageInputName")->Init(g_StrTable[_T("MarriageInputName")], g_StrTable[_T("MarriageInputNameTip")], _T("MarriageInputNameCommit"), GT_INVALID, MBIT_Name, NULL, TRUE);

    return 0;
}

bool MarriageFrame::IfCanGetToMarry()
{
    // 申请人是否未结婚

    // 申请人是否拥有MIN_MARRIAGE_GOLD金现金
    if (MSilver2DBGold(CurrencyMgr::Inst()->GetBagSilver()) < MIN_MARRIAGE_GOLD)
        return false;

    return true;
}

bool MarriageFrame::ifObjCanGetToMarry( LPCTSTR szPlayerName )
{
    DWORD playerId      = GT_INVALID;   //!< 目标玩家id
    Player* pPlayer     = NULL;         //!< 目标玩家obj

    // 通过player name 获得player id
    playerId = PlayerNameTab::Inst()->FindIDByName(szPlayerName);

    if (GT_INVALID == playerId)
        return false;

    // 保存当前目标信息
    m_dwTargetID    = playerId;
    m_strTargetName = szPlayerName;

    // 在九宫格取得player
    Role* pRole = RoleMgr::Inst()->FindRole(playerId);

    if (!P_VALID(pRole))
        return false;

    if (!pRole->IS_KIND_OF(Player))
        return false;

    pPlayer = static_cast<Player*>(pRole);

    // 取得本地玩家
    LocalPlayer* pLocalPlayer = RoleMgr::Inst()->GetLocalPlayer();
    if (!P_VALID(pLocalPlayer))
        return false;

    // 判断结婚对象是否与申请者为异性
    if (pLocalPlayer->GetAvatarAtt().bySex == pPlayer->GetAvatarAtt().bySex)
        return false;

    // 结婚对象与申请者是否互为好友，且友好度≥720

    // 结婚对象是否未结婚

    // 结婚对象是否在月老附近10格子以内

    return true;
}

DWORD MarriageFrame::OnMarriageInputNameCommit( tagMsgInputBoxEvent* pEvent )
{
    if (pEvent->eResult != MBF_OK)
        return 0;

    // 判断与对方是否有结婚条件
    if (!ifObjCanGetToMarry(pEvent->strInputName.c_str()))
    {
        // 提示错误
        MsgBox* pMsgBox = static_cast<MsgBox*>(GetObj("BoxObjCanNotMarry"));
        if (P_VALID(pMsgBox))
            KillObj("BoxObjCanNotMarry");

        CreateObj("BoxObjCanNotMarry", "MsgBox");
        TObjRef<MsgBox>("BoxObjCanNotMarry")->Init(_T(""), g_StrTable[_T("MarriageObjLimit")], _T("eventObjCanNotMarry"), MBF_OK, TRUE);

        return 0;
    }

    // 给目标玩家发送结婚请求
    tagNC_GetMarriageReq msg;
    msg.dwDstID = m_dwTargetID;
    msg.dwNpcID = m_dwNpcId;

    if (m_pSession->IsConnect())
        m_pSession->Send(&msg);

    return 0;
}

DWORD MarriageFrame::OnMarriageReqCheckEvent( tagMsgBoxEvent* pEvent )
{
    // 拒绝
    if (pEvent->eResult != MBF_OK)
    {
        tagNC_GetMarriageReqRes m;
        m.dwDstID       = m_dwTargetID;
        m.dwErrorCode   = E_Marriage_Refuse;
        if (m_pSession->IsConnect())
            m_pSession->Send(&m);

        return 0;
    }

    // 同意
    tagNC_GetMarriageReqRes m;
    m.dwDstID       = m_dwTargetID;
    m.dwErrorCode   = E_Marriage_Accept;
    if (m_pSession->IsConnect())
        m_pSession->Send(&m);

    return 0;
}