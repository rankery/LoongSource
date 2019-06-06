#include "StdAfx.h"
#include "PetManager.h"
#include "PetDef.h"
#include "PetEvent.h"
#include "RoleMgr.h"
#include "MsgBox.h"
#include "CombatSysUtil.h"
#include "FilterData.h"
#include "ItemMgr.h"
#include "Item.h"
#include "ItemProtoData.h"
#include "PetProtoData.h"
#include "PetInformationFrame.h"
#include "GroundItemMgr.h"
#include "LocalPlayer.h"
#include "NPC.h"
#include "PetBagFrame.h"
#include "../WorldDefine/pet_skill_define.h"
#include "../WorldDefine/loot.h"
#include "../WorldDefine/currency_define.h"


PetManager::PetManager(void) :
    m_Trunk(this),
    m_nPetNameMaxLength(X_SHORT_NAME),
    m_n64CurrPetEggID(GT_INVALID),
    bHasInitialized(false),
    m_autoSkillFirst(0),
    m_ErrGroundItemTime(0)
{
}

PetManager::~PetManager(void)
{
    ClearAll();
}

PetManager g_petManager;
PetManager* PetManager::Inst()
{
    return &g_petManager;
}

VOID PetManager::Init()
{
    // 获取工具
    m_pCmdMgr       =   TObjRef<NetCmdMgr>();
    m_pUtil         =   TObjRef<Util>();
    m_pFrameMgr     =   TObjRef<GameFrameMgr>();
    m_pSession      =   TObjRef<NetSession>();

    ClearAll();

    // 注册网络消息处理函数
    m_pCmdMgr->Register("NS_GetPetAttr",        (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_GetPetAttr),          _T("NS_GetPetAttr"));
    m_pCmdMgr->Register("NS_GetPetDispInfo",    (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_GetPetDispInfo),      _T("NS_GetPetDispInfo"));
    m_pCmdMgr->Register("NS_PetDispInfoChange", (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_PetDispInfoChange),   _T("NS_PetDispInfoChange"));
    m_pCmdMgr->Register("NS_UsePetEgg",         (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_UsePetEgg),           _T("NS_UsePetEgg"));
    m_pCmdMgr->Register("NS_PetAttrChange",     (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_PetAttrChange),       _T("NS_PetAttrChange"));
    m_pCmdMgr->Register("NS_PetEquip",          (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_PetEquip),            _T("NS_PetEquip"));
    m_pCmdMgr->Register("NS_PetUnequip",        (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_PetUnequip),          _T("NS_PetUnequip"));
    m_pCmdMgr->Register("NS_PetEquipPosSwap",   (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_PetEquipPosSwap),     _T("NS_PetEquipPosSwap"));
    m_pCmdMgr->Register("NS_PetPourExp",        (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_PetPourExp),          _T("NS_PetPourExp"));
    m_pCmdMgr->Register("NS_PetSkillUse",       (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_PetSkillUse),         _T("NS_PetSkillUse"));
    m_pCmdMgr->Register("NS_AddPetSkill",       (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_AddPetSkill),         _T("NS_AddPetSkill"));
    m_pCmdMgr->Register("NS_RemovePetSkill",    (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_RemovePetSkill),      _T("NS_RemovePetSkill"));
    m_pCmdMgr->Register("NS_PetLearnSkill",     (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_PetLearnSkill),       _T("NS_PetLearnSkill"));
    m_pCmdMgr->Register("NS_DeletePet",         (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_DeletePet),           _T("NS_DeletePet"));
    m_pCmdMgr->Register("NS_GetPetPourExpMoneyNeed",(NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_GetPetPourExpMoneyNeed),_T("NS_GetPetPourExpMoneyNeed"));
    m_pCmdMgr->Register("NS_PetUpStep",         (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_PetUpStep),           _T("NS_PetUpStep"));
    m_pCmdMgr->Register("NS_PetEnhance",        (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_PetEnhance),          _T("NS_PetEnhance"));
    m_pCmdMgr->Register("NS_PetSetLock",        (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_PetSetLock),          _T("NS_PetSetLock"));
    m_pCmdMgr->Register("NS_PetFood",           (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_PetFood),             _T("NS_PetFood"));
    m_pCmdMgr->Register("NS_PetSkillTimer",     (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_PetSkillTimer),       _T("NS_PetSkillTimer"));
    
    
    // 注册游戏消息处理函数
    m_pFrameMgr->RegisterEventHandle(_T("tagUsePetEggEvent"),   (FRAMEEVENTPROC)m_Trunk.sfp1(&PetManager::OnUsePetEggEventHandler));
    m_pFrameMgr->RegisterEventHandle(_T("InputPetName"),        (FRAMEEVENTPROC)m_Trunk.sfp1(&PetManager::OnCommitPetName));
    m_pFrameMgr->RegisterEventHandle(_T("BoxPetUpStep"),        (FRAMEEVENTPROC)m_Trunk.sfp1(&PetManager::OnBoxPetUpStep));
    m_pFrameMgr->RegisterEventHandle(_T("BoxUseExpPour"),       (FRAMEEVENTPROC)m_Trunk.sfp1(&PetManager::OnBoxUseExpPour));
    m_pFrameMgr->RegisterEventHandle(_T("tagUsePetFoodEvent"),  (FRAMEEVENTPROC)m_Trunk.sfp1(&PetManager::OnUsePetFood));
    
}

VOID PetManager::Destroy()
{
    // 注册网络消息处理函数
    m_pCmdMgr->UnRegister("NS_GetPetAttr",          (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_GetPetAttr));
    m_pCmdMgr->UnRegister("NS_GetPetDispInfo",      (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_GetPetDispInfo));
    m_pCmdMgr->UnRegister("NS_PetDispInfoChange",   (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_PetDispInfoChange));
    m_pCmdMgr->UnRegister("NS_UsePetEgg",           (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_UsePetEgg));
    m_pCmdMgr->UnRegister("NS_PetAttrChange",       (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_PetAttrChange));
    m_pCmdMgr->UnRegister("NS_PetEquip",            (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_PetEquip));
    m_pCmdMgr->UnRegister("NS_PetUnequip",          (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_PetUnequip));
    m_pCmdMgr->UnRegister("NS_PetEquipPosSwap",     (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_PetEquipPosSwap));
    m_pCmdMgr->UnRegister("NS_PetPourExp",          (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_PetPourExp));
    m_pCmdMgr->UnRegister("NS_PetSkillUse",         (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_PetSkillUse));
    m_pCmdMgr->UnRegister("NS_AddPetSkill",         (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_AddPetSkill));
    m_pCmdMgr->UnRegister("NS_RemovePetSkill",      (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_RemovePetSkill));
    m_pCmdMgr->UnRegister("NS_PetLearnSkill",       (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_PetLearnSkill));
    m_pCmdMgr->UnRegister("NS_DeletePet",           (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_DeletePet));
    m_pCmdMgr->UnRegister("NS_GetPetPourExpMoneyNeed",(NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_GetPetPourExpMoneyNeed));
    m_pCmdMgr->UnRegister("NS_PetUpStep",           (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_PetUpStep));
    m_pCmdMgr->UnRegister("NS_PetEnhance",          (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_PetEnhance));
    m_pCmdMgr->UnRegister("NS_PetSetLock",          (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_PetSetLock));
    m_pCmdMgr->UnRegister("NS_PetFood",             (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_PetFood));
    m_pCmdMgr->UnRegister("NS_PetSkillTimer",       (NETMSGPROC)m_Trunk.sfp2(&PetManager::OnNS_PetSkillTimer));

    // 注销游戏消息处理函数
    m_pFrameMgr->UnRegisterEventHandler(_T("tagUsePetEggEvent"),    (FRAMEEVENTPROC)m_Trunk.sfp1(&PetManager::OnUsePetEggEventHandler));
    m_pFrameMgr->UnRegisterEventHandler(_T("InputPetName"),         (FRAMEEVENTPROC)m_Trunk.sfp1(&PetManager::OnCommitPetName));
    m_pFrameMgr->UnRegisterEventHandler(_T("BoxPetUpStep"),         (FRAMEEVENTPROC)m_Trunk.sfp1(&PetManager::OnBoxPetUpStep));
    m_pFrameMgr->UnRegisterEventHandler(_T("BoxUseExpPour"),        (FRAMEEVENTPROC)m_Trunk.sfp1(&PetManager::OnBoxUseExpPour));
    m_pFrameMgr->UnRegisterEventHandler(_T("tagUsePetFoodEvent"),   (FRAMEEVENTPROC)m_Trunk.sfp1(&PetManager::OnUsePetFood));

    ClearAll();
}

void PetManager::BeginNC_GetPetAttr(  DWORD dwRoleID, DWORD dwPetID )
{
    tagNC_GetPetAttr msg;
    msg.dwRoleID = dwRoleID;
    msg.dwPetID = dwPetID;
    m_pSession->Send(&msg);
}

