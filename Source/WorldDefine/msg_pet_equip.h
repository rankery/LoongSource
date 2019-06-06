//-----------------------------------------------------------------------------
//! \filename   msg_pet_skill.h
//! \author     hyu/xlguo
//! \date       2009-5-11
//! \last       2009-5-11
//! \brief      宠物技能消息
//! \sa         pet_define.h
//-----------------------------------------------------------------------------
#pragma once
#pragma pack(push,1)

#include "pet_define.h"
#include "pet_equip_define.h"
#include "msg_common_errorcode.h"

// 宠物装备物品
CMD_START(NC_PetEquip)
    DWORD       dwPetID;        //!< 宠物ID
	INT8		n8DstPos;		//!< 宠物装备栏位置
    INT64       n64ItemID;      //!< 64位itemid
CMD_END

CMD_START(NS_PetEquip)
    DWORD               dwErrCode;      //!< 错误码
    DWORD               dwPetID;        //!< 宠物ID
    tagPetEquipMsgInfo  itemData;       //!< 返回装备信息
CMD_END

// 宠物卸装物品
CMD_START(NC_PetUnequip)
	DWORD		dwPetID;
    INT16		n16PosDst;		//!< 待放入背包中位置
    INT64       n64ItemID;      //!< 64位itemid
CMD_END

CMD_START(NS_PetUnequip)
    DWORD       dwErrCode;      //!< 错误码
    DWORD		dwPetID;
    INT64       n64ItemID;      //!< 64位itemid
CMD_END

// 宠物装备位置交换
CMD_START(NC_PetEquipPosSwap)
	DWORD		dwPetID;
	INT8		n8PosDst;		//!< 装备移动到的目的位置
	INT64       n64ItemID;      //!< 64位itemid
CMD_END

CMD_START(NS_PetEquipPosSwap)
	DWORD       dwErrCode;      //!< 错误码
    DWORD		dwPetID;
	INT8		n8PosDst;		//!< 装备移动到的目的位置
	INT8		n8PosSrc;		//!< 装备原位置
	INT64       n64ItemID;      //!< 64位itemid
CMD_END

#pragma pack(pop)