//-----------------------------------------------------------------------------
//! filename    msg_pet.h
//! author      hyu/xlguo
//! date        2009-3-24
//! last 
//! brief       宠物消息
//! sa          pet_define.h
//-----------------------------------------------------------------------------
#pragma once
#pragma pack(push,1)

#include "pet_define.h"
#include "msg_common_errorcode.h"

//----------------------------------------------------------------------------
// 错误码枚举
//----------------------------------------------------------------------------
enum 
{
	E_Pets_UnkownErr		=-1,
	E_Pets_Success			= 0,

	E_Pets_StateLimit               =   1,
	E_Pets_Soul_NotExist            =   2,
	E_Pets_Soul_AlreadyExist        =   3,
	E_Pets_Soul_PtrNotValid         =   4,
	E_Pets_Soul_NumExceed           =   5,
	E_Pets_Soul_PetEggErr           =   6,
	E_Pets_Soul_NameIllegal         =   7,
	E_Pets_Soul_MasterNotFound      =   8,
	E_Pets_Soul_RoleExpNotEnough    =   9,
	E_Pets_Soul_RoleMoneyNotEnough  =   10,
	E_Pets_Soul_GradeNotFit         =   11,

	E_Pets_Pet_CalledNumExceed      =   12,
	E_Pets_Pet_AlreadyCalled        =   13,
	E_Pets_Pet_NotCalled            =   14,
	E_Pets_Pet_NotExist             =   15,
	E_Pets_AlreadyMaxAptitude       =   16,
	E_Pets_QualityNotFit            =   17,
	E_Pets_EnhanceFailed            =   18,
	E_Pets_ItemNotValid             =   19,
	E_Pets_PetSkill_Master_NotExist =   21,
	E_Pets_PetSkill_InvalidSkillID  =   22,
	E_Pets_PetSkill_Skill_NotExist  =   23,
	E_Pets_PetSkill_AlreadyStarted  =   24,
	E_Pets_PetSkill_Learn_PetLvlLimit           =   25,
	E_Pets_PetSkill_Learn_HasHighLvlSkill       =   26,
	E_Pets_PetSkill_Learn_Type3NotFit           =   27,
	E_Pets_PetSkill_Learn_StageNotFit           =   28,
	E_Pets_PetSkill_Learn_LvlNotEnough          =   29,
	E_Pets_PetSkill_Learn_PotentialNotEnough    =   30,
	E_Pets_PetSkill_Learn_LvlNotRight           =   31,
	E_Pets_PetSkill_Learn_ItemInvalid           =   32,
	E_Pets_PetSkill_Learn_MaxLeanNum            =   33,
	E_Pets_PetSkill_Learn_StateLimit            =   34,
	E_Pets_PetSkill_Use_ItemNotExist            =   35,
	E_Pets_PetSkill_Use_CoolingDown             =   36,
	E_Pets_PetSkill_Use_WuXingEnergyNotEnough   =   37,
	E_Pets_PetSkill_Use_MaxTalentCount          =   38,
	E_Pets_PetSkill_Use_WorkCounting            =   39,
	E_Pets_PetSkill_Use_StateLimit              =   40,
	E_Pets_PetSkill_Use_NotActiveSkill          =   41,
	E_Pets_PetSkill_Use_SkillProtoInvalid       =   42,
	E_Pets_PetSkill_Use_SpiritNotEnough         =   43,
	E_Pets_PetSkill_Learn_NoPreSkill			=	44,
	E_Pets_Soul_CreateDBErr						=   45,		// 创建宠物时 数据库错误
	E_Pets_Soul_CreateInitErr					=   46,		// 创建宠物时 从数据库读取时初始化错误
	E_Pets_PetEquip_Soul_NotExist               =   50,
	E_Pets_PetEquip_Master_NotExist             =   51,
	E_Pets_PetEquip_BagIndexNotFree             =   52,
	E_Pets_PetEquip_BagFull                     =   53,
	E_Pets_PetEquip_PetEquipNotInBag            =   54,
	E_Pets_PetEquip_UnknownEquipProto           =   55,
	E_Pets_PetEquip_IsNotPetEquip               =   56,
	E_Pets_PetEquip_PetLevelNotEnough           =   57,
	E_Pets_PetEquip_EquipIsNotOnPet             =   58,
	E_Pets_PetEquip_EquipAlreadyInPos           =   59,
	E_Pets_PetEquip_InvalidDstPos               =   60,

