#include "StdAfx.h"
#include "PetSkillFrame.h"
#include "Item.h"
#include "ItemMgr.h"
#include "PetManager.h"
#include "ToolTipCreator.h"
#include "ToolTipFrame.h"
#include <fstream>
#include "LocalPlayer.h"
#include "RoleMgr.h"

PetSkillFrame::PetSkillFrame(void) : m_Trunk(this)
{
    for (int i = 0; i < ESSTE_End; ++i)
    {
        m_pPbtComposeSkill[i] = NULL;
    }
}

PetSkillFrame::~PetSkillFrame(void)
{
}

BOOL PetSkillFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
    BOOL bRet   =   GameFrame::Init(pMgr, pFather, dwParam);

    //! 注册游戏事件
    m_pFrameMgr->RegisterEventHandle(_T("tagPetSkillCfgOpen"),  (FRAMEEVENTPROC)m_Trunk.sfp1(&PetSkillFrame::OnPetSkillCfgOpen));

    return bRet;
}

BOOL PetSkillFrame::Destroy()
{
    BOOL bRet   =   GameFrame::Destroy();

    //! 注销游戏事件
    m_pFrameMgr->UnRegisterEventHandler(_T("tagPetSkillCfgOpen"),  (FRAMEEVENTPROC)m_Trunk.sfp1(&PetSkillFrame::OnPetSkillCfgOpen));

    //! 销毁窗体
    if( P_VALID(m_pWndForage) )
    {
        m_pGUI->AddToDestroyList(m_pWndForage);
        m_pWndForage = NULL;
    }
    if( P_VALID(m_pWndSupply) )
    {
        m_pGUI->AddToDestroyList(m_pWndSupply);
        m_pWndSupply = NULL;
    }
    if( P_VALID(m_pWndPickUp) )
    {
        m_pGUI->AddToDestroyList(m_pWndPickUp);
        m_pWndPickUp = NULL;
    }

    if (P_VALID(m_pWndTraining))
    {
        m_pGUI->AddToDestroyList(m_pWndTraining);
        m_pWndTraining = NULL;
    }

    return bRet;
}