DWORD PetManager::OnNS_GetPetAttr( tagNS_GetPetAttr* pMsg, DWORD )
{
    if (pMsg->dwRoleID == RoleMgr::Inst()->GetLocalPlayerID())
    {
        // 如果是本地玩家的宠物,填充属性
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
        pPetData->nAttrA[ECSPA_bLocked]         =   pMsg->petAttr.bLocked;
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
            if (skillData.nCurrCoolDown != GT_INVALID)
            {
                pSkill->bCoolingDown    =   true;
                pSkill->nMaxCoolDown    =   PetProtoData::Inst()->GetPetSkillMaxCd(pSkill->dwTypeID);
                pSkill->nCurrCoolDown   =   pSkill->nMaxCoolDown - skillData.nCurrCoolDown * TICK_TIME;
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

        _id2pet::iterator it = m_mapPetsOfLP.find(pMsg->petAttr.dwID);

        if (it != m_mapPetsOfLP.end())
        {
            SAFE_DELETE(it->second);
            it->second = pPetData;
        }
        else
        {
            _id2pet::_Pairib ret = m_mapPetsOfLP.insert(make_pair(pMsg->petAttr.dwID, pPetData));
            if (!ret.second)
                SAFE_DELETE(pPetData);
        }

        // 发送属性更新事件
        tagPetAttrChange e(_T("tagPetAttrChange"), NULL);
        e.eType     =   EPACT_Init;
        m_pFrameMgr->SendEvent(&e);
    }
    else
    {
        // 如果是远程玩家的宠物,发送游戏事件
        PetInformationFrame* pFrame = (PetInformationFrame*)m_pFrameMgr->GetFrame(_T("PetInformationFrame"));
        if (P_VALID(pFrame))
            pFrame->OnGetPetAttrEvent(pMsg);
    }

    bHasInitialized = true;

    return 0;
}

const tagPetDispInfo* PetManager::FindPetDispInfo( DWORD dwRoleID, DWORD dwPetID )
{
    if (GT_INVALID == dwRoleID)
        return NULL;

    _id2petdisp::iterator it = m_mapPetDispBuffer.find(dwPetID);
    if (it != m_mapPetDispBuffer.end())
    {
        return it->second;
    }
    else
    {
        // 发送列表里没有则发送请求
        set<DWORD>::iterator it = m_setDispSentReq.find(dwPetID);
        if (it == m_setDispSentReq.end())
        {
            tagNC_GetPetDispInfo msg;
            msg.dwRoleID    =   dwRoleID;
            msg.dwPetID     =   dwPetID;
            m_pSession->Send(&msg);

            // 插入发送请求列表中
            m_setDispSentReq.insert(dwPetID);
        }
        return NULL;
    }
}

DWORD PetManager::OnNS_GetPetDispInfo( tagNS_GetPetDispInfo* pMsg, DWORD )
{
    _id2petdisp::iterator it = m_mapPetDispBuffer.find(pMsg->dwPetID);
    if (it != m_mapPetDispBuffer.end())
    {
        *(it->second) = pMsg->DispData;
    }
    else
    {
        tagPetDispInfo* pData = new tagPetDispInfo();
        *pData = pMsg->DispData;
        _id2petdisp::_Pairib ret = m_mapPetDispBuffer.insert(make_pair(pMsg->dwPetID, pData));
        if (!ret.second)
            SAFE_DELETE(pData);
    }

    // 如果是本地玩家的宠物，装名字填充进本地宠物结构
    _id2pet::iterator itpet = m_mapPetsOfLP.find(pMsg->dwPetID);
    if (itpet != m_mapPetsOfLP.end())
    {
        itpet->second->strName = pMsg->DispData.szName;

        // 发送属性更新事件
        tagPetAttrChange e(_T("tagPetAttrChange"), NULL);
        e.eType     =   EPACT_PetDisp;
        e.dwPetID   =   pMsg->dwPetID;
        m_pFrameMgr->SendEvent(&e);
    }

    // 发送游戏事件
    tagPetDispChange e(_T("tagPetDispChange"), NULL);
    e.dwPetID   =   pMsg->dwPetID;
    m_pFrameMgr->SendEvent(&e);

    return 0;
}

DWORD PetManager::OnNS_PetDispInfoChange( tagNS_PetDispInfoChange* pMsg, DWORD )
{
    tagNS_GetPetDispInfo msg;
    msg.dwPetID  =   pMsg->dwPetID;
    msg.DispData =   pMsg->DispData;
    OnNS_GetPetDispInfo(&msg, 0);

    return 0;
}

void PetManager::ClearAll()
{
    // 释放资源
    for (_id2pet::iterator it = m_mapPetsOfLP.begin();
        it != m_mapPetsOfLP.end(); ++it)
    {
        SAFE_DELETE(it->second);
    }
    m_mapPetsOfLP.clear();

    for (_id2petdisp::iterator it = m_mapPetDispBuffer.begin();
        it != m_mapPetDispBuffer.end(); ++it)
    {
        SAFE_DELETE(it->second);
    }
    m_mapPetDispBuffer.clear();

    m_setDispSentReq.clear();
}

DWORD PetManager::OnUsePetEggEventHandler( tagUsePetEggEvent* pEvent )
{
    // 如果宠物栏位满了，则不能使用
    if (m_mapPetsOfLP.size() >= MAX_PET_COUNT)
    {
        CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("PetErrorMaxNum")]);

        return 0;
    }

    m_n64CurrPetEggID = pEvent->n64ItemID;

    if (P_VALID(GetObj("InputPetName")))
        KillObj("InputPetName");
    CreateObj("InputPetName", "MsgInputBox");
    MsgInputBox* pMsgBox = TObjRef<MsgInputBox>("InputPetName");
    
    // 宠物蛋名
    tstring strItemName;
    Item* pItem = ItemMgr::Inst()->GetPocketItemByID(pEvent->n64ItemID);
    if (P_VALID(pItem))
    {
        strItemName = pItem->GetItemName();
    }

    pMsgBox->Init(strItemName.c_str(), g_StrTable[_T("PetInfo01")], _T("InputPetName"), -1, MBIT_Name);

    return 0;
}

DWORD PetManager::OnCommitPetName( tagMsgInputBoxEvent* pEvent )
{
    if (MBF_OK == pEvent->eResult && MBIT_Name == pEvent->eInputType)
    {
        if (FilterData::Inst()->IsValidName(pEvent->strInputName.c_str(), m_nPetNameMaxLength) != E_Success)
        {
            CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("PetInfo02")]);
        }
        else if (pEvent->strInputName.length() > X_PET_NAME)
        {
            CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("PetInfo05")]);
        }
        else
        {
            tagNC_UsePetEgg msg;
            msg.n64ItemID    =   m_n64CurrPetEggID;
            _tcscpy_s(msg.szPetName, sizeof(msg.szPetName) / sizeof(TCHAR), pEvent->strInputName.c_str());
            m_pSession->Send(&msg);
        }
    }
    m_n64CurrPetEggID = GT_INVALID;

    return 0;
}

DWORD PetManager::OnNS_UsePetEgg( tagNS_UsePetEgg* pMsg, DWORD )
{
    if (pMsg->dwErrCode != E_Success)
    {
        CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("PetInfo03")]);
    }
    else
    {
        // 提示信息
        CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("PetInfo04")]);
    }

    return 0;
}

DWORD PetManager::OnNS_PetAttrChange( tagNS_PetAttrChange* pMsg, DWORD )
{
    if (pMsg->u8AttrType >= ECSPA_Begin && pMsg->u8AttrType < ECSPA_End)
    {
        _id2pet::iterator it = m_mapPetsOfLP.find(pMsg->dwPetID);
        if (it != m_mapPetsOfLP.end())
        {
            EPetAttrChangeType attrType = EPACT_PetValue;

            it->second->nAttrA[static_cast<ECSPetAtt>(pMsg->u8AttrType)] = pMsg->u32NewValue;
            if (pMsg->u8AttrType == ECSPA_PetState)
            {
                it->second->state.byPetState    =   static_cast<BYTE>(pMsg->u32NewValue);
                attrType = EPACT_PetState;
            }

            // 发送属性更新事件
            tagPetAttrChange e(_T("tagPetAttrChange"), NULL);
            e.eType     =   attrType;
            e.dwPetID   =   pMsg->dwPetID;
            m_pFrameMgr->SendEvent(&e);
        }
    }

    return 0;
}


const tagPet* PetManager::GetPet( DWORD dwPetID )
{
    _id2pet::const_iterator it = m_mapPetsOfLP.find(dwPetID);
    if (it != m_mapPetsOfLP.end())
        return it->second;
    else
        return NULL;
}

bool PetManager::AddEquip( DWORD dwPetID, const tagPetEquipMsgInfo& petEquip )
{
    _id2pet::iterator it = m_mapPetsOfLP.find(dwPetID);
    if (it != m_mapPetsOfLP.end())
    {
        const tagPetEquipProto* pData = ItemProtoData::Inst()->FindPetEquipByItemID(petEquip.dwItemProtoID);
        if (P_VALID(pData))
        {
            // 删除原位置装备
            tagPet::_pos2Equip::iterator equipIt;
            equipIt = it->second->mapEquip.find(petEquip.n8Pos);
            if (equipIt != it->second->mapEquip.end())
            {
                SAFE_DELETE(equipIt->second);
                it->second->mapEquip.erase(equipIt);
            }

            // 新建装备
            tagPetEquip* pEquip = new tagPetEquip();
            pEquip->n64ItemID       =   petEquip.n64ItemID;
            pEquip->dwItemProtoID   =   petEquip.dwItemProtoID;

            // 放入相应装备位置
            if (!it->second->mapEquip.insert(make_pair(petEquip.n8Pos, pEquip)).second)
            {
                SAFE_DELETE(pEquip);
            }
            
            return true;
        }
        else
        {
            return false;;
        }
    }
    else
    {
        return false;
    }
}

bool PetManager::RemoveEquip( DWORD dwPetID, INT64 n64ItemID )
{
    _id2pet::iterator it = m_mapPetsOfLP.find(dwPetID);
    if (it != m_mapPetsOfLP.end())
    {
        for (tagPet::_pos2Equip::iterator equipIt = it->second->mapEquip.begin();
            equipIt != it->second->mapEquip.end(); ++equipIt)
        {
            if (equipIt->second != NULL && equipIt->second->n64ItemID == n64ItemID)
            {
                SAFE_DELETE(equipIt->second);
                it->second->mapEquip.erase(equipIt);

                return true;
            }
        }

        return false;
    }
    else
    {
        return false;
    }
}

