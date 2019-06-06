#pragma once

#include "../WorldDefine/pet_define.h"
#include "../WorldDefine/pet_equip_define.h"
#include "../WorldDefine/msg_pet_equip.h"
#include "PetDef.h"

struct  tagPet;
struct  tagPetDispInfo;
struct  tagNS_GetRolePetsID;
struct  tagNS_GetPetAttr;
struct  tagNS_GetPetDispInfo;
struct  tagNS_PetDispInfoChange;
struct  tagNS_GetRoleCurrPetID;
struct  tagNS_CallPet;
struct  tagNS_UsePetEgg;
struct  tagUsePetEggEvent;
struct  tagMsgInputBoxEvent;
struct  tagNS_PetAttrChange;
class   Item;
struct  tagPetEquip;
struct  tagPetSkill;
struct  tagNS_PetPourExp;
struct  tagNS_PetSkillUse;
struct  tagNS_AddPetSkill;
struct  tagNS_RemovePetSkill;
struct  tagPetSkillMsgInfo;
struct  tagUsePetFoodEvent;

const int MAX_PET_COUNT             = 5;
const int MAX_PET_GATHER_DISTANCE   = 50;   //!< unit:tile

//! \class  PetManager
//! \author hyu
//! \date   2009-3-27
//! \brief  宠物管理器
//! \remark singleton
class PetManager
{
public:
    typedef map<DWORD, tagPet*>         _id2pet;
    typedef map<DWORD, tagPetDispInfo*> _id2petdisp;
public:
    PetManager(void);
    ~PetManager(void);

    VOID Init();
    VOID Destroy();

    void Update();

    //! Singleton
    static PetManager* Inst();

    //! 发送获得宠物属性请求(dwPetID为GT_INVALID时请求该角色所有宠物信息)
    void BeginNC_GetPetAttr( DWORD dwRoleID, DWORD dwPetID );
    DWORD OnNS_GetPetAttr(tagNS_GetPetAttr* pMsg, DWORD);

    //! 本地玩家属性发生变化
    DWORD OnNS_PetAttrChange(tagNS_PetAttrChange* pMsg, DWORD);

    //! 获得某一宠物非数值显示信息(需要本地缓冲上),如返回NULL,则发送请求
    const tagPetDispInfo* FindPetDispInfo(DWORD dwRoleID, DWORD dwPetID );
    DWORD OnNS_GetPetDispInfo(tagNS_GetPetDispInfo* pMsg, DWORD);
    DWORD OnNS_PetDispInfoChange(tagNS_PetDispInfoChange* pMsg, DWORD);

    //! 删除宠物
    void BeginNC_DeletePet(DWORD dwPetID);
    DWORD OnNS_DeletePet(tagNS_DeletePet* pMsg, DWORD);

    //! 设置宠物状态
    void BeginSetPetState( DWORD dwPetID, EPetState petState, bool bEnabled);

    //! 灌注经验
    void BeginNC_GetPetPourExpMoneyNeed(DWORD dwPetID);
    DWORD OnNS_GetPetPourExpMoneyNeed(tagNS_GetPetPourExpMoneyNeed* pMsg, DWORD);
    void BeginPourExp( DWORD dwPetID );
    DWORD OnNS_PetPourExp(tagNS_PetPourExp* pMsg, DWORD);

    //! 锁定宠物
    void BeginNC_PetSetLock(DWORD dwPetID, INT64 n64ItemID, bool bSet);
    DWORD OnNS_PetSetLock(tagNS_PetSetLock* pMsg, DWORD);
    void SetPetLock(INT64 n64ItemID, bool bSet);

    //! 玩家穿宠物装备
    void BeginNC_PetEquip(DWORD dwPetID, INT8 pos, INT64 n64ItemID);
    DWORD OnNS_PetEquip(tagNS_PetEquip* pMsg, DWORD);