BOOL PetSkillFrame::ReloadUI()
{
    XmlElement ele;
    // 觅食
    tstring strPath = g_strLocalPath + _T("\\ui\\foraging.xml");
    m_pGUI->LoadXml(&ele, "VFS_System", strPath.c_str());
    m_pGUI->ChangeXml(&ele, "forage_win\\forage_backpic\\itemback\\item", "ClassName", "ItemButton");
    m_pGUI->ChangeXml(&ele, "forage_win\\forage_backpic\\word2\\comboback", "CanMoveOutOfFather", "yes");

    m_pWndForage        =   m_pGUI->CreateWnd(_T("\\desktop"), &ele);
    ShowWndForage(FALSE);

    m_pCbCondition      =   static_cast<GUIComboBox*>(m_pWndForage->GetChild(_T("forage_backpic\\word2\\comboback")));      // 觅食条件
    m_pPbtEnable        =   static_cast<GUIPushButton*>(m_pWndForage->GetChild(_T("forage_backpic\\openbutt")));            // 开启觅食功能
    m_pBtFood           =   static_cast<ItemButton*>(m_pWndForage->GetChild(_T("forage_backpic\\itemback\\item")));         // 宠物食品
    m_pBtForageClose    =   static_cast<GUIButton*>(m_pWndForage->GetChild(_T("forage_backpic\\closebutt"))); // 关闭

    // 药童
    strPath = g_strLocalPath + _T("\\ui\\supply.xml");
    m_pGUI->LoadXml(&ele, "VFS_System", strPath.c_str());
    m_pGUI->ChangeXml(&ele, "supply_win\\supply_backpic\\itemback1\\item", "ClassName", "ItemButton");
    m_pGUI->ChangeXml(&ele, "supply_win\\supply_backpic\\itemback2\\item", "ClassName", "ItemButton");
    m_pGUI->ChangeXml(&ele, "supply_win\\supply_backpic\\itemback3\\item", "ClassName", "ItemButton");

    m_pWndSupply        =   m_pGUI->CreateWnd(_T("\\desktop"), &ele);
    ShowWndSupply(FALSE);

    m_pCbRedCondition       =   static_cast<GUIComboBox*>(m_pWndSupply->GetChild(_T("supply_backpic\\word2\\comboback")));      // 红药条件
    m_pPbtRedEnable         =   static_cast<GUIPushButton*>(m_pWndSupply->GetChild(_T("supply_backpic\\openbutt1")));           // 红药功能开启
    m_pBtRed                =   static_cast<ItemButton*>(m_pWndSupply->GetChild(_T("supply_backpic\\itemback1\\item")));        // 红药

    m_pCbBlueCondition      =   static_cast<GUIComboBox*>(m_pWndSupply->GetChild(_T("supply_backpic\\word4\\comboback")));      // 蓝药条件
    m_pPbtBlueEnable        =   static_cast<GUIPushButton*>(m_pWndSupply->GetChild(_T("supply_backpic\\openbutt2")));           // 蓝药功能开启
    m_pBtBlue               =   static_cast<ItemButton*>(m_pWndSupply->GetChild(_T("supply_backpic\\itemback2\\item")));        // 蓝药

    m_pCbToughnessCondition =   static_cast<GUIComboBox*>(m_pWndSupply->GetChild(_T("supply_backpic\\word6\\comboback")));      // 持久力条件
    m_pPbtToughnessEnable   =   static_cast<GUIPushButton*>(m_pWndSupply->GetChild(_T("supply_backpic\\openbutt3")));           // 持久力功能开启
    m_pBtToughness          =   static_cast<ItemButton*>(m_pWndSupply->GetChild(_T("supply_backpic\\itemback3\\item")));        // 持久力药

    m_pBtSupplyClose        =   static_cast<GUIButton*>(m_pWndSupply->GetChild(_T("supply_backpic\\closebutt")));               // 关闭
    

    // 分类拾取
    strPath = g_strLocalPath + _T("\\ui\\pickup.xml");
    m_pGUI->LoadXml(&ele, "VFS_System", strPath.c_str());

    m_pWndPickUp        =   m_pGUI->CreateWnd(_T("\\desktop"), &ele);
    ShowWndPickUp(FALSE);

    TCHAR tbuff[100];
    for (int i = 0; i != MAX_PICKUP_FUNC; ++i)
    {
        _sntprintf(tbuff, sizeof(tbuff) / sizeof(TCHAR), _T("pickup_backpic\\openbutt%d"), i + 4);
        m_pBtFunc[i]    =   static_cast<GUIPushButton*>(m_pWndPickUp->GetChild(tbuff));                                     //!< 功能开启
    }
    m_pBtPickClose      =   static_cast<GUIButton*>(m_pWndPickUp->GetChild(_T("pickup_backpic\\closebutt")));               //!< 关闭

    // 训炼
    strPath = g_strLocalPath + _T("\\ui\\pet_compose.xml");
    m_pGUI->LoadXml(&ele, "VFS_System", strPath.c_str());

    m_pWndTraining      = m_pGUI->CreateWnd(_T("\\desktop"), &ele);
    ShowWndTraining(FALSE);
    m_pPbtComposeSkill[ESSTE2_Smilt]        = static_cast<GUIPushButton*>(m_pWndTraining->GetChild(_T("petcompose_backpic\\frm\\openbutt4")));
    m_pPbtComposeSkill[ESSTE2_Artisan]      = static_cast<GUIPushButton*>(m_pWndTraining->GetChild(_T("petcompose_backpic\\frm\\openbutt7")));
    m_pPbtComposeSkill[ESSTE2_DanTraining]  = static_cast<GUIPushButton*>(m_pWndTraining->GetChild(_T("petcompose_backpic\\frm\\openbutt5")));
    m_pPbtComposeSkill[ESSTE2_Smith]        = static_cast<GUIPushButton*>(m_pWndTraining->GetChild(_T("petcompose_backpic\\frm\\openbutt8")));
    m_pPbtComposeSkill[ESSTE2_Casting]      = static_cast<GUIPushButton*>(m_pWndTraining->GetChild(_T("petcompose_backpic\\frm\\openbutt6")));
    m_pPbtComposeSkill[ESSTE2_Dressmaker]   = static_cast<GUIPushButton*>(m_pWndTraining->GetChild(_T("petcompose_backpic\\frm\\openbutt9")));
    m_pPbtComposeSkill[ESSTE2_Aechnics]     = static_cast<GUIPushButton*>(m_pWndTraining->GetChild(_T("petcompose_backpic\\frm\\openbutt11")));
    m_pPbtComposeSkill[ESSTE2_PointUp]      = static_cast<GUIPushButton*>(m_pWndTraining->GetChild(_T("petcompose_backpic\\frm\\openbutt12")));

    m_pBtTrainingConfirm    = static_cast<GUIButton*>(m_pWndTraining->GetChild(_T("petcompose_backpic\\surebutt")));
    m_pBtTrainingCanel      = static_cast<GUIButton*>(m_pWndTraining->GetChild(_T("petcompose_backpic\\canclebutt")));
    m_pBtTrainingClose      = static_cast<GUIButton*>(m_pWndTraining->GetChild(_T("petcompose_backpic\\closebutt")));

    m_pGUI->RegisterEventHandler(m_pWndForage->GetFullName().c_str(), m_Trunk.sfp1(&PetSkillFrame::EventHandlerForage));
    m_pGUI->RegisterEventHandler(m_pWndSupply->GetFullName().c_str(), m_Trunk.sfp1(&PetSkillFrame::EventHandlerSupply));
    m_pGUI->RegisterEventHandler(m_pWndPickUp->GetFullName().c_str(), m_Trunk.sfp1(&PetSkillFrame::EventHandlerPickUp));
    m_pGUI->RegisterEventHandler(m_pWndTraining->GetFullName().c_str(), m_Trunk.sfp1(&PetSkillFrame::EventHandlerTraining));

    // 初始化界面数据
    InitalUI();

    return TRUE;
}

