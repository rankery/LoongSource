#pragma once

#include "../WorldDefine/msg_pet.h"
#include "../WorldDefine/pet_skill_define.h"

//! 宠物属性发生变化的类型枚举
enum EPetAttrChangeType
{
    EPACT_Init,          //!< 所有初始化
    EPACT_Remove,        //!< 删除宠物
    EPACT_PetValue,      //!< 单个宠物发生数值属性变化
    EPACT_PetDisp,       //!< 显示属性变化
    EPACT_PetEquip,      //!< 装备变化
    EPACT_PetUnEquip,    //!< 装备卸除
    EPACT_PetState,      //!< 宠物状态变化
    EPACT_PetSkill       //!< 宠物技能变化
};

//! \file   PetEvent.h
//! \brief  宠物相关游戏事件
//! \author hyu
//! \date   2009-4-16
//! \last   2009-4-16

//! \struct tagGetRolePetsIDEvent
//! \brief  返回角色现在所有宠物信息
struct tagGetRolePetsIDEvent : public tagGameEvent
{
    DWORD               dwRoleID;       //!< 角色ID
    list<DWORD>         lstPets;        //!< 宠物列表

    tagGetRolePetsIDEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) :
    tagGameEvent(szEventName, pSenderFrame) {}
};

////! \struct tagGetPetAttrEvent
////! \brief  返回宠物属性
//struct tagGetPetAttrEvent : public tagGameEvent 
//{
//    DWORD           dwRoleID;           //!< 角色ID
//    DWORD           dwPetID;            //!< 宠物ID
//    tagPetInitAttr  petAttr;            //!< 宠物结构
//
//    tagGetPetAttrEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) :
//    tagGameEvent(szEventName, pSenderFrame) {}
//};

//! \struct tagPetAttrChange
//! \brief  宠物属性发生变化
struct tagPetAttrChange : public tagGameEvent 
{
    EPetAttrChangeType  eType;              //!< 变化类型

    DWORD           dwPetID;            //!< 宠物ID(仅当eType=EPACT_Init,EPACT_PetCall,EPACT_PetUnEquip无效)

    tagPetAttrChange(LPCTSTR szEventName, GameFrame* pSenderFrame) :
    tagGameEvent(szEventName, pSenderFrame) {}
};

//! \struct tagUsePetEggEvent
//! \brief  使用宠物蛋事件
struct tagUsePetEggEvent : public tagGameEvent
{
    INT64       n64ItemID;           //!< 物品64位ID
    tagUsePetEggEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) :
    tagGameEvent(szEventName, pSenderFrame) {}
};

//! \struct tagUsePetFoodEvent
//! \brief  使用宠物药品事件
struct tagUsePetFoodEvent : public tagGameEvent
{
    INT64       n64ItemID;           //!< 物品64位ID
    tagUsePetFoodEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) :
    tagGameEvent(szEventName, pSenderFrame) {}
};

//! \struct tagUsePetEquip
//! \brief  使用宠物装备事件
struct tagUsePetEquip : public tagGameEvent
{
    INT64       n64ItemID;           //!< 物品64位ID
    tagUsePetEquip(LPCTSTR szEventName, GameFrame* pSenderFrame) :
    tagGameEvent(szEventName, pSenderFrame) {}
};

//! \struct tagPetDispChange
//! \brief  请求宠物显示信息存入本地缓存通知
struct tagPetDispChange : public tagGameEvent 
{
    DWORD           dwPetID;            //!< 宠物ID
    tagPetDispChange(LPCTSTR szEventName, GameFrame* pSenderFrame) :
    tagGameEvent(szEventName, pSenderFrame) {}
};

//! \struct tagPetSkillCfgOpen
//! \brief  请求打开宠物技能设置窗体
struct tagPetSkillCfgOpen : public tagGameEvent 
{
    EPetskillType   eSkillType;        //!< 技能类型
    DWORD           dwPetID;            //!< 宠物ID
    DWORD           dwSkillID;          //!< 技能ID
    tagPetSkillCfgOpen(LPCTSTR szEventName, GameFrame* pSenderFrame) :
    tagGameEvent(szEventName, pSenderFrame) {}
};

//! \struct tagEnterMount
//! \brief  
struct tagEnterMount : public tagGameEvent 
{
	DWORD			dwOwnerID;
	DWORD           dwPetID;           //!< 坐骑ID
	DWORD           dwTypeID;          //!< TypeID
	tagEnterMount(LPCTSTR szEventName, GameFrame* pSenderFrame)
		: tagGameEvent(szEventName, pSenderFrame)
		, dwPetID(GT_INVALID)
		, dwTypeID(GT_INVALID)
	{}
};

//! \struct tagLeaveMount
//! \brief  
struct tagLeaveMount : public tagGameEvent 
{
	DWORD           dwPetID;           //!< 坐骑ID
	tagLeaveMount(LPCTSTR szEventName, GameFrame* pSenderFrame)
		: tagGameEvent(szEventName, pSenderFrame)
		, dwPetID(GT_INVALID)
	{}
};
