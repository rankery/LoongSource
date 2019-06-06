#include "StdAfx.h"
#include "PetInformationFrame.h"
#include "PetDef.h"
#include "PetEvent.h"
#include "PetShowWindow.h"
#include "PetManager.h"
#include "ItemProtoData.h"
#include "ToolTipCreator.h"
#include "ToolTipFrame.h"

PetInformationFrame::PetInformationFrame(void) :
    m_Trunk(this),
    m_pWnd(NULL),
    m_pCurrPet(NULL)
{
}

PetInformationFrame::~PetInformationFrame(void)
{
}

BOOL PetInformationFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);

    // 注册游戏事件
    m_pFrameMgr->RegisterEventHandle(_T("tagPetAttrChange"),  (FRAMEEVENTPROC)m_Trunk.sfp1(&PetInformationFrame::OnPetAttrChange));

    return bRet;
}

BOOL PetInformationFrame::Destroy()
{
    BOOL bRet = GameFrame::Destroy();

    //! 删掉pet
    if (P_VALID(m_pCurrPet))
        SAFE_DELETE(m_pCurrPet);

    //! 销毁窗体
    if( P_VALID(m_pWnd) )
    {
        m_pGUI->AddToDestroyList(m_pWnd);
        m_pWnd = NULL;
    }

    // 注销游戏事件
    m_pFrameMgr->UnRegisterEventHandler(_T("tagPetAttrChange"),     (FRAMEEVENTPROC)m_Trunk.sfp1(&PetInformationFrame::OnPetAttrChange));

    return bRet;
}

BOOL PetInformationFrame::ReloadUI()
{
    XmlElement ele;
    tstring strPath = g_strLocalPath + _T("\\ui\\pet2.xml");
    m_pGUI->LoadXml(&ele, "VFS_System", strPath.c_str());

    // 更改控件
    char chbuff[128];
    for (int i = 1; i != MAX_PET_EQUIP + 1; ++i)
    {
        _snprintf(chbuff, sizeof(chbuff) / sizeof(char), "pet2_win\\pet2back_pic\\pet2_pic\\pet_equipmentback\\itemback%d\\item%d", i, i);
        m_pGUI->ChangeXml(&ele, chbuff, "ClassName", "ItemButton");
    }
    for (int i = 1; i != MAX_PET_SKILL + 1; ++i)
    {
        _snprintf(chbuff, sizeof(chbuff) / sizeof(char), "pet2_win\\pet2back_pic\\pet2_pic\\petskill_back\\skilback%d\\skill%d", i, i);
        m_pGUI->ChangeXml(&ele, chbuff, "ClassName", "PetSkillButton");
    }

    m_pGUI->ChangeXml(&ele, "pet2_win\\pet2back_pic\\pet2_pic\\petshow_back", "ClassName", "PetShowWindow");

    // 创建
    m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &ele);

    // 注册事件处理函数
    m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&PetInformationFrame::EventHandler));

    // 获得控件
    TCHAR tbuff[128];
    for (int i = 0; i != MAX_PET_EQUIP; ++i)
    {
        int index = i + 1;
        _sntprintf(tbuff, sizeof(tbuff) / sizeof(TCHAR), _T("pet2back_pic\\pet2_pic\\pet_equipmentback\\itemback%d\\item%d"), index, index);
        m_pBtEquipA[i]  =   static_cast<ItemButton*>(m_pWnd->GetChild(tbuff));          // 宠物装备
    }
    for (int i = 0; i != MAX_PET_SKILL; ++i)
    {
        int index = i + 1;
        _sntprintf(tbuff, sizeof(tbuff) / sizeof(TCHAR), _T("pet2back_pic\\pet2_pic\\petskill_back\\skilback%d\\skill%d"), index, index);
        m_pBtSkillA[i]  =   static_cast<PetSkillButton*>(m_pWnd->GetChild(tbuff));         // 宠物技能
    }

    m_pStcPetShow       =   static_cast<PetShowWindow*>(m_pWnd->GetChild(_T("pet2back_pic\\pet2_pic\\petshow_back")));                // 宠物Avata

    m_pStcQuality       =   static_cast<GUIStatic*>(m_pWnd->GetChild(_T("pet2back_pic\\pet2_pic\\petproperty_back1\\num1")));         // 宠物智商
    m_pStcMatureTalent  =   static_cast<GUIStatic*>(m_pWnd->GetChild(_T("pet2back_pic\\pet2_pic\\petproperty_back1\\num2")));         // 成长资质
    m_pStcPotential     =   static_cast<GUIStatic*>(m_pWnd->GetChild(_T("pet2back_pic\\pet2_pic\\petproperty_back1\\num5")));         // 潜能
    m_pStcMagic         =   static_cast<GUIStatic*>(m_pWnd->GetChild(_T("pet2back_pic\\pet2_pic\\petproperty_back1\\num4")));         // 灵力
    m_pStcEnergy        =   static_cast<GUIStatic*>(m_pWnd->GetChild(_T("pet2back_pic\\pet2_pic\\petproperty_back1\\num3")));         // 精气
    m_pStcRidingSpeed   =   static_cast<GUIStatic*>(m_pWnd->GetChild(_T("pet2back_pic\\pet2_pic\\petproperty_back1\\num6")));         // 骑乘速度

    m_pStcNick          =   static_cast<GUIStatic*>(m_pWnd->GetChild(_T("pet2back_pic\\pet2_pic\\petsituation_back\\name")));         // 宠物昵称
    m_pStcLevel         =   static_cast<GUIStatic*>(m_pWnd->GetChild(_T("pet2back_pic\\pet2_pic\\petsituation_back\\stage")));        // 级别
    m_pStcCurrExp       =   static_cast<GUIStatic*>(m_pWnd->GetChild(_T("pet2back_pic\\pet2_pic\\petsituation_back\\num5")));         // 当前经验
    m_pStcLevelUpNeed   =   static_cast<GUIStatic*>(m_pWnd->GetChild(_T("pet2back_pic\\pet2_pic\\petsituation_back\\num6")));         // 升级经验

    m_pBtClose          =   static_cast<GUIButton*>(m_pWnd->GetChild(_T("pet2back_pic\\closebutt")));                                // 关闭

    // 默认关闭窗口
    ShowWnd(FALSE);

    return TRUE;
}