VOID PetSkillFrame::Update()
{
}

VOID PetSkillFrame::ShowWndForage( BOOL bVal )
{
    m_pWndForage->SetInvisible(!bVal);
    if (bVal)
    {
        m_pWndForage->FlipToTop();
    }
    else
    {

    }
}

VOID PetSkillFrame::ShowWndSupply( BOOL bVal )
{
    m_pWndSupply->SetInvisible(!bVal);
    if (bVal)
    {
        m_pWndSupply->FlipToTop();
    }
    else
    {

    }
}

VOID PetSkillFrame::ShowWndPickUp( BOOL bVal )
{
    m_pWndPickUp->SetInvisible(!bVal);
    if (bVal)
    {
        m_pWndPickUp->FlipToTop();
    }
    else
    {

    }
}

//void PetSkillFrame::SavePetSkillCfgToFile(EPetskillType ePetSkillType)
//{
//    switch (ePetSkillType)
//    {
//    case EPT_Forage:
//        {
//            // 觅食
//            ofstream ofile(_T("PetForage.dat"), ios::trunc | ios::binary);
//            ofile.write(reinterpret_cast<char*>(&m_ForageCfg), sizeof(m_ForageCfg));
//            ofile.close();
//        }
//    	break;
//    case EPT_PickUp:
//        {
//            // 分类拾取
//            ofstream ofile(_T("PetPickup.dat"), ios::trunc | ios::binary);
//            ofile.write(reinterpret_cast<char*>(&m_PickupCfg), sizeof(m_PickupCfg));
//            ofile.close();
//        }
//        break;
//    case EPT_MedicineFeed:
//        {
//            // 药童
//            ofstream ofile(_T("PetMedfeed.dat"), ios::trunc | ios::binary);
//            ofile.write(reinterpret_cast<char*>(&m_MedicineFeedCfg), sizeof(m_MedicineFeedCfg));
//            ofile.close();
//        }
//        break;
//    }
//}
//
//void PetSkillFrame::LoadPetSkillCfgToFile(EPetskillType ePetSkillType)
//{
//    switch (ePetSkillType)
//    {
//    case EPT_Forage:
//        {
//            // 觅食
//            ifstream infile(_T("PetForage.dat"), ios::binary);
//            infile.read(reinterpret_cast<char*>(&m_ForageCfg), sizeof(m_ForageCfg));
//            infile.close();
//        }
//        break;
//    case EPT_PickUp:
//        {
//            // 分类拾取
//            ifstream infile(_T("PetPickup.dat"), ios::binary);
//            infile.read(reinterpret_cast<char*>(&m_PickupCfg), sizeof(m_PickupCfg));
//            infile.close();
//        }
//        break;
//    case EPT_MedicineFeed:
//        {
//            // 药童
//            ifstream infile(_T("PetMedfeed.dat"), ios::binary);
//            infile.read(reinterpret_cast<char*>(&m_MedicineFeedCfg), sizeof(m_MedicineFeedCfg));
//            infile.close();
//        }
//        break;
//    }
//}

