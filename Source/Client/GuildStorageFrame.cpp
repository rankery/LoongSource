#include "StdAfx.h"
#include "GuildStorageFrame.h"
#include "ItemButton.h"
#include "ItemMgr.h"
#include "Container.h"
#include "DragBtn.h"
#include "ItemEvent.h"
#include "GuildMgr.h"
#include "QuestMgr.h"
#include "ItemFrame.h"
#include "ToolTipCreator.h"
#include "ToolTipFrame.h"
#include "CombatSysUtil.h"
#include "RoleMgr.h"
#include "..\WorldDefine\shop_define.h"
#include "..\WorldDefine\msg_item.h"

const INT GuildStore_MAX_COL = 7;
const INT GuildStore_MAX_ROW = 5;
const SIZE GuildStore_UINT_SIZE = {36,36};
const SIZE GuildStore_SPACE_SIZE = {9,7};

GuildStorageFrame::GuildStorageFrame(void) :
    m_Trunk(this),
    m_n16CurPage(0),
    m_pWnd(NULL),
    m_dwNPCID(GT_INVALID)
{
    m_updateTimeFirst   =   Kernel::Inst()->GetAccumTimeDW();
    m_updateTimeDistance=   Kernel::Inst()->GetAccumTimeDW();
}

GuildStorageFrame::~GuildStorageFrame(void)
{
}

BOOL GuildStorageFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
    BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);

    // register game event handler
    m_pFrameMgr->RegisterEventHandle(_T("ItemEvent_UpdateItem"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&GuildStorageFrame::OnEventUpdateLPItem));
    m_pFrameMgr->RegisterEventHandle(_T("InitGuildWareEvent"),	    (FRAMEEVENTPROC)m_Trunk.sfp1(&GuildStorageFrame::OnInitGuildWareEvent));
    m_pFrameMgr->RegisterEventHandle(_T("Close_GuildWare"),	        (FRAMEEVENTPROC)m_Trunk.sfp1(&GuildStorageFrame::OnEventCloseGuildWare));
    m_pFrameMgr->RegisterEventHandle(_T("ItemEvent_Move2Guild"),    (FRAMEEVENTPROC)m_Trunk.sfp1(&GuildStorageFrame::OnEventItemPosChange));

    return bRet;
}

BOOL GuildStorageFrame::Destroy()
{
    BOOL bRet = GameFrame::Destroy();

    // destroy window
    if( P_VALID(m_pWnd) )
    {
        m_pGUI->AddToDestroyList(m_pWnd);
        m_pWnd = NULL;
    }

    // unregister game event handler
    m_pFrameMgr->UnRegisterEventHandler(_T("ItemEvent_UpdateItem"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&GuildStorageFrame::OnEventUpdateLPItem));
    m_pFrameMgr->UnRegisterEventHandler(_T("InitGuildWareEvent"),   (FRAMEEVENTPROC)m_Trunk.sfp1(&GuildStorageFrame::OnInitGuildWareEvent));
    m_pFrameMgr->UnRegisterEventHandler(_T("Close_GuildWare"),      (FRAMEEVENTPROC)m_Trunk.sfp1(&GuildStorageFrame::OnEventCloseGuildWare));
    m_pFrameMgr->UnRegisterEventHandler(_T("ItemEvent_Move2Guild"), (FRAMEEVENTPROC)m_Trunk.sfp1(&GuildStorageFrame::OnEventItemPosChange));

    return bRet;
}

