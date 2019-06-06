#include "StdAfx.h"
#include "ClanTreasureFrame.h"
#include "CombatSysUtil.h"
#include "ToolTipCreator.h"
#include "ToolTipFrame.h"
#include "ItemProtoData.h"

ClanTreasureFrame::ClanTreasureFrame(void) : m_Trunk(this), m_nSelected(GT_INVALID), m_nCurrPage(1)
{
}

ClanTreasureFrame::~ClanTreasureFrame(void)
{
}

BOOL ClanTreasureFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
    BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);

    m_pFrameMgr->RegisterEventHandle(_T("SetClanTreasureOpen"),     (FRAMEEVENTPROC)m_Trunk.sfp1(&ClanTreasureFrame::OnOpenClanTreasure));
    m_pFrameMgr->RegisterEventHandle(_T("SetClanTreasureClose"),    (FRAMEEVENTPROC)m_Trunk.sfp1(&ClanTreasureFrame::OnCloseClanTreasure));
    m_pFrameMgr->RegisterEventHandle(_T("tagNewActivedTreasure"),   (FRAMEEVENTPROC)m_Trunk.sfp1(&ClanTreasureFrame::OnNewActivedTreasure));
    m_pFrameMgr->RegisterEventHandle(_T("tagActTreasureRezult"),    (FRAMEEVENTPROC)m_Trunk.sfp1(&ClanTreasureFrame::OnActiveTreasureRet));

    return bRet;
}

BOOL ClanTreasureFrame::Destroy()
{
    BOOL bRet = GameFrame::Destroy();

    m_pFrameMgr->UnRegisterEventHandler(_T("SetClanTreasureOpen"),  (FRAMEEVENTPROC)m_Trunk.sfp1(&ClanTreasureFrame::OnOpenClanTreasure));
    m_pFrameMgr->UnRegisterEventHandler(_T("SetClanTreasureClose"), (FRAMEEVENTPROC)m_Trunk.sfp1(&ClanTreasureFrame::OnCloseClanTreasure));
    m_pFrameMgr->UnRegisterEventHandler(_T("tagNewActivedTreasure"),(FRAMEEVENTPROC)m_Trunk.sfp1(&ClanTreasureFrame::OnNewActivedTreasure));
    m_pFrameMgr->UnRegisterEventHandler(_T("tagActTreasureRezult"), (FRAMEEVENTPROC)m_Trunk.sfp1(&ClanTreasureFrame::OnActiveTreasureRet));

    /* 清除名人堂窗体 */
    if (P_VALID(m_pWnd))
    {
        m_pGUI->UnRegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&ClanTreasureFrame::EventHandler));
        m_pGUI->AddToDestroyList(m_pWnd);
        m_pWnd = NULL;
    }

    return bRet;
}

