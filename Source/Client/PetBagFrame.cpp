#include "StdAfx.h"
#include "PetBagFrame.h"
#include "ItemMgr.h"
#include "Item.h"
#include "ItemFrame.h"
#include "ToolTipFrame.h"
#include "ToolTipCreator.h"
#include "..\WorldDefine\shop_define.h"
#include "..\WorldDefine\base_define.h"
#include "PetManager.h"
#include "PetDef.h"

PetBagFrame::PetBagFrame(void) :
    m_Trunk(this),
    m_pWnd(0),
    m_focusPet(0),
    m_dwFocusPetID(GT_INVALID)
{
    for (int i = 0; i < PETBAG_MAX_UNIT; ++i)
        m_pUnitItem[i] = 0;
}

PetBagFrame::~PetBagFrame(void)
{
}

BOOL PetBagFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
    BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);

    m_updateTimeFirst = Kernel::Inst()->GetAccumTimeDW();

    return bRet;
}

BOOL PetBagFrame::Destroy()
{
    BOOL bRet = GameFrame::Destroy();

    if (P_VALID(m_pWnd))
    {
        m_pGUI->AddToDestroyList(m_pWnd);
        m_pWnd = 0;
    }

    return bRet;
}

BOOL PetBagFrame::ReloadUI()
{
    // load ui resources
    XmlElement ele;
    tstring strPath = g_strLocalPath + _T("\\ui\\petbag.xml");
    m_pGUI->LoadXml(&ele, "VFS_System", strPath.c_str());

    // create the window
    m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &ele);

    // register ui event handle
    m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&PetBagFrame::EventHandler));

    // controls
    m_pStcUnitBack  = static_cast<GUIStatic*>(m_pWnd->GetChild(_T("petbagpic\\bagpic\\iconpic")));
    m_pButton       = static_cast<GUIButton*>(m_pWnd->GetChild(_T("petbagpic\\button")));
    m_pBtClose      = static_cast<GUIButton*>(m_pWnd->GetChild(_T("petbagpic\\closebutt")));

    // create item units
    for(int i = 0; i < PETBAG_MAX_COL; ++i)
    {
        for (int j = 0; j < PETBAG_MAX_ROW; ++j)
        {
            XmlElement ele;
            stringstream strTemp;
            int pos = PETBAG_MAX_COL * j + i;
            strTemp << "itemunit" << pos;
            ele.SetAttribute("Name", strTemp.str().c_str());
            ele.SetAttribute("ClassName", "ItemButton");
            ele.SetAttribute("Size_x", PETBAG_UINT_SIZE.cx);
            ele.SetAttribute("Size_y", PETBAG_UINT_SIZE.cy);
            ele.SetAttribute("LogicPos_x", 4 + PETBAG_SPACE_SIZE.cx*i + PETBAG_UINT_SIZE.cx*i);
            ele.SetAttribute("LogicPos_y", 4 + PETBAG_SPACE_SIZE.cy*j + PETBAG_UINT_SIZE.cy*j);
            ele.SetAttribute("Font", m_pUtil->UnicodeToUnicode8(g_StrTable[_T("Font_Lishu")]));
            ele.SetAttribute("FontWidth", 8);
            ele.SetAttribute("FontHeight", 16);
            ele.SetAttribute("TextRect_left", 0);
            ele.SetAttribute("TextRect_top", 0);
            ele.SetAttribute("TextRect_right", PETBAG_UINT_SIZE.cx);
            ele.SetAttribute("TextRect_bottom", PETBAG_UINT_SIZE.cy);
            ele.SetAttribute("TextColor", 0xFFFFF2A4);
            ele.SetAttribute("TextAlign", "RightBottom");
            m_pUnitItem[pos] = (ItemButton*)m_pGUI->CreateWnd(m_pStcUnitBack->GetFullName().c_str(), &ele);

            // set item disable
            m_pUnitItem[pos]->RefreshItem(0, 0, 0);
        }
    }

    // hide the window
    m_pWnd->SetInvisible(TRUE);

    // set focus pet id invalid
    m_focusPet = NULL;
    m_dwFocusPetID = GT_INVALID;

    return TRUE;
}

VOID PetBagFrame::Update()
{
    // 当窗体打开时
    if (P_VALID(m_pWnd))
    {
        if (m_pWnd->IsInvisible())
            return;
    }
    else
    {
        return;
    }

    if (!P_VALID(PetManager::Inst()->GetPet(m_dwFocusPetID)))
        return;

    // 每1秒更新下行囊
    if (Kernel::Inst()->GetAccumTimeDW() - m_updateTimeFirst > 1000)
    {
        RefreshItem();
        m_updateTimeFirst = Kernel::Inst()->GetAccumTimeDW();
    }

    GameFrame::Update();
}

BOOL PetBagFrame::EscCancel()
{
    if(m_pWnd->IsInvisible())
        return FALSE;

    return TRUE;
}

VOID PetBagFrame::ShowWnd( BOOL bShow )
{
    if (m_pWnd->IsInvisible() == bShow)
    {
        m_pWnd->SetInvisible(!bShow);
        m_pWnd->FlipToTop();
        RefreshItem();
    }

    if (bShow)
    {

    }
    else
    {

    }
}

