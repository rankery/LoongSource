#include "StdAfx.h"
#include "GuildStorePermitFrame.h"
#include "..\WorldDefine\guild_define.h"
#include "..\WorldDefine\msg_guild.h"
#include "PlayerNameTab.h"
#include "GuildEvent.h"
#include "RoleMgr.h"
#include "QuestMgr.h"

GuildStorePermitFrame::GuildStorePermitFrame(void) :
    m_Trunk(this)
{
}

GuildStorePermitFrame::~GuildStorePermitFrame(void)
{
}

BOOL GuildStorePermitFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
    BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);

    m_pCmdMgr->Register("NS_GetGuildWarePriList",			(NETMSGPROC)m_Trunk.sfp2(&GuildStorePermitFrame::OnNS_GetGuildWarePriList),			_T("NS_GetGuildWarePriList"));

    m_pFrameMgr->RegisterEventHandle(_T("tagGuildWarePriChange"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&GuildStorePermitFrame::OnChangePrivacy));
    m_pFrameMgr->RegisterEventHandle(_T("tagRoleGetNameEvent"),		    (FRAMEEVENTPROC)m_Trunk.sfp1(&GuildStorePermitFrame::OnGetRoleName));

    return bRet;
}

BOOL GuildStorePermitFrame::Destroy()
{
    BOOL bRet = GameFrame::Destroy();

    m_pCmdMgr->UnRegister("NS_GetGuildWarePriList",			(NETMSGPROC)m_Trunk.sfp2(&GuildStorePermitFrame::OnNS_GetGuildWarePriList));

    m_pFrameMgr->UnRegisterEventHandler(_T("tagGuildWarePriChange"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&GuildStorePermitFrame::OnChangePrivacy));
    m_pFrameMgr->UnRegisterEventHandler(_T("tagRoleGetNameEvent"),		    (FRAMEEVENTPROC)m_Trunk.sfp1(&GuildStorePermitFrame::OnGetRoleName));

    //! 销毁窗体
    if( P_VALID(m_pWnd) )
    {
        m_pGUI->AddToDestroyList(m_pWnd);
        m_pWnd = NULL;
    }

    //! 注销游戏事件

    return bRet;
}

BOOL GuildStorePermitFrame::ReloadUI()
{
    // load
    XmlElement ele;
    tstring strPath = g_strLocalPath + _T("\\ui\\guildwarehouse_limits.xml");
    m_pGUI->LoadXml(&ele, "VFS_System", strPath.c_str());

    // create
    m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &ele);
    m_pBtClose      =   (GUIButton*)    m_pWnd->GetChild(_T("guildlimits_pic\\closebutt"));
    m_pBtActive     =   (GUIButton*)    m_pWnd->GetChild(_T("guildlimits_pic\\openbutt"));
    m_pBtDiscard    =   (GUIButton*)    m_pWnd->GetChild(_T("guildlimits_pic\\canclebutt"));
    m_pLBList       =   (GUIListBox*)   m_pWnd->GetChild(_T("guildlimits_pic\\limitslistback_pic\\limitslist"));

    // resize listbox
    GUIStatic* pStcColumn0  =   (GUIStatic*)m_pWnd->GetChild(_T("guildlimits_pic\\limitslistback_pic\\list1"));
    GUIStatic* pStcColumn1  =   (GUIStatic*)m_pWnd->GetChild(_T("guildlimits_pic\\limitslistback_pic\\list2"));
    m_pLBList->Clear();
    m_pLBList->SetColNum(2, 130);
    m_pLBList->SetColWidth(0, pStcColumn0->GetSize().x);
    m_pLBList->SetColWidth(1, pStcColumn1->GetSize().x);

    // register ui event handler
    m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&GuildStorePermitFrame::EventHandler));

    // default close window
    ShowWnd(FALSE);

    return TRUE;
}

VOID GuildStorePermitFrame::Update()
{
    // 当窗体打开时
    if (P_VALID(m_pWnd))
    {
        if (m_pWnd->IsInvisible())
            return;
    }

    if(Kernel::Inst()->GetAccumTimeDW() - m_updateTimeDistance > 1000)//每隔1秒检测距离目标NPC的距离
    {
        if (RoleMgr::Inst()->IsOutValidDist( QuestMgr::Inst()->GetTalkNPCGlobalID() ) )
        {
            ShowWnd(FALSE);
            return;
        }
    }

    GameFrame::Update();
}

