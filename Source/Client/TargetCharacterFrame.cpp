#include "StdAfx.h"
#include "TargetCharacterFrame.h"
#include "RemoteRoleStateFrame.h"
#include <iomanip>
#include "..\WorldDefine\role_att.h"

//向FWQ发送请救的延迟时间
#define Max_Require_Time 5000

TargetCharacterFrame::TargetCharacterFrame(void) :
    m_Trunk(this),
    m_dwCurrPlayerID(GT_INVALID),
    m_bneedRefreshRepute(false),
    m_timeFirst(0)
{
}

TargetCharacterFrame::~TargetCharacterFrame(void)
{
}

BOOL TargetCharacterFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
    BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);
    
    // 注册网络事件
    m_pCmdMgr->Register("NS_GetReputation",     (NETMSGPROC)m_Trunk.sfp2(&TargetCharacterFrame::OnNS_GetReputation),    _T("NS_GetReputation"));
    m_pCmdMgr->Register("NS_GetRoleTitles",     (NETMSGPROC)m_Trunk.sfp2(&TargetCharacterFrame::OnNS_GetRoleTitles),    _T("NS_GetRoleTitles"));

    // 注册游戏事件
    m_pFrameMgr->RegisterEventHandle(_T("tagDispPlayerState"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&TargetCharacterFrame::OnDispPlayerState));

    return bRet;
}

BOOL TargetCharacterFrame::Destroy()
{
	BOOL bRet = GameFrame::Destroy();

    // 注销网络事件
    m_pCmdMgr->UnRegister("NS_GetReputation",   (NETMSGPROC)m_Trunk.sfp2(&TargetCharacterFrame::OnNS_GetReputation));
    m_pCmdMgr->UnRegister("NS_GetRoleTitles",   (NETMSGPROC)m_Trunk.sfp2(&TargetCharacterFrame::OnNS_GetRoleTitles));

    // 注销游戏事件
    m_pFrameMgr->UnRegisterEventHandler(_T("tagDispPlayerState"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&TargetCharacterFrame::OnDispPlayerState));
	if( P_VALID(m_pWnd) )
	{
		m_pGUI->AddToDestroyList(m_pWnd);
		m_pWnd = NULL;
	}

    return bRet;
}

