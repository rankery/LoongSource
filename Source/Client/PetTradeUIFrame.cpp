#include "StdAfx.h"
#include "PetDef.h"
#include "PetTradeUIFrame.h"
#include "LockStatic.h"
#include "RoleMgr.h"
#include "PlayerNameTab.h"
#include "PetManager.h"
#include "PetTradeFrame.h"
#include "CurrencyMgr.h"
#include "PetInformationFrame.h"
#include "..\WorldDefine\currency_define.h"
#include "..\WorldDefine\msg_pet_exchange.h"

PetTradeUIFrame::PetTradeUIFrame(void) :
    m_Trunk(this),
    m_dwTargetID(GT_INVALID)
{
}

PetTradeUIFrame::~PetTradeUIFrame(void)
{
}

BOOL PetTradeUIFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
    BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);

    // 注册网络事件
    m_pCmdMgr->Register("NS_PetExchangeAdd_2Src",   (NETMSGPROC)m_Trunk.sfp2(&PetTradeUIFrame::OnNS_PetExchangeAdd_2Src),   _T("NS_PetExchangeAdd_2Src"));
    m_pCmdMgr->Register("NS_PetExchangeAdd_2Dst",   (NETMSGPROC)m_Trunk.sfp2(&PetTradeUIFrame::OnNS_PetExchangeAdd_2Dst),   _T("NS_PetExchangeAdd_2Dst"));
    m_pCmdMgr->Register("NS_PetExchangeDec",        (NETMSGPROC)m_Trunk.sfp2(&PetTradeUIFrame::OnNS_PetExchangeDec),        _T("NS_PetExchangeDec"));
    m_pCmdMgr->Register("NS_PetExchangeDec_2Dst",   (NETMSGPROC)m_Trunk.sfp2(&PetTradeUIFrame::OnNS_PetExchangeDec_2Dst),   _T("NS_PetExchangeDec_2Dst"));
    m_pCmdMgr->Register("NS_PetExchangeMoney",      (NETMSGPROC)m_Trunk.sfp2(&PetTradeUIFrame::OnNS_PetExchangeMoney),      _T("NS_PetExchangeMoney"));
    m_pCmdMgr->Register("NS_PetExchangeLock",       (NETMSGPROC)m_Trunk.sfp2(&PetTradeUIFrame::OnNS_PetExchangeLock),       _T("NS_PetExchangeLock"));

    // 注册游戏事件
    m_pFrameMgr->RegisterEventHandle(_T("tagPetDispChange"),  (FRAMEEVENTPROC)m_Trunk.sfp1(&PetTradeUIFrame::OnPetDispInfo));
    m_pFrameMgr->RegisterEventHandle(_T("MsgBox_PetExchangeMoney"), (FRAMEEVENTPROC)m_Trunk.sfp1(&PetTradeUIFrame::OnEventExchangeMoney));

    return bRet;
}

BOOL PetTradeUIFrame::Destroy()
{
    BOOL bRet = GameFrame::Destroy();

    //! 销毁窗体
    if( P_VALID(m_pWnd) )
    {
        m_pGUI->AddToDestroyList(m_pWnd);
        m_pWnd = NULL;
    }

    // 注销网络事件
    m_pCmdMgr->UnRegister("NS_PetExchangeAdd_2Src",   (NETMSGPROC)m_Trunk.sfp2(&PetTradeUIFrame::OnNS_PetExchangeAdd_2Src));
    m_pCmdMgr->UnRegister("NS_PetExchangeAdd_2Dst",   (NETMSGPROC)m_Trunk.sfp2(&PetTradeUIFrame::OnNS_PetExchangeAdd_2Dst));
    m_pCmdMgr->UnRegister("NS_PetExchangeDec",        (NETMSGPROC)m_Trunk.sfp2(&PetTradeUIFrame::OnNS_PetExchangeDec));
    m_pCmdMgr->UnRegister("NS_PetExchangeDec_2Dst",   (NETMSGPROC)m_Trunk.sfp2(&PetTradeUIFrame::OnNS_PetExchangeDec_2Dst));
    m_pCmdMgr->UnRegister("NS_PetExchangeMoney",      (NETMSGPROC)m_Trunk.sfp2(&PetTradeUIFrame::OnNS_PetExchangeMoney));
    m_pCmdMgr->UnRegister("NS_PetExchangeLock",       (NETMSGPROC)m_Trunk.sfp2(&PetTradeUIFrame::OnNS_PetExchangeLock));

    // 注销游戏事件
    m_pFrameMgr->UnRegisterEventHandler(_T("tagPetDispChange"),  (FRAMEEVENTPROC)m_Trunk.sfp1(&PetTradeUIFrame::OnPetDispInfo));
    m_pFrameMgr->UnRegisterEventHandler(_T("MsgBox_PetExchangeMoney"), (FRAMEEVENTPROC)m_Trunk.sfp1(&PetTradeUIFrame::OnEventExchangeMoney));

    return bRet;
}

