#include "StdAfx.h"
#include "ProfessionFrame.h"
#include "CombatSysUtil.h"
#include "LocalPlayer.h"
#include "RoleMgr.h"
#include "ProfessionMgr.h"
#include "NPCTalkFrame.h"

ProfessionFrame::ProfessionFrame(void) :
    m_Trunk(this)
{
}

ProfessionFrame::~ProfessionFrame(void)
{
}

BOOL ProfessionFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
    BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);

    // 注册游戏事件
    m_pFrameMgr->RegisterEventHandle(_T("eventSetClass"),       (FRAMEEVENTPROC)m_Trunk.sfp1(&ProfessionFrame::EventSetClass));
    m_pFrameMgr->RegisterEventHandle(_T("eventChangeClass"),    (FRAMEEVENTPROC)m_Trunk.sfp1(&ProfessionFrame::EventChangeClass));

    return bRet;
}

BOOL ProfessionFrame::Destroy()
{
    BOOL bRet = GameFrame::Destroy();

    // 注销游戏事件
    m_pFrameMgr->UnRegisterEventHandler(_T("eventSetClass"),       (FRAMEEVENTPROC)m_Trunk.sfp1(&ProfessionFrame::EventSetClass));
    m_pFrameMgr->UnRegisterEventHandler(_T("eventChangeClass"),    (FRAMEEVENTPROC)m_Trunk.sfp1(&ProfessionFrame::EventChangeClass));

    //! 销毁窗体
    if( P_VALID(m_pWnd) )
    {
        m_pGUI->AddToDestroyList(m_pWnd);
        m_pWnd = NULL;
    }

    //! 销毁对话框
    if (P_VALID(GetObj("SetClassConfirm")))
        KillObj("SetClassConfirm");
    if (P_VALID(GetObj("ChangeClassConfirm")))
        KillObj("ChangeClassConfirm");

    return bRet;
}

BOOL ProfessionFrame::ReloadUI()
{
    XmlElement ele;
    tstring strPath = g_strLocalPath + _T("\\ui\\job.xml");
    m_pGUI->LoadXml(&ele, "VFS_System", strPath.c_str());

    // 创建
    m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &ele);

    // 注册事件处理函数
    m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&ProfessionFrame::EventHandler));

    // 获得控件
    m_pBtClose      = (GUIButton*)m_pWnd->GetChild(_T("job_pic\\closebutt"));
    m_pBtChange     = (GUIButton*)m_pWnd->GetChild(_T("job_pic\\changebutt"));
    m_pBtCancel     = (GUIButton*)m_pWnd->GetChild(_T("job_pic\\canclebutt"));
    for (int i = 0; i != MAX_CLASS_BUTTON_COUNT; ++i)
    {
        tstringstream tss;
        tss << _T("job_pic\\frm\\opjobutt") << i + 1;
        m_pPbClass[i] = (GUIPushButton*)m_pWnd->GetChild(tss.str().c_str());
    }

    // 初始窗体关闭
    ShowWnd(FALSE);

    return TRUE;
}

VOID ProfessionFrame::Update()
{

}

VOID ProfessionFrame::Render3D()
{

}

BOOL ProfessionFrame::EscCancel()
{
    if (m_pWnd->IsInvisible())
        return FALSE;

    ShowWnd(FALSE);

    return TRUE;
}