    E_PetPourExpNotEnough                       =   70,
	E_PetRoleLvlNotEnough						=	71,
	E_Pets_Mount_DstRoleNotFound				=	72,
	E_Pets_Mount_MountSoulNotFound				=	73,
	E_Pets_Mount_SrcRoleNotFound				=	74,
	E_Pets_Mount_DstRoleRefuse					=	75,
	E_Pets_Lock_AlreadyLocked					=	76,
	E_Pets_Lock_NotLocked						=	77,
	E_Pets_Lock_Locked							=	78,
	E_Pets_Delete_HasEquip						=	79,			// 宠物携带装备，无法删除
    E_Pets_Carrylevel_NotEnough                 =   80,
	E_Pets_Food_CoolingDown		                =   81,			// 宠物食品正在冷却
	E_Pets_Soul_IsWorking						=	82,			// 工作中

	E_Pets_PetSkill_NoMaster					=	91,			// 无主人
	E_Pets_PetSkill_NoBody						=	92,			// 无实体
	E_Pets_PetSkill_NoMap						=	93,			// 无地图
	

	E_Pets_PetSkill_Gather_NoResource			=	94,			// 无资源点
	E_Pets_PetSkill_Gather_NoGatherSkill		=	95,			// 无生产技能
	E_Pets_PetSkill_Gather_NoBagSpace			=	96,			// 背包空间不足
	E_Pets_PetSkill_Gather_ResIsTaged			=	97,			// 资源已被占用

	E_Pets_PetSkill_Sale_NoBagItem				=	100,		// 背包无此物品
	E_Pets_PetSkill_Sale_BagItemCoolingDown		=	101,		// 背包物品冷却中
	E_Pets_PetSkill_Sale_BagItemCantSale		=	102,		// 背包物品无法销售
	E_Pets_PetSkill_Sale_BagLock				=	103,		// 行囊被锁

	E_Pets_PetSkill_Experance_NoProductSkill	=	110,		// 没有生产技能
	
	E_Pets_PetSkill_Store_NoBagItem				=	120,		// 指定物品不存在
	E_Pets_PetSkill_Store_NoBagItemCoolingDown	=	121,		// 指定物品冷却中
	E_Pets_PetSkill_Store_BagLock				=	122,		// 背包锁定

	E_Pets_PetSkill_Wuxing_BagFull				=	130,		// 背包已满无法获得物品
	E_Pets_PetSkill_Wuxing_ItemNotValid			=	131,		// 物品无效

	E_Pets_PetSkill_MasterDead					=	140,			// 主人已死
	E_Pets_PetSkill_RoleLvlLimit				=	141,			// 主人等级不足
	E_Pets_PetSkill_MedicineCooldown			=	142,			// 药品冷却中
	E_Pets_Soul_IngegrateErr					=	150,		// 整合到role时错误

    E_Pets_Skill_MountingLimit                  =   300,        // 驾御状态下无法使用该灵兽技能
    E_Pets_Skill_UnMountingLimit                =   301,        // 非驾御状态下无法使用该灵兽技能
    E_Pets_Skill_PreparingLimit                 =   302,        // 预备驾御状态下无法使用该灵兽技能
    E_Pets_Skill_UnPreparingLimit               =   303,        // 非预备驾御状态下无法使用该灵兽技能
    E_Pets_Skill_WorkingLimit                   =   304,        // 工作状态下无法使用该灵兽技能
    E_Pets_Skill_UnWorkingLimit                 =   305,        // 非工作状态下无法使用该灵兽技能
    E_Pets_Skill_CalledLimit                    =   306,        // 召唤状态下无法使用该灵兽技能
    E_Pets_Skill_UnCalledLimit                  =   307,        // 非召唤状态下无法使用该灵兽技能

