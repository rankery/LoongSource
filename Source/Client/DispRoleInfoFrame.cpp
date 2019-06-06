#include "StdAfx.h"
#include "DispRoleInfoFrame.h"
#include "PlayerNameTab.h"
#include "CombatSysUtil.h"

DispRoleInfoFrame::DispRoleInfoFrame(void) : m_Trunk(this) , m_bisRefreshing(false)
{
}

DispRoleInfoFrame::~DispRoleInfoFrame(void)
{
}

BOOL DispRoleInfoFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);

    //注册网络消处理函数
    m_pCmdMgr->Register("NS_GetRoleVCard", (NETMSGPROC)m_Trunk.sfp2(&DispRoleInfoFrame::OnNS_GetRoleVCard), _T("NS_GetRoleVCard"));

    //注册游戏事件处理函数
	m_pMgr->RegisterEventHandle(_T("Open_RoleInfo"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&DispRoleInfoFrame::OnOpenInfo));
	m_pMgr->RegisterEventHandle(_T("Close_RoleInfo"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&DispRoleInfoFrame::OnCloseInfo));
    m_pMgr->RegisterEventHandle(_T("tagRoleGetNameEvent"),    (FRAMEEVENTPROC)m_Trunk.sfp1(&DispRoleInfoFrame::OnRoleGetNameEvent));

	return bRet;
}

BOOL DispRoleInfoFrame::Destroy()
{
	BOOL bRet = GameFrame::Destroy();

    //注册网络消处处理函数
    m_pCmdMgr->UnRegister("NS_GetRoleVCard", (NETMSGPROC)m_Trunk.sfp2(&DispRoleInfoFrame::OnNS_GetRoleVCard));

    //注销游戏事件处理函数
    m_pMgr->UnRegisterEventHandler(_T("Open_RoleInfo"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&DispRoleInfoFrame::OnOpenInfo));
    m_pMgr->UnRegisterEventHandler(_T("Close_RoleInfo"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&DispRoleInfoFrame::OnCloseInfo));
    m_pMgr->UnRegisterEventHandler(_T("tagRoleGetNameEvent"),    (FRAMEEVENTPROC)m_Trunk.sfp1(&DispRoleInfoFrame::OnRoleGetNameEvent));

	if( P_VALID(m_pWnd) )
	{
		m_pGUI->AddToDestroyList(m_pWnd);
		m_pWnd = NULL;
	}

	return bRet;
}

BOOL DispRoleInfoFrame::ReloadUI()
{
	// 创建
	XmlElement ele;
	tstring strPath = g_strLocalPath + _T("\\ui\\information2.xml");
	m_pGUI->LoadXml(&ele, "VFS_System", strPath.c_str());
    m_pGUI->ChangeXml(&ele, "informationwin2\\informpic\\informback\\information\\headpicback\\headpic", "ClassName", "HttpPicEx");

	m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &ele);

    m_pStcName = (GUIStatic*)m_pWnd->GetChild(_T("informpic\\informback\\name"));      // 名字
	m_pStcSpouse = (GUIStatic*)m_pWnd->GetChild(_T("informpic\\informback\\information\\spouse"));			//配偶
	m_pStcGender = (GUIStatic*)m_pWnd->GetChild(_T("informpic\\informback\\information\\gender"));			//性别
	m_pStcConstellation = (GUIStatic*)m_pWnd->GetChild(_T("informpic\\informback\\information\\constellation"));	//星座
	m_pStcCity = (GUIStatic*)m_pWnd->GetChild(_T("informpic\\informback\\information\\city"));				//城市
	m_pStcChZodiac = (GUIStatic*)m_pWnd->GetChild(_T("informpic\\informback\\information\\Zodiac"));			//生肖
	m_pEditBoxExPresent = (GUIEditBoxEx*)m_pWnd->GetChild(_T("informpic\\informback\\information\\presentationpic\\presentation"));	//个性签名
	m_pEditNameHistory = (GUIEditBoxEx*)m_pWnd->GetChild(_T("informpic\\informback\\information\\presentationpic2\\presentation"));	//更名记录
	m_pStcHeadPic = (HttpPicEx*)m_pWnd->GetChild(_T("informpic\\informback\\information\\headpicback\\headpic"));			//头像

	m_pBtRefresh = (GUIButton*)m_pWnd->GetChild(_T("informpic\\informback\\refresh"));			//刷新
	m_pBtClose = (GUIButton*)m_pWnd->GetChild(_T("informpic\\informback\\close"));				//关闭

	m_pBtQuit = (GUIButton*)m_pWnd->GetChild(_T("informpic\\closebutt"));				//关闭窗口


	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(),
		m_Trunk.sfp1(&DispRoleInfoFrame::EventHandler));

    // 初始化窗口关闭
	ShowWnd(FALSE);

	return TRUE;
}