    //! 玩家卸载宠物装备
    void BeginNC_PetUnequip(DWORD dwPetID, INT64 n64ItemID, INT16 n16PosDst);
    DWORD OnNS_PetUnequip(tagNS_PetUnequip* pMsg, DWORD);

    //! 玩家同一装备栏交换位置
    void BeginNC_PetEquipPosSwap(DWORD dwPetID, INT64 n64ItemID, INT8 n8PosDst);
    DWORD OnNS_PetEquipPosSwap(tagNS_PetEquipPosSwap* pMsg, DWORD);

    // 技能相关
    void BeginNS_PetLearnSkill(DWORD dwPetID, INT64 n64ItemID);
    DWORD OnNS_PetLearnSkill(tagNS_PetLearnSkill* pMsg, DWORD);
    //! 使用技能
    void TryToUsePetSkill(DWORD dwPetID, DWORD dwSkillID);
    //! 使用小贩技能
    void TryToSellItem(DWORD dwPetID, DWORD dwSkillID);
    //! 使用训炼技能
    void TryToTraining(DWORD dwPetID, DWORD dwSkillTypeID, DWORD dwPetSkillID);

    //! 发送使用技能请求
    void BeginNC_PetSkillUse(DWORD dwPetID, DWORD dwSkillID, BYTE* pData, size_t dataLength);
    //! 使用技能请求返回
    DWORD OnNS_PetSkillUse(tagNS_PetSkillUse* pMsg, DWORD);

    //! 获得一个技能
    DWORD OnNS_AddPetSkill(tagNS_AddPetSkill* pMsg, DWORD);

    //! 遗忘一个技能
    DWORD OnNS_RemovePetSkill(tagNS_RemovePetSkill* pMsg, DWORD);

    //! 获当前冷却时间
    void GetSkillCDTime(DWORD dwPetID, DWORD dwSkillID, FLOAT &fCDCount, FLOAT &fRestoreRatio);
    //! 更新是否冷却
    void UpdateSkillCD();

    //! 开始设置技能冷却
    void SetSkillCD(tagPetSkill* pSkill, int tick);

    //! 收到服务器同步开始冷却消息
    DWORD OnNS_PetSkillTimer(tagNS_PetSkillTimer* pMsg,DWORD);



    // 技能设置信息
    void SetSkillForageCfg(DWORD dwPetID, tagPetSkillForage& cfg);
    void SetSkillPickupCfg(DWORD dwPetID, tagPetSkillPickup& cfg);
    void SetSkillSupplyCfg(DWORD dwPetID, tagPetSkillSupply& cfg);

    //! 自动使用宠物技能
    void AutoUseSkill();

    //! 升阶物品使用
    void OnUseLvlupStep(Item* pItem);
    DWORD OnNS_PetUpStep(tagNS_PetUpStep* pMsg, DWORD);
    DWORD OnBoxPetUpStep(tagMsgBoxEvent* pEvent);
    DWORD m_pLupSkillID;
    DWORD m_pLupStep;

    //! 灌注物品使用
    void OnUseExpPour(Item* pItem);
    DWORD OnBoxUseExpPour(tagMsgBoxEvent* pEvent);
    INT64 m_tmpPourItemID;

    //! 提升资质使用
    void OnUseEnhance(Item* pItem);
    DWORD OnNS_PetEnhance(tagNS_PetEnhance* pMsg, DWORD);

    //! 灵兽药品使用
    DWORD OnUsePetFood(tagUsePetFoodEvent* pEvent);
    DWORD OnNS_PetFood(tagNS_PetFood* pMsg, DWORD);

    // 容器相关

