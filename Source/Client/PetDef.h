#pragma once

#include "../WorldDefine/pet_define.h"
#include "../WorldDefine/msg_pet.h"
#include "../WorldDefine/pet_skill_define.h"
#include "../WorldDefine/msg_pet_skill.h"
#include "../WorldDefine/pet_equip_define.h"

class Item;


//-----------------------------------------------------------------------------
// 宠物装备结构
//-----------------------------------------------------------------------------
struct tagPetEquip
{
    INT64           n64ItemID;              //!< 装备物品64位ID
    DWORD           dwItemProtoID;          //!< 物品原型ID
};

//-----------------------------------------------------------------------------
// 宠物技能结构
//-----------------------------------------------------------------------------
struct tagPetSkill
{
    DWORD	dwTypeID;							// TypeID(带等级)
    bool    bCoolingDown;                       // 是否在冷却
    INT     nCurrCoolDown;                      // 当前冷却时间(仅供下线后再登录还未完成冷却时，同步用，其它情况均为0)
    INT     nMaxCoolDown;                       // 最大冷却时间
    DWORD   dwTimeFirst;                        // 开始冷却时的初始累积时间
};


//-----------------------------------------------------------------------------
// 设置宠物技能设置结构
//-----------------------------------------------------------------------------

// 药童
struct tagPetSkillSupply
{
    UINT8       u8MinHP;            // HP低于n%时服用
    INT64       dwHPItemID;         // 加HP物品ID
    bool        bHPEnabled;         // HP功能是否开启

    UINT8       u8MinMP;            // MP低于n%时服用
    INT64       dwMPItemID;         // 加MP物品ID
    bool        bisMPEnabled;       // MP功能是否开启

    UINT8       u8Toughness;        // 持久力
    INT64       dwTouItemID;        // 物品id
    bool        bisTouEnabled;      // 加持久力功能是否开启
};

// 觅食
struct tagPetSkillForage
{
    UINT8       u8MinSpirit;        // 灵力低于n%时使用
    INT64       dwItemID;           // 物品ID
    bool        bisEnabled;         // 功能是否开启
};

// 拾取
struct tagPetSkillPickup
{
    bool            bMoney:1;             // 是否拾取金钱
    bool            bEquip:1;             // 是否拾取装备
    bool            bMaterial:1;          // 是否拾取材料
    bool            bTrophy:1;            // 是否拾取战利品
    bool            bMedicine:1;          // 是否拾取药品
    bool            bOthers:1;            // 是否拾取其他掉落
};


//-----------------------------------------------------------------------------
// 宠物结构
//-----------------------------------------------------------------------------

const float   PET_SCALE_PERCENT_BASE  =   10000;

//! struct  tagPetProto
//! brief   宠物类型原始数据
struct tagPetProto 
{
    DWORD           dwID;                           //!< 类型ID(注意与玩家拥有的宠物ID不同)

    // 数组依次为幼年模型，成年模型
    tstring         strModelName[EPMT_NUM]; //!< 模型
    tstring         strModelPath[EPMT_NUM]; //!< 模型路径
    EAniType        eAniType[EPMT_NUM];     //!< 模型动画类型
    list<tstring>   lstSkinPath[EPMT_NUM];  //!< 模型蒙皮路径
    tstring         strAct1;                //!< 骑乘动画动作1
    tstring         strAct2;                //!< 骑乘动画动作2
    Vector3         boundingBox;            //!< 用于骑乘模型检测碰撞时的包围盒大小
    float           scalePercent;           //!< 以PET_SCALE_PERCENT_BASE为底数的百分比，亦会对碰撞盒产生影响
    int             carryLevel;             //!< 携带等级
    bool            bBinding;               //!< 是否绑定
};


//! \struct  tagPet
//! \brief   出生在游戏世界中宠物
//! \deatil  宠物总属性结构
//! \sa      EPetAttribute
struct tagPet
{
    typedef map<DWORD, tagPetSkill*> _id2skillInfo;
    typedef map<int, INT64> _pos2petbag;
    typedef map<int, tagPetEquip*> _pos2Equip;

    DWORD                   dwID;                       //!< 当宠物出生时,在游戏世界生成的唯一ID
    DWORD                   dwTypeID;                   //!< 宠物类型ID,对应tagPetProto中的ID
    tstring                 strName;                    //!< 玩家起的宠物名,以后可通过某些道具修改
    UPetState               state;                      //!< 宠物状态
    INT                     nAttrA[ECSPA_End];          //!< 宠物数值属性
    _pos2Equip              mapEquip;                   //!< 宠物装备栏
    _id2skillInfo           Skill;                      //!< 宠物拥有的技能
    tagPetSkillForage       skillForageCfg;
    tagPetSkillPickup       skillPickupCfg;
    tagPetSkillSupply       skillMedicineFeedCfg;
    _pos2petbag             bag;                        //!< 宠物行囊中的物品链接(行囊位置2物品64位id)

    tagPet()
    {
        state.byPetState = 0;

        skillForageCfg.bisEnabled   =   false;
        skillForageCfg.dwItemID     =   GT_INVALID;
        skillForageCfg.u8MinSpirit  =   0;

        skillPickupCfg.bEquip   =   false;
        skillPickupCfg.bMaterial=   false;
        skillPickupCfg.bMedicine=   false;
        skillPickupCfg.bMoney   =   false;
        skillPickupCfg.bTrophy  =   false;
        skillPickupCfg.bOthers  =   false;

        skillMedicineFeedCfg.bHPEnabled     =   false;
        skillMedicineFeedCfg.bisMPEnabled   =   false;
        skillMedicineFeedCfg.bisTouEnabled  =   false;
        skillMedicineFeedCfg.dwHPItemID     =   GT_INVALID;
        skillMedicineFeedCfg.dwMPItemID     =   GT_INVALID;
        skillMedicineFeedCfg.dwTouItemID    =   GT_INVALID;
        skillMedicineFeedCfg.u8MinHP        =   0;
        skillMedicineFeedCfg.u8MinMP        =   0;
        skillMedicineFeedCfg.u8Toughness    =   0;
    }
    ~tagPet()
    {
        // 释放装备
        for (_pos2Equip::iterator it = mapEquip.begin();
            it != mapEquip.end(); ++it)
        {
            SAFE_DELETE(it->second);
        }
        mapEquip.clear();

        // 清空行囊
        bag.clear();

        // 清空技能
        for (_id2skillInfo::iterator it = Skill.begin();
            it != Skill.end(); ++it)
        {
            SAFE_DELETE(it->second);
        }
        Skill.clear();
    }
};

struct tagPetSkillData
{
    DWORD                   dwPetSkillID;	    // 宠物技能ID
    DWORD                   dwPetSkillTypeID;   // 宠物技能类型ID
    int                     nLevel;             // 宠物技能级别

    EPetskillType           eType;			// 宠物技能类型
    EPetskillType2          eType2;			// 宠物技能类型2
    EPetskillType3          eType3;			// 宠物技能类型3
    EPetskillCastType       eCastType;      // 施放类型
    EPetskillCastCondition  eCastCondition; // 施放状态
    DWORD                   dwCDTime;       // 冷却时间
    DWORD                   cast_condition; // 投放条件
    int                     wuxing_cost;
    int                     spirit_cost;

    tstring                 strType;        // 类型
    tstring                 strName;        // 技能名称
    tstring                 strDesc;        // 技能描述
    tstring                 strIconPath;    // 图标文件
};