    E_PetErrorEnd,
};



//------------------------------------------------------------------------------

//! \struct  tagNC_GetPetAttr
//! \brief   获得宠物属性
CMD_START(NC_GetPetAttr)
    DWORD           dwRoleID;       //!< 角色ID
    DWORD           dwPetID;        //!< 宠物ID(GT_INVALID时为取此玩家所有宠物)
CMD_END

//! \struct  tagNS_GetPetAttr
//! \brief   获得宠物属性
CMD_START(NS_GetPetAttr)
    DWORD           dwRoleID;       //!< 玩家ID
    tagPetInitAttr	petAttr;        //!< 宠物结构
CMD_END

//! \struct  tagNC_DeletePet
//! \brief   删除宠物
CMD_START(NC_DeletePet)
    DWORD           dwPetID;        //!< 宠物id
CMD_END

CMD_START(NS_DeletePet)
    DWORD           dwErrCode;      //!< 错误码
    DWORD           dwPetID;        //!< 宠物id
CMD_END

//------------------------------------------------------------------------------

//! \struct  tagNC_GetPetDispInfo
//! \brief   获得某一宠物非数值显示信息(需要本地缓冲上)
CMD_START(NC_GetPetDispInfo)
    DWORD           dwRoleID;                   //!< 角色ID
    DWORD           dwPetID;                    //!< 角色拥有的宠物ID
CMD_END

//! \struct  tagNS_GetPetDispInfo
//! \brief   同上,返回
CMD_START(NS_GetPetDispInfo)
    DWORD           dwPetID;                    //!< 宠物ID
    tagPetDispInfo  DispData;                   //!< 信息
CMD_END

//! \struct  tagNS_PetDispInfoChange
//! \brief   宠物同屏显示信息更改时的九宫格广播,即宠物名,成长阶段变更时广播
CMD_START(NS_PetDispInfoChange)
    DWORD           dwPetID;                    //!< 宠物ID
    tagPetDispInfo  DispData;                   //!< 信息
CMD_END

//-----------------------------------------------------------------------------
//  本地角色宠物相关
//-----------------------------------------------------------------------------

CMD_START(NC_UsePetEgg)
    INT64           n64ItemID;                  //!< 宠物蛋id
    TCHAR           szPetName[X_SHORT_NAME];    //!< 玩家起的宠物名
CMD_END

CMD_START(NS_UsePetEgg)
    INT64           n64ItemID;                  //!< 宠物蛋64位id
    DWORD			dwErrCode;                  //!< 错误码
CMD_END

//! \struct  tagNS_PetAttrChange
//! \brief   同步本地角色宠物数值属性变化,针对要求实时性强的数据字段,通过服务器主动变化通知
CMD_START(NS_PetAttrChange)
    DWORD           dwPetID;        //!< 宠物ID
    INT8            u8AttrType;     //!< 宠物属性类别(ECSPetAtt)
    INT32           u32NewValue;    //!< 属性新值
CMD_END

//! \struct tagNC_SetPetState
CMD_START(NC_SetPetState)
    DWORD           dwPetID;        //!< 宠物ID
    UINT8           ePetState;      //!< 状态类型(EPetStateType)
    bool            bVal;           //!< 状态值
CMD_END

//! \struct tagNS_SetPetState
CMD_START(NS_SetPetState)
	DWORD           dwPetID;        //!< 宠物ID
	UINT8           ePetState;      //!< 状态类型(EPetStateType)
	DWORD			dwErrCode;