bool PetManager::RemoveEquip( DWORD dwPetID, int pos )
{
    _id2pet::iterator it = m_mapPetsOfLP.find(dwPetID);
    if (it != m_mapPetsOfLP.end())
    {
        tagPet::_pos2Equip::iterator equipIt = it->second->mapEquip.find(pos);
        if (equipIt != it->second->mapEquip.end())
        {
            SAFE_DELETE(equipIt->second);
            it->second->mapEquip.erase(equipIt);
        }

        return true;
    }
    else
    {
        return false;
    }
}
void PetManager::BeginNC_PetEquip( DWORD dwPetID, INT8 pos, INT64 n64ItemID )
{
    tagNC_PetEquip msg;
    msg.dwPetID     =   dwPetID;
    msg.n8DstPos    =   pos;
    msg.n64ItemID   =   n64ItemID;
    m_pSession->Send(&msg);
}

DWORD PetManager::OnNS_PetEquip( tagNS_PetEquip* pMsg, DWORD )
{
    if (pMsg->dwErrCode == E_Success)
    {
        bool ret = AddEquip(pMsg->dwPetID, pMsg->itemData);
        ASSERT(ret);

        // 发送属性更新事件
        tagPetAttrChange e(_T("tagPetAttrChange"), NULL);
        e.eType     =   EPACT_PetEquip;
        e.dwPetID   =   pMsg->dwPetID;
        m_pFrameMgr->SendEvent(&e);
    }
    else
    {
        ShowErrorMessage(pMsg->dwErrCode);
    }

    return 0;
}

void PetManager::BeginNC_PetUnequip( DWORD dwPetID, INT64 n64ItemID, INT16 n16PosDst )
{
    tagNC_PetUnequip msg;
    msg.dwPetID         =   dwPetID;
    msg.n16PosDst       =   n16PosDst;
    msg.n64ItemID       =   n64ItemID;
    m_pSession->Send(&msg);
}

DWORD PetManager::OnNS_PetUnequip( tagNS_PetUnequip* pMsg, DWORD )
{
    if (pMsg->dwErrCode == E_Success)
    {
        RemoveEquip(pMsg->dwPetID, pMsg->n64ItemID);

        // 发送属性更新事件
        tagPetAttrChange e(_T("tagPetAttrChange"), NULL);
        e.eType     =   EPACT_PetEquip;
        m_pFrameMgr->SendEvent(&e);
    }
    else
    {
        ShowErrorMessage(pMsg->dwErrCode);
    }

    return 0;
}

DWORD PetManager::GetCurrPetID()
{
    for (_id2pet::const_iterator it = m_mapPetsOfLP.begin();
        it != m_mapPetsOfLP.end(); ++it)
    {
        if (it->second->state.bisCalled)
        {
            return it->second->dwID;
        }
    }

    return GT_INVALID;
}

void PetManager::AddToBag( DWORD petId, int pos, Item* pItem )
{
    // 如果有重复物品，则返回
    if (P_VALID(GetItem(petId, pItem->GetItemId())))
        return;

    _id2pet::iterator it = m_mapPetsOfLP.find(petId);
    if (it != m_mapPetsOfLP.end())
    {
        it->second->bag[pos] = pItem->GetItemId();
        // 锁定item
        //pItem->Lock();
    }
}

void PetManager::PopFromBag( DWORD petId, int pos )
{
    _id2pet::iterator it = m_mapPetsOfLP.find(petId);
    if (it != m_mapPetsOfLP.end())
    {
        // 解锁item
        Item* pItem = GetItem(petId, pos);
        if (P_VALID(pItem))
            //pItem->UnLock();

        it->second->bag.erase(pos);
    }
}

void PetManager::SwapBagItem( DWORD petId, int posSrc, int posDst )
{
    _id2pet::iterator it = m_mapPetsOfLP.find(petId);
    if (it != m_mapPetsOfLP.end())
    {
        INT64 tempSrc = 0;
        INT64 tempDst = 0;

        tagPet::_pos2petbag::iterator bagIt = it->second->bag.find(posSrc);
        tempSrc = bagIt == it->second->bag.end() ? 0 : bagIt->second;
        bagIt = it->second->bag.find(posDst);
        tempDst = bagIt == it->second->bag.end() ? 0 : bagIt->second;

        it->second->bag.erase(posSrc);
        it->second->bag.erase(posDst);

        if (tempDst != 0)
            it->second->bag.insert(make_pair(posSrc, tempDst));

        if (tempSrc != 0)
            it->second->bag.insert(make_pair(posDst, tempSrc));
    }
}

void PetManager::ClearBag( DWORD petId )
{


    _id2pet::iterator it = m_mapPetsOfLP.find(petId);
    if (it != m_mapPetsOfLP.end())
    {
        for (tagPet::_pos2petbag::const_iterator itbag = it->second->bag.begin();
            itbag != it->second->bag.end(); ++itbag)
        {
            Item* pItem = ItemMgr::Inst()->GetConItemByID(EICT_Bag, itbag->second);
            if (P_VALID(pItem))
            {
                //pItem->UnLock();
            }
        }

        it->second->bag.clear();
    }
}

Item* PetManager::GetItem( DWORD petId, int pos )
{
    _id2pet::iterator it = m_mapPetsOfLP.find(petId);
    if (it != m_mapPetsOfLP.end())
    {
        tagPet::_pos2petbag::iterator bagIt = it->second->bag.find(pos);
        INT64 itemID = bagIt == it->second->bag.end() ? NULL : bagIt->second;
        return ItemMgr::Inst()->GetConItemByID(EICT_Bag, itemID);
    }

    return NULL;
}

Item* PetManager::GetItem( DWORD petId, INT64 n64ItemID )
{
    _id2pet::iterator itPet = m_mapPetsOfLP.find(petId);
    if (itPet != m_mapPetsOfLP.end())
    {
        for (tagPet::_pos2petbag::iterator it = itPet->second->bag.begin();
            it != itPet->second->bag.end(); ++it)
        {
            if (it->second == n64ItemID)
            {
                return ItemMgr::Inst()->GetConItemByID(EICT_Bag, n64ItemID);
            }
        }
    }

    return NULL;
}

void PetManager::BeginNC_PetEquipPosSwap( DWORD dwPetID, INT64 n64ItemID, INT8 n8PosDst )
{
    tagNC_PetEquipPosSwap msg;
    msg.dwPetID     =   dwPetID;
    msg.n64ItemID   =   n64ItemID;
    msg.n8PosDst    =   n8PosDst;

    m_pSession->Send(&msg);
}

DWORD PetManager::OnNS_PetEquipPosSwap( tagNS_PetEquipPosSwap* pMsg, DWORD )
{
    _id2pet::iterator it = m_mapPetsOfLP.find(pMsg->dwPetID);
    if (it != m_mapPetsOfLP.end())
    {
        if (pMsg->dwErrCode == E_Success)
        {
            // 执行交换
            tagPetEquip* equipSrc   = NULL;
            tagPetEquip* equipDst   = NULL;

            tagPet::_pos2Equip::iterator equipIt;
            equipIt = it->second->mapEquip.find(pMsg->n8PosSrc);
            if (equipIt != it->second->mapEquip.end())
            {
                equipSrc = equipIt->second;
                it->second->mapEquip.erase(equipIt);
            }

            equipIt = it->second->mapEquip.find(pMsg->n8PosDst);
            if (equipIt != it->second->mapEquip.end())
            {
                equipDst = equipIt->second;
                it->second->mapEquip.erase(equipIt);
            }

            if (equipDst != NULL)
            {
                if (!it->second->mapEquip.insert(make_pair(pMsg->n8PosSrc, equipDst)).second)
                {
                    SAFE_DELETE(equipDst);
                }
            }
            if (equipSrc != NULL)
            {
                if (!it->second->mapEquip.insert(make_pair(pMsg->n8PosDst, equipSrc)).second)
                {
                    SAFE_DELETE(equipSrc);
                }
            }
        }
        else
        {
            ShowErrorMessage(pMsg->dwErrCode);
        }
    }

    return 0;
}

const tagPetEquip* PetManager::FindPetEquip( DWORD dwPetID, INT8 pos )
{
    const tagPet* pPet = GetPet(dwPetID);
    if (P_VALID(pPet))
    {
        tagPet::_pos2Equip::const_iterator it = pPet->mapEquip.find(pos);
        if (it != pPet->mapEquip.end())
        {
            return it->second;
        }
    }

    return NULL;
}

void PetManager::BeginPourExp( DWORD dwPetID )
{
    tagNC_PetPourExp e;
    e.dwPetID   =   dwPetID;
    e.n64ItemID =   GT_INVALID;
    m_pSession->Send(&e);
}

DWORD PetManager::OnNS_PetPourExp( tagNS_PetPourExp* pMsg, DWORD )
{
    if (pMsg->dwErrCode == E_Success)
    {

    }
    else
    {
        ShowErrorMessage(pMsg->dwErrCode);
    }

    return 0;
}

void PetManager::BeginSetPetState( DWORD dwPetID, EPetState petState, bool bEnabled )
{
    tagNC_SetPetState e;
    e.dwPetID       =   dwPetID;
    e.ePetState     =   static_cast<INT8>(petState);
    e.bVal          =   bEnabled;

    m_pSession->Send(&e);
}

bool PetManager::IsPetCalled( DWORD dwPetID )
{
    const tagPet* pPet = GetPet(dwPetID);
    return pPet == NULL ? false : pPet->state.bisCalled;
}

