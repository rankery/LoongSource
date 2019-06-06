#include "StdAfx.h"
#include "PetFrame.h"
#include "RoleMgr.h"
#include "PetManager.h"
#include "PetDef.h"
#include "PetEvent.h"
#include "PetShowWindow.h"
#include "PetBagFrame.h"
#include "CombatSysUtil.h"
#include "Item.h"
#include "ItemMgr.h"
#include "ToolTipCreator.h"
#include "ToolTipFrame.h"
#include "DragBtn.h"
#include "PetInformationFrame.h"
#include "PetTradeUIFrame.h"
#include "PetProtoData.h"

PetFrame::PetFrame(void) :
    m_Trunk(this),
    m_dwCurrSelectPetID(GT_INVALID),
    m_pPetBagFrame(0)
{
}

PetFrame::~PetFrame(void)
{
}

BOOL PetFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);

    //! 注册游戏事件
    m_pFrameMgr->RegisterEventHandle(_T("Open_Pet"),            (FRAMEEVENTPROC)m_Trunk.sfp1(&PetFrame::OnOpen_Pet));
	m_pFrameMgr->RegisterEventHandle(_T("Close_Pet"),            (FRAMEEVENTPROC)m_Trunk.sfp1(&PetFrame::OnClose_Pet));
    m_pFrameMgr->RegisterEventHandle(_T("tagPetAttrChange"),    (FRAMEEVENTPROC)m_Trunk.sfp1(&PetFrame::OnPetAttrChange));
    m_pFrameMgr->RegisterEventHandle(_T("tagUsePetEquip"),      (FRAMEEVENTPROC)m_Trunk.sfp1(&PetFrame::OnUsePetEquipHandler));
    m_pFrameMgr->RegisterEventHandle(_T("BoxDeletePet"),        (FRAMEEVENTPROC)m_Trunk.sfp1(&PetFrame::OnCommitDeletePet));
    m_pFrameMgr->RegisterEventHandle(_T("BoxPourPetExp"),        (FRAMEEVENTPROC)m_Trunk.sfp1(&PetFrame::OnCommitPourExp));

    //! create pet bag frame
    PetBagFrame *pFrame = (PetBagFrame*)m_pFrameMgr->GetFrame(_T("PetBag"));
    if (!P_VALID(pFrame))
        m_pPetBagFrame = static_cast<PetBagFrame*>(m_pFrameMgr->CreateFrame(m_strName.c_str(), _T("PetBag"), _T("PetBagFrame"), 0));

    return bRet;
}

BOOL PetFrame::Destroy()
{
    BOOL bRet = GameFrame::Destroy();

    //! 销毁窗体
    if( P_VALID(m_pWnd) )
    {
        m_pGUI->AddToDestroyList(m_pWnd);
        m_pWnd = NULL;
    }

    //! 注销游戏事件
    m_pFrameMgr->UnRegisterEventHandler(_T("Open_Pet"),         (FRAMEEVENTPROC)m_Trunk.sfp1(&PetFrame::OnOpen_Pet));
	m_pFrameMgr->UnRegisterEventHandler(_T("Close_Pet"),        (FRAMEEVENTPROC)m_Trunk.sfp1(&PetFrame::OnClose_Pet));
    m_pFrameMgr->UnRegisterEventHandler(_T("tagPetAttrChange"), (FRAMEEVENTPROC)m_Trunk.sfp1(&PetFrame::OnPetAttrChange));
    m_pFrameMgr->UnRegisterEventHandler(_T("tagUsePetEquip"),   (FRAMEEVENTPROC)m_Trunk.sfp1(&PetFrame::OnUsePetEquipHandler));
    m_pFrameMgr->UnRegisterEventHandler(_T("BoxDeletePet"),     (FRAMEEVENTPROC)m_Trunk.sfp1(&PetFrame::OnCommitDeletePet));
    m_pFrameMgr->UnRegisterEventHandler(_T("BoxPourPetExp"),    (FRAMEEVENTPROC)m_Trunk.sfp1(&PetFrame::OnCommitPourExp));

    return bRet;
}

