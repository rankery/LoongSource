#pragma once

//------------------------------------------------------------------------
// 所有网络消息共同的错误码
// 
// 除了E_Success，所有错误码均为负数，各个消息独有的错误码可从1开始
//------------------------------------------------------------------------
enum
{
	E_Success						= 0,
	E_SystemError					= -1,

	E_Item_NotFound					= -2,		// 没有找到物品
	E_Item_NotEquipment				= -3,		// 不是装备

	E_Item_CanNotAdd				= -4,		// 不能添加到指定的容器中
	E_Item_CanNotRemove				= -5,		// 不能从容器删除物品
	E_Item_CanNotMove_Other			= -6,		// 不能移动到其他容器中
	E_Item_AddFailed				= -7,		// 向容器中插入时失败

	E_Item_Place_NotFree			= -8,		// 目标位置不为空

	E_Item_LevelLimit				= -9,		// 等级不满足要求
	E_Item_SexLimit					= -10,		// 性别不满足要求
	E_Item_ClassLimit				= -11,		// 职业不满足要求
	E_Item_TalentLimit				= -12,		// 天资不满足要求
	E_Item_ClanRepLimit				= -13,		// 氏族声望不满足要求
	E_Item_OtherClanRepLimit		= -14,		// 其他声望不满足要求
	E_Item_RoleStatusLimit			= -15,		// 玩家状态(战斗，非战斗)不满足要求
	E_Item_AttA_Limit				= -16,		// 一级属性不满足

	E_Item_NotEnough				= -20,		// 物品个数不够

	E_Item_MaxHold					= -25,		// 物品达到可拥有上限

	E_Item_Used_NotValid			= -30,		// 使用的物品非法

	E_Bag_NotEnoughSpace			= -100,		// 背包没有足够的空间
	E_Con_NotEnoughSpace			= -101,		// 没有足够的空间
	E_Con_PswNotPass				= -102,		// 未输入过行囊密码

	E_BagYuanBao_NotEnough			= -200,		// 背包中元宝不够
	E_BagSilver_NotEnough			= -201,		// 背包中金钱不够
	E_ExVolume_NotEnough			= -202,		// 角色赠点不够

	E_Role_Not_ExistIn_World		= -300,		// 游戏世界中不存在指定角色
	E_Role_Not_Online				= -301,		// 角色不在线
	E_Role_CantMove					= -302,		// 角色处于不可以移动状态

	E_NPC_NotFound					= -310,		// 没有找到指定NPC
	E_NPC_NotValid					= -311,		// 职能类型不对
	E_NPC_TooFar					= -312,		// 距离NPC太远

	E_Filter_Name_InFilterFile		= -351,		// 在过滤词表中
	E_Filter_Name_TooLong			= -352,		// 名字太长
	E_Filter_Name_Null				= -353,		// 名字为空
	E_Filter_Name_Illegal			= -354,		// 名字包括特殊字符
	E_Filter_Chat_NoContent			= -355,		// 聊天内容为空
	E_Filter_Text_TooLong			= -356,		// 内容太长

	E_Return_NotNeedFeed			= -10000,	// 不用向客户端反馈 -- 该值为服务器反馈客户端消息用
};