BOOL PetTradeUIFrame::ReloadUI()
{
    XmlElement ele;
    tstring strPath = g_strLocalPath + _T("\\ui\\petexchange.xml");
    m_pGUI->LoadXml(&ele, "VFS_System", strPath.c_str());

    // 更改控件
    m_pGUI->ChangeXml(&ele, "petexchange_win\\petexchang_backpic\\petbackpic1\\lock1", "ClassName", "LockStatic");
    m_pGUI->ChangeXml(&ele, "petexchange_win\\petexchang_backpic\\petbackpic2\\lock2", "ClassName", "LockStatic");

    // 创建
    m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &ele);

    // 注册事件处理函数
    m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&PetTradeUIFrame::EventHandler));

    m_pBtTrade          =   static_cast<GUIButton*> (m_pWnd->GetChild(_T("petexchang_backpic\\ready")));
    m_pBtConfirm        =   static_cast<GUIButton*> (m_pWnd->GetChild(_T("petexchang_backpic\\sure")));
    m_pBtCancel         =   static_cast<GUIButton*> (m_pWnd->GetChild(_T("petexchang_backpic\\cancle")));
    m_pBtClose          =   static_cast<GUIButton*> (m_pWnd->GetChild(_T("petexchang_backpic\\closebutt")));

    m_pStcNameTarget    =   static_cast<GUIStatic*> (m_pWnd->GetChild(_T("petexchang_backpic\\playername1")));
    m_pLbTarget         =   static_cast<GUIListBox*>(m_pWnd->GetChild(_T("petexchang_backpic\\petbackpic1\\listback1\\list1")));
    m_pStcTargetGold    =   static_cast<GUIStatic*> (m_pWnd->GetChild(_T("petexchang_backpic\\petbackpic1\\num1")));
    m_pStcTargetSilver  =   static_cast<GUIStatic*> (m_pWnd->GetChild(_T("petexchang_backpic\\petbackpic1\\num2")));
    m_pLstcTarget       =   static_cast<LockStatic*>(m_pWnd->GetChild(_T("petexchang_backpic\\petbackpic1\\lock1")));

    m_pStcNameMyself    =   static_cast<GUIStatic*> (m_pWnd->GetChild(_T("petexchang_backpic\\playername2")));
    m_pLbMyself         =   static_cast<GUIListBox*>(m_pWnd->GetChild(_T("petexchang_backpic\\petbackpic2\\listback2\\list2")));
    m_pStcMyGold        =   static_cast<GUIStatic*> (m_pWnd->GetChild(_T("petexchang_backpic\\petbackpic2\\putpic1")));
    m_pStcMySilver      =   static_cast<GUIStatic*> (m_pWnd->GetChild(_T("petexchang_backpic\\petbackpic2\\putpic2")));
    m_pLstcMyself       =   static_cast<LockStatic*>(m_pWnd->GetChild(_T("petexchang_backpic\\petbackpic2\\lock2")));

    LockDstControl(FALSE);
    LockSrcControl(FALSE);

    return TRUE;
}

VOID PetTradeUIFrame::Update()
{

}

BOOL PetTradeUIFrame::EscCancel()
{
    
    return TRUE;
}