VOID DispRoleInfoFrame::Update()
{
    /*! 计算累积时间，让玩家点击刷新时有所延迟 */
    static DWORD dwAccumTime = 0;
    if (m_bisRefreshing)
    {
        dwAccumTime += Kernel::Inst()->GetDeltaTimeDW();
        if (dwAccumTime >= TITLE_REFRESH_DELAY)
        {
            dwAccumTime = 0;
            m_bisRefreshing = false;
            m_pBtRefresh->SetEnable(true);
            m_pBtRefresh->SetRefresh();
        }
    }
}

VOID DispRoleInfoFrame::ShowWnd( BOOL bVal )
{
	m_pWnd->SetInvisible(!bVal);

	if (bVal)	//打开窗口
	{
        m_pWnd->FlipToTop();
	}

}

DWORD DispRoleInfoFrame::EventHandler( tagGUIEvent* pEvent )
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);

	switch (pEvent->eEvent)
	{
	case EGUIE_Click:
		if (pWnd == m_pBtRefresh)	//刷新
		{
            SendGetRoleVCard();
            m_bisRefreshing = true;
            m_pBtRefresh->SetEnable(false);
            m_pBtRefresh->SetRefresh();
		}
		else if (pWnd == m_pBtClose || pWnd == m_pBtQuit)	//关闭
		{
            ShowWnd(FALSE);
		}
		break;
	}
	return 0;
}

DWORD DispRoleInfoFrame::OnOpenInfo( tagOpenRoleInfoEvent* pGameEvent )
{
    m_dwCurrRoleID = pGameEvent->dwRoleID;

    SendGetRoleVCard();


	return 0;
}

DWORD DispRoleInfoFrame::OnCloseInfo( tagGameEvent* pGameEvent )
{
	ShowWnd(FALSE);

	return 0;
}