    //! 返回容器
    const _id2pet* GetPetList() { return &m_mapPetsOfLP; }
    //! 返回宠物结构
    const tagPet* GetPet(DWORD dwPetID);
    //! 返回当前召唤的宠物(GT_INVALID为无如唤宠物)
    DWORD GetCurrPetID();
    //! 返回宠物的装备
    const tagPetEquip* FindPetEquip(DWORD dwPetID, INT8 pos);
    //! 返回宠物的技能
    const tagPetSkill* FindPetSkill(DWORD dwPetID, DWORD dwSkillID);
    //! 按类型查找
    const tagPetSkill* FindPetSkillbyType(DWORD dwPetID, EPetskillType eType);
    //! 返回宠物状态
    bool IsPetCalled(DWORD dwPetID);
    bool IsPetWorking(DWORD dwPetID);
    bool IsPetPreparing(DWORD dwPetID);
    bool IsPetRiding(DWORD dwPetID);
    //! 得到宠物可携带等级
    int GetCarryLevel(DWORD dwPetID);
    //! 得到宠物当前骑乘数量
    int GetPetCurrMountNum(DWORD dwPetID);
    //! 得到宠物最大骑乘数量
    int GetPetMaxMountNum(DWORD dwPetID);
    
    // 宠物行囊相关

    void AddToBag(DWORD petId, int pos, Item* pItem);
    void PopFromBag(DWORD petId, int pos);
    void SwapBagItem(DWORD petId, int posSrc, int posDst);
    void ClearBag(DWORD petId);
    Item* GetItem(DWORD petId, int pos);   // 无效返回GT_INVALID
    Item* GetItem(DWORD petId, INT64 n64ItemID);    // 无效返回GT_INVALID

    bool IsExistInErrGroundItem(INT64 n64serial);

private:
    //! 释放资源
    void ClearAll();

    //! 使用宠物蛋
    DWORD OnUsePetEggEventHandler(tagUsePetEggEvent* pEvent);
    //! 输入完消息框返回的事件
    DWORD OnCommitPetName(tagMsgInputBoxEvent* pEvent);
    //! 使用完宠物蛋后的反馈
    DWORD OnNS_UsePetEgg(tagNS_UsePetEgg* pMsg, DWORD);

    //! 加入一个装备到宠物装备栏
    bool AddEquip(DWORD dwPetID, const tagPetEquipMsgInfo& petEquip);
    //! 从宠物装备栏移出一个装备
    bool RemoveEquip(DWORD dwPetID, INT64 n64ItemID);
    bool RemoveEquip(DWORD dwPetID, int pos);

    //! 显示错误码
    void ShowErrorMessage(DWORD dwErrCode);

    //! 技能
public:
    bool AddSkill(DWORD dwPetID, const tagPetSkillMsgInfo& petSkill);
    bool RemoveSkill(DWORD dwPetID, DWORD dwSkillID);
    // ! 是否当前状态可以投放技能
    bool IfCanCastSkill(DWORD dwPetID, DWORD dwSkillID, DWORD &dwErrCodeOut);

private:
    //------------------------------------------------------------------------
    // 工具
    //------------------------------------------------------------------------
    TSFPTrunk<PetManager>       m_Trunk;
    TObjRef<Util>				m_pUtil;
    TObjRef<NetCmdMgr>			m_pCmdMgr;
    TObjRef<GameFrameMgr>		m_pFrameMgr;
    TObjRef<NetSession>			m_pSession;

    //------------------------------------------------------------------------
    // 本地玩家宠物属性缓冲
    //------------------------------------------------------------------------
private:
    _id2pet                     m_mapPetsOfLP;          //!< 本地玩家宠物缓冲

    _id2petdisp                 m_mapPetDispBuffer;     //!< 宠物显示信息缓冲
    set<DWORD>                  m_setDispSentReq;       //!< 发送的请求宠物显示信息请求列表

    INT                         m_nPetNameMaxLength;    //!< 宠物名字最大长度

    INT64                       m_n64CurrPetEggID;      //!< 当前要使用的宠物蛋id

    bool                        bHasInitialized;        //!< 是否初始化完毕

    DWORD                       m_autoSkillFirst;       //!< 使用自动技能需要间隔时间

    set<INT64>                  m_setErrGroundItemSerial;  //!< 自动拾取错误的地物表，定时清空
    DWORD                       m_ErrGroundItemTime;        //!< 上次清空拾取错误列表时间
};