bool PetManager::IsPetWorking( DWORD dwPetID )
{
    const tagPet* pPet = GetPet(dwPetID);
    return pPet == NULL ? false : pPet->state.bisWorking;
}

bool PetManager::IsPetPreparing( DWORD dwPetID )
{
    const tagPet* pPet = GetPet(dwPetID);
    return pPet == NULL ? false : pPet->state.bisPreparing;
}

bool PetManager::IsPetRiding( DWORD dwPetID )
{
    const tagPet* pPet = GetPet(dwPetID);
    return pPet == NULL ? false : pPet->state.bisRiding;
}

void PetManager::Update()
{
    //! 每隔20秒清空错误地物表
    if (Kernel::Inst()->GetAccumTimeDW() - m_ErrGroundItemTime > 20000)
    {
        m_setErrGroundItemSerial.clear();
        m_ErrGroundItemTime = Kernel::Inst()->GetAccumTimeDW();
    }

    //! 更新技能冷却状态
    UpdateSkillCD();

    //! 每隔0.5秒自动查看自动技能使用
    if (Kernel::Inst()->GetAccumTimeDW() - m_autoSkillFirst > 500)
    {
        AutoUseSkill();
        m_autoSkillFirst = Kernel::Inst()->GetAccumTimeDW();
    }
}

const tagPetSkill* PetManager::FindPetSkill( DWORD dwPetID, DWORD dwSkillID )
{
    const tagPet* pPet = GetPet(dwPetID);
    if (P_VALID(pPet))
    {
        tagPet::_id2skillInfo::const_iterator it = pPet->Skill.find(dwSkillID);
        if (it != pPet->Skill.end())
        {
            return it->second;
        }
    }

    return NULL;
}

void PetManager::TryToUsePetSkill( DWORD dwPetID, DWORD dwSkillID )
{
    // 取得宠物原型
    const tagPet* pPet = GetPet(dwPetID);
    if (!P_VALID(pPet))
        return;

    // 查看是否有此技能
    tagPetSkill* pSkill = const_cast<tagPetSkill*>(FindPetSkill(dwPetID, dwSkillID));
    if (!P_VALID(pSkill))
        return;

    // 取得技能原型
    const tagPetSkillData* pSkillData = PetProtoData::Inst()->FindPetSkill(dwSkillID);
    if (!P_VALID(pSkillData))
        return;

    switch (pSkillData->eType)
    {
    case EPT_Gather:
        {
            if (!pSkill->bCoolingDown)
            {
                DWORD dwErrCode;

                if (IfCanCastSkill(dwPetID, dwSkillID, dwErrCode))
                {
                    // 玩家是否有采集技能


                    // 是否周围有采集点
                    // 取离玩家最近的采集点
                    map<float, Role*> mapGather;
                    mapGather.clear();

                    const map<DWORD,Role*>& roleMap = RoleMgr::Inst()->GetRoleMap();
                    for(map<DWORD,Role*>::const_iterator it=roleMap.begin();
                        it!=roleMap.end(); ++it)
                    {
                        if (it->second->IS_KIND_OF(NPC)
                            && static_cast<NPC*>(it->second)->IsAttributeInited()
                            && static_cast<NPC*>(it->second)->IsGather())
                        {
                            LocalPlayer* pLp = RoleMgr::Inst()->GetLocalPlayer();
                            if (P_VALID(pLp))
                            {
                                float fDistance = Vec3DistSq( pLp->GetPos(), it->second->GetPos() );
                                float fYDist = abs(pLp->GetPos().y - it->second->GetPos().y);
                                if (fYDist <= MAX_PET_GATHER_DISTANCE * TILE_SCALE)
                                    mapGather.insert(make_pair(fDistance, it->second) );
                            }
                        }
                    }

                    if (!mapGather.empty())
                    {
                        map<float, Role*>::iterator itGather = mapGather.begin();

                        tagPetGatherDataNC data;
                        data.dwResourceID   = itGather->second->GetID();
                        BeginNC_PetSkillUse(dwPetID, dwSkillID, (BYTE*)(&data), sizeof(data));
                    }
                    else
                    {
                        ShowErrorMessage(E_Pets_PetSkill_Gather_NoResource);
                    }
                }
                else
                {
                    ShowErrorMessage(dwErrCode);
                }
            }
        }
    	break;
    case EPT_PickUp:
        {
            tagPetSkillCfgOpen e(_T("tagPetSkillCfgOpen"), NULL);
            e.dwPetID       =   dwPetID;
            e.dwSkillID     =   dwSkillID;
            e.eSkillType    =   EPT_PickUp;

            m_pFrameMgr->SendEvent(&e);
        }
        break;
    case EPT_MedicineFeed:
        {
            tagPetSkillCfgOpen e(_T("tagPetSkillCfgOpen"), NULL);
            e.dwPetID       =   dwPetID;
            e.dwSkillID     =   dwSkillID;
            e.eSkillType    =   EPT_MedicineFeed;

            m_pFrameMgr->SendEvent(&e);
        }
        break;
    case EPT_Forage:
        {
            tagPetSkillCfgOpen e(_T("tagPetSkillCfgOpen"), NULL);
            e.dwPetID       =   dwPetID;
            e.dwSkillID     =   dwSkillID;
            e.eSkillType    =   EPT_Forage;

            m_pFrameMgr->SendEvent(&e);
        }
        break;
    case EPT_Experience:
        {
            tagPetSkillCfgOpen e(_T("tagPetSkillCfgOpen"), NULL);
            e.dwPetID       =   dwPetID;
            e.dwSkillID     =   dwSkillID;
            e.eSkillType    =   EPT_Experience;

            m_pFrameMgr->SendEvent(&e);
        }
        break;
    case EPT_Deliver:
        {
        }
        break;
    case EPT_Sale:
        {
            PetBagFrame* pBagFrame = (PetBagFrame*)m_pFrameMgr->GetFrame(_T("PetBag"));
            if (P_VALID(pBagFrame))
            {
                pBagFrame->SetFocusPetID(dwPetID);
                pBagFrame->SetUseType(PetBagFrame::EPBST_SELL);
                pBagFrame->SetCurrSkillID(dwSkillID);
                pBagFrame->ShowWnd(TRUE);
            }
        }
        break;
    case EPT_StoreHouse:
        {
        }
        break;
    case EPT_Strengthen:
        {
        }
        break;
    case EPT_Buff:
        {
            if (!pSkill->bCoolingDown)
            {
                DWORD dwErrCode;

                if (IfCanCastSkill(dwPetID, dwSkillID, dwErrCode))
                {
                    BeginNC_PetSkillUse(dwPetID, dwSkillID, NULL, 0);
                }
                else
                {
                    ShowErrorMessage(dwErrCode);
                }
            }
        }
        break;
    case EPT_WuXing:
        {
            if (!pSkill->bCoolingDown)
            {
                DWORD dwErrCode;
                if (IfCanCastSkill(dwPetID, dwSkillID, dwErrCode))
                {
                    BeginNC_PetSkillUse(dwPetID, dwSkillID, NULL, 0);
                }
                else
                {
                    ShowErrorMessage(dwErrCode);
                }
            }
        }
        break;
    }
}

void PetManager::BeginNC_PetSkillUse( DWORD dwPetID, DWORD dwSkillID, BYTE* pData, size_t dataLength )
{
    if (P_VALID(pData))
    {
        size_t skillSize = (sizeof(tagNC_PetSkillUse) - 1) + dataLength;

        BYTE* pTmpData = new BYTE[skillSize];

        tagNC_PetSkillUse data;
        data.dwPetID    =   dwPetID;
        data.dwSkillID  =   dwSkillID;
        data.dwSize     =   skillSize;

        memcpy(pTmpData, &data, sizeof(tagNC_PetSkillUse));
        memcpy(pTmpData + sizeof(tagNC_PetSkillUse) - 1, pData, dataLength);

        tagNC_PetSkillUse* pSkillMsg = (tagNC_PetSkillUse*)pTmpData;

        m_pSession->Send(pSkillMsg);

        SAFE_DELETEA(pTmpData);
    }
    else
    {
        tagNC_PetSkillUse* pSkillMsg;

        pSkillMsg = new tagNC_PetSkillUse();
        pSkillMsg->dwPetID      =   dwPetID;
        pSkillMsg->dwSkillID    =   dwSkillID;

        m_pSession->Send(pSkillMsg);

        SAFE_DELETEA(pSkillMsg);
    }
}