BOOL GuildStorePermitFrame::EscCancel()
{
    if( m_pWnd->IsInvisible() )
        return FALSE;

    m_pWnd->SetInvisible(TRUE);
    return TRUE;
}

DWORD GuildStorePermitFrame::EventHandler( tagGUIEvent* pEvent )
{
    GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);

    switch (pEvent->eEvent)
    {
    case EGUIE_Click:
        {
            if (pWnd == m_pBtClose)
            {
                ShowWnd(FALSE);
            }
            else if (pWnd == m_pBtActive)
            {
                // 打开权限
                DWORD roleId;
                BOOL privacy;
                if (FindPrivacyByRow(m_pLBList->GetCurSelectedRow(), roleId, privacy))
                {
                    if (!privacy)
                    {
                        SetNetPrivacy(true, roleId);
                    }
                }
            }
            else if (pWnd == m_pBtDiscard)
            {
                // 取消权限
                DWORD roleId;
                BOOL privacy;
                if (FindPrivacyByRow(m_pLBList->GetCurSelectedRow(), roleId, privacy))
                {
                    if (privacy)
                    {
                        SetNetPrivacy(false, roleId);
                    }
                }
            }
        }
        break;
    case EGUIE_ItemClick:
        {
            if (pWnd == m_pLBList)
            {
                // 点击列表行
                int rowIndex = (int)pEvent->dwParam1;

                ClickList(rowIndex);
            }
        }
        break;
    case EGUIE_ItemDblClick:
        {
            if (pWnd == m_pLBList)
            {
                // 双击列表行
                int rowIndex = (int)pEvent->dwParam1;
                m_pLBList->SetCurSelectedRow(rowIndex);

                DWORD roleId;
                BOOL privacy;
                if (FindPrivacyByRow(m_pLBList->GetCurSelectedRow(), roleId, privacy))
                {
                    SetNetPrivacy(!privacy, roleId);
                }
            }
        }
        break;
    }
	return 0;
}

VOID GuildStorePermitFrame::ShowWnd( BOOL bVal )
{
    m_pWnd->SetInvisible(!bVal);
    if (bVal)
    {
        m_pWnd->FlipToTop();

        Refresh();
    }
    else
    {

    }
}

DWORD GuildStorePermitFrame::OnChangePrivacy( tagGuildWarePriChange* pGameEvent )
{
    SetPrivacy(pGameEvent->dwRoleID, pGameEvent->bCanUse);

    ClickList(m_pLBList->GetCurSelectedRow());
	return 0;
}

void GuildStorePermitFrame::BeginNC_GetGuildWarePriList()
{
    tagNC_GetGuildWarePriList e;
    e.dwNPCID   =   m_dwNpcId;

    m_pSession->Send(&e);
}

DWORD GuildStorePermitFrame::OnNS_GetGuildWarePriList( tagNS_GetGuildWarePriList* pMsg, DWORD )
{
    for (int i = 0; i < pMsg->nNum; ++i)
    {
        tagGuildWarePri* pData = (tagGuildWarePri*)(pMsg->sGuildWarePri + i);

        m_mapRole2Permit.insert(make_pair(pData->dwRoleID, pData->bCanUse));
    }

    RefreshToList();
	return 0;
}

void GuildStorePermitFrame::ClearUI()
{
    m_mapRole2Permit.clear();
    m_mapRow2Role.clear();
    m_mapRole2Row.clear();
    m_pLBList->Clear();
    m_pBtActive->SetEnable(false);
    m_pBtDiscard->SetEnable(false);
}

void GuildStorePermitFrame::Refresh()
{
    ClearUI();

    // 请求列表
    BeginNC_GetGuildWarePriList();

}