BOOL ClanTreasureFrame::ReloadUI()
{
    // 图标文件
    strSelectIcon   =   _T("data\\ui\\Common\\L_icon-l2.bmp");
    strItemIcon     =   _T("data\\ui\\sys\\l_diban-a.bmp");

    char    cbuff[128];
    TCHAR   tbuff[128];

    // 加载界面
    XmlElement ele;
    tstring strPath = g_strLocalPath + _T("\\ui\\racetreasure.xml");
    m_pGUI->LoadXml(&ele, "VFS_System", strPath.c_str());

    // 更改控件
    for (INT i = 1; i <= MAX_TREASURE_PLACE; ++i)
    {
        _snprintf(cbuff, sizeof(cbuff)/sizeof(char), "rtreasure_win\\rtreasure_backpic\\rtreasure_back\\weaponback%d", i);
        m_pGUI->ChangeXml(&ele, cbuff, "ClassName", "StaticEx");
        _snprintf(cbuff, sizeof(cbuff)/sizeof(char), "rtreasure_win\\rtreasure_backpic\\rtreasure_back\\weaponback%d\\weaponback", i);
        m_pGUI->ChangeXml(&ele, cbuff, "ClassName", "StaticEx");
        _snprintf(cbuff, sizeof(cbuff)/sizeof(char), "rtreasure_win\\rtreasure_backpic\\rtreasure_back\\weaponback%d\\weaponback\\new", i);
        m_pGUI->ChangeXml(&ele, cbuff, "ClassName", "ItemButton");
    }

    // 创建窗体
    m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &ele);

    for (INT index = 0; index < MAX_TREASURE_PLACE; ++index)
    {
        INT i = index + 1;

        // 图标底板
        _sntprintf(tbuff, sizeof(tbuff)/sizeof(TCHAR), _T("rtreasure_backpic\\rtreasure_back\\weaponback%d"), i);
        m_pStcItemPad[index]    =   (StaticEx*)m_pWnd->GetChild(tbuff);

        // 珍宝名
        _sntprintf(tbuff, sizeof(tbuff)/sizeof(TCHAR), _T("rtreasure_backpic\\rtreasure_back\\weaponback%d\\wname"), i);
        m_pStcName[index]       =   (GUIStatic*)m_pWnd->GetChild(tbuff);

        // 消耗贡献值
        _sntprintf(tbuff, sizeof(tbuff)/sizeof(TCHAR), _T("rtreasure_backpic\\rtreasure_back\\weaponback%d\\state"), i);
        m_pStcStus[index]       =   (GUIStatic*)m_pWnd->GetChild(tbuff);

        // 珍宝图标
        _sntprintf(tbuff, sizeof(tbuff)/sizeof(TCHAR), _T("rtreasure_backpic\\rtreasure_back\\weaponback%d\\weaponback"), i);
        m_pStcIcon[index]       =   (StaticEx*)m_pWnd->GetChild(tbuff);

        // 珍宝按钮
        _sntprintf(tbuff, sizeof(tbuff)/sizeof(TCHAR), _T("rtreasure_backpic\\rtreasure_back\\weaponback%d\\weaponback\\new"), i);
        m_pIbtTrea[index]       =   (ItemButton*)m_pWnd->GetChild(tbuff);
    }

    // 激活珍宝
    m_pBtActiveTreasure         =   (GUIButton*)m_pWnd->GetChild(_T("rtreasure_backpic\\activebutt"));

    // 珍宝剩余激活次数
    m_pStcRestnum               =   (GUIStatic*)m_pWnd->GetChild(_T("rtreasure_backpic\\times"));

    // 页数
    m_pStcPage                  =   (GUIStatic*)m_pWnd->GetChild(_T("rtreasure_backpic\\new00"));
    // 前一页
    m_pBtPrevPage               =   (GUIButton*)m_pWnd->GetChild(_T("rtreasure_backpic\\new"));
    // 下一页
    m_pBtNextPage               =   (GUIButton*)m_pWnd->GetChild(_T("rtreasure_backpic\\new0"));

    // 关闭按钮
    m_pBtClose                  =   (GUIButton*)m_pWnd->GetChild(_T("rtreasure_backpic\\closebutt"));



    // 注册事件处理函数
    m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&ClanTreasureFrame::EventHandler));

    // 默认关闭界面
    m_pWnd->SetInvisible(TRUE);

    // 默认激活按钮不可用
    m_pBtActiveTreasure->SetEnable(false);

    // 默认可激活次数为0
    _sntprintf(tbuff, sizeof(tbuff)/sizeof(TCHAR), _T("%s0"), g_StrTable[_T("ClanTreaActCount")]);
    m_pStcRestnum->SetText(tbuff);

    return TRUE;
}

VOID ClanTreasureFrame::Update()
{

}

DWORD ClanTreasureFrame::OnGameEvent( tagGameEvent* pGameEvent )
{
	return 0;
}

DWORD ClanTreasureFrame::EventHandler( tagGUIEvent* pEvent )
{
    GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);

    switch (pEvent->eEvent)
    {
    case EGUIE_Click:
        if (pWnd == m_pBtClose)
        {
            // 关闭界面
            ShowWnd(false);
        }
        else if (pWnd == m_pBtNextPage)
        {
            // 下一页
        }
        else if (pWnd == m_pBtPrevPage)
        {
            // 上一页
        }
        else if (pWnd == m_pBtActiveTreasure)
        {
            // 激活珍宝
            BeginActive();
        }
    case EGUIE_Drag:
        // ItemButton 左键单击
        for (INT i = 0; i < MAX_TREASURE_PLACE; ++i)
            if (pWnd == m_pIbtTrea[i])
                SelectItemPad(i);
        break;
    case EGUIE_MoveIntoCtrl:
        // 显示tips
        for (INT i = 0; i < MAX_TREASURE_PLACE; ++i)
            if (pWnd == m_pIbtTrea[i])
            {
                const tagTreasureMgrData* pTrea = GetTreaByPos(m_nCurrPage, i);
                if (P_VALID(pTrea))
                {
                    const tagItemProto* pItem = ItemProtoData::Inst()->FindItemProto(pTrea->dwItemID);
                    BYTE quality = P_VALID(pItem) ? pItem->byQuality : 0;

                    if (pTrea->bActived)
                    {
                        ShowTip(pWnd, ToolTipCreator::Inst()->GetItemTips(pTrea->dwItemID, quality, pTrea->dwNameID).c_str());
                    }
                    else
                    {
                        ShowTip(pWnd, ToolTipCreator::Inst()->GetItemTips(pTrea->dwItemID, quality).c_str());
                    }
                }
            }
        break;
    case EGUIE_MoveOutOfCtrl:
        // 关闭tips
        ShowTip(pWnd, NULL);
        break;
    }

	return 0;
}