DWORD PetManager::OnNS_PetSkillUse( tagNS_PetSkillUse* pMsg, DWORD )
{
    tagPetSkill* pSkill = const_cast<tagPetSkill*>(FindPetSkill(pMsg->dwPetID, pMsg->dwSkillID));
    if (P_VALID(pSkill))
    {
        const tagPetSkillData* pSkillData = PetProtoData::Inst()->FindPetSkill(pSkill->dwTypeID);
        if (pSkillData)
        {
            if (pSkillData->eType == EPT_Forage)
            {
                if (pMsg->dwErrCode == E_Success)
                {
                    // 开始冷却
                    pSkill->bCoolingDown    =   true;
                    pSkill->nCurrCoolDown   =   0;
                    pSkill->nMaxCoolDown    =   PetProtoData::Inst()->GetPetSkillMaxCd(pSkill->dwTypeID);
                    pSkill->dwTimeFirst     =   Kernel::Inst()->GetAccumTimeDW();
                }
                else
                {
                    ShowErrorMessage(pMsg->dwErrCode);
                }
            }
            else if (pSkillData->eType == EPT_PickUp)
            {
                if (pMsg->dwErrCode == E_Success)
                {
                    // 开始冷却
                    pSkill->bCoolingDown    =   true;
                    pSkill->nCurrCoolDown   =   0;
                    pSkill->nMaxCoolDown    =   PetProtoData::Inst()->GetPetSkillMaxCd(pSkill->dwTypeID);
                    pSkill->dwTimeFirst     =   Kernel::Inst()->GetAccumTimeDW();
                }
                else
                {
                    // 加入到不能拾取的表里，防止失败的物品不停的向服务器发送请求
                    INT64* pN64serial = (INT64*)pMsg->byData;
                    m_setErrGroundItemSerial.insert(*pN64serial);

                    ShowErrorMessage(pMsg->dwErrCode);
                }
            }
            else if (pSkillData->eType == EPT_MedicineFeed)
            {
                if (pMsg->dwErrCode == E_Success)
                {
                    // 开始冷却
                    pSkill->bCoolingDown    =   true;
                    pSkill->nCurrCoolDown   =   0;
                    pSkill->nMaxCoolDown    =   PetProtoData::Inst()->GetPetSkillMaxCd(pSkill->dwTypeID);
                    pSkill->dwTimeFirst     =   Kernel::Inst()->GetAccumTimeDW();
                }
                else
                {
                    ShowErrorMessage(pMsg->dwErrCode);
                }
            }
            else if (pSkillData->eType == EPT_Buff)
            {
                if (pMsg->dwErrCode == E_Success)
                {
                    // 开始冷却
                    pSkill->bCoolingDown    =   true;
                    pSkill->nCurrCoolDown   =   0;
                    pSkill->nMaxCoolDown    =   PetProtoData::Inst()->GetPetSkillMaxCd(pSkill->dwTypeID);
                    pSkill->dwTimeFirst     =   Kernel::Inst()->GetAccumTimeDW();
                }
                else
                {
                    ShowErrorMessage(pMsg->dwErrCode);
                }
            }
            else if (pSkillData->eType == EPT_WuXing)
            {
                if (pMsg->dwErrCode == E_Success)
                {
                    tagPetWuXingItem* pItem = (tagPetWuXingItem*)pMsg->byData;
                    const tagItemDisplayInfo* pDisp = ItemProtoData::Inst()->FindItemDisplay(pItem->dwItemTypeID);
                    if (P_VALID(pItem))
                    {
                    }

                    // 开始冷却
                    pSkill->bCoolingDown    =   true;
                    pSkill->nCurrCoolDown   =   0;
                    pSkill->nMaxCoolDown    =   PetProtoData::Inst()->GetPetSkillMaxCd(pSkill->dwTypeID);
                    pSkill->dwTimeFirst     =   Kernel::Inst()->GetAccumTimeDW();
                }
                else
                {
                    ShowErrorMessage(pMsg->dwErrCode);
                }

            }
            else if (pSkillData->eType == EPT_Sale)
            {
                if (pMsg->dwErrCode == E_Success)
                {
                }
                else
                {
                    ShowErrorMessage(pMsg->dwErrCode);
                }
                
                // 清空pet bag,解锁item
                const tagPet* pPet = GetPet(pMsg->dwPetID);
                if (P_VALID(pPet))
                {
                    ClearBag(pMsg->dwPetID);
                }

            }
            else if (pSkillData->eType == EPT_Gather)
            {
                if (pMsg->dwErrCode == E_Success)
                {
                    // 如果是第一次使用技能返回
                    if (pMsg->dwSize == sizeof(tagNS_PetSkillUse))
                    {
                        //// 开始冷却
                        //pSkill->bCoolingDown    =   true;
                        //pSkill->nCurrCoolDown   =   0;
                        //pSkill->nMaxCoolDown    =   PetProtoData::Inst()->GetPetSkillMaxCd(pSkill->dwTypeID);
                        //pSkill->dwTimeFirst     =   Kernel::Inst()->GetAccumTimeDW();

                        return 0;
                    }

                    tagPetGatherDataNS* pGatherData = (tagPetGatherDataNS*)pMsg->byData;
                    if (tagPetGatherDataNS::ER_Continue == pGatherData->eRet)
                    {
                        //// 如果成功，则继续采集当前采集点
                        //DWORD dwErrorMsg = GT_INVALID;
                        //if (IfCanCastSkill(pMsg->dwPetID, pMsg->dwSkillID, dwErrorMsg))
                        //{
                        //    tagPetGatherDataNC data;
                        //    data.dwResourceID   = pGatherData->dwResourceID;
                        //    BeginNC_PetSkillUse(pMsg->dwPetID, pMsg->dwSkillID, (BYTE*)(&data), sizeof(data));
                        //}
                    }
                    else
                    {
                        CombatSysUtil::Inst()->ShowScreenCenterMsg(_T("资源点已死亡"));
                    }
                }
                else
                {
                    ShowErrorMessage(pMsg->dwErrCode);
                }
            }
            else if (pSkillData->eType == EPT_Experience)
            {
                if (pMsg->dwErrCode == E_Success)
                {
                    // 开始冷却
                    pSkill->bCoolingDown    =   true;
                    pSkill->nCurrCoolDown   =   0;
                    pSkill->nMaxCoolDown    =   PetProtoData::Inst()->GetPetSkillMaxCd(pSkill->dwTypeID);
                    pSkill->dwTimeFirst     =   Kernel::Inst()->GetAccumTimeDW();

                    return 0;
                }
                else
                {
                    ShowErrorMessage(pMsg->dwErrCode);
                }
            }
        }
    }

    return 0;
}

DWORD PetManager::OnNS_AddPetSkill( tagNS_AddPetSkill* pMsg, DWORD )
{
    tagPetSkillMsgInfo skillInfo;
    skillInfo.dwTypeID      =   pMsg->dwSkillID;
    skillInfo.nCurrCoolDown =   0;
    skillInfo.nMaxCoolDown  =   0;

    if (AddSkill(pMsg->dwPetID, skillInfo))
    {
        // 发送属性更新事件
        tagPetAttrChange e(_T("tagPetAttrChange"), NULL);
        e.eType     =   EPACT_PetSkill;
        e.dwPetID   =   pMsg->dwPetID;
        m_pFrameMgr->SendEvent(&e);
    }

    return 0;
}

DWORD PetManager::OnNS_RemovePetSkill( tagNS_RemovePetSkill* pMsg, DWORD )
{
    if (RemoveSkill(pMsg->dwPetID, pMsg->dwSkillID))
    {
        // 发送属性更新事件
        tagPetAttrChange e(_T("tagPetAttrChange"), NULL);
        e.eType     =   EPACT_PetSkill;
        e.dwPetID   =   pMsg->dwPetID;
        m_pFrameMgr->SendEvent(&e);
    }

    return 0;
}

bool PetManager::AddSkill( DWORD dwPetID, const tagPetSkillMsgInfo& petSkill )
{
    tagPet* pPet = const_cast<tagPet*>(GetPet(dwPetID));
    if (!P_VALID(pPet))
        return false;

    const tagPetSkill* pPetSkill = FindPetSkill(dwPetID, petSkill.dwTypeID);

    if (P_VALID(pPetSkill))
    {
        SAFE_DELETE(pPetSkill);
        pPet->Skill.erase(petSkill.dwTypeID);
    }

    tagPetSkill* pPetNewSkill = new tagPetSkill();
    pPetNewSkill->dwTypeID      =   petSkill.dwTypeID;
    pPetNewSkill->nCurrCoolDown =   petSkill.nCurrCoolDown;
    pPetNewSkill->nMaxCoolDown  =   petSkill.nMaxCoolDown;
    if (petSkill.nMaxCoolDown != GT_INVALID)
    {
        pPetNewSkill->bCoolingDown    =   true;
        pPetNewSkill->dwTimeFirst     =   Kernel::Inst()->GetAccumTimeDW();
    }
    else
    {
        pPetNewSkill->bCoolingDown    =   false;
        pPetNewSkill->nCurrCoolDown   =   0;
        pPetNewSkill->dwTimeFirst     =   0;
    }

    if (!pPet->Skill.insert(make_pair(pPetNewSkill->dwTypeID, pPetNewSkill)).second)
    {
        SAFE_DELETE(pPetNewSkill);
        return false;
    }

    return true;
}

bool PetManager::RemoveSkill( DWORD dwPetID, DWORD dwSkillID )
{
    tagPet* pPet = const_cast<tagPet*>(GetPet(dwPetID));
    if (!P_VALID(pPet))
        return false;

    const tagPetSkill* pPetSkill = FindPetSkill(dwPetID, dwSkillID);

    if (P_VALID(pPetSkill))
    {
        SAFE_DELETE(pPetSkill);
        pPet->Skill.erase(dwSkillID);
    }

    return true;
}

void PetManager::UpdateSkillCD()
{
    // 编历所有宠物所有技能
    for (_id2pet::iterator itPet = m_mapPetsOfLP.begin();
        itPet != m_mapPetsOfLP.end(); ++itPet)
    {
        for (tagPet::_id2skillInfo::iterator itSkill = itPet->second->Skill.begin();
            itSkill != itPet->second->Skill.end(); ++itSkill)
        {
            if (itSkill->second->bCoolingDown)
            {
                //离上一次使用的时间
                INT nPassMilliSecord = itSkill->second->nCurrCoolDown + Kernel::Inst()->GetAccumTimeDW() - itSkill->second->dwTimeFirst;

                if (nPassMilliSecord >= itSkill->second->nMaxCoolDown)
                {
                    itSkill->second->bCoolingDown   =   false;
                    itSkill->second->nCurrCoolDown  =   0;
                    itSkill->second->nMaxCoolDown   =   GT_INVALID;
                }
            }
        }
    }
}