BOOL GuildStorageFrame::ReloadUI()
{
    // load
    XmlElement ele;
    tstring strPath = g_strLocalPath + _T("\\ui\\guild_warehouse.xml");
    m_pGUI->LoadXml(&ele, "VFS_System", strPath.c_str());

    // create
    m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &ele);
    GUIStatic* pStcStore = (GUIStatic*)m_pWnd->GetChild(_T("guildwarehouseback_pic\\whback_pic\\itemback_pic"));
    m_pBtClose = (GUIButton*)m_pWnd->GetChild(_T("guildwarehouseback_pic\\closebutt"));
    for (int i = 0; i < MAX_GUILDWARE_NUM; ++i)
    {
        tstringstream tsstream;
        tsstream << _T("guildwarehouseback_pic\\fra\\whbutt") << i + 1;
        m_pPbt[i] = (GUIPushButton*)m_pWnd->GetChild(tsstream.str().c_str());
    }

    // create item unit
    for(int i = 0; i < GuildStore_MAX_COL; ++i)
    {
        for (int j = 0; j < GuildStore_MAX_ROW; ++j)
        {
            XmlElement ele;
            stringstream sstream;
            sstream << "unititem" << GuildStore_MAX_COL * j + i;
            ele.SetAttribute("Name", sstream.str().c_str());
            ele.SetAttribute("ClassName", "ItemButton");
            ele.SetAttribute("Size_x", GuildStore_UINT_SIZE.cx);
            ele.SetAttribute("Size_y", GuildStore_UINT_SIZE.cy);
            ele.SetAttribute("LogicPos_x", 4 + GuildStore_SPACE_SIZE.cx*i + GuildStore_UINT_SIZE.cx*i);
            ele.SetAttribute("LogicPos_y", 4 + GuildStore_SPACE_SIZE.cy*j + GuildStore_UINT_SIZE.cy*j);
            ele.SetAttribute("Font", m_pUtil->UnicodeToUnicode8(g_StrTable[_T("Font_MSYH")]));
            //ele.SetAttribute("FontWidth", 8);
            //ele.SetAttribute("FontHeight", 16);
            ele.SetAttribute("TextRect_left", 0);
            ele.SetAttribute("TextRect_top", 0);
            ele.SetAttribute("TextRect_right", GuildStore_UINT_SIZE.cx);
            ele.SetAttribute("TextRect_bottom", GuildStore_UINT_SIZE.cy);
            ele.SetAttribute("TextColor", 0xFFFFFFFF);
            ele.SetAttribute("TextAlign", "RightBottom");
            m_pUnitItem[GuildStore_MAX_COL * j + i] = (ItemButton*)m_pGUI->CreateWnd(pStcStore->GetFullName().c_str(), &ele);
        }
    }

    // register ui event handler
    m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&GuildStorageFrame::EventHandler));

    // open the window
    ShowWnd(FALSE);
    m_pPbt[0]->SetState(EGUIBS_PushDown);

    return TRUE;
}

VOID GuildStorageFrame::Update()
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

    // 每3秒更新下仓库
    if (Kernel::Inst()->GetAccumTimeDW() - m_updateTimeFirst > 3000)
    {
        GuildMgr::Inst()->BeginNC_GetGuildWare(m_dwNPCID);


        m_updateTimeFirst = Kernel::Inst()->GetAccumTimeDW();
    }

    GameFrame::Update();
}

BOOL GuildStorageFrame::EscCancel()
{
    if( m_pWnd->IsInvisible() )
        return FALSE;

    ShowWnd(FALSE);
    return TRUE;
}

