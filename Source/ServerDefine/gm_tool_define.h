//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: msg_rt_c.h
// author: 
// actor:
// data: 2009-04-27
// last:
// brief: GM服务端与GM工具之间的数据定义
//-----------------------------------------------------------------------------
#pragma once
#pragma pack(push, 1)

#define Notice_Title 30
#define Notice_Content 256
#define Notice_Link 256
#define System_Cmd_Length 1024

// 列举与定时有关的消息
enum ETimeLimitedMsg
{
	ETLM_Start,

	ETLM_Double = 1,
	ETLM_RightNotice,
	ETLM_AutoNotice,
	ETLM_AutoChatNotice,

	ETLM_End
};

enum 
{
	E_GMTool_Err							= -1,
	E_GMTool_Success						= 0,

	E_GMTool_GM_AlreadyExist				= 1,		// GM已经存在
	E_GMTool_GM_NotExist					= 2,		// GM不存在
	E_GMTool_GM_CreateFail					= 3,		// GM创建失败

	E_GMTool_ResumeRole_NameAlreadyExist	= 10,		// 角色名已经存在（重新起名）
	E_GMTool_ResumeRole_RoleNotExist		= 11,		// 恢复的角色不存在

	E_GMTool_ModifyRole_RoleNotExist		= 20,		// 角色不存在

	E_GMTool_NullContent					= 30,		// 内容不能为空

	E_GMTool_RoleNotExist					= 40,		// 角色不存在

	E_GMTool_DeleteItem_ItemNotExist		= 50,		// 物品不存在
	E_GMTool_CreateItem_EquipNumErr			= 51,		// 装备数量错误
	E_GMTool_CreateItem_ItemQualityErr		= 52,		// 物品品级错误

	E_GMTool_Time_Err						= 100,		// 时间不对	

	E_RTClient_IpBind_Err					= 110,		// ip绑定错误
	E_RTClient_Ip_Err						= 111,		// ip绑定错误

	E_GMTool_TimeSetError					= 120,		// 时间设置错误，设置的开始时间已过

	R_RTClient_NotEnoughPri					= 130,		// 权限不够，不能查询

	E_GMTool_ResumeItemNotExit				= 140,		// 要恢复的装备不存在
	E_GMTool_ResumeItemNotEquip				= 141,		// 要恢复的物品不是装备
};

enum EDoubleType
{
	EDoubleType_NULL		= -1,
	EDoubleType_Exp			= 0,				//双倍经验
	EDoubleType_Item		= 1,				//双倍物品
	EDoubleType_End			= 2,
};

const INT NUM_EDT			= EDoubleType_End - EDoubleType_Exp;
#define EDT_VALID(edt)	((edt) > EDoubleType_NULL && (edt) < EDoubleType_End)	


//马跑灯/自动循环公告颜色设置
enum EColorType
{
	EColorType_NULL			= -1,
	EColorType_Red			= 0,
	EColorType_Yellow		= 1,
	EColorType_Green		= 2,	
	EColorType_Blue			= 3,
	EColorType_Orange		= 4,
	EColorType_End			= 5,
};


#pragma pack(pop)