BOOL TargetCharacterFrame::ReloadUI()
{
    // 创建
    XmlElement ele;
    tstring strPath = g_strLocalPath + _T("\\ui\\rolestate2.xml");
    m_pGUI->LoadXml(&ele, "VFS_System", strPath.c_str());

    // 更改控件
    m_pGUI->ChangeXml(&ele, "ra_win2\\ra_pic\\rstatepic\\equipback\\role", "ClassName", "PlayerViewWindow");
    m_pGUI->ChangeXml(&ele, "ra_win2\\ra_pic\\roletitle\\titlelistpic\\titlelist", "ClassName", "TreeEx");
	m_pGUI->ChangeXml(&ele, "ra_win2\\ra_pic\\rstatepic\\equipback\\armet\\armetpic", "ClassName", "ItemButton");
	m_pGUI->ChangeXml(&ele, "ra_win2\\ra_pic\\rstatepic\\equipback\\mantle\\mantlepic", "ClassName", "ItemButton");
	m_pGUI->ChangeXml(&ele, "ra_win2\\ra_pic\\rstatepic\\equipback\\armor\\armorpic", "ClassName", "ItemButton");
	m_pGUI->ChangeXml(&ele, "ra_win2\\ra_pic\\rstatepic\\equipback\\sash\\sashpic", "ClassName", "ItemButton");
	m_pGUI->ChangeXml(&ele, "ra_win2\\ra_pic\\rstatepic\\equipback\\cuff\\cuffpic", "ClassName", "ItemButton");
	m_pGUI->ChangeXml(&ele, "ra_win2\\ra_pic\\rstatepic\\equipback\\mainwp\\mainwppic", "ClassName", "ItemButton");
	m_pGUI->ChangeXml(&ele, "ra_win2\\ra_pic\\rstatepic\\equipback\\minorwp\\minorwppic", "ClassName", "ItemButton");
	m_pGUI->ChangeXml(&ele, "ra_win2\\ra_pic\\rstatepic\\equipback\\ring1\\ring1pic", "ClassName", "ItemButton");
	m_pGUI->ChangeXml(&ele, "ra_win2\\ra_pic\\rstatepic\\equipback\\ring2\\ring2pic", "ClassName", "ItemButton");
	m_pGUI->ChangeXml(&ele, "ra_win2\\ra_pic\\rstatepic\\equipback\\boot\\bootpic", "ClassName", "ItemButton");
	m_pGUI->ChangeXml(&ele, "ra_win2\\ra_pic\\rstatepic\\equipback\\pants\\pantspic", "ClassName", "ItemButton");
	m_pGUI->ChangeXml(&ele, "ra_win2\\ra_pic\\rstatepic\\equipback\\necklace\\nlpic", "ClassName", "ItemButton");
	m_pGUI->ChangeXml(&ele, "ra_win2\\ra_pic\\rstatepic\\equipback\\mask\\maskpic", "ClassName", "ItemButton");

    for (int i = 0; i < ECLT_NUM; ++i)
    {
        char cbuff[128];
        _snprintf(cbuff, sizeof(cbuff)/sizeof(char),
            "ra_win2\\ra_pic\\repute\\reputebackpic\\propic%d\\pro%d",
            i + 1, i + 1);
        m_pGUI->ChangeXml(&ele, cbuff, "ClassName", "ProgressEx");
    }
    m_pWnd      =   m_pGUI->CreateWnd(_T("\\desktop"), &ele);
    
    m_pEquip    =   m_pWnd->GetChild(_T("ra_pic\\rstatepic"));
    m_pRepute   =   m_pWnd->GetChild(_T("ra_pic\\repute"));
    m_pTitle    =   m_pWnd->GetChild(_T("ra_pic\\roletitle"));

    m_pBtEquip  =   static_cast<GUIPushButton*>(m_pWnd->GetChild(_T("ra_pic\\winopt\\optbutt1")));
    m_pBtReput  =   static_cast<GUIPushButton*>(m_pWnd->GetChild(_T("ra_pic\\winopt\\optbutt3")));
    m_pBtTitle  =   static_cast<GUIPushButton*>(m_pWnd->GetChild(_T("ra_pic\\winopt\\optbutt30")));

    m_pBtClose  =   static_cast<GUIButton*>(m_pWnd->GetChild(_T("ra_pic\\closebutt")));

    for (INT i = 1; i != ECLT_NUM + 1; ++i)
    {
        INT index = i - 1;
        TCHAR tbuff[128];
        size_t count    =   sizeof(tbuff) / sizeof(TCHAR);
        _sntprintf(tbuff, count, _T("ra_pic\\repute\\reputebackpic\\propic%d\\pro%d"), i, i);
        m_pPrgrssReputeValue[index]     =   static_cast<ProgressEx*>(m_pWnd->GetChild(tbuff));
        m_pPrgrssReputeValue[index]->SetSpeed(100000000);
        _sntprintf(tbuff, count, _T("ra_pic\\repute\\reputebackpic\\propic%d\\pro%d\\extent%d"), i, i, i);
        m_pStcReputeLevel[index]        =   static_cast<GUIStatic*>(m_pWnd->GetChild(tbuff));
        _sntprintf(tbuff, count, _T("ra_pic\\repute\\reputebackpic\\num%d"), i);
        m_pStcContrib[index]            =   static_cast<GUIStatic*>(m_pWnd->GetChild(tbuff));
        _sntprintf(tbuff, count, _T("ra_pic\\repute\\reputebackpic\\hero%d"), i);
        m_pStcHero[index]               =   static_cast<GUIStatic*>(m_pWnd->GetChild(tbuff));
    }

    m_pTreeTitleList    =   static_cast<TreeEx*>(m_pWnd->GetChild(_T("ra_pic\\roletitle\\titlelistpic\\titlelist")));
    m_pStcexDescription =   static_cast<GUIStaticEx*>(m_pWnd->GetChild(_T("ra_pic\\roletitle\\titledescriptionback\\titlelistbackpic\\description")));

    // 注册事件处理函数
    m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&TargetCharacterFrame::EventHandler));

    // 初始化所有title列表
    InitTree();

    // 初始状态
    m_pWnd->SetInvisible(TRUE);

    m_pEquip->SetInvisible(TRUE);
    m_pRepute->SetInvisible(FALSE);
    m_pTitle->SetInvisible(TRUE);

    m_pBtEquip->SetEnable(true);
    m_pBtReput->SetEnable(true);
    m_pBtTitle->SetEnable(true);

	m_pRRSF = (RemoteRoleStateFrame*)m_pFrameMgr->CreateFrame(m_strName.c_str(), _T("RemoteRoleState"), _T("RemoteRoleStateFrame"), 0);

    m_pBtEquip->SetState(EGUIBS_PushDown, FALSE);

    return TRUE;
}