DWORD ClanTreasureFrame::OnOpenClanTreasure( tagOpenClanTreasure* pGameEvent )
{
    m_eClan = pGameEvent->eClanTypeValue;
    ShowWnd(true);

	return 0;
}

DWORD ClanTreasureFrame::OnCloseClanTreasure( tagGameEvent* pGameEvent )
{
    ShowWnd(false);

	return 0;
}

VOID ClanTreasureFrame::ShowWnd( bool bval )
{
    m_pWnd->SetInvisible(!bval);

    if (bval)
    {
        m_pWnd->FlipToTop();

        m_pFrameMgr->SendEvent(&tagGameEvent(_T("ClanTreaOpen"), this));

        // 更新
        UpdateClanTreasure();
    }
    else
    {
        m_pFrameMgr->SendEvent(&tagGameEvent(_T("ClanTreaClose"), this));
    }
}

VOID ClanTreasureFrame::SelectItemPad( INT nIndex )
{
    if (nIndex >= 0 && nIndex < MAX_TREASURE_PLACE)
    {
        if (m_nSelected >= 0 && m_nSelected < MAX_TREASURE_PLACE)
        {
            // 清除当前选择的
            m_pStcItemPad[m_nSelected]->SetPicFile(strItemIcon);
        }
        // 设置新选择的
        m_pStcItemPad[nIndex]->SetPicFile(strSelectIcon);
        m_nSelected = nIndex;

        // 设置激活按钮
        const tagTreasureMgrData* pTrea = GetTreaByPos(m_nCurrPage, m_nSelected);
        if (P_VALID(pTrea) && !pTrea->bActived)
            m_pBtActiveTreasure->SetEnable(true);
        else
            m_pBtActiveTreasure->SetEnable(false);
    }
    else if (nIndex == GT_INVALID)
    {
        //m_pStcItemPad[m_nSelected]->SetPicFile(strItemIcon);
        m_nSelected =   GT_INVALID;
    }
}

DWORD ClanTreasureFrame::OnNewActivedTreasure( tagNewActivedTreasure* pGameEvent )
{
    UpdateClanTreasure();

	return 0;
}

DWORD ClanTreasureFrame::OnActiveTreasureRet( tagActTreasureRezult* pGameEvent )
{
    // 激活氏族珍宝后的错误处理
    switch (pGameEvent->dwErrCode)
    {
    case E_FrameHall_Success:
        // 成功
        CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("ClanTreaTip0")]);
    	break;
    case E_FrameHall_ItemIsNotTreasure:
        // 物品不是珍宝
        CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("ClanTreaTip1")]);
        break;
    case E_FrameHall_TreasureAlreadyAct:
        // 珍宝已激活
        CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("ClanTreaTip2")]);
        break;
    case E_FrameHall_ActCountNotEnough:
        // 激活次数不够
        CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("ClanTreaTip3")]);
        break;
    case E_FrameHall_ClanColNotEnough:
        // 氏族贡献不够
        CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("ClanTreaTip4")]);
        break;
    case E_FrameHall_UnknownError:
        // 未知错误
        CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("ClanTreaTip5")]);
        break;
    }

	return 0;
}

VOID ClanTreasureFrame::UpdateClanTreasure()
{
    InitTreasure();
    RefreshToUI();
}

VOID ClanTreasureFrame::InitTreasure()
{
    typedef TreasureMgr::_MapTreasure::const_iterator CIT;
    const TreasureMgr::_MapTreasure actived = TreasureMgr::Inst()->GetAllActived(m_eClan);
    const TreasureMgr::_MapTreasure unactived = TreasureMgr::Inst()->GetAllUnActived(m_eClan);

    // 将全部数据转入本地m_vecTreasure
    m_vecTreasure.clear();

    // 已激活的珍宝在先
    for (CIT cit = actived.begin(); cit != actived.end(); ++cit)
        m_vecTreasure.push_back(cit->second);

    // 未激活珍宝在后
    for (CIT cit = unactived.begin(); cit != unactived.end(); ++cit)
        m_vecTreasure.push_back(cit->second);

    // 计算页码数
    const INT TreaCount = m_vecTreasure.size();
    if (0 == TreaCount)
    {
        m_nPageCount    =   1;
    }
    else
    {
        // 处理整页和零页
        const INT res   = (INT)(TreaCount / MAX_TREASURE_PLACE);
        const INT rest  = (INT)(TreaCount % MAX_TREASURE_PLACE);
        m_nPageCount = (0 == rest) ? res : res + 1;
    }

    // 选中第一页
    m_nCurrPage         =   1;

    // 激活钮不可用
    m_pBtActiveTreasure->SetEnable(false);
}