VOID PetTradeUIFrame::SetTargetPlayerID( DWORD dwTargetID )
{
    // 清空缓存
    ClearBuffer();
    RefreshToUI();

    m_dwTargetID = dwTargetID;
    // 设置对方玩家姓名
    LPCTSTR szName = PlayerNameTab::Inst()->FindNameByID(dwTargetID);
    m_pStcNameTarget->SetText(szName);
    m_pStcNameTarget->SetRefresh(true);
    // 设置自己姓名
    LPCTSTR szMine = PlayerNameTab::Inst()->FindNameByID(RoleMgr::Inst()->GetLocalPlayerID());
    m_pStcNameMyself->SetText(szMine);
    m_pStcNameMyself->SetRefresh(true);
}

VOID PetTradeUIFrame::LockSrcControl( BOOL bLock )
{
    m_pLstcMyself->SetInvisible(!bLock);
    m_pBtConfirm->SetEnable(bLock ? false : true);
    m_pBtConfirm->SetRefresh(TRUE);

    SetExchangeEnable();
}

VOID PetTradeUIFrame::LockDstControl( BOOL bLock )
{
    m_pLstcTarget->SetInvisible(!bLock);

    SetExchangeEnable();
}

BOOL PetTradeUIFrame::EventHandler( tagGUIEvent* pEvent )
{
    GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);

    switch (pEvent->eEvent)
    {
    case EGUIE_DragStart:
        {
            if (pWnd == m_pStcMyGold || pWnd == m_pStcMySilver)
            {
                if(GetObj("PetExchangeMoneyBox"))
                    KillObj("PetExchangeMoneyBox");

                CreateObj("PetExchangeMoneyBox", "MsgMoneyInputBox");
                TObjRef<MsgMoneyInputBox>("PetExchangeMoneyBox")->Init(
                    _T(""), g_StrTable[_T("PetExchange_InputMoney")], _T("MsgBox_PetExchangeMoney"), 
                    CurrencyMgr::Inst()->GetBagSilver(),
                    GT_INVALID, 0, 0, MBMT_Silver);
            }
        }
        break;
    case EGUIE_Click:
        {
            if (pWnd == m_pBtCancel || pWnd == m_pBtClose)
            {
                BeginNC_PetExchangeCancel();
            }
            else if (pWnd == m_pBtConfirm)
            {
                BeginNC_PetExchangeLock();
            }
            else if (pWnd == m_pBtTrade)
            {
                BeginNC_PetExchangeVerify();
            }
            else
            {
                // 如果本地交易框已锁定
                if (!m_pLstcMyself->IsInvisible())
                    break;
            }
        }
        break;
    case EGUIE_ItemClick:
        {
            if (pWnd == m_pLbTarget)
            {
                DWORD petId = FindPetByRow(m_mapRow2PetTarget, (INT)pEvent->dwParam1);
                if (petId != GT_INVALID)
                {
                    PetInformationFrame* pFrame = (PetInformationFrame*)m_pFrameMgr->GetFrame(_T("PetInformationFrame"));
                    if (!P_VALID(pFrame))
                        pFrame = (PetInformationFrame*)m_pFrameMgr->CreateFrame(m_strName.c_str(), _T("PetInformationFrame"), _T("PetInformationFrame"), 0);

                    pFrame->GetPetReq(m_dwTargetID, petId);
                }
            }
            else if (pWnd == m_pLbMyself)
            {
                DWORD petId = FindPetByRow(m_mapRow2PetMyself, (INT)pEvent->dwParam1);
                if (petId != GT_INVALID)
                {
                    BeginNC_PetExchangeDec(petId);
                }
            }
        }
        break;
    }

    return TRUE;
}

VOID PetTradeUIFrame::SetExchangeEnable()
{
    bool bEnable = !(m_pLstcTarget->IsInvisible() || m_pLstcMyself->IsInvisible());
    m_pBtTrade->SetEnable(bEnable);
    m_pBtTrade->SetRefresh(TRUE);
}

DWORD PetTradeUIFrame::OnNS_PetExchangeAdd_2Src( tagNS_PetExchangeAdd_2Src* pMsg, DWORD )
{
    if (pMsg->dwErrorCode == E_Success)
    {
        const tagPet* pPet = PetManager::Inst()->GetPet(pMsg->dwPetID);
        if (P_VALID(pPet))
        {
            // 插入本地缓存
            RemoveRowByPet(m_mapRow2PetMyself, pPet->dwID);
            int i = 0;
            for (i = 0; FindPetByRow(m_mapRow2PetMyself, i) != GT_INVALID ; ++i)
            {
            }
            m_mapRow2PetMyself.insert(make_pair(i, pPet->dwID));

            if(!m_pLstcTarget->IsInvisible())
                LockDstControl(FALSE);

            RefreshToUI();
        }
    }
    else
    {
        ShowExchangeErrorMsg(pMsg->dwErrorCode);
    }

    return 0;
}