DWORD TargetCharacterFrame::OnDispPlayerState( tagDispPlayerState* pMsg )
{
    m_dwCurrPlayerID        =   pMsg->dwRoleID;

    ShowWnd(TRUE);

    if (!m_pEquip->IsInvisible() && IS_PLAYER(m_dwCurrPlayerID) )
    {
        // 装备
		ResetRemoteRoleEquip();
    }
    else if (!m_pRepute->IsInvisible())
    {
        // 声望
        ResetRepute();
        BeginNC_GetReputation(m_dwCurrPlayerID);
    }
    else if (!m_pTitle->IsInvisible())
    {
        // 称号
        ResetTitle();
        BeginNC_GetRoleTitles(m_dwCurrPlayerID);
    }

	return 0;
}

VOID TargetCharacterFrame::ShowWnd( BOOL bVal )
{
    m_pWnd->SetInvisible(!bVal);

    if (bVal)
    {

    }
    else
    {
		tagGameEvent evt(_T("DeleRemoteRoleEquipInfo"), NULL);
		m_pFrameMgr->SendEvent(&evt);
    }
}

DWORD TargetCharacterFrame::EventHandler( tagGUIEvent* pEvent )
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
        }
        break;
    case EGUIE_PushButtonDown:
        {
            if (pEvent->dwParam1 != 1) // 不是按下不处理
                return 0;

            if (pWnd == m_pBtEquip)
            {
                // 装备
                m_pEquip->SetInvisible(FALSE);
                m_pRepute->SetInvisible(TRUE);
                m_pTitle->SetInvisible(TRUE);

				ResetRemoteRoleEquip();
            }
            else if (pWnd == m_pBtReput)
            {
                // 声望
                m_pEquip->SetInvisible(TRUE);
                m_pRepute->SetInvisible(FALSE);
                m_pTitle->SetInvisible(TRUE);
                
                // 获得声望请求
                ResetRepute();
                if (m_dwCurrPlayerID != GT_INVALID && !m_pWnd->IsInvisible())
                    BeginNC_GetReputation(m_dwCurrPlayerID);
            }
            else if (pWnd == m_pBtTitle)
            {
                // 称号
                m_pEquip->SetInvisible(TRUE);
                m_pRepute->SetInvisible(TRUE);
                m_pTitle->SetInvisible(FALSE);

                // 获得称号请求
                ResetTitle();
                if (m_dwCurrPlayerID != GT_INVALID && !m_pWnd->IsInvisible())
                    BeginNC_GetRoleTitles(m_dwCurrPlayerID);
            }
        }
        break;
    case EGUIE_ItemClick:
        if (pWnd == m_pTreeTitleList)	// 选中某一称号
        {
            SelectRoleTitle();
        }
        break;
    }
	return 0;
}