CMD_END

//! \struct tagNC_PetPourExp
//! \brief  灌注经验
CMD_START(NC_GetPetPourExpMoneyNeed)
	DWORD           dwPetID;        //!< 宠物ID
CMD_END

//! \struct tagPetNS_PourExp
CMD_START(NS_GetPetPourExpMoneyNeed)
	DWORD           dwPetID;        //!< 宠物id
	INT64			n64MoneyNeed;
CMD_END


//! \struct tagNC_PetPourExp
//! \brief  灌注经验
CMD_START(NC_PetPourExp)
    DWORD           dwPetID;        //!< 宠物ID
	INT64			n64ItemID;		//!< 64位物品ID
CMD_END

//! \struct tagNS_PetPourExp
CMD_START(NS_PetPourExp)
    DWORD           dwErrCode;      //!< 错误码
    DWORD           dwPetID;        //!< 宠物id
CMD_END

// 第一次客户端发n64itemid有效
// 第一次服务器回ndststep和ndststep有效
// 第二次客户端发ndststep和ndststep有效 n64itemid无效
//! \struct tagNC_PetUpStep
//! \brief  升阶
CMD_START(NC_PetUpStep)
	INT64			n64ItemID;
	INT				nDstStep;
	DWORD			dwSkillID;
CMD_END

//! \struct tagNC_PetUpStep
CMD_START(NS_PetUpStep)
	DWORD           dwErrCode;      //!< 错误码
	INT64			n64ItemID;
	INT				nDstStep;
	DWORD			dwSkillID;
CMD_END


//! \struct tagNC_PetEnhance
//! \brief  资质提升
CMD_START(NC_PetEnhance)
	INT64			n64ItemID;
CMD_END

//! \struct tagNS_PetEnhance
CMD_START(NS_PetEnhance)
	DWORD           dwErrCode;      //!< 错误码
	INT64			n64ItemID;
CMD_END

//! \struct tagNC_PetFood
//! \brief  使用药品
CMD_START(NC_PetFood)
    INT64			n64ItemID;
CMD_END

//! \struct tagNS_PetFood
CMD_START(NS_PetFood)
    DWORD           dwErrCode;      //!< 错误码
    INT64			n64ItemID;
CMD_END

//! \struct tagNS_Mount
//! \骑乘
CMD_START(NS_Mount)
	DWORD	dwRoleID;
	DWORD	dwPetTypeID;
	DWORD	dwPetID;
CMD_END

//! \struct tagNS_Mount2
//! \第二人骑乘
CMD_START(NS_Mount2)
	DWORD	dwPetID;
	DWORD	dwRoleID;
CMD_END

// 邀请骑乘
// 主驾驭者->服务器	NC_MountInvite 
// 服务器->被邀请者	NS_MountOnInvite
// 被邀请者->服务器	NC_MountOnInvite
// 服务器->主驾驭者	NS_MountInvite

//! \struct tagNS_MountInvite
//! \被邀请
CMD_START(NS_MountOnInvite)
	DWORD	dwSrcRoleID;
CMD_END

CMD_START(NC_MountOnInvite)
	DWORD	dwSrcRoleID;
	BOOL	bReceive;
CMD_END

//! \struct tagNS_MountInvite
//! \邀请
CMD_START(NC_MountInvite)
	DWORD	dwDstRoleID;
CMD_END

CMD_START(NS_MountInvite)
	DWORD	dwDstRoleID;
	DWORD	dwErrCode;
CMD_END

//! \struct tagNS_PetSetLock
//! \邀请
CMD_START(NC_PetSetLock)
	DWORD	dwPetID;
	INT64	n64ItemID;
	bool	bSet;
CMD_END

CMD_START(NS_PetSetLock)
	DWORD	dwPetID;
	DWORD	dwErrCode;
CMD_END


#pragma pack(pop)