DWORD PetTradeUIFrame::OnNS_PetExchangeAdd_2Dst( tagNS_PetExchangeAdd_2Dst* pMsg, DWORD )
{
    // 插入本地缓存
    RemoveRowByPet(m_mapRow2PetTarget, pMsg->dwPetID);
    int i = 0;
    for (i = 0; FindPetByRow(m_mapRow2PetTarget, i) != GT_INVALID ; ++i)
    {
    }
    m_mapRow2PetTarget.insert(make_pair(i, pMsg->dwPetID));

    if(!m_pLstcMyself->IsInvisible())
        LockSrcControl(FALSE);

    RefreshToUI();

    return 0;
}

DWORD PetTradeUIFrame::OnNS_PetExchangeDec( tagNS_PetExchangeDec* pMsg, DWORD )
{
    if (pMsg->dwErrorCode == E_Success)
    {
        RemoveRowByPet(m_mapRow2PetMyself, pMsg->dwPetID);

        if(!m_pLstcTarget->IsInvisible())
            LockDstControl(FALSE);

        RefreshToUI();
    }
    else
    {
        ShowExchangeErrorMsg(pMsg->dwErrorCode);
    }

    return 0;
}

DWORD PetTradeUIFrame::OnNS_PetExchangeDec_2Dst( tagNS_PetExchangeDec_2Dst* pMsg, DWORD )
{
    RemoveRowByPet(m_mapRow2PetTarget, pMsg->dwPetID);

    if(!m_pLstcMyself->IsInvisible())
        LockDstControl(FALSE);

    RefreshToUI();

    return 0;
}

DWORD PetTradeUIFrame::OnNS_PetExchangeMoney( tagNS_PetExchangeMoney* pMsg, DWORD )
{
    if (pMsg->dwErrorCode == E_Success)
    {
        if(pMsg->dwSrcID == m_dwTargetID)
        {
            SetMoneyDst(pMsg->n64Money);
        }
        else
        {
            SetMoneySrc(pMsg->n64Money);
        }
    }
    else
    {
        ShowExchangeErrorMsg(pMsg->dwErrorCode);
    }
    return 0;
}

DWORD PetTradeUIFrame::OnNS_PetExchangeLock( tagNS_PetExchangeLock* pMsg, DWORD )
{
    if (pMsg->dwErrorCode == E_Success)
    {
        if (pMsg->dwSrcID == m_dwTargetID)
        {
            LockDstControl(true);
        }
        else
        {
            LockSrcControl(true);
        }
    }
    else
    {
        ShowExchangeErrorMsg(pMsg->dwErrorCode);
    }
    return 0;
}

VOID PetTradeUIFrame::BeginNC_PetExchangeCancel()
{
    tagNC_PetExchangeCancel msg;
    m_pSession->Send(&msg);
}

VOID PetTradeUIFrame::BeginNC_PetExchangeVerify()
{
    tagNC_PetExchangeVerify msg;
    m_pSession->Send(&msg);

    m_pBtTrade->SetEnable(false);
    m_pBtTrade->SetRefresh(TRUE);
}

VOID PetTradeUIFrame::BeginNC_PetExchangeLock()
{
    tagNC_PetExchangeLock msg;
    m_pSession->Send(&msg);
}

VOID PetTradeUIFrame::BeginNC_PetExchangeAdd( DWORD dwPetID )
{
    tagNC_PetExchangeAdd msg;
    msg.dwPetID = dwPetID;
    m_pSession->Send(&msg);
}

VOID PetTradeUIFrame::BeginNC_PetExchangeDec( DWORD dwPetID )
{
    tagNC_PetExchangeDec msg;
    msg.dwPetID = dwPetID;
    m_pSession->Send(&msg);
}