VOID TargetCharacterFrame::ResetRepute()
{
    for (INT i = 0; i != ECLT_NUM; ++i)
    {
        m_pPrgrssReputeValue[i]->SetValue(0);
        m_pStcReputeLevel[i]->SetText(_T(""));
        m_pStcContrib[i]->SetText(_T(""));
        m_pStcHero[i]->SetText(_T(""));
    }
}

VOID TargetCharacterFrame::BeginNC_GetReputation( DWORD dwRoleID )
{
    tagNC_GetReputation e;
    e.dwRoleID      =   m_dwCurrPlayerID;
    if (m_pSession->IsConnect())
        m_pSession->Send(&e);
}

DWORD TargetCharacterFrame::OnNS_GetReputation( tagNS_GetReputation* pMsg, DWORD )
{
    if (pMsg->dwRoleID != m_dwCurrPlayerID)
        return 0;

    for (INT i = 0; i != ECLT_NUM; ++i)
    {
        /*! 读取声望级别 */
        tagReputeLevel ReputeLevel;
        ReputeMgr::Inst()->UlityCalReputeLevel(ReputeLevel, pMsg->ReputeData.nrValue[i]);
        /*! 读取声望值 */
        INT nRepute = pMsg->ReputeData.nrValue[i];
        /*! 设置颜色 */
        ReputeMgr::Inst()->UlitySetPrgrssColor(m_pPrgrssReputeValue[i], ReputeLevel.eColor);
        /*! 设置状态条数据 */
        m_pPrgrssReputeValue[i]->SetMaxValue((FLOAT)ReputeLevel.nDiffValue);
        m_pPrgrssReputeValue[i]->SetValue((FLOAT)nRepute - ReputeLevel.nFrom);
        /*! 设置贡献值 */
        TCHAR szTemp[128];
        _itot(pMsg->ReputeData.ncValue[i], szTemp, 10);
        m_pStcContrib[i]->SetText(szTemp);
        /*! 设置名人标记 */
        LPCTSTR szFame = pMsg->ReputeData.bisFame[(ECLanType)i] ? g_StrTable[_T("ClanFame")] : _T("");
        m_pStcHero[i]->SetText(szFame);
        /*! 设置级别文字 */
        tstringstream str;
        str << ReputeLevel.szLevel << _T(" ") << setw(6) << pMsg->ReputeData.nrValue[i];
        m_pStcReputeLevel[i]->SetText(str.str().c_str());
    }

    m_bneedRefreshRepute = true;
    m_timeFirst = Kernel::Inst()->GetAccumTimeDW();
	return 0;
}

VOID TargetCharacterFrame::ResetTitle()
{
    // 清空所有获得的
    for (map<DWORD, tagTitleList>::iterator iter = m_mapOwnedTitles.begin();
        iter != m_mapOwnedTitles.end(); ++iter)
        iter->second.bOwend = false;

    // 拥有数清零
    for (map<DWORD, tagTitleTypeActivedCount>::iterator itActCount = m_mapActivedCount.begin();
        itActCount != m_mapActivedCount.end(); ++itActCount)
        itActCount->second.bCount = 0;

    //! 更新title列表
    UpdateTree();

    //! 刷新界面
    SelectRoleTitle();
}

VOID TargetCharacterFrame::BeginNC_GetRoleTitles( DWORD dwRoleID )
{
    //! 取得角色已获得的所有称号
    tagNC_GetRoleTitles msg;
    msg.dwRoleID = dwRoleID;
    m_pSession->Send(&msg);
}