void PetManager::GetSkillCDTime( DWORD dwPetID, DWORD dwSkillID, FLOAT &fCDCount, FLOAT &fRestoreRatio )
{
    tagPetSkill* pSkillData = const_cast<tagPetSkill*>(FindPetSkill(dwPetID, dwSkillID));
    if (P_VALID(pSkillData))
    {
        if (pSkillData->bCoolingDown)
        {
            FLOAT fCDTime = (FLOAT)pSkillData->nMaxCoolDown;
            //离上一次使用的时间
            INT nPassMilliSecord = pSkillData->nCurrCoolDown + Kernel::Inst()->GetAccumTimeDW() - pSkillData->dwTimeFirst;
            //倒计时时间
            fCDCount = fCDTime - nPassMilliSecord;	
            //倒计时旋转律
            fRestoreRatio = (fCDTime - fCDCount) / fCDTime + 0.000001f;
        }
        else
        {
            fCDCount        =   0;
            fRestoreRatio   =   0;
        }
    }
    else
    {
        fCDCount        =   0;
        fRestoreRatio   =   0;
    }
}

void PetManager::SetSkillForageCfg( DWORD dwPetID, tagPetSkillForage& cfg )
{
    tagPet* pPet = const_cast<tagPet*>(GetPet(dwPetID));
    if (P_VALID(pPet))
    {
        pPet->skillForageCfg = cfg;
    }
}

void PetManager::SetSkillPickupCfg( DWORD dwPetID, tagPetSkillPickup& cfg )
{
    tagPet* pPet = const_cast<tagPet*>(GetPet(dwPetID));
    if (P_VALID(pPet))
    {
        pPet->skillPickupCfg = cfg;
    }
}

void PetManager::SetSkillSupplyCfg( DWORD dwPetID, tagPetSkillSupply& cfg )
{
    tagPet* pPet = const_cast<tagPet*>(GetPet(dwPetID));
    if (P_VALID(pPet))
    {
        pPet->skillMedicineFeedCfg = cfg;
    }
}

void PetManager::AutoUseSkill()
{
    if (!bHasInitialized)
        return;

    // 取得召唤宠物
    DWORD petID = GetCurrPetID();
    if (GT_INVALID == petID)
        return;

    const tagPet* pPet = GetPet(petID);
    if (!P_VALID(pPet))
        return;

    // 自动觅食
    const tagPetSkill* pSkillForage = FindPetSkillbyType(petID, EPT_Forage);
    if (P_VALID(pSkillForage))
    {
        DWORD dwErrCode;
        if (IfCanCastSkill(petID, pSkillForage->dwTypeID, dwErrCode))
        {
            if (pPet->skillForageCfg.bisEnabled)
            {
                // 当灵力不足时
                if (pPet->nAttrA[ECSPA_nSpirit] * 100 / pPet->nAttrA[ECSPA_nSpiritMax] < pPet->skillForageCfg.u8MinSpirit)
                {
                    // 看看物品
                    Item* item = ItemMgr::Inst()->GetConItemByID(EICT_Bag, pPet->skillForageCfg.dwItemID);
                    if (P_VALID(item) && item->GetProto()->nPrepareTime == 0)
                    {
                        // 物品冷却时间
                        float fCDCount, fRestoreRatio;
                        ItemMgr::Inst()->GetItemCurCDTime(item->GetItemTypeID(), fCDCount, fRestoreRatio);
                        if (fRestoreRatio > 1)
                        {
                            // 使用药品
                            tagPetForageData data;
                            data.n64ItemID  =   pPet->skillForageCfg.dwItemID;
                            BeginNC_PetSkillUse(petID, pSkillForage->dwTypeID, (BYTE*)(&data), sizeof(data));
                        }
                    }
                }
            }
        }
    }

    // 自动拾取
    const tagPetSkill* pSkillPickUp = FindPetSkillbyType(petID, EPT_PickUp);
    if (P_VALID(pSkillPickUp))
    {
        const tagPetSkillData* pSkillData = PetProtoData::Inst()->FindPetSkill(pSkillPickUp->dwTypeID);
        if (P_VALID(pSkillData))
        {
            DWORD dwErrCode;
            if (IfCanCastSkill(petID, pSkillPickUp->dwTypeID, dwErrCode))
            {
                LocalPlayer* pRole = RoleMgr::Inst()->GetLocalPlayer();
                if (pRole)
                {
                    tagGroundItem* pGroundItem = GroundItemMgr::Inst()->GetNearestCanPickUpItem(
                        pRole->GetPos(),
                        X_DEF_AUTO_PICKUP_DIST,
                        pPet->skillPickupCfg.bMoney,
                        pPet->skillPickupCfg.bEquip,
                        pPet->skillPickupCfg.bMaterial,
                        pPet->skillPickupCfg.bTrophy,
                        pPet->skillPickupCfg.bMedicine,
                        pPet->skillPickupCfg.bOthers);

                    if (P_VALID(pGroundItem))
                    {
                        tagPetPickUpData data;
                        data.n64GroundSerial = pGroundItem->n64Serial;
                        BeginNC_PetSkillUse(petID, pSkillPickUp->dwTypeID, (BYTE*)(&data), sizeof(data));
                    }
                }
            }
        }
    }

    // 自动喂药
    const tagPetSkill* pSkillMedicineFeed = FindPetSkillbyType(petID, EPT_MedicineFeed);
    if (P_VALID(pSkillMedicineFeed))
    {
        DWORD dwErrCode;
        if (IfCanCastSkill(petID, pSkillMedicineFeed->dwTypeID, dwErrCode))
        {
            // 取得本地玩家指针
            LocalPlayer* pLp = RoleMgr::Inst()->GetLocalPlayer();
            if (P_VALID(pLp))
            {
                //! 体力不足
                if (pLp->GetAttribute(ERA_HP) * 100 / pLp->GetAttribute(ERA_MaxHP) < pPet->skillMedicineFeedCfg.u8MinHP)
                {
                    if (pPet->skillMedicineFeedCfg.bHPEnabled)
                    {
                        // 看看物品
                        Item* item = ItemMgr::Inst()->GetConItemByID(EICT_Bag, pPet->skillMedicineFeedCfg.dwHPItemID);
                        if (P_VALID(item) && item->GetProto()->nPrepareTime == 0)
                        {
                            // 物品冷却时间
                            float fCDCount;
                            float fRestoreRatio = 1.1f;
                            ItemMgr::Inst()->GetItemCurCDTime(item->GetItemTypeID(), fCDCount, fRestoreRatio);
                            if (fRestoreRatio > 1)
                            {
                                tagPetSupplyData data;
                                data.eSupplyType    =   EPST_Health;
                                data.n64ItemID      =   pPet->skillMedicineFeedCfg.dwHPItemID;
                                BeginNC_PetSkillUse(petID, pSkillMedicineFeed->dwTypeID, (BYTE*)(&data), sizeof(data));
                            }
                        }
                    }
                }

                //! 真气不足
                if (pLp->GetAttribute(ERA_MP) * 100 / pLp->GetAttribute(ERA_MaxMP) < pPet->skillMedicineFeedCfg.u8MinMP)
                {
                    if (pPet->skillMedicineFeedCfg.bisMPEnabled)
                    {
                        // 看看物品
                        Item* item = ItemMgr::Inst()->GetConItemByID(EICT_Bag, pPet->skillMedicineFeedCfg.dwMPItemID);
                        if (P_VALID(item) && item->GetProto()->nPrepareTime == 0)
                        {
                            // 物品冷却时间
                            float fCDCount;
                            float fRestoreRatio = 1.1f;
                            ItemMgr::Inst()->GetItemCurCDTime(item->GetItemTypeID(), fCDCount, fRestoreRatio);
                            if (fRestoreRatio > 1)
                            {
                                tagPetSupplyData data;
                                data.eSupplyType    =   EPST_Power;
                                data.n64ItemID      =   pPet->skillMedicineFeedCfg.dwMPItemID;
                                BeginNC_PetSkillUse(petID, pSkillMedicineFeed->dwTypeID, (BYTE*)(&data), sizeof(data));
                            }
                        }
                    }
                }

                //! 持久力不足
                if (pLp->GetAttribute(ERA_Endurance) * 100 / pLp->GetAttribute(ERA_MaxEndurance) < pPet->skillMedicineFeedCfg.u8Toughness)
                {
                    if (pPet->skillMedicineFeedCfg.bisTouEnabled)
                    {
                        // 看看物品
                        Item* item = ItemMgr::Inst()->GetConItemByID(EICT_Bag, pPet->skillMedicineFeedCfg.dwTouItemID);
                        if (P_VALID(item) && item->GetProto()->nPrepareTime == 0)
                        {
                            // 物品冷却时间
                            float fCDCount;
                            float fRestoreRatio = 1.1f;
                            ItemMgr::Inst()->GetItemCurCDTime(item->GetItemTypeID(), fCDCount, fRestoreRatio);
                            if (fRestoreRatio > 1)
                            {
                                tagPetSupplyData data;
                                data.eSupplyType    =   EPST_Toughness;
                                data.n64ItemID      =   pPet->skillMedicineFeedCfg.dwTouItemID;
                                BeginNC_PetSkillUse(petID, pSkillMedicineFeed->dwTypeID, (BYTE*)(&data), sizeof(data));
                            }
                        }
                    }
                }
            }
        }
    }
}

