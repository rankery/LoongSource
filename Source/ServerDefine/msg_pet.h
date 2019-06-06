//-----------------------------------------------------------------------------
// filename: msg_pet.h
// author: 
// actor:
// data: 2009-05-12
// last: 
// brief: 宠物服务器消息格式
//-----------------------------------------------------------------------------
#pragma once
#include "pet_define.h"
#pragma pack(push, 1)

//-----------------------------------------------------------------------------
// 初始化时获得petid
//-----------------------------------------------------------------------------
CMD_START(NDBC_GetPetInfo)
CMD_END;

CMD_START(NDBS_GetPetInfo)
	DWORD	dwMaxPetID;
CMD_END;

//-----------------------------------------------------------------------------
// 创建宠物灵魂
//-----------------------------------------------------------------------------
CMD_START(NDBC_CreatePetSoul)
	INT64				n64Serial;
	tagDBPetSoulCreate	createData;
CMD_END;

CMD_START(NDBS_CreatePetSoul)
	INT64				n64Serial;
	DWORD				dwErrCode;
	tagDBPet			petDataLoad;
CMD_END;

//-----------------------------------------------------------------------------
// 更新PetSoul
//-----------------------------------------------------------------------------
CMD_START(NDBC_UpdatePetSoul)
	INT				nPetSoulNum;
	tagDBPetAtt		petAtt;
CMD_END;

// CMD_START(NDBS_UpdatePetSoul)
// CMD_END;

//-----------------------------------------------------------------------------
// 删除PetSoul
//-----------------------------------------------------------------------------
CMD_START(NDBC_DelPetSoul)
	INT				nPetSoulNum;
	DWORD			dwPetID[1];
CMD_END;

// CMD_START(NDBS_DelPetSoul)
// CMD_END;

//-----------------------------------------------------------------------------
// 宠物装备添加删除
//-----------------------------------------------------------------------------
CMD_START(NDBC_PetEquip)
	tagItem item;	
	bool	bInsert;
CMD_END;



//-----------------------------------------------------------------------------
// 插入宠物技能
//-----------------------------------------------------------------------------
CMD_START(NDBC_InsertPetSkill)
	DWORD			dwPetID;
	tagDBPetSkill	petSkill;
CMD_END;

//-----------------------------------------------------------------------------
// 删除宠物技能
//-----------------------------------------------------------------------------
CMD_START(NDBC_DelPetSkill)
	DWORD	dwPetID;	
	DWORD	dwPetSkillTypeID;
CMD_END;

#pragma pack(pop)