VOID PetInformationFrame::Update()
{

}

VOID PetInformationFrame::Render3D()
{
    m_pStcPetShow->Render3D();
    GameFrame::Render3D();
}

BOOL PetInformationFrame::EscCancel()
{
    if( m_pWnd->IsInvisible() )
        return FALSE;

    m_pWnd->SetInvisible(TRUE);

    return TRUE;
}

VOID PetInformationFrame::ShowWnd( BOOL bVal )
{
    m_pWnd->SetInvisible(!bVal);
    if (bVal)
    {
        m_pWnd->FlipToTop();
    }
    else
    {
    }
}
DWORD PetInformationFrame::EventHandler( tagGUIEvent* pEvent )
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
                        if (P_VALID(m_pCurrPet))
                        {
                            tagPet::_pos2Equip::const_iterator it = m_pCurrPet->mapEquip.find(i);
                            if (it != m_pCurrPet->mapEquip.end())
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

DWORD PetInformationFrame::OnGetPetAttrEvent( tagNS_GetPetAttr* pMsg )
{
    // 清空ui
    ClearUI();

    // 填充数据
    tagPet* pPetData = new tagPet();
    pPetData->dwID                          =   pMsg->petAttr.dwID;
    pPetData->dwTypeID                      =   pMsg->petAttr.dwTypeID;
    pPetData->nAttrA[ECSPA_nQuality]        =   pMsg->petAttr.nQuality;
    pPetData->nAttrA[ECSPA_nAptitude]       =   pMsg->petAttr.nAptitude;
    pPetData->nAttrA[ECSPA_nSpiritMax]      =   pMsg->petAttr.nSpiritMax;
    pPetData->nAttrA[ECSPA_nSpirit]         =   pMsg->petAttr.nSpirit;
    pPetData->nAttrA[ECSPA_nPotential]      =   pMsg->petAttr.nPotential;
    pPetData->nAttrA[ECSPA_nWuXingEnergy]   =   pMsg->petAttr.nWuXingEnergy;
    pPetData->nAttrA[ECSPA_nLevel]          =   pMsg->petAttr.nLevel;
    pPetData->nAttrA[ECSPA_nExpCurrent]     =   pMsg->petAttr.nExpCurrent;
    pPetData->nAttrA[ECSPA_nExpLevelUp]     =   pMsg->petAttr.nExpLevelUp;
    pPetData->nAttrA[ECSPA_nMountSpeed]     =   pMsg->petAttr.nMountSpeed;
    pPetData->nAttrA[ECSPA_BagGrid]         =   pMsg->petAttr.nBagGrid;
    pPetData->nAttrA[ECSPA_PetState]        =   pMsg->petAttr.byPetState;
    pPetData->nAttrA[ECSPA_nMountNum]       =   pMsg->petAttr.nMountNum;
    pPetData->nAttrA[ECSPA_nMountNumMax]    =   pMsg->petAttr.nMountNumMax;
    pPetData->state.byPetState              =   static_cast<BYTE>(pMsg->petAttr.byPetState);

    const tagPetDispInfo* petInfo = PetManager::Inst()->FindPetDispInfo(pMsg->dwRoleID, pMsg->petAttr.dwID);
    if (P_VALID(petInfo))
    {
        pPetData->strName = petInfo->szName;
    }

    tagPetEquipMsgInfo* pEquipAddr      =   reinterpret_cast<tagPetEquipMsgInfo*>(pMsg->petAttr.byData);
    tagPetSkillMsgInfo* pSkillAddr      =   reinterpret_cast<tagPetSkillMsgInfo*>(pEquipAddr + pMsg->petAttr.nPetEquipNum);

    // 填充装备
    for (int i = 0; i != pMsg->petAttr.nPetEquipNum; ++i)
    {
        // 获取装备数据
        tagPetEquipMsgInfo equipData = pEquipAddr[i];

        const tagPetEquipProto* pData = ItemProtoData::Inst()->FindPetEquipByItemID(equipData.dwItemProtoID);
        if (P_VALID(pData))
        {
            // 删除原位置装备
            tagPet::_pos2Equip::iterator equipIt;
            equipIt = pPetData->mapEquip.find(equipData.n8Pos);
            if (equipIt != pPetData->mapEquip.end())
            {
                SAFE_DELETE(equipIt->second);
                pPetData->mapEquip.erase(equipIt);
            }

            // 新建装备
            tagPetEquip* pEquip = new tagPetEquip();
            pEquip->n64ItemID       =   equipData.n64ItemID;
            pEquip->dwItemProtoID   =   equipData.dwItemProtoID;

            // 放入相应装备位置
            if (!pPetData->mapEquip.insert(make_pair(equipData.n8Pos, pEquip)).second)
            {
                SAFE_DELETE(pEquip);
            }
        }
    }

    // 填充技能
    for (int i = 0; i != pMsg->petAttr.nPetSkillNum; ++i)
    {
        // 获取技能数据
        tagPetSkillMsgInfo skillData = pSkillAddr[i];

        // 删除原技能
        tagPet::_id2skillInfo::iterator skillIt;
        skillIt = pPetData->Skill.find(skillData.dwTypeID);
        if (skillIt != pPetData->Skill.end())
        {
            SAFE_DELETE(skillIt->second);
            pPetData->Skill.erase(skillData.dwTypeID);
        }

        // 新建技能
        tagPetSkill* pSkill = new tagPetSkill();
        pSkill->dwTypeID        =   skillData.dwTypeID;
        pSkill->nCurrCoolDown   =   skillData.nCurrCoolDown;
        pSkill->nMaxCoolDown    =   skillData.nMaxCoolDown;
        if (skillData.nMaxCoolDown != GT_INVALID)
        {
            pSkill->bCoolingDown    =   true;
            pSkill->nCurrCoolDown   =   skillData.nCurrCoolDown;
            pSkill->dwTimeFirst     =   Kernel::Inst()->GetAccumTimeDW();
        }
        else
        {
            pSkill->bCoolingDown    =   false;
            pSkill->nCurrCoolDown   =   0;
            pSkill->dwTimeFirst     =   0;
        }

        if (!pPetData->Skill.insert(make_pair(pSkill->dwTypeID, pSkill)).second)
        {
            SAFE_DELETE(pSkill);
        }
    }

    m_currRoleId = pMsg->dwRoleID;
    SetPet(pPetData);

    return 0;
}

VOID PetInformationFrame::ClearUI()
{
    // 装备
    for (int i = 0; i != MAX_PET_EQUIP; ++i)
        m_pBtEquipA[i]->RefreshItem(GT_INVALID);

    // 技能
    for (int i = 0; i != MAX_PET_SKILL; ++i)
        m_pBtSkillA[i]->RefrashSkill(GT_INVALID, GT_INVALID);

    // 文本框
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
}

void PetInformationFrame::SetPet( tagPet* pPet )
{
    if (P_VALID(m_pCurrPet))
        SAFE_DELETE(m_pCurrPet);

    m_pCurrPet = pPet;

    //! 刷新界面数据
    RefreshToUI();
    //! 刷新当前装备
    RefreshEquip();
    //! 刷新当前技能
    RefreshSkill();
    //! 刷新当前模型
    RefreshPetShow();

    ShowWnd(TRUE);
}

DWORD PetInformationFrame::OnPetAttrChange( tagPetAttrChange* pEvent )
{
    if (EPACT_PetDisp == pEvent->eType)
    {
        if (P_VALID(m_pCurrPet) && pEvent->dwPetID == m_pCurrPet->dwID)
        {
            const tagPetDispInfo* petInfo = PetManager::Inst()->FindPetDispInfo(m_currRoleId, m_pCurrPet->dwID);
            if (P_VALID(petInfo))
            {
                m_pCurrPet->strName     =   petInfo->szName;
            }
        }
    }

    RefreshToUI();

    return 0;
}

VOID PetInformationFrame::RefreshToUI()
{
    // 无宠物选中，则清空界面
    if (!P_VALID(m_pCurrPet))
    {
        ClearUI();
        return;
    }

    // 填充数据

    m_pStcQuality->SetText(TtoString<INT>(m_pCurrPet->nAttrA[ECSPA_nQuality]).c_str());
    m_pStcMatureTalent->SetText(TtoString<INT>(m_pCurrPet->nAttrA[ECSPA_nAptitude]).c_str());
    m_pStcPotential->SetText(TtoString<INT>(m_pCurrPet->nAttrA[ECSPA_nPotential]).c_str());
    m_pStcMagic->SetText(TtoString<INT>(m_pCurrPet->nAttrA[ECSPA_nWuXingEnergy]).c_str());
    tstringstream energy;
    energy << m_pCurrPet->nAttrA[ECSPA_nSpirit] << _T("/") << m_pCurrPet->nAttrA[ECSPA_nSpiritMax];
    m_pStcEnergy->SetText(energy.str().c_str());
    m_pStcRidingSpeed->SetText(TtoString<INT>(m_pCurrPet->nAttrA[ECSPA_nMountSpeed]).c_str());
    m_pStcLevel->SetText(GetLevelText(m_pCurrPet->nAttrA[ECSPA_nLevel]).c_str());
    m_pStcCurrExp->SetText(TtoString<INT>(m_pCurrPet->nAttrA[ECSPA_nExpCurrent]).c_str());
    m_pStcLevelUpNeed->SetText(TtoString<INT>(m_pCurrPet->nAttrA[ECSPA_nExpLevelUp]).c_str());

    m_pStcNick->SetText(m_pCurrPet->strName.c_str());
}

VOID PetInformationFrame::RefreshEquip()
{
    // 无宠物选中，则清空界面
    if (!P_VALID(m_pCurrPet))
    {
        ClearUI();
        return;
    }

    // fill equipment
    for (int i = 0; i < MAX_PET_EQUIP; ++i)
    {
        tagPet::_pos2Equip::const_iterator it = m_pCurrPet->mapEquip.find(i);
        if (it != m_pCurrPet->mapEquip.end())
        {
            const tagPetEquip* petEquip = it->second;
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
}

VOID PetInformationFrame::RefreshSkill()
{
    // 无宠物选中，则清空界面
    if (!P_VALID(m_pCurrPet))
    {
        ClearUI();
        return;
    }

    // clear skill container
    for (int i = 0; i != MAX_PET_SKILL; ++i)
        m_pBtSkillA[i]->RefrashSkill(GT_INVALID, GT_INVALID);

    // fill skill
    int i = 0;
    
    for (tagPet::_id2skillInfo::const_iterator it = m_pCurrPet->Skill.begin();
        it != m_pCurrPet->Skill.end(); ++it)
    {
        m_pBtSkillA[i]->RefrashSkill(m_pCurrPet->dwID, it->second->dwTypeID);
        ++i;
    }
}

VOID PetInformationFrame::RefreshPetShow()
{
    // 无宠物选中，则清空界面
    if (!P_VALID(m_pCurrPet))
    {
        ClearUI();
        m_pStcPetShow->SetPetShow(GT_INVALID);
        return;
    }

    m_pStcPetShow->SetPetShow(m_pCurrPet->dwTypeID);
}

tstring PetInformationFrame::GetLevelText( int nLevel )
{
    // 获得等级
    int step, grade;
    TransVLevel2StepGrade(nLevel, step, grade);

    TCHAR szTxt[256];
    _sntprintf(szTxt, sizeof(szTxt) / sizeof(TCHAR), g_StrTable[_T("PetLevel")], step, grade);

    return szTxt;
}

VOID PetInformationFrame::GetPetReq( DWORD dwRoleID, DWORD dwPetID )
{
    ClearUI();

    ShowWnd(FALSE);

    SetPet(NULL);

    PetManager::Inst()->BeginNC_GetPetAttr(dwRoleID, dwPetID);
}