BOOL PetFrame::ReloadUI()
{
    XmlElement ele;
    tstring strPath = g_strLocalPath + _T("\\ui\\pet.xml");
    m_pGUI->LoadXml(&ele, "VFS_System", strPath.c_str());


    // 更改控件
    char chbuff[128];
    for (int i = 1; i != MAX_PET_EQUIP + 1; ++i)
    {
        _snprintf(chbuff, sizeof(chbuff) / sizeof(char), "pet_win\\petback_pic\\pet_pic\\pet_equipmentback\\itemback%d\\item%d", i, i);
        m_pGUI->ChangeXml(&ele, chbuff, "ClassName", "ItemButton");
    }
    for (int i = 1; i != MAX_PET_SKILL + 1; ++i)
    {
        _snprintf(chbuff, sizeof(chbuff) / sizeof(char), "pet_win\\petback_pic\\pet_pic\\petskill_back\\skilback%d\\skill%d", i, i);
        m_pGUI->ChangeXml(&ele, chbuff, "ClassName", "PetSkillButton");
    }

    //m_pGUI->ChangeXml(&ele, "pet_win\\petback_pic\\pet_pic\\petshow_back\\petmood", "ClassName", "StaticEx");
    m_pGUI->ChangeXml(&ele, "pet_win\\petback_pic\\pet_pic\\petshow_back", "ClassName", "PetShowWindow");

    // 创建
    m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &ele);

    // 注册事件处理函数
    m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&PetFrame::EventHandler));

    // 获得控件
    TCHAR tbuff[128];
    for (int i = 0; i != MAX_PET_EQUIP; ++i)
    {
        int index = i + 1;
        _sntprintf(tbuff, sizeof(tbuff) / sizeof(TCHAR), _T("petback_pic\\pet_pic\\pet_equipmentback\\itemback%d\\item%d"), index, index);
        m_pBtEquipA[i]  =   static_cast<ItemButton*>(m_pWnd->GetChild(tbuff));          // 宠物装备
    }
    for (int i = 0; i != MAX_PET_SKILL; ++i)
    {
        int index = i + 1;
        _sntprintf(tbuff, sizeof(tbuff) / sizeof(TCHAR), _T("petback_pic\\pet_pic\\petskill_back\\skilback%d\\skill%d"), index, index);
        m_pBtSkillA[i]  =   static_cast<PetSkillButton*>(m_pWnd->GetChild(tbuff));         // 宠物技能
    }
    

    //m_pStcMood          =   static_cast<StaticEx*>(m_pWnd->GetChild(_T("petback_pic\\pet_pic\\petshow_back\\petmood")));            // 心情
    
    m_pStcPetShow       =   static_cast<PetShowWindow*>(m_pWnd->GetChild(_T("petback_pic\\pet_pic\\petshow_back")));                // 宠物Avata
    m_pStcState         =   static_cast<GUIStatic*>(m_pWnd->GetChild(_T("petback_pic\\pet_pic\\petshow_back\\state")));             // 状态

    m_pLbPetList        =   static_cast<GUIListBox*>(m_pWnd->GetChild(_T("petback_pic\\pet_pic\\petlist_back\\petlist")));          // 宠物列表

    m_pBtCall           =   static_cast<GUIButton*>(m_pWnd->GetChild(_T("petback_pic\\pet_pic\\callbutt")));                        // 召唤
    m_pBtSetFree        =   static_cast<GUIButton*>(m_pWnd->GetChild(_T("petback_pic\\pet_pic\\release")));                         // 放生
    m_pBtPerfuse        =   static_cast<GUIButton*>(m_pWnd->GetChild(_T("petback_pic\\pet_pic\\perfusebutt")));                     // 灌注
    m_pBtRide           =   static_cast<GUIButton*>(m_pWnd->GetChild(_T("petback_pic\\pet_pic\\ridebutt")));                        // 骑乘
    m_pBtTrade          =   static_cast<GUIButton*>(m_pWnd->GetChild(_T("petback_pic\\pet_pic\\exchangebutt")));                    // 交易

    m_pStcQuality       =   static_cast<GUIStatic*>(m_pWnd->GetChild(_T("petback_pic\\pet_pic\\petproperty_back1\\num1")));         // 宠物智商
    m_pStcMatureTalent  =   static_cast<GUIStatic*>(m_pWnd->GetChild(_T("petback_pic\\pet_pic\\petproperty_back1\\num2")));         // 成长资质
    m_pStcPotential     =   static_cast<GUIStatic*>(m_pWnd->GetChild(_T("petback_pic\\pet_pic\\petproperty_back1\\num5")));         // 潜能
    m_pStcMagic         =   static_cast<GUIStatic*>(m_pWnd->GetChild(_T("petback_pic\\pet_pic\\petproperty_back1\\num4")));         // 灵力
    m_pStcEnergy        =   static_cast<GUIStatic*>(m_pWnd->GetChild(_T("petback_pic\\pet_pic\\petproperty_back1\\num3")));         // 精气
    m_pStcRidingSpeed   =   static_cast<GUIStatic*>(m_pWnd->GetChild(_T("petback_pic\\pet_pic\\petproperty_back1\\num6")));         // 骑乘速度

    m_pStcNick          =   static_cast<GUIStatic*>(m_pWnd->GetChild(_T("petback_pic\\pet_pic\\petsituation_back\\name")));         // 宠物昵称
    m_pStcLevel         =   static_cast<GUIStatic*>(m_pWnd->GetChild(_T("petback_pic\\pet_pic\\petsituation_back\\stage")));        // 级别
    m_pStcCurrExp       =   static_cast<GUIStatic*>(m_pWnd->GetChild(_T("petback_pic\\pet_pic\\petsituation_back\\num5")));         // 当前经验
    m_pStcLevelUpNeed   =   static_cast<GUIStatic*>(m_pWnd->GetChild(_T("petback_pic\\pet_pic\\petsituation_back\\num6")));         // 升级经验

    m_pBtClose          =   static_cast<GUIButton*>(m_pWnd->GetChild(_T("petback_pic\\closebutt")));                                // 关闭

    // 调整列表列宽
    tagPoint pt = m_pLbPetList->GetSize();
    m_pLbPetList->SetColWidth(0, pt.x);

    m_pLbPetList->SetCurrentTextRow(0);

    // 初始窗体关闭
    ShowWnd(FALSE);

    // 清空ui
    ClearUI();

    return TRUE;
}