VOID PetTradeUIFrame::ShowExchangeErrorMsg( DWORD dwErrorCode )
{
    PetTradeFrame* pTrade = (PetTradeFrame*)m_pFather;
    pTrade->ShowExchangeErrorMsg(dwErrorCode);
}

DWORD PetTradeUIFrame::FindPetByRow( _row2pet &row2pet, int rowIndex )
{
    _row2pet::iterator it = row2pet.find(rowIndex);
    if (it != row2pet.end())
    {
        return it->second;
    }
    else
    {
        return GT_INVALID;
    }
}

int PetTradeUIFrame::FindRowByPet( _row2pet &row2pet, DWORD petId )
{
    for (_row2pet::iterator it = row2pet.begin();
        it != row2pet.end(); ++it)
    {
        if (it->second == petId)
            return it->first;
    }

    return GT_INVALID;
}

void PetTradeUIFrame::RemoveRowByPet( _row2pet &row2pet, DWORD petId )
{
    for (_row2pet::iterator it = row2pet.begin();
        it != row2pet.end(); ++it)
    {
        if (it->second == petId)
        {
            row2pet.erase(it);
            return;
        }
    }
}

VOID PetTradeUIFrame::RefreshToUI()
{
    m_pLbTarget->Clear();
    for (_row2pet::iterator it = m_mapRow2PetTarget.begin();
        it != m_mapRow2PetTarget.end(); ++it)
    {
        const tagPetDispInfo* pDisp = PetManager::Inst()->FindPetDispInfo(m_dwTargetID, it->second);
        if (P_VALID(pDisp))
            m_pLbTarget->SetText(it->first, 0, pDisp->szName);
        else
            m_pLbTarget->SetText(it->first, 0, _T("loading..."));
    }

    m_pLbMyself->Clear();
    for (_row2pet::iterator it = m_mapRow2PetMyself.begin();
        it != m_mapRow2PetMyself.end(); ++it)
    {
        const tagPetDispInfo* pDisp = PetManager::Inst()->FindPetDispInfo(RoleMgr::Inst()->GetLocalPlayerID(), it->second);
        if (P_VALID(pDisp))
            m_pLbMyself->SetText(it->first, 0, pDisp->szName);
        else
            m_pLbMyself->SetText(it->first, 0, _T("loading..."));
    }
}

VOID PetTradeUIFrame::SetMoneySrc( INT64 n64Silver )
{
    TCHAR szTmp[X_SHORT_NAME] = {0};
    _sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("%d"), MSilver2DBGold(n64Silver));
    m_pStcMyGold->SetText(szTmp);
    m_pStcMyGold->SetRefresh(TRUE);
    _sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("%d"), MSilver2DBSilver(n64Silver));
    m_pStcMySilver->SetText(szTmp);
    m_pStcMySilver->SetRefresh(TRUE);

    if(!m_pLstcTarget->IsInvisible())
        LockDstControl(FALSE);
}

VOID PetTradeUIFrame::SetMoneyDst( INT64 n64Silver )
{
    TCHAR szTmp[X_SHORT_NAME] = {0};
    _sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("%d"), MSilver2DBGold(n64Silver));
    m_pStcTargetGold->SetText(szTmp);
    m_pStcTargetGold->SetRefresh(TRUE);
    _sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("%d"), MSilver2DBSilver(n64Silver));
    m_pStcTargetSilver->SetText(szTmp);
    m_pStcTargetSilver->SetRefresh(TRUE);

    if(!m_pLstcMyself->IsInvisible())
        LockSrcControl(FALSE);
}

DWORD PetTradeUIFrame::OnPetDispInfo( tagPetDispChange* e )
{
    RefreshToUI();

    return 0;
}

DWORD PetTradeUIFrame::OnEventExchangeMoney( tagMsgMoneyBoxEvent* pGameEvent )
{
    if (MBF_OK == pGameEvent->eResult)
    {
        if (pGameEvent->n64Silver > 0)
        {
            tagNC_PetExchangeMoney m;
            m.n64Money = pGameEvent->n64Silver;
            m_pSession->Send(&m);
        }
    }
    return 0;
}

VOID PetTradeUIFrame::DestroyMsgBox()
{
    if(GetObj("PetExchangeMoneyBox"))
        KillObj("PetExchangeMoneyBox");
}