DWORD PetBagFrame::EventHandler( tagGUIEvent* pEvent )
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
            else if (pWnd == m_pButton)
            {
                switch (m_eCurrType)
                {
                case EPBST_SELL:
                    {
                        if (!P_VALID(PetManager::Inst()->GetPet(m_dwFocusPetID)))
                            return 0;

                        PetManager::Inst()->TryToSellItem(m_focusPet->dwID, m_dwCurrSkillID);
                        ShowWnd(FALSE);
                    }
                    break;
                }
            }
            else if(pWnd->GetName().substr(0, 8) == _T("itemunit"))
            {
                for(INT i = 0; i < PETBAG_MAX_UNIT; ++i)
                {
                    if(m_pUnitItem[i] == pWnd)
                    {
                        if (!P_VALID(PetManager::Inst()->GetPet(m_dwFocusPetID)))
                            return 0;

                        Item* pItem = P_VALID(m_focusPet) ? PetManager::Inst()->GetItem(m_focusPet->dwID, i) : NULL;
                        if(P_VALID(pItem))
                        {
                            PetManager::Inst()->PopFromBag(m_focusPet->dwID, i);
                            RefreshItem();
                        }
                        break;
                    }
                }
            }
        }
        break;
    case EGUISCB_Drop:
        {
            if (pWnd->GetName().substr(0, 8) == _T("itemunit"))
            {
                if (EDT_Item == pEvent->dwParam1)
                {
                    for (int i = 0; i < m_unitsAvailCnt; ++i)
                    {
                        if (m_pUnitItem[i] == pWnd)
                        {
                            Item* item = ItemMgr::Inst()->GetPocketItem(static_cast<INT16>(LOWORD(pEvent->dwParam2)));

                            if (!P_VALID(PetManager::Inst()->GetPet(m_dwFocusPetID)))
                                return 0;

                            if (P_VALID(m_focusPet))
                                PetManager::Inst()->AddToBag(m_focusPet->dwID, i, item);

                            RefreshItem();

                            break;
                        }
                    }
                }
            }
        }
        break;
    case EGUIE_MoveIntoCtrl:
        {
            if (pWnd->GetName().substr(0, 8) == _T("itemunit"))
            {
                for(INT i = 0; i < PETBAG_MAX_UNIT; ++i)
                {
                    if(m_pUnitItem[i] == pWnd)
                    {
                        if (!P_VALID(PetManager::Inst()->GetPet(m_dwFocusPetID)))
                            return 0;

                        Item* pItem = P_VALID(m_focusPet) ? PetManager::Inst()->GetItem(m_focusPet->dwID, i) : NULL;
                        if(P_VALID(pItem))
                        {
                            tstring strPrice = _T("");
                            if(EIST_ItemShopVend == ((ItemFrame*)m_pFather)->GetCurStrategy())
                            {
                                tagItemCost cost(pItem->IsVendible() ? pItem->GetItemPrice() : GT_INVALID, ECCT_Null, 0, 0, GT_INVALID);
                                strPrice = ToolTipCreator::Inst()->GetPriceTips(&cost);
                            }
                            ShowTip(pWnd, ToolTipCreator::Inst()->GetItemTips(pItem).c_str(), strPrice.c_str());

                            // is equipment? show equipment diff information
                            if(MIsEquipment(pItem->GetItemTypeID()))
                            {
                                ToolTipCreator::Inst()->ShowEquipCompare(pItem->GetItemTypeID());
                            }
                        }
                        break;
                    }
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

    return 0;
}

void PetBagFrame::SetFocusPetID( DWORD petId )
{
    m_focusPet = PetManager::Inst()->GetPet(petId);
    m_dwFocusPetID = petId;
    if (m_focusPet != NULL)
        m_unitsAvailCnt = m_focusPet->nAttrA[ECSPA_BagGrid];
    else
        m_unitsAvailCnt = 0;

    RefreshItem();
}

void PetBagFrame::RefreshItem()
{
    // refresh units item
    int maxUnit = m_unitsAvailCnt;
    if (maxUnit > PETBAG_MAX_UNIT)
        maxUnit = PETBAG_MAX_UNIT;
    if (maxUnit < 0)
        maxUnit = 0;

    // clear
    for (int i = 0; i < maxUnit; ++i)
    {
        m_pUnitItem[i]->RefreshItem();
    }

    for (int i = maxUnit; i < PETBAG_MAX_UNIT; ++i)
    {
        m_pUnitItem[i]->RefreshItem(0, 0, 0);
    }

    // add item in bag
    if (!P_VALID(PetManager::Inst()->GetPet(m_dwFocusPetID)))
        return;

    if (P_VALID(m_focusPet))
    {
        for (tagPet::_pos2petbag::const_iterator it = m_focusPet->bag.begin();
            it != m_focusPet->bag.end(); ++it)
        {
            Item* pItem = ItemMgr::Inst()->GetConItemByID(EICT_Bag, it->second);
            if (P_VALID(pItem))
            {
                m_pUnitItem[it->first]->RefreshItem(pItem->GetItemTypeID(), pItem->GetItemQuantity(), pItem->GetItemQuality());
            }
        }
    }
}