const tagPetSkill* PetManager::FindPetSkillbyType( DWORD dwPetID, EPetskillType eType )
{
    const tagPet* pPet = GetPet(dwPetID);
    if (P_VALID(pPet))
    {
         for (tagPet::_id2skillInfo::const_iterator skillIt = pPet->Skill.begin();
             skillIt != pPet->Skill.end(); ++skillIt)
         {
             DWORD dwPetSkillID = skillIt->first;
             const tagPetSkillData* skillData = PetProtoData::Inst()->FindPetSkill(dwPetSkillID);
             if (P_VALID(skillData) && skillData->eType == eType)
             {
                 return skillIt->second;
             }
         }
    }

    return NULL;
}

DWORD PetManager::OnNS_PetLearnSkill( tagNS_PetLearnSkill* pMsg, DWORD )
{
    if (pMsg->dwErrCode != E_Success)
    {
        ShowErrorMessage(pMsg->dwErrCode);
    }

    return 0;
}

void PetManager::BeginNS_PetLearnSkill( DWORD dwPetID, INT64 n64ItemID )
{
    tagNC_PetLearnSkill msg;
    msg.dwPetID     =   dwPetID;
    msg.n64ItemID   =   n64ItemID;
    m_pSession->Send(&msg);
}

bool PetManager::IsExistInErrGroundItem( INT64 n64serial )
{
    set<INT64>::iterator it = m_setErrGroundItemSerial.find(n64serial);
    return it != m_setErrGroundItemSerial.end();
}

bool PetManager::IfCanCastSkill( DWORD dwPetID, DWORD dwSkillID, DWORD &dwErrCodeOut )
{
    dwErrCodeOut = E_Success;

    // 取得宠物原型
    const tagPet* pPet = GetPet(dwPetID);
    if (!P_VALID(pPet))
    {
        dwErrCodeOut = E_Pets_Pet_NotExist;
        return false;
    }

    // 查看是否有此技能
    const tagPetSkill* pSkill = FindPetSkill(dwPetID, dwSkillID);
    if (!P_VALID(pSkill))
    {
        dwErrCodeOut = E_Pets_PetSkill_Skill_NotExist;
        return false;
    }

    // 取得技能原型
    const tagPetSkillData* pSkillData = PetProtoData::Inst()->FindPetSkill(dwSkillID);
    if (!P_VALID(pSkillData))
    {
        dwErrCodeOut = E_Pets_PetSkill_InvalidSkillID;
        return false;
    }

    // 取得投放状态
    BYTE byState = 0;

    // 是否召唤
    byState |= ( pPet->state.bisCalled      ? EPSF_Called   :   EPSF_UnCalled);
    if ((byState & pSkillData->cast_condition) != byState)
    {
        if (pPet->state.bisCalled)
            dwErrCodeOut = E_Pets_Skill_CalledLimit;
        else
            dwErrCodeOut = E_Pets_Skill_UnCalledLimit;

        return false;
    }

    // 是否工作
    byState |= ( pPet->state.bisWorking     ? EPSF_Working  :   EPSF_UnWorking);
    if ((byState & pSkillData->cast_condition) != byState)
    {
        if (pPet->state.bisWorking)
            dwErrCodeOut = E_Pets_Skill_WorkingLimit;
        else
            dwErrCodeOut = E_Pets_Skill_UnWorkingLimit;

        return false;
    }

    // 是否预备驾御
    byState |= ( pPet->state.bisPreparing   ? EPSF_Preparing:   EPSF_UnPreparing);
    if ((byState & pSkillData->cast_condition) != byState)
    {
        if (pPet->state.bisPreparing)
            dwErrCodeOut = E_Pets_Skill_PreparingLimit;
        else
            dwErrCodeOut = E_Pets_Skill_UnPreparingLimit;

        return false;
    }

    // 是否骑乘
    byState |= ( pPet->state.bisRiding      ? EPSF_Mounting :   EPSF_UnMounting);
    if ((byState & pSkillData->cast_condition) != byState)
    {
        if (pPet->state.bisRiding)
            dwErrCodeOut = E_Pets_Skill_MountingLimit;
        else
            dwErrCodeOut = E_Pets_Skill_UnMountingLimit;

        return false;
    }

    // 是否冷却中
    if (pSkill->bCoolingDown)
    {
        dwErrCodeOut = E_Pets_PetSkill_Use_CoolingDown;
        return false;
    }

    // 五行力消耗
    if (pPet->nAttrA[ECSPA_nWuXingEnergy] < pSkillData->wuxing_cost)
    {
        dwErrCodeOut = E_Pets_PetSkill_Use_WuXingEnergyNotEnough;
        return false;
    }

    // 灵力消耗
    if (pPet->nAttrA[ECSPA_nSpirit] < pSkillData->spirit_cost)
    {
        dwErrCodeOut = E_Pets_PetSkill_Use_SpiritNotEnough;
        return false;
    }

    return true;
}

void PetManager::BeginNC_DeletePet( DWORD dwPetID )
{
    tagNC_DeletePet e;
    e.dwPetID   =   dwPetID;
    
    m_pSession->Send(&e);
}

DWORD PetManager::OnNS_DeletePet( tagNS_DeletePet* pMsg, DWORD )
{
    if (pMsg->dwErrCode == E_Success)
    {
        _id2pet::iterator it = m_mapPetsOfLP.find(pMsg->dwPetID);
        if (it != m_mapPetsOfLP.end())
        {
            DWORD petId = it->first;

            SAFE_DELETE(it->second);
            m_mapPetsOfLP.erase(it);

            // 发送游戏事件
            tagPetAttrChange e(_T("tagPetAttrChange"), NULL);
            e.dwPetID   =   petId;
            e.eType     =   EPACT_Remove;
            m_pFrameMgr->SendEvent(&e);
        }
    }
    else
    {
        ShowErrorMessage(pMsg->dwErrCode);
    }

    return 0;
}

void PetManager::BeginNC_GetPetPourExpMoneyNeed( DWORD dwPetID )
{
    tagNC_GetPetPourExpMoneyNeed msg;
    msg.dwPetID =   dwPetID;

    m_pSession->Send(&msg);
}

DWORD PetManager::OnNS_GetPetPourExpMoneyNeed( tagNS_GetPetPourExpMoneyNeed* pMsg, DWORD )
{

    TCHAR szTemp[255];
    INT32 gold      = MSilver2DBGold(pMsg->n64MoneyNeed);
    INT32 silver    = MSilver2DBSilver(pMsg->n64MoneyNeed);
    _sntprintf(szTemp, sizeof(szTemp) / sizeof(TCHAR), g_StrTable[_T("PetPourExp")], gold, silver);

    if (P_VALID(GetObj("BoxPourPetExp")))
        KillObj("BoxPourPetExp");
    CreateObj("BoxPourPetExp", "MsgBox");
    TObjRef<MsgBox>("BoxPourPetExp")->Init(_T(""), szTemp, _T("BoxPourPetExp"), (MsgBoxFlag)(MBF_OK | MBF_Cancel), TRUE);

    return 0;
}

void PetManager::OnUseLvlupStep( Item* pItem )
{
    // 灵兽是否召唤
    const tagPet* pPet = GetPet(GetCurrPetID());
    if (!P_VALID(pPet))
    {
        CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("PetErrorMsg_14")]);
        return;
    }

    // 判断是否经验满值状态
    if (pPet->nAttrA[ECSPA_nExpCurrent] != pPet->nAttrA[ECSPA_nExpLevelUp])
    {
        CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("PetLveelNotFull")]);
        return;
    }

    // 判断物品等级是否等于灵兽的可携带等级
    if (pItem->GetItemLevel() != GetCarryLevel(pPet->dwID))
    {
        TCHAR szTemp[255];
        _sntprintf(szTemp, sizeof(szTemp) / sizeof(TCHAR), g_StrTable[_T("PetLveelItemError")], GetCarryLevel(pPet->dwID));
        CombatSysUtil::Inst()->ShowScreenCenterMsg(szTemp);
        return;
    }

    tagNC_PetUpStep msg;
    msg.n64ItemID   =   pItem->GetItemId();
    msg.dwSkillID   =   GT_INVALID;
    msg.nDstStep    =   GT_INVALID;

    m_pSession->Send(&msg);
}

DWORD PetManager::OnNS_PetUpStep( tagNS_PetUpStep* pMsg, DWORD )
{
    if (pMsg->dwErrCode == E_Success)
    {
        const tagPet* pPet = GetPet(GetCurrPetID());
        if (P_VALID(pPet))
        {
            TCHAR szTemp[255];

            const tagPetSkillData* pSkill = PetProtoData::Inst()->FindPetSkill(pMsg->dwSkillID);
            if (P_VALID(pSkill))
            {
                _sntprintf(szTemp, sizeof(szTemp) / sizeof(TCHAR),
                    g_StrTable[_T("PetStepUp1")],
                    pPet->strName.c_str(), pMsg->nDstStep, pSkill->strName.c_str());
            }
            else
            {
                //_sntprintf(szTemp, sizeof(szTemp) / sizeof(TCHAR),
                //    g_StrTable[_T("PetStepUp2")],
                //    pPet->strName.c_str(), pMsg->nDstStep);
                tagNC_PetUpStep msg;
                msg.n64ItemID   =   GT_INVALID;
                msg.dwSkillID   =   pMsg->dwSkillID;
                msg.nDstStep    =   pMsg->nDstStep;

                m_pSession->Send(&msg);

                return 0;
            }

            m_pLupSkillID   =   pMsg->dwSkillID;
            m_pLupStep      =   pMsg->nDstStep;

            if (P_VALID(GetObj("BoxPetUpStep")))
                KillObj("BoxPetUpStep");
            CreateObj("BoxPetUpStep", "MsgBox");
            TObjRef<MsgBox>("BoxPetUpStep")->Init(_T(""), szTemp, _T("BoxPetUpStep"), (MsgBoxFlag)(MBF_OK | MBF_Cancel), TRUE);
        }
    }
    else
    {
        ShowErrorMessage(pMsg->dwErrCode);
    }

    return 0;
}