VOID GuildStorageFrame::EventHandler( tagGUIEvent* pEvent )
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

            for (INT i = 0; i < SPACE_ONE_GUILDWARE; ++i)
            {
                if (pWnd == (GUIWnd*)m_pUnitItem[i])
                {
                    if (GuildMgr::Inst()->isWareOpretable())
                    {
                        OnItemChangeEx(EICT_GuildWare, EICT_Bag, (INT16)(m_n16CurPage * SPACE_ONE_GUILDWARE + i));
                    }
                    else
                    {
                        CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("GuildPrivacyUseErr")]);
                    }
                    break;
                }
            }
        }
        break;
    case EGUIE_PushButtonDown:
        {
            if (pEvent->dwParam1 != 1)//不是按下不处理
                return;

            // press switch page button
            for (INT i = 0; i < MAX_GUILDWARE_NUM; ++i)
            {
                if(pWnd == m_pPbt[i])
                {
                    ShowCurPage(i);
                    return;
                }
            }
        }
        break;
    case EGUIE_Drag:
        {
            for (INT i = 0; i < SPACE_ONE_GUILDWARE; ++i)
            {
                if (pWnd == (GUIWnd*)m_pUnitItem[i])
                {
                    if (GuildMgr::Inst()->isWareOpretable())
                    {
                        Item* pItem = GetItem((INT16)(m_n16CurPage * SPACE_ONE_GUILDWARE + i));
                        if(P_VALID(pItem))
                        {
                            DragBtn* pDrag = (DragBtn*)m_pGUI->GetWnd(_T("\\desktop\\DragButton"));
                            if(P_VALID(pDrag))
                            {
                                pEvent->dwParam1 = EDT_GuildWare;
                                pEvent->dwParam2 = MAKELONG(pItem->GetPos(), pItem->GetItemQuantity());
                                pEvent->dwParam3 = pItem->GetItemTypeID();
                                pDrag->OnEvent(pEvent);
                            }
                        }
                    }
                    else
                    {
                        CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("GuildPrivacyUseErr")]);
                    }

                    break;
                }
            }
        }
        break;
    case EGUISCB_Drop:
        {

            for (INT i = 0; i < SPACE_ONE_GUILDWARE; ++i)
            {
                if (pWnd == (GUIWnd*)m_pUnitItem[i])
                {
                    if (GuildMgr::Inst()->isWareOpretable())
                    {
                        if(EDT_GuildWare == (EDragType)pEvent->dwParam1)
                        {
                            OnItemChange((INT16)LOWORD(pEvent->dwParam2), (INT16)(m_n16CurPage*SPACE_ONE_WARE + i));
                        }
                        else if(EDT_Item == (EDragType)pEvent->dwParam1)
                        {
                            // 判断是否绑定
                            Item* pItem = ItemMgr::Inst()->GetConItem(EICT_Bag, (INT16)LOWORD(pEvent->dwParam2));
                            if (P_VALID(pItem) && !pItem->IsItemBinded())
                            {
                                OnItemChangeEx(EICT_Bag, EICT_GuildWare, (INT16)LOWORD(pEvent->dwParam2), (INT16)(m_n16CurPage*SPACE_ONE_WARE + i));
                            }
                            else
                            {
                                CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("GuildWareBinding")]);
                            }
                        }
                        break;
                    }
                    else
                    {
                        CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("GuildPrivacyUseErr")]);
                    }

                }
            }
        }
        break;
    case EGUIE_MoveIntoCtrl:
        {
            for(INT i = 0; i < SPACE_ONE_GUILDWARE; ++i)
            {
                if(pWnd == (GUIWnd*)m_pUnitItem[i])
                {
                    Item* pItem = GetItem((INT16)(m_n16CurPage*SPACE_ONE_GUILDWARE + i));
                    if(P_VALID(pItem))
                    {
                        tstring strPrice = _T("");
                        if(EIST_ItemShopVend == ((ItemFrame*)m_pFather)->GetCurStrategy())
                        {
                            tagItemCost cost(pItem->IsVendible() ? pItem->GetItemPrice() : GT_INVALID, ECCT_Null, 0, 0, GT_INVALID);
                            strPrice = ToolTipCreator::Inst()->GetPriceTips(&cost);
                        }
                        ShowTip(pWnd, ToolTipCreator::Inst()->GetItemTips(pItem).c_str(), strPrice.c_str());

                        // 如果是装备显示装备比较
                        if(MIsEquipment(pItem->GetItemTypeID()))
                        {
                            ToolTipCreator::Inst()->ShowEquipCompare(pItem->GetItemTypeID());
                        }
                    }
                    break;
                }
            }
        }
        break;
    case EGUIE_MoveOutOfCtrl:
        {
            ShowTip(pWnd, NULL);
        }
        break;
    }
}

VOID GuildStorageFrame::ShowWnd( BOOL bVal )
{
    ItemFrame* pFrame = (ItemFrame*)m_pFrameMgr->GetFrame(_T("Item"));

    m_pWnd->SetInvisible(!bVal);
    if (bVal)
    {
        m_pWnd->FlipToTop();
        ShowCurPage(m_n16CurPage);

        if(P_VALID(pFrame) /*&& GuildMgr::Inst()->isWareOpretable()*/)
            pFrame->SetCurStrategy(EIST_GuildStorage);
        else
            pFrame->SetCurStrategy(EIST_ItemPocket);
    }
    else
    {
        if(P_VALID(pFrame))
            pFrame->SetCurStrategy(EIST_ItemPocket);
    }
}