VOID PetFrame::Update()
{
    GameFrame::Update();
}


BOOL PetFrame::EscCancel()
{
	if( m_pWnd->IsInvisible() )
		return FALSE;

	m_pWnd->SetInvisible(TRUE);
	m_pMgr->SendEvent( &tagGameEvent( _T("Close_Pet_UI"), this ) );
	return TRUE;
}



DWORD PetFrame::OnOpen_Pet( tagGameEvent* pMsg )
{
    ShowWnd(TRUE);

    return 0;
}

DWORD PetFrame::OnClose_Pet( tagGameEvent* pMsg )
{
	ShowWnd(FALSE);

    return 0;
}

VOID PetFrame::ShowWnd( BOOL bVal )
{
    m_pWnd->SetInvisible(!bVal);
    if (bVal)
    {
        m_pWnd->FlipToTop();
        RefreshPetList();


        // 选择宠物
        map<INT, DWORD>::const_iterator it = m_mapRow2Pet.find(m_pLbPetList->GetCurSelectedRow());
        if (it != m_mapRow2Pet.end())
            m_dwCurrSelectPetID = it->second;
        else
            m_dwCurrSelectPetID = GT_INVALID;


        RefreshToUI();
        RefreshEquip();
        RefreshSkill();
        RefreshPetShow();
    }
    else
    {
        m_pMgr->SendEvent( &tagGameEvent( _T("Close_Pet_UI"), this ) );
    }
}

