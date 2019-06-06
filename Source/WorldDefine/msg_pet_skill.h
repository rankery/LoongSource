//-----------------------------------------------------------------------------
//! filename    msg_pet_skill.h
//! author      hyu/xlguo
//! date        2009-3-24
//! last 
//! brief       宠物技能消息
//! sa          pet_define.h
//-----------------------------------------------------------------------------
#pragma once
#pragma pack(push,1)

#include "pet_define.h"
#include "pet_skill_define.h"
#include "msg_common_errorcode.h"

//----------------------------------------------------------------------------
// 错误码枚举
//----------------------------------------------------------------------------
enum EPetSkillMsgErrCode
{
    E_PetSkill_Success              =   E_Success,	// 成功
	E_PetSkill_InUse				,				// 正在使用
	E_PetSkill_CoolDown				,				// 冷却中
    E_PetSkill_Unknown				,				// 未知
};

//-----------------------------------------------------------------------------
// 添加，更新，删除宠物技能(只对本地玩家，缓冲于本地)
//-----------------------------------------------------------------------------
CMD_START(NS_AddPetSkill)
    DWORD               dwPetID;        //!< 宠物ID
    DWORD				dwSkillID;      //!< 技能数据
CMD_END

CMD_START(NS_RemovePetSkill)
    DWORD               dwPetID;        //!< 宠物ID
    DWORD               dwSkillID;      //!< 宠物技能ID
CMD_END

//-----------------------------------------------------------------------------
// 使用技能
//-----------------------------------------------------------------------------
CMD_START(NC_PetSkillUse)
	DWORD               dwPetID;        //!< 宠物ID
	DWORD               dwSkillID;      //!< 宠物技能ID
	BYTE				byData[1];		//!< 其它数据
CMD_END

CMD_START(NS_PetSkillUse)
	DWORD               dwPetID;        //!< 宠物ID
	DWORD               dwSkillID;      //!< 宠物技能ID
	DWORD				dwErrCode;		//!< 返回值
	BYTE				byData[1];		//!< 其它数据
CMD_END

//-----------------------------------------------------------------------------
// 终止工作中的技能
//-----------------------------------------------------------------------------
CMD_START(NC_PetSkillStopWorking)
	DWORD               dwPetID;        //!< 宠物ID
	DWORD               dwSkillID;      //!< 宠物技能ID
CMD_END

CMD_START(NS_PetSkillStopWorking)
	DWORD               dwPetID;        //!< 宠物ID
	DWORD               dwSkillID;      //!< 宠物技能ID
	DWORD				dwErrCode;		//!< 返回值
CMD_END

//-----------------------------------------------------------------------------
// 学习技能
//-----------------------------------------------------------------------------
CMD_START(NC_PetLearnSkill)
	DWORD               dwPetID;        //!< 宠物ID
	INT64				n64ItemID;      //!< 技能书id
CMD_END

CMD_START(NS_PetLearnSkill)
	DWORD               dwPetID;        //!< 宠物ID
	INT64				n64ItemID;      //!< 技能书id
	DWORD				dwErrCode;		
CMD_END

CMD_START(NS_PetSkillTimer)
	DWORD				dwPetID;		//!< 宠物id
	DWORD				dwPetSkillID;	//!< 宠物技能id
	BYTE				byType;			//!< 计时类型EPetTimerType
	INT					nValue;		//!< 计时值
CMD_END

#pragma pack(pop)