Item* GuildStorageFrame::GetItem( INT16 index )
{
    return ItemMgr::Inst()->GetGuildStore()->GetValue(index);
}

VOID GuildStorageFrame::RefreshItem( INT16 index )
{
    DWORD dwTypeID = GT_INVALID;
    INT16 n16Quantity = 0;
    BYTE byQuality = 0;

    INT nMaxSize = ItemMgr::Inst()->GetGuildStore()->MaxSize();
    if(index >= nMaxSize && index < SPACE_ALL_GUILDWARE)
    {
        dwTypeID = 0;
    }
    else
    {
        Item* pItem = GetItem(index);
        if (P_VALID(pItem))
        {
            dwTypeID = pItem->GetItemTypeID();
            n16Quantity = pItem->GetItemQuantity();
            byQuality = pItem->GetItemQuality();
        }
    }

    m_pUnitItem[index % SPACE_ONE_GUILDWARE]->RefreshItem(dwTypeID, n16Quantity, byQuality);
}

VOID GuildStorageFrame::ShowCurPage( INT16 page )
{
    m_n16CurPage = page;
    for (INT i = 0; i < SPACE_ONE_GUILDWARE; ++i)
    {
        RefreshItem(page * SPACE_ONE_GUILDWARE + i);
    }
}

VOID GuildStorageFrame::OnItemChange( INT16 n16PosSrc, INT16 n16PosDst /*= GT_INVALID*/ )
{
    if(n16PosSrc == n16PosDst)
        return;

    Item* pItem = GetItem(n16PosSrc);
    if(P_VALID(pItem))
    {
        tagNC_ItemPosChange m;
        m.eConType = EICT_GuildWare;
        m.n16Num = 0;
        m.n16PosDst = n16PosDst;
        m.n64Serial = pItem->GetItemId();
        m.dwNPCID = m_dwNPCID;
        m_pSession->Send(&m);
    }
}

VOID GuildStorageFrame::OnItemChangeEx( EItemConType ePosSrc, EItemConType ePosDst, INT16 n16PosSrc, INT16 n16PosDst /*= GT_INVALID*/ )
{
    Item* pItem = ItemMgr::Inst()->GetConItem(ePosSrc, n16PosSrc);
    if(P_VALID(pItem))
    {
        tagNC_ItemPosChangeEx m;
        m.eConTypeDst = ePosDst;
        m.eConTypeSrc = ePosSrc;
        m.n16PosDst = n16PosDst;
        m.n64Serial1 = pItem->GetItemId();
        m.dwNPCID = m_dwNPCID;
        m_pSession->Send(&m);
    }
}

DWORD GuildStorageFrame::OnEventUpdateLPItem( tagUpdateLPItemEvent* pGameEvent )
{
    if(EICT_GuildWare == pGameEvent->eType)
    {
        if(IsCurPage(pGameEvent->n16Index))
        {
            RefreshItem(pGameEvent->n16Index);
        }
    }
	return 0;
}

DWORD GuildStorageFrame::OnInitGuildWareEvent( tagGameEvent* pGameEvent )
{
    if (m_pWnd->IsInvisible())
    {
        ShowWnd(TRUE);
    }
    
    ShowCurPage(m_n16CurPage);
	return 0;
}

DWORD GuildStorageFrame::OnEventCloseGuildWare( tagGameEvent* pGameEvent )
{
    ShowWnd(FALSE);
	return 0;
}

DWORD GuildStorageFrame::OnEventItemPosChange( tagItemPosChangeExEvent* pGameEvent )
{
    if(EICT_GuildWare == pGameEvent->eTypeDst)
    {
        OnItemChangeEx(pGameEvent->eTypeSrc, EICT_GuildWare, pGameEvent->n16IndexSrc);
    }
	return 0;
}