DWORD PetFrame::EventHandler( tagGUIEvent* pEvent )
{
    GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);

    switch (pEvent->eEvent)
    {
    case EGUIE_Click:
        {
            if (pWnd == m_pBtClose)
            {
                // 关闭
                ShowWnd(FALSE);
				m_pMgr->SendEvent( &tagGameEvent( _T("Close_Pet_UI"), this ) );
            }
            else if (pWnd == m_pBtCall)
            {
                // 召唤
                if (m_dwCurrSelectPetID != GT_INVALID)
                {
                    PetManager::Inst()->BeginSetPetState(m_dwCurrSelectPetID, EPS_Called, !PetManager::Inst()->IsPetCalled(m_dwCurrSelectPetID));
                }
            }
            else if (pWnd == m_pBtSetFree)
            {
                // 放生
                if (m_dwCurrSelectPetID != GT_INVALID)
                {
                    if (P_VALID(GetObj("BoxDeletePet")))
                        KillObj("BoxDeletePet");
                    CreateObj("BoxDeletePet", "MsgBox");
                    TObjRef<MsgBox>("BoxDeletePet")->Init(_T(""), g_StrTable[_T("PetDelete")], _T("BoxDeletePet"), (MsgBoxFlag)(MBF_OK | MBF_Cancel), TRUE);
                }
            }
            else if (pWnd == m_pBtPerfuse)
            {
                // 灌注
                PetManager::Inst()->BeginNC_GetPetPourExpMoneyNeed(m_dwCurrSelectPetID);
            }
            else if (pWnd == m_pBtRide)
            {
                // 预备驾御
                if (m_dwCurrSelectPetID != GT_INVALID)
                {
                    PetManager::Inst()->BeginSetPetState(m_dwCurrSelectPetID, EPS_Preparing, !PetManager::Inst()->IsPetPreparing(m_dwCurrSelectPetID));
                }
            }
            else if (pWnd == m_pBtTrade)
            {
                // 交易
                if (m_dwCurrSelectPetID != GT_INVALID)
                {
                    PetTradeUIFrame* pFrame = (PetTradeUIFrame*)m_pMgr->GetFrame(_T("PetTradeUIFrame"));
                    if (P_VALID(pFrame))
                    {
                        pFrame->BeginNC_PetExchangeAdd(m_dwCurrSelectPetID);
                    }
                }
            }
            else if (pWnd->GetName().substr(0, 4) == _T("item"))
            {
                for (int i = 0; i < MAX_PET_EQUIP; ++i)
                {
                    if (m_pBtEquipA[i] == pWnd)
                    {
                        const tagPet* pPet = PetManager::Inst()->GetPet(m_dwCurrSelectPetID);
                        if (P_VALID(pPet))
                        {
                            tagPet::_pos2Equip::const_iterator it = pPet->mapEquip.find(i);
                            if (it != pPet->mapEquip.end())
                            {
                                // 卸载物品
                                PetManager::Inst()->BeginNC_PetUnequip(pPet->dwID, it->second->n64ItemID, GT_INVALID);
                            }
                        }

                        break;
                    }
                }
            }
            else if (pWnd->GetName().substr(0, 5) == _T("skill"))
            {
                // 点击技能
                for (int i = 0; i < MAX_PET_SKILL; ++i)
                {
                    if (m_pBtSkillA[i] == pWnd)
                    {
                        const tagPetSkillData* pSkillData = m_pBtSkillA[i]->GetSkillData();
                        if (P_VALID(pSkillData))
                        {
                            PetManager::Inst()->TryToUsePetSkill(m_dwCurrSelectPetID, pSkillData->dwPetSkillID);
                        }
                        break;
                    }
                }
            }
        }
        break;
    case EGUIE_ItemClick:
        {
            if (pWnd == m_pLbPetList)
            {
                // 选择宠物
                map<INT, DWORD>::const_iterator it = m_mapRow2Pet.find((INT)pEvent->dwParam1);
                if (it != m_mapRow2Pet.end())
                    m_dwCurrSelectPetID = it->second;
                else
                    m_dwCurrSelectPetID = GT_INVALID;

                // 刷新数据到界面
                RefreshPetShow();
                RefreshToUI();
                RefreshEquip();
                RefreshSkill();
            }
        }
        break;
    case EGUIE_Drag:
        {
            if (pWnd->GetName().substr(0, 4) == _T("item"))
            {
                for (int i = 0; i < MAX_PET_EQUIP; ++i)
                {
                    if (m_pBtEquipA[i] == pWnd)
                    {
                        DragBtn* pDrag = (DragBtn*)m_pGUI->GetWnd(_T("\\desktop\\DragButton"));
                        if (P_VALID(pDrag))
                        {
                            const tagPetEquip* petEquip = PetManager::Inst()->FindPetEquip(m_dwCurrSelectPetID, i);
                            if (P_VALID(petEquip))
                            {
                                //发送游戏事件
                                DragBtn* pDrag = (DragBtn*)m_pGUI->GetWnd(_T("\\desktop\\DragButton"));
                                if (P_VALID(pDrag))
                                {
                                    tagGUIEvent event(m_pBtEquipA[i], EGUIE_Drag, NULL);
                                    POINT pt;
                                    ::GetCursorPos(&pt);
                                    event.dwParam1 = EDT_PetEquip;
                                    event.dwParam2 = m_dwCurrSelectPetID;
                                    event.dwParam3 = i;
                                    event.dwParam4 = MAKELONG(pt.x, pt.y);
                                    pDrag->OnEvent(&event);
                                }
                            }

                        }
                    }
                }
            }
        }
        break;
    case EGUISCB_Drop:
        {
            if (pWnd->GetName().substr(0, 4) == _T("item"))
            {
                for (int i = 0; i < MAX_PET_EQUIP; ++i)
                {
                    if (m_pBtEquipA[i] == pWnd)
                    {
                        if (EDT_Item == pEvent->dwParam1)
                        {
                            Item* item = ItemMgr::Inst()->GetPocketItem(static_cast<INT16>(LOWORD(pEvent->dwParam2)));

                            if (P_VALID(item))
                            {
                                if (item->GetItemSpecFunc() == EISF_PetEquipment && m_dwCurrSelectPetID != GT_INVALID)
                                {
                                    PetManager::Inst()->BeginNC_PetEquip(m_dwCurrSelectPetID, (INT8)i, item->GetItemId());
                                }
                            }
                        }
                        else if (EDT_PetEquip == pEvent->dwParam1)
                        {
                            const tagPetEquip* pPetEquip = PetManager::Inst()->FindPetEquip(m_dwCurrSelectPetID, (INT8)pEvent->dwParam3);
                            if (P_VALID(pPetEquip))
                                PetManager::Inst()->BeginNC_PetEquipPosSwap(m_dwCurrSelectPetID, pPetEquip->n64ItemID, i);
                        }
                    }
                }
            }
        }
        break;
    case EGUIE_MoveIntoCtrl:
        {
            if (pWnd->GetName().substr(0, 4) == _T("item"))
            {
                for(INT i = 0; i < MAX_PET_EQUIP; ++i)
                {
                    if(m_pBtEquipA[i] == pWnd)
                    {
                        const tagPet* pPet = PetManager::Inst()->GetPet(m_dwCurrSelectPetID);
                        if (P_VALID(pPet))
                        {
                            tagPet::_pos2Equip::const_iterator it = pPet->mapEquip.find(i);
                            if (it != pPet->mapEquip.end())
                            {
                                ShowTip(pWnd, ToolTipCreator::Inst()->GetItemTips(it->second->dwItemProtoID).c_str());
                            }
                        }
                        break;
                    }
                }
            }
            else if (pWnd->GetName().substr(0, 5) == _T("skill"))
            {
                for (int i = 0; i < MAX_PET_SKILL; ++i)
                {
                    if (m_pBtSkillA[i] == pWnd)
                    {
                        const tagPetSkillData* pSkillData = m_pBtSkillA[i]->GetSkillData();
                        if (P_VALID(pSkillData))
                        {
                            ShowTip(pWnd, ToolTipCreator::Inst()->GetPetSkillTips(pSkillData->dwPetSkillID).c_str());
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

VOID PetFrame::RefreshToUI()
{
    // 无宠物选中，则清空界面
    if (GT_INVALID == m_dwCurrSelectPetID)
    {
        ClearUI();
        return;
    }

    // 填充数据
    const tagPet* pPet = PetManager::Inst()->GetPet(m_dwCurrSelectPetID);
    if (NULL == pPet)
    {
        ClearUI();
        return;
    }

    const tagPetProto* pPetProto = PetProtoData::Inst()->FindPetProto(pPet->dwTypeID);
    if (!P_VALID(pPetProto))
    {
        ClearUI();
        return;
    }

    //m_pStcState->SetText(TtoString<INT>(pPet->nAttrA[ECSPA_PetState]).c_str());
    tstringstream someText;
    someText << g_StrTable[_T("PetCarryLevel")] << PetManager::Inst()->GetCarryLevel(m_dwCurrSelectPetID);
    if (pPetProto->bBinding)
        someText << _T('\n') << g_StrTable[_T("PetBinding")];
    if (pPet->nAttrA[ECSPA_bLocked])
        someText << _T('\n') << g_StrTable[_T("PetLocked")];
    m_pStcPetShow->SetText(someText.str().c_str());
    m_pStcState->SetRefresh(TRUE);
    m_pStcQuality->SetText(GetQualityText(pPet->nAttrA[ECSPA_nQuality]));
    m_pStcQuality->SetRefresh(TRUE);
    m_pStcMatureTalent->SetText(TtoString<INT>(pPet->nAttrA[ECSPA_nAptitude]).c_str());
    m_pStcMatureTalent->SetRefresh(TRUE);
    m_pStcPotential->SetText(TtoString<INT>(pPet->nAttrA[ECSPA_nPotential]).c_str());
    m_pStcPotential->SetRefresh(TRUE);
    m_pStcMagic->SetText(TtoString<INT>(pPet->nAttrA[ECSPA_nWuXingEnergy]).c_str());
    m_pStcMagic->SetRefresh(TRUE);
    tstringstream energy;
    energy << pPet->nAttrA[ECSPA_nSpirit] << _T("/") << pPet->nAttrA[ECSPA_nSpiritMax];
    m_pStcEnergy->SetText(energy.str().c_str());
    m_pStcEnergy->SetRefresh(TRUE);
    m_pStcRidingSpeed->SetText(TtoString<INT>(pPet->nAttrA[ECSPA_nMountSpeed]).c_str());
    m_pStcRidingSpeed->SetRefresh(TRUE);
    m_pStcLevel->SetText(GetLevelText(pPet->nAttrA[ECSPA_nLevel]).c_str());
    m_pStcLevel->SetRefresh(TRUE);
    m_pStcCurrExp->SetText(TtoString<INT>(pPet->nAttrA[ECSPA_nExpCurrent]).c_str());
    m_pStcCurrExp->SetRefresh(TRUE);
    m_pStcLevelUpNeed->SetText(TtoString<INT>(pPet->nAttrA[ECSPA_nExpLevelUp]).c_str());
    m_pStcLevelUpNeed->SetRefresh(TRUE);

    m_pStcNick->SetText(pPet->strName.c_str());
    m_pStcNick->SetRefresh(TRUE);

    // 设置按钮
   if (pPet->state.bisCalled)
   {
       m_pBtCall->SetText(g_StrTable[_T("PetButtonRest")]);
   }
   else
   {
       m_pBtCall->SetText(g_StrTable[_T("PetButtonCall")]);
   }

   if (pPet->state.bisPreparing)
   {
       m_pBtRide->SetText(g_StrTable[_T("PetButtonUnPrepare")]);
   }
   else
   {
       m_pBtRide->SetText(g_StrTable[_T("PetButtonPrepare")]);
   }

   if (pPet->nAttrA[ECSPA_bLocked] || pPetProto->bBinding)
   {
       m_pBtTrade->SetEnable(false);
   }
   else
   {
       m_pBtTrade->SetEnable(true);
   }

   if (pPet->nAttrA[ECSPA_bLocked])
   {
       m_pBtSetFree->SetEnable(false);
   }
   else
   {
       m_pBtSetFree->SetEnable(true);
   }

   m_pBtCall->SetEnable(true);
   m_pBtPerfuse->SetEnable(true);
   m_pBtRide->SetEnable(true);
}

VOID PetFrame::RefreshPetList()
{
    // 清除
    m_mapRow2Pet.clear();
    m_pLbPetList->Clear();

    // 填充
    const PetManager::_id2pet* pPetList = PetManager::Inst()->GetPetList();
    int i = 0;
    for (PetManager::_id2pet::const_iterator it = pPetList->begin();
        it != pPetList->end(); ++it)
    {
        DWORD dwColor = GT_INVALID;
        
        if (it->second->dwID == m_dwCurrSelectPetID)
        {
            m_pLbPetList->SetCurSelectedRow(i);
        }

        if (it->second->dwID == PetManager::Inst()->GetCurrPetID())
        {
            dwColor = 0xFFFFAE00;
        }
        m_pLbPetList->SetText(i, 0, it->second->strName.c_str(), dwColor);
        m_mapRow2Pet.insert(make_pair(i, it->second->dwID));

        ++i;
    }

    // 选择宠物
    map<INT, DWORD>::const_iterator it = m_mapRow2Pet.find(m_pLbPetList->GetCurSelectedRow());
    if (it != m_mapRow2Pet.end())
        m_dwCurrSelectPetID = it->second;
    else
        m_dwCurrSelectPetID = GT_INVALID;

    // 刷新数据到界面
    RefreshPetShow();
    RefreshToUI();
    RefreshEquip();
    RefreshSkill();
}

VOID PetFrame::ClearUI()
{
    // 装备
    for (int i = 0; i != MAX_PET_EQUIP; ++i)
        m_pBtEquipA[i]->RefreshItem(GT_INVALID);

    // 技能
    for (int i = 0; i != MAX_PET_SKILL; ++i)
        m_pBtSkillA[i]->RefrashSkill(GT_INVALID, GT_INVALID);

    // 文本框
    m_pStcState->SetText(_T(""));
    m_pStcQuality->SetText(_T(""));
    m_pStcMatureTalent->SetText(_T(""));
    m_pStcPotential->SetText(_T(""));
    m_pStcMagic->SetText(_T(""));
    m_pStcEnergy->SetText(_T(""));
    m_pStcRidingSpeed->SetText(_T(""));
    m_pStcNick->SetText(_T(""));
    m_pStcLevel->SetText(_T(""));
    m_pStcCurrExp->SetText(_T(""));
    m_pStcLevelUpNeed->SetText(_T(""));

    m_pBtCall->SetText(g_StrTable[_T("PetButtonCall")]);
    m_pBtCall->SetEnable(false);
    m_pBtSetFree->SetEnable(false);
    m_pBtPerfuse->SetEnable(false);
    m_pBtRide->SetText(g_StrTable[_T("PetButtonPrepare")]);
    m_pBtRide->SetEnable(false);
    m_pBtTrade->SetEnable(true);

}

DWORD PetFrame::OnUsePetEquipHandler( tagUsePetEquip* pEvent )
{
    DWORD CurrPetID = PetManager::Inst()->GetCurrPetID();

    // is there an active pet ?
    if (GT_INVALID == CurrPetID)
    {
        CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("PetErrorMsg_14")]);
        return 0;
    }

    // try to equip
    PetManager::Inst()->BeginNC_PetEquip(CurrPetID, GT_INVALID, pEvent->n64ItemID);

    return 0;
}

DWORD PetFrame::OnPetAttrChange( tagPetAttrChange* pEvent )
{
    switch (pEvent->eType)
    {
    case EPACT_Init:
        RefreshPetList();
        RefreshToUI();
        RefreshEquip();
        RefreshSkill();
        RefreshPetShow();
    	break;
    case EPACT_Remove:
        RefreshPetList();
        RefreshToUI();
        RefreshEquip();
        RefreshSkill();
        RefreshPetShow();
        break;
    case EPACT_PetValue:
        RefreshPetList();
        RefreshToUI();
        break;
    case EPACT_PetState:
        RefreshPetList();
        RefreshToUI();
        break;
    case EPACT_PetDisp:
        RefreshPetList();
        RefreshToUI();
        break;
    case EPACT_PetEquip:
        RefreshToUI();
        RefreshEquip();
        break;
    case EPACT_PetUnEquip:
        RefreshEquip();
        RefreshPetList();
        break;
    case EPACT_PetSkill:
        RefreshSkill();
        break;
    }

    return 0;
}

VOID PetFrame::RefreshEquip()
{
    // no pet selected
    if (GT_INVALID == m_dwCurrSelectPetID)
    {
        ClearUI();
        return;
    }

    // fill equipment
    for (int i = 0; i < MAX_PET_EQUIP; ++i)
    {
        const tagPetEquip* petEquip = PetManager::Inst()->FindPetEquip(m_dwCurrSelectPetID, (INT8)i);
        if (P_VALID(petEquip))
        {
            m_pBtEquipA[i]->RefreshItem(petEquip->dwItemProtoID);
        }
        else
        {
            m_pBtEquipA[i]->RefreshItem();
        }
    }
}

VOID PetFrame::Render3D()
{
    m_pStcPetShow->Render3D();
    GameFrame::Render3D();
}

VOID PetFrame::RefreshPetShow()
{
    if (m_dwCurrSelectPetID != GT_INVALID)
    {
        const tagPet* pet = PetManager::Inst()->GetPet(m_dwCurrSelectPetID);
        if (P_VALID(pet))
        {
            m_pStcPetShow->SetPetShow(pet->dwTypeID);
        }
        else
        {
            m_pStcPetShow->SetPetShow(GT_INVALID);
        }
    }
    else
    {
        m_pStcPetShow->SetPetShow(GT_INVALID);
    }
        
}

VOID PetFrame::RefreshSkill()
{
    // no pet selected
    if (GT_INVALID == m_dwCurrSelectPetID)
    {
        ClearUI();
        return;
    }

    // clear skill container
    for (int i = 0; i != MAX_PET_SKILL; ++i)
        m_pBtSkillA[i]->RefrashSkill(GT_INVALID, GT_INVALID);

    // fill skill
    int i = 0;
    const tagPet* pPet = PetManager::Inst()->GetPet(m_dwCurrSelectPetID);
    if (P_VALID(pPet))
    {
        for (tagPet::_id2skillInfo::const_iterator it = pPet->Skill.begin();
            it != pPet->Skill.end(); ++it)
        {
            m_pBtSkillA[i]->RefrashSkill(m_dwCurrSelectPetID, it->second->dwTypeID);
            ++i;
        }
    }
}

DWORD PetFrame::OnCommitDeletePet( tagMsgBoxEvent* pEvent )
{
    if (MBF_OK == pEvent->eResult)
    {
        PetManager::Inst()->BeginNC_DeletePet(m_dwCurrSelectPetID);
    }

    return 0;
}

bool PetFrame::IsWindowVisible()
{
    if (P_VALID(m_pWnd) && !m_pWnd->IsInvisible())
    {
        return true;
    }
    else
    {
        return false;
    }
}

tstring PetFrame::GetLevelText( int nLevel )
{
    // 获得等级
    int step, grade;
    TransVLevel2StepGrade(nLevel, step, grade);

    TCHAR szTxt[256];
    _sntprintf(szTxt, sizeof(szTxt) / sizeof(TCHAR), g_StrTable[_T("PetLevel")], step, grade);

    return szTxt;
}

DWORD PetFrame::OnCommitPourExp( tagMsgBoxEvent* pEvent )
{
    if (MBF_OK == pEvent->eResult)
    {
        // 判断等级
        const tagPet* pPet = PetManager::Inst()->GetPet(m_dwCurrSelectPetID);
        if (P_VALID(pPet))
        {
            int nStep, nGrade;
            TransVLevel2StepGrade(pPet->nAttrA[ECSPA_nLevel], nStep, nGrade);
            if (pPet->nAttrA[ECSPA_nExpCurrent] == pPet->nAttrA[ECSPA_nExpLevelUp])
            {
                CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("PetLveelFull")]);
            }
            else
            {
                PetManager::Inst()->BeginPourExp(m_dwCurrSelectPetID);
            }
        }
    }

    return 0;
}

LPCTSTR PetFrame::GetQualityText( int nQuality )
{
    if (nQuality < 0 || nQuality >= 5)
        return _T("");
    tstringstream sstream;
    sstream << _T("PetQuality_") << nQuality;
    return g_StrTable[sstream.str().c_str()];
}