DWORD TargetCharacterFrame::OnNS_GetRoleTitles( tagNS_GetRoleTitles* pMsg, DWORD )
{
    switch (pMsg->dwErrorCode)
    {
    case E_Title_Success:
        if (m_dwCurrPlayerID == pMsg->dwRoleID)
        {
            for (map<DWORD, tagTitleList>::iterator iter = m_mapOwnedTitles.begin();
                iter != m_mapOwnedTitles.end(); ++iter)
                iter->second.bOwend = false;

            // 拥有数清零
            for (map<DWORD, tagTitleTypeActivedCount>::iterator itActCount = m_mapActivedCount.begin();
                itActCount != m_mapActivedCount.end(); ++itActCount)
                itActCount->second.bCount = 0;

            for (int i = 0, offset = 0;
                i < (int)pMsg->u16TitleNum;
                ++i, offset += sizeof(UINT16))
            {
                UINT16 titleID = 0;
                memcpy(&titleID, pMsg->byData + offset, sizeof(UINT16));

                map<DWORD, tagTitleList>::iterator iter =
                    m_mapOwnedTitles.find((DWORD)titleID);
                if (iter != m_mapOwnedTitles.end())
                {
                    iter->second.bOwend = true;

                    map<DWORD, tagTitleTypeActivedCount>::iterator itCount = m_mapActivedCount.find(iter->second.dwTypeID);
                    if (itCount != m_mapActivedCount.end())
                    {
                        itCount->second.bCount++;
                    }
                }
            }

            //! 更新title列表
            UpdateTree();
            //! 刷新界面
            SelectRoleTitle();
        }
        break;
    }

    return 0;
}

VOID TargetCharacterFrame::InitTree()
{
    //! 从数据中读取称号内容到tree控件中
    //! root
    m_pTreeTitleList->InsterItem(NULL, _T("root"), _T(""));

    const map<DWORD, tagRoleTitleTypeInfo>* pMapType = RoleTitleProtoData::Inst()->GetTypeMap();
    for (map<DWORD, tagRoleTitleTypeInfo>::const_iterator citer = pMapType->begin();
        citer != pMapType->end(); ++citer)
    {
        TCHAR cBuff1[32];
        TCHAR cBuff2[32];
        _sntprintf(cBuff1, sizeof(cBuff1)/sizeof(TCHAR), _T("%d"), citer->first);
        _sntprintf(cBuff2, sizeof(cBuff2)/sizeof(TCHAR),
            _T("%s [%d/%d]"),
            citer->second.strName.c_str(),
            0, citer->second.nTitlesCount);
        DWORD abc = m_pTreeTitleList->InsterItem(_T("\\root"), cBuff1, cBuff2);

        tagTitleTypeActivedCount aData;
        aData.strName       =   citer->second.strName;
        aData.bCount        =   0;
        aData.nTotal        =   citer->second.nTitlesCount;
        aData.dwTreeItemID  =   abc;
        m_mapActivedCount.insert(make_pair(citer->first, aData));
    }

    const map<DWORD, tagRoleTitleProto>* pMapTitle
        = RoleTitleProtoData::Inst()->GetTitleMap();
    for (map<DWORD, tagRoleTitleProto>::const_iterator citer = pMapTitle->begin();
        citer != pMapTitle->end(); ++citer)
    {

        TCHAR cBuff1[32];
        TCHAR cBuff2[32];
        _sntprintf(cBuff1, sizeof(cBuff1)/sizeof(TCHAR), _T("\\root\\%d"), citer->second.m_dwTypeID);
        _sntprintf(cBuff2, sizeof(cBuff2)/sizeof(TCHAR), _T("%d"), citer->first);
        //! 在每个item的text里存放title id

        LPCTSTR szTemp = g_StrTable[_T("RoleTitle2")];

        tagTitleList ttl;
        ttl.bOwend = false;
        ttl.dwTypeID = citer->second.m_dwTypeID;
        ttl.dwItemID = m_pTreeTitleList->InsterItem(cBuff1, cBuff2, szTemp);;
        m_mapOwnedTitles.insert(make_pair(citer->first, ttl));

        m_pTreeTitleList->SetItemColor(ttl.dwItemID, citer->second.m_dwColor);
    }
}