DWORD ProfessionFrame::EventHandler( tagGUIEvent* pEvent )
{
    GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);

    switch (pEvent->eEvent)
    {
    case EGUIE_Click:
        {
            if (pWnd == m_pBtClose)
            {
                //　关闭
                ShowWnd(FALSE);
            }
            else if (pWnd == m_pBtChange)
            {
                // 转职
                if (ESClassType_NEW == m_eType)
                {
                    // 是否可以转职
                    //if (...)

                    // 确认对话框
                    if (P_VALID(GetObj("SetClassConfirm")))
                        KillObj("SetClassConfirm");
                    CreateObj("SetClassConfirm", "MsgBox");

                    TCHAR szTemp[100];
                    _sntprintf(szTemp, sizeof(szTemp) / sizeof(TCHAR),
                        g_StrTable[_T("SetClassConfirm")],
                        ProfessionMgr::Inst().GetClassName(m_eCurrClassType));

                    TObjRef<MsgBox>("SetClassConfirm")->Init(_T(""), szTemp, _T("eventSetClass"), (MsgBoxFlag)(MBF_OK|MBF_Cancel), TRUE);
                }
                else if (ESClassType_Change == m_eType)
                {
                    // 是否可以更换职业
                    // if (...)

                    // 确认对话框
                    if (P_VALID(GetObj("ChangeClassConfirm")))
                        KillObj("ChangeClassConfirm");
                    CreateObj("ChangeClassConfirm", "MsgBox");
                    
                    TCHAR szTemp[100];
                    _sntprintf(szTemp, sizeof(szTemp) / sizeof(TCHAR),
                        g_StrTable[_T("ChangeClassConfirm")],
                        ProfessionMgr::Inst().GetClassName(m_eCurrClassType));

                    TObjRef<MsgBox>("ChangeClassConfirm")->Init(_T(""), szTemp, _T("eventChangeClass"), (MsgBoxFlag)(MBF_OK|MBF_Cancel), TRUE);
                }
            }
            else if (pWnd == m_pBtCancel)
            {
                // 放弃
                ShowWnd(FALSE);
            }
        }
        break;
    case EGUIE_PushButtonDown:
        {
            for (int i = 0; i != MAX_CLASS_BUTTON_COUNT; ++i)
            {
                if (pWnd == m_pPbClass[i])
                {
                    m_eCurrClassType = (EClassType)(i + 2);

                    LocalPlayer* pLp = RoleMgr::Inst()->GetLocalPlayer();
                    if (P_VALID(pLp))
                    {
                        EClassType classType = pLp->GetPlayerClass();
                        if (classType - 2 == i)
                        {
                            m_pBtChange->SetEnable(false);
                        }
                        else
                        {
                            m_pBtChange->SetEnable(true);
                        }
                    }

                    return 0;
                }
            }
        }
        break;
    }

    return 0;
}

VOID ProfessionFrame::ShowWnd( BOOL bVal )
{
    m_pWnd->SetInvisible(!bVal);

    if (bVal)
    {
        m_pWnd->FlipToTop();

        m_pPbClass[0]->SetState(EGUIBS_PushDown,TRUE);
        m_eCurrClassType = EV_Sword;

        LocalPlayer* pLp = RoleMgr::Inst()->GetLocalPlayer();
        if (P_VALID(pLp))
        {
            EClassType classType = pLp->GetPlayerClass();
            if (classType >= EV_Sword && classType <= EV_Musician)
            {
                m_pPbClass[classType - 2]->SetState(EGUIBS_PushDown,TRUE);
                m_eCurrClassType = classType;
            }
        }
    }
    else
    {

    }
}

DWORD ProfessionFrame::EventSetClass( tagMsgBoxEvent* pEvent )
{
    if (MBF_OK == pEvent->eResult)
    {
        ProfessionMgr::Inst().BeginNC_SetClass(m_dwNpcId, m_eCurrClassType);

        ShowWnd(FALSE);

        // 关掉npctalk
        NPCTalkFrame* pFrame = (NPCTalkFrame*)m_pFrameMgr->GetFrame(_T("NPCTalk"));
        if (P_VALID(pFrame))
            m_pFrameMgr->AddToDestroyList(pFrame);
    }
    
    return 0;
}

DWORD ProfessionFrame::EventChangeClass( tagMsgBoxEvent* pEvent )
{
    if (MBF_OK == pEvent->eResult)
    {
        ProfessionMgr::Inst().BeginNC_ChangeClass(m_dwNpcId, m_eCurrClassType);

        ShowWnd(FALSE);

        // 关掉npctalk
        NPCTalkFrame* pFrame = (NPCTalkFrame*)m_pFrameMgr->GetFrame(_T("NPCTalk"));
        if (P_VALID(pFrame))
            m_pFrameMgr->AddToDestroyList(pFrame);
    }

    return 0;
}