DWORD PetSkillFrame::EventHandlerForage( tagGUIEvent* pEvent )
{
    GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);

    switch (pEvent->eEvent)
    {
    case EGUIE_Click:
        {
            if (pWnd == m_pBtForageClose)
            {
                // 关闭
                ShowWndForage(FALSE);
                SavePetSkillCfg(EPT_Forage);
            }
        }
        break;
    case EGUIE_PushButtonDown:
        {
            if (pWnd == m_pPbtEnable)
            {
                tagPet* pPet = const_cast<tagPet*>(PetManager::Inst()->GetPet(m_CurrPetID));
                if (P_VALID(pPet))
                {
                    pPet->skillForageCfg.bisEnabled = (m_pPbtEnable->GetState() == EGUIBS_PushDown);
                }
            }
        }
        break;
    case EGUIE_ItemClick:
        {
            if (pWnd == m_pCbCondition->GetListBox())
            {
                int row = pEvent->dwParam1;
                int nlimit = 10;
                tstring str = m_pCbCondition->GetListBox()->GetText(row, 0);
                tstringstream tsstream;
                tsstream << str.substr(0, 2);
                tsstream >> nlimit;

                // save config
                tagPet* pPet = const_cast<tagPet*>(PetManager::Inst()->GetPet(m_CurrPetID));
                if (P_VALID(pPet))
                {
                    pPet->skillForageCfg.u8MinSpirit = nlimit;
                }
            }
        }
        break;
    case EGUISCB_Drop:
        {
            if (pWnd == m_pBtFood)
            {
                Item* item = ItemMgr::Inst()->GetPocketItem(static_cast<INT16>(LOWORD(pEvent->dwParam2)));
                if (P_VALID(item) && item->GetProto()->nPrepareTime == 0)
                {
                    m_pBtFood->RefreshItem(item->GetItemTypeID(), item->GetItemQuantity(), item->GetItemQuality());
                    tagPet* pPet = const_cast<tagPet*>(PetManager::Inst()->GetPet(m_CurrPetID));
                    if (P_VALID(pPet))
                    {
                        pPet->skillForageCfg.dwItemID   =   item->GetItemId();
                    }
                }
            }
        }
        break;
    case EGUIE_MoveIntoCtrl:
        {
            if(m_pBtFood == pWnd)
            {
                tagPet* pPet = const_cast<tagPet*>(PetManager::Inst()->GetPet(m_CurrPetID));
                if (P_VALID(pPet))
                {
                    Item* pItem = ItemMgr::Inst()->GetConItemByID(EICT_Bag, pPet->skillForageCfg.dwItemID);
                    if (P_VALID(pItem))
                    {
                        ShowTip(pWnd, ToolTipCreator::Inst()->GetItemTips(pItem).c_str());
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
    case EGUIE_Active:
        if( pWnd == m_pCbCondition->GetEditBox() )
            m_pGUI->SetActive( m_pCbCondition->GetListBox() );
        break;
    }

    return 0;
}

DWORD PetSkillFrame::EventHandlerSupply( tagGUIEvent* pEvent )
{
    GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);

    switch (pEvent->eEvent)
    {
    case EGUIE_Click:
        {
            if (pWnd == m_pBtSupplyClose)
            {
                // 关闭
                ShowWndSupply(FALSE);
                SavePetSkillCfg(EPT_MedicineFeed);
            }
            else if (pWnd == m_pBtRed)
            {
                m_pBtRed->RefreshItem();
                tagPet* pPet = const_cast<tagPet*>(PetManager::Inst()->GetPet(m_CurrPetID));
                if (P_VALID(pPet))
                {
                    pPet->skillMedicineFeedCfg.dwHPItemID   =   GT_INVALID;
                }
            }
            else if (pWnd == m_pBtBlue)
            {
                m_pBtBlue->RefreshItem();
                tagPet* pPet = const_cast<tagPet*>(PetManager::Inst()->GetPet(m_CurrPetID));
                if (P_VALID(pPet))
                {
                    pPet->skillMedicineFeedCfg.dwMPItemID   =   GT_INVALID;
                }
            }
            else if (pWnd == m_pBtToughness)
            {
                m_pBtToughness->RefreshItem();
                tagPet* pPet = const_cast<tagPet*>(PetManager::Inst()->GetPet(m_CurrPetID));
                if (P_VALID(pPet))
                {
                    pPet->skillMedicineFeedCfg.dwTouItemID   =   GT_INVALID;
                }
            }
        }
        break;
    case EGUIE_PushButtonDown:
        {
            if (pWnd == m_pPbtRedEnable)
            {
                tagPet* pPet = const_cast<tagPet*>(PetManager::Inst()->GetPet(m_CurrPetID));
                if (P_VALID(pPet))
                {
                    pPet->skillMedicineFeedCfg.bHPEnabled = (m_pPbtRedEnable->GetState() == EGUIBS_PushDown);
                }
            }
            else if (pWnd == m_pPbtBlueEnable)
            {
                tagPet* pPet = const_cast<tagPet*>(PetManager::Inst()->GetPet(m_CurrPetID));
                if (P_VALID(pPet))
                {
                    pPet->skillMedicineFeedCfg.bisMPEnabled = (m_pPbtBlueEnable->GetState() == EGUIBS_PushDown);
                }
            }
            else if (pWnd == m_pPbtToughnessEnable)
            {
                tagPet* pPet = const_cast<tagPet*>(PetManager::Inst()->GetPet(m_CurrPetID));
                if (P_VALID(pPet))
                {
                    pPet->skillMedicineFeedCfg.bisTouEnabled = (m_pPbtToughnessEnable->GetState() == EGUIBS_PushDown);
                }
            }
        }
        break;
    case EGUIE_ItemClick:
        {
            if (pWnd == m_pCbRedCondition->GetListBox())
            {
                int row = pEvent->dwParam1;
                int nlimit = 10;
                tstring str = m_pCbRedCondition->GetListBox()->GetText(row, 0);
                tstringstream tsstream;
                tsstream << str.substr(0, 2);
                tsstream >> nlimit;

                // save config
                tagPet* pPet = const_cast<tagPet*>(PetManager::Inst()->GetPet(m_CurrPetID));
                if (P_VALID(pPet))
                {
                    pPet->skillMedicineFeedCfg.u8MinHP = nlimit;
                }
            }
            else if (pWnd == m_pCbBlueCondition->GetListBox())
            {
                int row = pEvent->dwParam1;
                int nlimit = 10;
                tstring str = m_pCbBlueCondition->GetListBox()->GetText(row, 0);
                tstringstream tsstream;
                tsstream << str.substr(0, 2);
                tsstream >> nlimit;

                // save config
                tagPet* pPet = const_cast<tagPet*>(PetManager::Inst()->GetPet(m_CurrPetID));
                if (P_VALID(pPet))
                {
                    pPet->skillMedicineFeedCfg.u8MinMP = nlimit;
                }
            }
            else if (pWnd == m_pCbToughnessCondition->GetListBox())
            {
                int row = pEvent->dwParam1;
                int nlimit = 10;
                tstring str = m_pCbToughnessCondition->GetListBox()->GetText(row, 0);
                tstringstream tsstream;
                tsstream << str.substr(0, 2);
                tsstream >> nlimit;

                // save config
                tagPet* pPet = const_cast<tagPet*>(PetManager::Inst()->GetPet(m_CurrPetID));
                if (P_VALID(pPet))
                {
                    pPet->skillMedicineFeedCfg.u8Toughness = nlimit;
                }
            }
        }
        break;
    case EGUISCB_Drop:
        {
            if (pWnd == m_pBtRed)
            {
                Item* item = ItemMgr::Inst()->GetPocketItem(static_cast<INT16>(LOWORD(pEvent->dwParam2)));
                if (P_VALID(item) && item->GetProto()->nPrepareTime == 0)
                {
                    if (IfCanUseHPItem(item))
                    {
                        m_pBtRed->RefreshItem(item->GetItemTypeID(), item->GetItemQuantity(), item->GetItemQuality());
                        tagPet* pPet = const_cast<tagPet*>(PetManager::Inst()->GetPet(m_CurrPetID));
                        if (P_VALID(pPet))
                        {
                            pPet->skillMedicineFeedCfg.dwHPItemID   =   item->GetItemId();
                        }
                    }
                }
            }
            else if (pWnd == m_pBtBlue)
            {
                Item* item = ItemMgr::Inst()->GetPocketItem(static_cast<INT16>(LOWORD(pEvent->dwParam2)));
                if (P_VALID(item) && item->GetProto()->nPrepareTime == 0)
                {
                    if (IfCanUseMPItem(item))
                    {
                        m_pBtBlue->RefreshItem(item->GetItemTypeID(), item->GetItemQuantity(), item->GetItemQuality());
                        tagPet* pPet = const_cast<tagPet*>(PetManager::Inst()->GetPet(m_CurrPetID));
                        if (P_VALID(pPet))
                        {
                            pPet->skillMedicineFeedCfg.dwMPItemID   =   item->GetItemId();
                        }
                    }
                }
            }
            else if (pWnd == m_pBtToughness)
            {
                Item* item = ItemMgr::Inst()->GetPocketItem(static_cast<INT16>(LOWORD(pEvent->dwParam2)));
                if (P_VALID(item) && item->GetProto()->nPrepareTime == 0)
                {
                    if (IfCanUseToughnessItem(item))
                    {
                        m_pBtToughness->RefreshItem(item->GetItemTypeID(), item->GetItemQuantity(), item->GetItemQuality());
                        tagPet* pPet = const_cast<tagPet*>(PetManager::Inst()->GetPet(m_CurrPetID));
                        if (P_VALID(pPet))
                        {
                            pPet->skillMedicineFeedCfg.dwTouItemID   =   item->GetItemId();
                        }
                    }
                }
            }
        }
        break;
    case EGUIE_MoveIntoCtrl:
        {
            if(m_pBtRed == pWnd)
            {
                tagPet* pPet = const_cast<tagPet*>(PetManager::Inst()->GetPet(m_CurrPetID));
                if (P_VALID(pPet))
                {
                    Item* pItem = ItemMgr::Inst()->GetConItemByID(EICT_Bag, pPet->skillMedicineFeedCfg.dwHPItemID);
                    if (P_VALID(pItem))
                    {
                        ShowTip(pWnd, ToolTipCreator::Inst()->GetItemTips(pItem).c_str());
                    }
                }
            }
            else if(m_pBtBlue == pWnd)
            {
                tagPet* pPet = const_cast<tagPet*>(PetManager::Inst()->GetPet(m_CurrPetID));
                if (P_VALID(pPet))
                {
                    Item* pItem = ItemMgr::Inst()->GetConItemByID(EICT_Bag, pPet->skillMedicineFeedCfg.dwMPItemID);
                    if (P_VALID(pItem))
                    {
                        ShowTip(pWnd, ToolTipCreator::Inst()->GetItemTips(pItem).c_str());
                    }
                }
            }
            else if(m_pBtToughness == pWnd)
            {
                tagPet* pPet = const_cast<tagPet*>(PetManager::Inst()->GetPet(m_CurrPetID));
                if (P_VALID(pPet))
                {
                    Item* pItem = ItemMgr::Inst()->GetConItemByID(EICT_Bag, pPet->skillMedicineFeedCfg.dwTouItemID);
                    if (P_VALID(pItem))
                    {
                        ShowTip(pWnd, ToolTipCreator::Inst()->GetItemTips(pItem).c_str());
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
    case EGUIE_Active:
        if( pWnd == m_pCbRedCondition->GetEditBox() )
            m_pGUI->SetActive( m_pCbRedCondition->GetListBox() );
        else if( pWnd == m_pCbBlueCondition->GetEditBox() )
            m_pGUI->SetActive( m_pCbBlueCondition->GetListBox() );
        else if( pWnd == m_pCbToughnessCondition->GetEditBox() )
            m_pGUI->SetActive( m_pCbToughnessCondition->GetListBox() );
        break;
    }

    return 0;
}

DWORD PetSkillFrame::EventHandlerPickUp( tagGUIEvent* pEvent )
{
    GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);

    switch (pEvent->eEvent)
    {
    case EGUIE_Click:
        {
            if (pWnd == m_pBtPickClose)
            {
                // 关闭
                ShowWndPickUp(FALSE);
                SavePetSkillCfg(EPT_PickUp);
            }
        }
        break;
    case EGUIE_PushButtonDown:
        {
            tagPet* pPet = const_cast<tagPet*>(PetManager::Inst()->GetPet(m_CurrPetID));
            if (P_VALID(pPet))
            {
                if (pWnd == m_pBtFunc[0])
                {
                    pPet->skillPickupCfg.bMoney = (m_pBtFunc[0]->GetState() == EGUIBS_PushDown);
                }
                else if (pWnd == m_pBtFunc[1])
                {
                    pPet->skillPickupCfg.bTrophy = (m_pBtFunc[1]->GetState() == EGUIBS_PushDown);
                }
                else if (pWnd == m_pBtFunc[2])
                {
                    pPet->skillPickupCfg.bMaterial = (m_pBtFunc[2]->GetState() == EGUIBS_PushDown);
                }
                else if (pWnd == m_pBtFunc[3])
                {
                    pPet->skillPickupCfg.bEquip = (m_pBtFunc[3]->GetState() == EGUIBS_PushDown);
                }
                else if (pWnd == m_pBtFunc[4])
                {
                    pPet->skillPickupCfg.bMedicine = (m_pBtFunc[4]->GetState() == EGUIBS_PushDown);
                }
                else if (pWnd == m_pBtFunc[5])
                {
                    pPet->skillPickupCfg.bOthers = (m_pBtFunc[5]->GetState() == EGUIBS_PushDown);
                }
            }

        }
        break;
    }

    return 0;
}

DWORD PetSkillFrame::EventHandlerTraining( tagGUIEvent* pEvent )
{
    GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);

    switch (pEvent->eEvent)
    {
    case EGUIE_Click:
        {
            if (pWnd == m_pBtTrainingClose || pWnd == m_pBtTrainingCanel)
            {
                // 关闭
                ShowWndTraining(FALSE);
            }
            else if (pWnd == m_pBtTrainingConfirm)
            {
                // 确定
                for (int i = 0; i < ESSTE_End; ++i)
                {
                    if (P_VALID(m_pPbtComposeSkill[i])
                        && EGUIBS_PushDown == m_pPbtComposeSkill[i]->GetState())
                    {
                        PetManager::Inst()->TryToTraining(m_CurrPetID, GetComposeSkill((ESkillTypeEx2)i), m_CurrSkillID);
                        ShowWndTraining(FALSE);

                        return 0;
                    }
                }

                ShowWndTraining(FALSE);
            }
        }
        break;
    case EGUIE_PushButtonDown:
        {
        }
        break;
    }

    return 0;
}

DWORD PetSkillFrame::OnPetSkillCfgOpen( tagPetSkillCfgOpen* pEvent )
{
    m_CurrPetID     =   pEvent->dwPetID;
    m_CurrSkillID   =   pEvent->dwSkillID;

    switch (pEvent->eSkillType)
    {
    case EPT_Forage:
        {
            LoadPetSkillCfg(EPT_Forage);
            ShowWndForage(TRUE);
        }
        break;
    case EPT_PickUp:
        {
            LoadPetSkillCfg(EPT_PickUp);
            ShowWndPickUp(TRUE);
        }
        break;
    case EPT_MedicineFeed:
        {
            LoadPetSkillCfg(EPT_MedicineFeed);
            ShowWndSupply(TRUE);
        }
        break;
    case EPT_Experience:
        {
            ShowWndTraining(TRUE);
        }
        break;
    }

    return 0;
}

void PetSkillFrame::SavePetSkillCfg( EPetskillType ePetSkillType )
{

}

void PetSkillFrame::LoadPetSkillCfg( EPetskillType ePetSkillType )
{
    switch (ePetSkillType)
    {
    case EPT_Forage:
        {
            // 觅食
            tagPet* pPet = const_cast<tagPet*>(PetManager::Inst()->GetPet(m_CurrPetID));
            if (P_VALID(pPet))
            {
                // 开关
                m_pPbtEnable->SetState(pPet->skillForageCfg.bisEnabled ? EGUIBS_PushDown : EGUIBS_PopUp);
                // 物品
                Item* pItem = ItemMgr::Inst()->GetConItemByID(EICT_Bag, pPet->skillForageCfg.dwItemID);
                if (P_VALID(pItem))
                {
                    m_pBtFood->RefreshItem(pItem->GetItemTypeID(), pItem->GetItemQuantity(), pItem->GetItemQuality());
                }
                else
                {
                    m_pBtFood->RefreshItem();
                }
                // 灵力限制
                if (pPet->skillForageCfg.u8MinSpirit != 0)
                {
                    tstringstream tsstream;
                    tsstream << pPet->skillForageCfg.u8MinSpirit << _T('%');
                    m_pCbCondition->GetEditBox()->SetText(tsstream.str().c_str());
                }
            }
        }
        break;
    case EPT_PickUp:
        {
            // 拾取
            tagPet* pPet = const_cast<tagPet*>(PetManager::Inst()->GetPet(m_CurrPetID));
            if (P_VALID(pPet))
            {
                m_pBtFunc[0]->SetState(pPet->skillPickupCfg.bMoney      ? EGUIBS_PushDown : EGUIBS_PopUp);
                m_pBtFunc[1]->SetState(pPet->skillPickupCfg.bTrophy     ? EGUIBS_PushDown : EGUIBS_PopUp);
                m_pBtFunc[2]->SetState(pPet->skillPickupCfg.bMaterial   ? EGUIBS_PushDown : EGUIBS_PopUp);
                m_pBtFunc[3]->SetState(pPet->skillPickupCfg.bEquip      ? EGUIBS_PushDown : EGUIBS_PopUp);
                m_pBtFunc[4]->SetState(pPet->skillPickupCfg.bMedicine   ? EGUIBS_PushDown : EGUIBS_PopUp);
                m_pBtFunc[5]->SetState(pPet->skillPickupCfg.bOthers     ? EGUIBS_PushDown : EGUIBS_PopUp);
            }
        }
        break;
    case EPT_MedicineFeed:
        {
            // 喂药
            tagPet* pPet = const_cast<tagPet*>(PetManager::Inst()->GetPet(m_CurrPetID));
            if (P_VALID(pPet))
            {
                // 开关
                m_pPbtRedEnable->SetState(pPet->skillMedicineFeedCfg.bHPEnabled ? EGUIBS_PushDown : EGUIBS_PopUp);
                m_pPbtBlueEnable->SetState(pPet->skillMedicineFeedCfg.bisMPEnabled ? EGUIBS_PushDown : EGUIBS_PopUp);
                m_pPbtToughnessEnable->SetState(pPet->skillMedicineFeedCfg.bisTouEnabled ? EGUIBS_PushDown : EGUIBS_PopUp);

                // 体力
                Item* pItem = ItemMgr::Inst()->GetConItemByID(EICT_Bag, pPet->skillMedicineFeedCfg.dwHPItemID);
                if (P_VALID(pItem))
                {
                    m_pBtRed->RefreshItem(pItem->GetItemTypeID(), pItem->GetItemQuantity(), pItem->GetItemQuality());
                }
                else
                {
                    m_pBtRed->RefreshItem();
                }
                // 真气
                pItem = ItemMgr::Inst()->GetConItemByID(EICT_Bag, pPet->skillMedicineFeedCfg.dwMPItemID);
                if (P_VALID(pItem))
                {
                    m_pBtBlue->RefreshItem(pItem->GetItemTypeID(), pItem->GetItemQuantity(), pItem->GetItemQuality());
                }
                else
                {
                    m_pBtBlue->RefreshItem();
                }
                // 持久力
                pItem = ItemMgr::Inst()->GetConItemByID(EICT_Bag, pPet->skillMedicineFeedCfg.dwTouItemID);
                if (P_VALID(pItem))
                {
                    m_pBtToughness->RefreshItem(pItem->GetItemTypeID(), pItem->GetItemQuantity(), pItem->GetItemQuality());
                }
                else
                {
                    m_pBtToughness->RefreshItem();
                }
                // 限制
                if (pPet->skillMedicineFeedCfg.u8MinHP != 0)
                {
                    tstringstream tsstream;
                    tsstream << pPet->skillMedicineFeedCfg.u8MinHP << _T('%');
                    m_pCbRedCondition->GetEditBox()->SetText(tsstream.str().c_str());
                }
                else
                {
                    m_pCbRedCondition->GetEditBox()->SetText(_T(""));
                }

                if (pPet->skillMedicineFeedCfg.u8MinMP != 0)
                {
                    tstringstream tsstream;
                    tsstream << pPet->skillMedicineFeedCfg.u8MinMP << _T('%');
                    m_pCbBlueCondition->GetEditBox()->SetText(tsstream.str().c_str());
                }
                else
                {
                    m_pCbBlueCondition->GetEditBox()->SetText(_T(""));
                }

                if (pPet->skillMedicineFeedCfg.u8Toughness != 0)
                {
                    tstringstream tsstream;
                    tsstream << pPet->skillMedicineFeedCfg.u8Toughness << _T('%');
                    m_pCbToughnessCondition->GetEditBox()->SetText(tsstream.str().c_str());
                }
                else
                {
                    m_pCbToughnessCondition->GetEditBox()->SetText(_T(""));
                }
            }
        }
        break;
    }
}

void PetSkillFrame::InitalUI()
{
    // 填充combobox
    for (int i = 0; i < 9; ++i)
    {
        tstringstream tsstream;
        tsstream << (i + 1) * 10 << _T('%');
        m_pCbCondition->GetListBox()->SetText(i, 0, tsstream.str().c_str());
        m_pCbRedCondition->GetListBox()->SetText(i, 0, tsstream.str().c_str());
        m_pCbBlueCondition->GetListBox()->SetText(i, 0, tsstream.str().c_str());
        m_pCbToughnessCondition->GetListBox()->SetText(i, 0, tsstream.str().c_str());
    }
    
    // 调整大小
    ResizeCombobox(m_pCbCondition, 9);
    ResizeCombobox(m_pCbRedCondition, 9);
    ResizeCombobox(m_pCbBlueCondition, 9);
    ResizeCombobox(m_pCbToughnessCondition, 9);
}

void PetSkillFrame::ResizeCombobox( GUIComboBox* pComboBox, int rowCount )
{
    tagPoint ptSize	= pComboBox->GetListBox()->GetSize();

    ptSize.x = pComboBox->GetEditBox()->GetSize().x;
    ptSize.y = pComboBox->GetListBox()->GetRowHeight() * rowCount;

    pComboBox->GetListBox()->SetResizable(TRUE);
    pComboBox->GetListBox()->SetColWidth(0, ptSize.x);
    pComboBox->GetListBox()->Resize(ptSize);
    pComboBox->GetListBox()->SetResizable(FALSE);
}

BOOL PetSkillFrame::EscCancel()
{
    if (m_pWndForage->IsInvisible() 
        && m_pWndSupply->IsInvisible()
        && m_pWndPickUp->IsInvisible()
        && m_pWndTraining->IsInvisible())
        return FALSE;

    ShowWndForage(FALSE);
    ShowWndSupply(FALSE);
    ShowWndPickUp(FALSE);
    ShowWndTraining(FALSE);

    return TRUE;
}

VOID PetSkillFrame::ShowWndTraining( BOOL bVal )
{
    m_pWndTraining->SetInvisible(!bVal);
    if (bVal)
    {
        m_pWndTraining->FlipToTop();
    }
    else
    {

    }
}

DWORD PetSkillFrame::GetComposeSkill( ESkillTypeEx2 eType )
{
    switch (eType)
    {
    case ESSTE2_Smilt:
        return 90002;
        break;
    case ESSTE2_Artisan:
        return 90003;
        break;
    case ESSTE2_DanTraining:
        return 90004;
        break;
    case ESSTE2_Smith:
        return 90005;
        break;
    case ESSTE2_Casting:
        return 90006;
        break;
    case ESSTE2_Dressmaker:
        return 90007;
        break;
    case ESSTE2_Aechnics:
        return 90008;
        break;
    case ESSTE2_PointUp:
        return 90009;
        break;
    }

    return GT_INVALID;
}

bool PetSkillFrame::IfCanUseHPItem( Item* pItem )
{
    // 获得人物等级
    LocalPlayer* pLp = RoleMgr::Inst()->GetLocalPlayer();
    if (!P_VALID(pLp))
        return false;

    if (pLp->GetRoleLevel() < pItem->GetProto()->byMinUseLevel)
        return false;

    return true;
}

bool PetSkillFrame::IfCanUseMPItem( Item* pItem )
{
    // 获得人物等级
    LocalPlayer* pLp = RoleMgr::Inst()->GetLocalPlayer();
    if (!P_VALID(pLp))
        return false;

    if (pLp->GetRoleLevel() < pItem->GetProto()->byMinUseLevel)
        return false;

    return true;
}

bool PetSkillFrame::IfCanUseToughnessItem( Item* pItem )
{
    // 获得人物等级
    LocalPlayer* pLp = RoleMgr::Inst()->GetLocalPlayer();
    if (!P_VALID(pLp))
        return false;

    if (pLp->GetRoleLevel() < pItem->GetProto()->byMinUseLevel)
        return false;

    return true;
}