VOID TargetCharacterFrame::UpdateTree()
{
    map<DWORD, tagRoleTitleProto>::const_iterator tIter;

    for (map<DWORD, tagTitleList>::const_iterator citer = m_mapOwnedTitles.begin();
        citer != m_mapOwnedTitles.end(); ++citer)
    {
        tIter = RoleTitleProtoData::Inst()->GetTitleMap()->find(citer->first);
        if (tIter != RoleTitleProtoData::Inst()->GetTitleMap()->end())
        {
            LPCTSTR szTemp = NULL;
            tstring strTemp;
            if (citer->second.bOwend)
            {
                szTemp = tIter->second.m_szTitleName;
            }
            else
                szTemp = g_StrTable[_T("RoleTitle2")];

            m_pTreeTitleList->SetShowText(citer->second.dwItemID, szTemp);
        }
    }

    for (map<DWORD, tagTitleTypeActivedCount>::const_iterator itCount = m_mapActivedCount.begin();
        itCount != m_mapActivedCount.end(); ++itCount)
    {
        tstringstream sstream;
        sstream << itCount->second.strName << _T(" [") << itCount->second.bCount << _T("/") << itCount->second.nTotal << _T("]");
        m_pTreeTitleList->SetShowText(itCount->second.dwTreeItemID, sstream.str().c_str());
    }

    m_pTreeTitleList->SetRefresh();
}

VOID TargetCharacterFrame::SelectRoleTitle()
{
    DWORD pSelItemID = m_pTreeTitleList->GetCurrentSelItem();
    tagGUITreeItem* pTreeItem = m_pTreeTitleList->GetItem(pSelItemID);
    if (P_VALID(pTreeItem))
    {
        UINT16 selTID = (UINT16)_tstoi(pTreeItem->strText.c_str());
        bool bOwend = m_mapOwnedTitles[(DWORD)selTID].bOwend;

        if (true == bOwend && 2 == pTreeItem->nLayer)
        {
            map<DWORD, tagRoleTitleProto>::const_iterator citer
                = RoleTitleProtoData::Inst()->GetTitleMap()->find((DWORD)selTID);
            if (citer != RoleTitleProtoData::Inst()->GetTitleMap()->end())
            {
                m_pStcexDescription->SetText(citer->second.m_strDescription.c_str());
                m_pStcexDescription->SetRefresh();
            }
        }
        else
        {
            m_pStcexDescription->SetText(_T(""));
            m_pStcexDescription->SetRefresh();
        }
    }
}

BOOL TargetCharacterFrame::EscCancel()
{
	if(m_pWnd->IsInvisible())
		return FALSE;
	
	ShowWnd(FALSE);
	return TRUE;
}

VOID TargetCharacterFrame::ResetRemoteRoleEquip()
{
	if( !P_VALID(m_pRRSF) )
		return;

	DWORD time = timeGetTime();
	//5秒内如果查看的还是当前这个人则不向服务器发消息了
	if( m_dwCurrPlayerID==m_pRRSF->GetDisplayRemoteRoleID() && time-m_pRRSF->GetRequireTime()<Max_Require_Time )
	{
		m_pRRSF->UpdateEquipAvatar();
		m_pRRSF->UpdateAllEquip();
		return;
	}
	// 装备
	tagNC_GetRemoteRoleEquipInfo msg;
	msg.dwRoleID = m_dwCurrPlayerID;
	m_pSession->Send(&msg);
	m_pRRSF->SetDisplayRemoteRoleID(m_dwCurrPlayerID);
	m_pRRSF->SetRequireTime(time);
	m_pRRSF->UpdateEquipAvatar();
}

VOID TargetCharacterFrame::Update()
{
    if (m_pWnd->IsInvisible())
        return;

    if (m_bneedRefreshRepute && Kernel::Inst()->GetAccumTimeDW() - m_timeFirst > 50)
    {
        m_pWnd->SetRefresh(TRUE);
        m_bneedRefreshRepute = false;
    }
}