DWORD PetManager::OnBoxPetUpStep( tagMsgBoxEvent* pEvent )
{
    if (MBF_OK == pEvent->eResult)
    {
        tagNC_PetUpStep msg;
        msg.n64ItemID   =   GT_INVALID;
        msg.dwSkillID   =   m_pLupSkillID;
        msg.nDstStep    =   m_pLupStep;

        m_pSession->Send(&msg);
    }

    return 0;
}

void PetManager::OnUseExpPour( Item* pItem )
{
    const tagPet* pPet = GetPet(GetCurrPetID());
    if (P_VALID(pPet))
    {
        TCHAR szTemp[255];
        _sntprintf(szTemp, sizeof(szTemp) / sizeof(TCHAR),
            g_StrTable[_T("PetUseItem")],
            pPet->strName.c_str(), pItem->GetItemName().c_str());

        m_tmpPourItemID =   pItem->GetItemId();

        if (P_VALID(GetObj("BoxUseExpPour")))
            KillObj("BoxUseExpPour");
        CreateObj("BoxUseExpPour", "MsgBox");
        TObjRef<MsgBox>("BoxUseExpPour")->Init(_T(""), szTemp, _T("BoxUseExpPour"), (MsgBoxFlag)(MBF_OK | MBF_Cancel), TRUE);
    }
    else
    {

    }

}

DWORD PetManager::OnBoxUseExpPour( tagMsgBoxEvent* pEvent )
{
    if (MBF_OK == pEvent->eResult)
    {
        tagNC_PetPourExp msg;
        msg.dwPetID     =   GetCurrPetID();
        msg.n64ItemID   =   m_tmpPourItemID;

        m_pSession->Send(&msg);
    }

    return 0;
}

void PetManager::OnUseEnhance( Item* pItem )
{
    tagNC_PetEnhance msg;
    msg.n64ItemID   =   pItem->GetItemId();

    m_pSession->Send(&msg);
}

DWORD PetManager::OnNS_PetEnhance( tagNS_PetEnhance* pMsg, DWORD )
{
    if (pMsg->dwErrCode == E_Success)
    {

    }
    else
    {
        ShowErrorMessage(pMsg->dwErrCode);
    }

    return 0;
}

int PetManager::GetCarryLevel( DWORD dwPetID )
{
    const tagPet* pPet = GetPet(dwPetID);
    if (P_VALID(pPet))
    {
        const tagPetProto* pPetData = PetProtoData::Inst()->FindPetProto(pPet->dwTypeID);
        if (P_VALID(pPetData))
        {
            return pPetData->carryLevel;
        }
    }

    return 0;
}

void PetManager::ShowErrorMessage( DWORD dwErrCode )
{
    if (dwErrCode >= E_Success && dwErrCode < E_PetErrorEnd)
    {
        tstringstream tss;
        tss << _T("PetErrorMsg_") << dwErrCode;
        CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[tss.str().c_str()]);
    }
}

int PetManager::GetPetCurrMountNum( DWORD dwPetID )
{
    const tagPet* pPet = GetPet(dwPetID);
    if (P_VALID(pPet))
    {
        return pPet->nAttrA[ECSPA_nMountNum];
    }

    return 0;
}

int PetManager::GetPetMaxMountNum( DWORD dwPetID )
{
    const tagPet* pPet = GetPet(dwPetID);
    if (P_VALID(pPet))
    {
        return pPet->nAttrA[ECSPA_nMountNumMax];
    }

    return 0;
}

void PetManager::BeginNC_PetSetLock( DWORD dwPetID, INT64 n64ItemID, bool bSet )
{
    tagNC_PetSetLock msg;

    msg.dwPetID     = dwPetID;
    msg.n64ItemID   = n64ItemID;
    msg.bSet        = bSet;

    if (m_pSession->IsConnect())
        m_pSession->Send(&msg);
}

DWORD PetManager::OnNS_PetSetLock( tagNS_PetSetLock* pMsg, DWORD )
{
    if (E_Success != pMsg->dwErrCode)
    {
        ShowErrorMessage(pMsg->dwErrCode);
    }

    return 0;
}

void PetManager::SetPetLock( INT64 n64ItemID, bool bSet )
{
    DWORD currPetId = GetCurrPetID();
    if (GT_INVALID == currPetId)
        ShowErrorMessage(E_Pets_Pet_NotCalled);
    else
        BeginNC_PetSetLock(currPetId, n64ItemID, bSet);
}

DWORD PetManager::OnUsePetFood( tagUsePetFoodEvent* pEvent )
{
    // 先判断是否有召唤宠物
    DWORD dwPetId = GetCurrPetID();
    if (GT_INVALID == dwPetId)
    {
        ShowErrorMessage(E_Pets_Pet_NotCalled);
        return 0;
    }

    // 当前召唤宠物的携带等级是否够
    Item* pItem = ItemMgr::Inst()->GetConItemByID(EICT_Bag, pEvent->n64ItemID);
    if (!P_VALID(pItem) || GetCarryLevel(dwPetId) < pItem->GetProto()->nSpecFuncVal1)
    {
        ShowErrorMessage(E_Pets_Carrylevel_NotEnough);
        return 0;
    }

    // 发送使用请求
    tagNC_PetFood msg;
    msg.n64ItemID   = pEvent->n64ItemID;

    if (m_pSession->IsConnect())
        m_pSession->Send(&msg);

    return 0;
}

DWORD PetManager::OnNS_PetFood( tagNS_PetFood* pMsg, DWORD )
{
    if (E_Success != pMsg->dwErrCode)
    {
        ShowErrorMessage(pMsg->dwErrCode);
    }

    return 0;
}

void PetManager::TryToSellItem( DWORD dwPetID, DWORD dwSkillID )
{
    // 取得宠物原型
    const tagPet* pPet = GetPet(dwPetID);
    if (!P_VALID(pPet))
        return;

    // 查看是否有此技能
    tagPetSkill* pSkill = const_cast<tagPetSkill*>(FindPetSkill(dwPetID, dwSkillID));
    if (!P_VALID(pSkill))
        return;

    // 取得技能原型
    const tagPetSkillData* pSkillData = PetProtoData::Inst()->FindPetSkill(dwSkillID);
    if (!P_VALID(pSkillData))
        return;

    tagPetSellData data;
    data.nNum = pPet->bag.size();
    size_t itemsSize = sizeof(tagPetSellData) - 1 + pPet->bag.size() * sizeof(INT64);
    BYTE* pNewData = new BYTE[itemsSize];
    memcpy(pNewData, &data, sizeof(tagPetSellData) - 1);

    int i = 0;
    for (tagPet::_pos2petbag::const_iterator it = pPet->bag.begin();
        it != pPet->bag.end(); ++it)
    {
        INT64 n64ItemID = it->second;
        memcpy(pNewData + sizeof(tagPetSellData) - 1 + i * sizeof(INT64), &n64ItemID, sizeof(INT64));
        ++i;
    }

    DWORD dwErrorMsg = GT_INVALID;
    if (IfCanCastSkill(dwPetID, dwSkillID, dwErrorMsg))
    {
        BeginNC_PetSkillUse(dwPetID, dwSkillID, pNewData, itemsSize);
    }
    else
    {
        ShowErrorMessage(dwErrorMsg);
    }

    SAFE_DEL_ARRAY(pNewData);
}

void PetManager::TryToTraining( DWORD dwPetID, DWORD dwSkillTypeID, DWORD dwPetSkillID )
{
    // 取得宠物原型
    const tagPet* pPet = GetPet(dwPetID);
    if (!P_VALID(pPet))
        return;

    // 查看是否有此技能
    tagPetSkill* pSkill = const_cast<tagPetSkill*>(FindPetSkill(dwPetID, dwPetSkillID));
    if (!P_VALID(pSkill))
        return;

    // 取得技能原型
    const tagPetSkillData* pSkillData = PetProtoData::Inst()->FindPetSkill(dwPetSkillID);
    if (!P_VALID(pSkillData))
        return;

    tagPetExperenceData expData;
    expData.dwTypeID    = dwSkillTypeID;

    DWORD dwErrorMsg = GT_INVALID;
    if (IfCanCastSkill(dwPetID, dwPetSkillID, dwErrorMsg))
    {
        BeginNC_PetSkillUse(dwPetID, dwPetSkillID, (BYTE*)(&expData), sizeof(expData));
    }
    else
    {
        ShowErrorMessage(dwErrorMsg);
    }
}

void PetManager::SetSkillCD( tagPetSkill* pSkill, int tick )
{
    if (!P_VALID(pSkill))
        return;

    pSkill->bCoolingDown    =   true;
    pSkill->nCurrCoolDown   =   0;
    pSkill->nMaxCoolDown    =   tick * TICK_TIME;
    pSkill->dwTimeFirst     =   Kernel::Inst()->GetAccumTimeDW();
}

DWORD PetManager::OnNS_PetSkillTimer( tagNS_PetSkillTimer* pMsg, DWORD )
{
    tagPetSkill* pSkill = const_cast<tagPetSkill*>(FindPetSkill(pMsg->dwPetID, pMsg->dwPetSkillID));
    const tagPetSkillData* pSkillData = PetProtoData::Inst()->FindPetSkill(pMsg->dwPetSkillID);
    if (P_VALID(pSkill) &&
        P_VALID(pSkillData) &&
        (pSkillData->eType == EPT_Gather || pSkillData->eType == EPT_Sale)
        )
    {
        SetSkillCD(pSkill, pMsg->nValue);
    }

    return 0;
}