VOID ClanTreasureFrame::RefreshToUI()
{
    // 清空界面
    for (INT i = 0; i != MAX_TREASURE_PLACE; ++i)
    {
        m_pIbtTrea[i]->RefreshItem();
        m_pStcItemPad[i]->SetPicFile(strItemIcon);
        m_pStcName[i]->SetText(_T(""));
        m_pStcStus[i]->SetText(_T(""));
    }

    SelectItemPad();

    // 刷新页码
    TCHAR tcbuff[16];
    _itot(m_nCurrPage, tcbuff, 10);
    m_pStcPage->SetText(tcbuff);

    // 无列表则返回
    if (m_vecTreasure.empty())
        return;

    // 跟据当前页计算列表显示范围
    typedef vector<tagTreasureMgrData*>::const_iterator CIT;
    INT treaCount   =   m_vecTreasure.size();

    INT nBegin      =   (m_nCurrPage - 1) * MAX_TREASURE_PLACE;
    INT nEnd        =   nBegin + MAX_TREASURE_PLACE;
    if (nEnd > treaCount)
        nEnd        =   treaCount;

    CIT citBegin    =   m_vecTreasure.begin() + nBegin;
    CIT citEnd      =   m_vecTreasure.begin() + nEnd;

    // 显示到控件
    INT index   =   0;
    for (CIT cit = citBegin; cit != citEnd; ++cit)
    {
        // 将珍宝填充到按钮
        m_pIbtTrea[index]->RefreshItem((*cit)->dwItemID);

        // 显示名字
        tstring strName = (*cit)->strNamePrefix + (*cit)->strNameSuffix;
        m_pStcName[index]->SetText(strName.c_str());

        // 显示激活状态/贡献值
        TCHAR tbuff[128];
        if ((*cit)->bActived)
            _tcscpy_s(tbuff, sizeof(tbuff)/sizeof(TCHAR), g_StrTable[_T("ClanTreaStus1")]);
        else
            _sntprintf(tbuff, sizeof(tbuff)/sizeof(TCHAR), _T("%s %d"), g_StrTable[_T("ClanTreaStus2")], (*cit)->nConNeed);
        m_pStcStus[index]->SetText(tbuff);
        
        // 下一个控件
        ++index;
    }

    // 显示剩余激活次数
    RefreshActCntToUI();
}

VOID ClanTreasureFrame::RefreshActCntToUI()
{
    TCHAR tbuff[64];
    _sntprintf(tbuff, sizeof(tbuff)/sizeof(TCHAR), _T("%s%d"), g_StrTable[_T("ClanTreaActCount")], ReputeMgr::Inst()->GetActCount(m_eClan));
    m_pStcRestnum->SetText(tbuff);
}

VOID ClanTreasureFrame::PagePrev()
{
    if (m_nCurrPage > 1)
    {
        // 页面减一
        --m_nCurrPage;

        // 刷新界面
        RefreshToUI();
    }
}

VOID ClanTreasureFrame::PageNext()
{
    if (m_nCurrPage < m_nPageCount)
    {
        // 页面加一
        ++m_nCurrPage;

        // 刷新界面
        RefreshToUI();
    }
}

VOID ClanTreasureFrame::BeginActive()
{
    // 无选择则返回
    if (GT_INVALID == m_nSelected)
        return;

    // 找到当前选择的itemid
    const tagTreasureMgrData* currdata = GetTreaByPos(m_nCurrPage, m_nSelected);
    if (P_VALID(currdata))
    {
        // 激活,反馈信息在OnActiveTreasureRet中处理
        TreasureMgr::Inst()->ActiveTreasure(currdata->dwTreasureID);
    }
}

const tagTreasureMgrData* ClanTreasureFrame::GetTreaByPos( INT currPage, INT pos )
{
    // 判断pos是否有效
    if (pos < 0 || pos >= MAX_TREASURE_PLACE)
        return NULL;

    // 找到当前选择的itemid
    INT treaCount   =   m_vecTreasure.size();
    INT index = (currPage - 1) * MAX_TREASURE_PLACE + pos;
    if (index >= treaCount)
        index = treaCount;

    typedef vector<tagTreasureMgrData*>::const_iterator CIT;
    CIT citSelect   =   m_vecTreasure.begin() + index;

    // 判断此id是否有效
    if (citSelect < m_vecTreasure.begin() || citSelect >= m_vecTreasure.end())
        return NULL;
    else
        return *citSelect;
}