void GuildStorePermitFrame::RefreshToList()
{
    m_mapRow2Role.clear();
    m_mapRole2Row.clear();
    m_pLBList->Clear();
    
    int rowIndex = 0;

    map<DWORD, BOOL>::iterator it;
    for (it = m_mapRole2Permit.begin(); it != m_mapRole2Permit.end(); ++it)
    {
        m_pLBList->SetText(rowIndex, 0, GetRoleName(it->first).c_str());
        m_pLBList->SetText(rowIndex, 1, GetPrivacyText(it->second), GetPrivacyColor(it->second));

        m_mapRow2Role.insert(make_pair(rowIndex, it->first));
        m_mapRole2Row.insert(make_pair(it->first, rowIndex));

        ++rowIndex;
    }

    m_pLBList->SetCurSelectedRow(0);
    ClickList(m_pLBList->GetCurSelectedRow());
}

tstring GuildStorePermitFrame::GetRoleName( DWORD dwRoleID )
{
    LPCTSTR szName = PlayerNameTab::Inst()->FindNameByID(dwRoleID);

    tstring strName = NULL == szName ? _T("loading...") : szName;

    return strName;
}

bool GuildStorePermitFrame::FindPrivacyByRoleID( DWORD dwRoleID, BOOL &ret )
{
    map<DWORD, BOOL>::iterator it = m_mapRole2Permit.find(dwRoleID);
    if (it != m_mapRole2Permit.end())
    {
        ret = it->second;

        return true;
    }

    return false;
}

bool GuildStorePermitFrame::FindRoleIDByRow( int rowIndex, DWORD &ret )
{
    map<int, DWORD>::iterator it = m_mapRow2Role.find(rowIndex);
    if (it != m_mapRow2Role.end())
    {
        ret = it->second;

        return true;
    }

    return false;
}

bool GuildStorePermitFrame::FindRowByRoleID( DWORD dwRoleID, int &ret )
{
    map<DWORD, int>::iterator it = m_mapRole2Row.find(dwRoleID);
    if (it != m_mapRole2Row.end())
    {
        ret = it->second;

        return true;
    }

    return false;
}

bool GuildStorePermitFrame::FindPrivacyByRow( int rowIndex, DWORD &dwRoleID, BOOL &ret )
{
    if (FindRoleIDByRow(rowIndex, dwRoleID))
    {
        if (FindPrivacyByRoleID(dwRoleID, ret))
        {
            return true;
        }
    }

    return false;
}

void GuildStorePermitFrame::ClickList(int rowIndex)
{
    DWORD roleId;
    BOOL privacy;
    if (FindPrivacyByRow(rowIndex, roleId, privacy))
    {
        m_pBtActive->SetEnable(!privacy);
        m_pBtDiscard->SetEnable(privacy);
    }
    else
    {
        m_pBtActive->SetEnable(false);
        m_pBtDiscard->SetEnable(false);                    
    }

    m_pBtActive->SetRefresh(TRUE);
    m_pBtDiscard->SetRefresh(TRUE);
}

void GuildStorePermitFrame::SetNetPrivacy( bool bVal, DWORD dwRoleId )
{
    tagNC_GuildWarePri e;
    e.dwNPCID   =   m_dwNpcId;
    e.dwRoleID  =   dwRoleId;
    e.bCanUse   =   bVal;

    m_pSession->Send(&e);
}

bool GuildStorePermitFrame::SetPrivacy( DWORD roleId, BOOL bVal )
{
    map<DWORD, BOOL>::iterator it = m_mapRole2Permit.find(roleId);
    if (it != m_mapRole2Permit.end())
    {
        it->second  =   bVal;
    }
    else
    {
        return false;
    }

    int rowIndex;
    if (FindRowByRoleID(roleId, rowIndex))
    {
        m_pLBList->SetText(rowIndex, 1, GetPrivacyText(bVal), GetPrivacyColor(bVal));
    }
    else
    {
        return false;
    }

    return true;
}

DWORD GuildStorePermitFrame::OnGetRoleName( tagRoleGetNameEvent* pGameEvent )
{
    if (!pGameEvent->bResult)
        return 0;

    int rowIndex;
    if (FindRowByRoleID(pGameEvent->dwRoleID, rowIndex))
    {
        m_pLBList->SetText(rowIndex, 0, pGameEvent->strRoleName.c_str());
    }
	return 0;
}