DWORD DispRoleInfoFrame::OnNS_GetRoleVCard( tagNS_GetRoleVCard* pMsg, DWORD dwParam )
{
    if (pMsg->dwErrorCode != E_Success)
    {
        if (pMsg->dwErrorCode == E_VCard_NotVisible)
        {
            TCHAR ch[100] = {0};
            _sntprintf(ch, sizeof(ch) / sizeof(TCHAR),
                g_StrTable[_T("InformationInvisable")],
                PlayerNameTab::Inst()->FindNameByID(pMsg->dwRoleID));
            CombatSysUtil::Inst()->ShowScreenCenterMsg(ch);
        }

        return 0;
    }

    if (pMsg->dwRoleID != m_dwCurrRoleID)
        return 0;

    //获得已设置数据
    tagVCardData* pVCardData = (tagVCardData*)pMsg->pData;

    map<DWORD, INT>::iterator it;

    //配偶
    DWORD m_dwCurrMateID = pVCardData->dwMateRoleID;
    if(m_dwCurrMateID)    //有配偶
    {
        LPCTSTR szMateName = PlayerNameTab::Inst()->FindNameByID(m_dwCurrMateID);
        if(szMateName != NULL&&tstring(_T("GFC_NULL")) != szMateName)   //配偶名有效并且在缓存中
        {
            m_pStcSpouse->SetText(szMateName);
        }
        else if(szMateName == NULL)
        {
            m_pStcSpouse->SetText(_T(""));
        }
    }

    //性别
    LPCTSTR szSexText;
    switch (pVCardData->customVCardData.bySex)
    {
    case 1:
        szSexText = g_StrTable[_T("RoleInfoMale")];
    	break;
    case 2:
        szSexText = g_StrTable[_T("RoleInfoFemale")];
        break;
    case 3:
        szSexText = g_StrTable[_T("RoleInfoDefault")];
        break;
    default:
        szSexText = _T("");
        break;
    }
    m_pStcGender->SetText(szSexText);

    //星座
    const map<DWORD, tagConstellationProto>* pCstMap = RoleInfoProtoData::inst()->GetConstellation();
    map<DWORD, tagConstellationProto>::const_iterator itCst = pCstMap->find((DWORD)pVCardData->customVCardData.byConstellation);
    if (itCst != pCstMap->end())
    {
        m_pStcConstellation->SetText(itCst->second.strName.c_str());
    }
    else
    {
        m_pStcConstellation->SetText(_T(""));
    }
    

    //生肖
    const map<DWORD, tagChinesZodiacProto>* pCzcMap = RoleInfoProtoData::inst()->GetChineseZodiac();
    map<DWORD, tagChinesZodiacProto>::const_iterator itCzc = pCzcMap->find((DWORD)pVCardData->customVCardData.byChineseZodiac);
    if (itCzc != pCzcMap->end())
    {
        m_pStcChZodiac->SetText(itCzc->second.strName.c_str());
    }
    else
    {
        m_pStcChZodiac->SetText(_T(""));
    }

    // 区域，省份，城市
    tstring strCity = _T("");

    const map<DWORD, tagAreaProto>* pAreaMap = RoleInfoProtoData::inst()->GetArea();
    map<DWORD, tagAreaProto>::const_iterator itArea = pAreaMap->find((DWORD)pVCardData->customVCardData.byArea);
    if (itArea != pAreaMap->end())
    {
       strCity += itArea->second.strName;

       // 省份
       map<DWORD, tagProvinceProto>::const_iterator itProvince = itArea->second.mapProvince.find((DWORD)pVCardData->customVCardData.byProvince);
       if (itProvince != itArea->second.mapProvince.end())
       {
            strCity += _T(" ");
            strCity += itProvince->second.strName;
       }
    }

    // 城市
    strCity += _T(" ");
    strCity += pVCardData->customVCardData.chCity;
    m_pStcCity->SetText(strCity.c_str());

    //头像URL
    TCHAR szImageURL[200] = {_T('\0')};
    memcpy(szImageURL, pVCardData->customVCardData.byData, (INT)pVCardData->customVCardData.byHeadUrlSize);
    tstring strImageURL = szImageURL;
    m_pStcHeadPic->SetPicURL(strImageURL);

    if (pVCardData->customVCardData.bySex == 1)
        m_pStcHeadPic->SetDefaultPic(MALE_PIC_PATH);
    else if (pVCardData->customVCardData.bySex == 2)
        m_pStcHeadPic->SetDefaultPic(FEMALE_PIC_PATH);
    else
        m_pStcHeadPic->SetDefaultPic(_T(""));

    m_pStcHeadPic->SetInvisible(FALSE);
    m_pStcHeadPic->SetRefresh(TRUE);

    // 个性签名
    TCHAR szbuff[200] = {_T('\0')};
    INT nOffset = (INT)pVCardData->customVCardData.byHeadUrlSize;
    INT nSize = (INT)pVCardData->customVCardData.bySignatureSize;
    memcpy(szbuff, pVCardData->customVCardData.byData + nOffset, nSize);
    m_pEditBoxExPresent->SetText(szbuff);

	// 更名记录
	TCHAR szNameHistory[X_HUGE_NAME] = {_T('\0')};
	nOffset += nSize;
	nSize = (INT)pVCardData->customVCardData.byNameHistory;
	memcpy(szNameHistory, pVCardData->customVCardData.byData + nOffset, nSize);
	m_pEditNameHistory->SetText(szNameHistory);

    ShowWnd(TRUE);

	return 0;
}
VOID DispRoleInfoFrame::ClearAll()
{
    m_pStcName->SetText(_T(""));
    m_pStcSpouse->SetText(_T(""));
    m_pStcGender->SetText(_T(""));
    m_pStcConstellation->SetText(_T(""));
    m_pStcChZodiac->SetText(_T(""));
    m_pStcCity->SetText(_T(""));
    m_pEditBoxExPresent->SetText(_T(""));
	m_pEditNameHistory->SetText(_T(""));
}

VOID DispRoleInfoFrame::SendGetRoleVCard()
{
    //清除
    ClearAll();

    //请求获得远程玩家名贴数据
    tagNC_GetRoleVCard msg;
    msg.dwRoleID = m_dwCurrRoleID;
    m_pSession->Send(&msg);

    //请求获得远程玩家姓名
    tstring strRoleName = PlayerNameTab::Inst()->FindNameByID(m_dwCurrRoleID);
    if (strRoleName != _T(""))
        m_pStcName->SetText(strRoleName.c_str());
}

DWORD DispRoleInfoFrame::OnRoleGetNameEvent( tagRoleGetNameEvent* pGameEvent )
{
    if (pGameEvent->dwRoleID == m_dwCurrRoleID)
        m_pStcName->SetText(pGameEvent->strRoleName.c_str());
    if(pGameEvent->dwRoleID == m_dwCurrMateID)
        m_pStcSpouse->SetText(pGameEvent->strRoleName.c_str());
	return 0;
}