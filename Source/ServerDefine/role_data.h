//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: role_data.h
// author: 
// actor:
// data: 2008-06-04
// last:
// brief: 人物数据的读取和保存相关消息
//-----------------------------------------------------------------------------
#pragma once

#pragma pack(push)
#pragma pack(1)

const INT32 MAX_ITEM_NEEDLOG_NUM		= 100000;


#include "../WorldDefine/msg_common_errorcode.h"
#include "../WorldDefine/base_define.h"
#include "../WorldDefine/RoleDefine.h"
#include "../WorldDefine/container_define.h"
#include "../WorldDefine/ItemDefine.h"
#include "role_data_define.h"

enum
{
	E_DBInsert_RoleNameExist		= 1,		// 创建角色，名称已存在(与其他角色或门派名称相同)
	E_DBInsert_RoleFailed			= 2,		// 数据库中创建角色失败
	E_DBInsert_FriendFailed			= 3,		// 数据库中添加好友失败
	E_DBInsert_EnemyFailed			= 4,		// 数据库中添加仇敌失败
	E_DBInsert_FriendshipFailed		= 5,		// 数据库中添加友好度失败
	E_DBInsert_BlackFailed			= 6,		// 添加黑名单失败

	E_DBLoad_SimRoleFailed			= 51,		// 该帐号下无角色，或读取帐号角色基本信息失败	
	E_DBLoad_RoleDataFailed			= 52,		// 从数据库中读取某个角色失败	
	E_DBLoad_SkillFailed			= 53,		// 从数据库中读取技能列表失败	
	E_DBLoad_RoleBuffFailed			= 54,		// 从数据库中读取角色Buff失败	
	//E_DBLoad_TalentFailed			= 55,		// 从数据库中读取天资列表失败	
	E_DBLoad_TitleFailed			= 56,		// 从数据库中读取称号列表失败	
	E_DBLoad_ItemFailed				= 57,		// 从数据库中读取物品列表失败
	E_DBLoad_EquipFailed			= 58,		// 从数据库中读取装备列表失败	
	E_DBLoad_FriendFailed			= 59,		// 从数据库中读取好友列表失败	
	E_DBLoad_EnemyFailed			= 60,		// 从数据库中读取仇敌列表失败	
	E_DBLoad_QuestFailed			= 61,		// 从数据库中读取任务列表失败	
	E_DBLoad_QuestDoneFailed		= 62,		// 从数据库中读取已完成任务列表失败		
	E_DBLoad_SimQuestFailed			= 63,		// 从数据库中读取简易任务列表失败	
	E_DBLoad_SimQuestDoneFailed		= 64,		// 从数据库中读取已完成简易任务列表失败		
	E_DBLoad_FriendshipFailed		= 65,		// 从数据库中读取好友度列表失败	

//	E_DBLoad_BaiBao_NoNewItem		= 65,		// 没有新的需要发放到百宝袋中的物品(装备)
	E_DBLoad_BaiBao_Failed			= 66,
	E_DBLoad_ItemNeedLog_Failed		= 67,
	E_DBSave_RoleFailed				= 70,		// 数据库保存失败

	E_DBLoad_BaiBaoLog_Failed		= 71,		// 百宝袋历史记录读取失败

	E_BackupDelRoleDataFailed		= 80,		// 备份被删除角色相关数据失败
	E_BackupDelItemFailed			= 81,		// 备份被删除角色物品数据失败
	E_DelRoleDataFailed				= 82,		// 删除角色属性相关数据失败
	E_RecordDelRoleDataFailed		= 83,		// 角色备份删除成功，但写role_del表失败

//	E_DelItemFailed					= 90,		// 删除被删除角色物品数据失败
//	E_DBDelete_RoleBuffFailed		= 91,		// 删除角色Buff数据失败
	E_DBDelete_FriendFailed			= 92,		// 删除好友失败
	E_DBDelete_EnemyFailed			= 93,		// 删除仇敌失败
	E_DBDelete_RoleFailed			= 94,		// 删除角色失败
	E_DBDelete_FriendshipFailed		= 95,		// 删除友好度失败
	E_DBDelete_BlackFailed			= 96,		// 删除黑名单失败


	E_ChangeRoleNameFailed			= 100,		// 修改角色名称失败

	E_DBLoad_RepRankData_Failed		= 120,		// 读取声望排名错误

	E_DBUpdate_StrengthRankingsFailed	=	130,// 更新实力排行榜失败
	E_DBUpdate_MoneyRankingsFailed		=	131,// 更新资金排行榜失败
	E_DBUpdate_FameRankingsFailed		=	132,// 更新声望排行榜失败
	E_DBUpdate_LevelRankingsFailed		=	133,// 更新等级排行榜失败

};

CMD_START(Heartbeat)			// 心跳
	DWORD dwGoldenCode;		// 服务器金色代码
	DWORD dwHeartbeat;		// 服务器心跳次数
CMD_END;


CMD_START(N_Certification)
	DWORD	dwGoldenCode;
	TCHAR	szWorldName[X_SHORT_NAME];
CMD_END;

CMD_START(NDBC_WorldShutDown)
CMD_END;

CMD_START(NDBS_WorldShutDown)
CMD_END;


CMD_START(NDBC_LoadAllRoleInfo)
CMD_END;

CMD_START(NDBS_LoadAllRoleInfo)
	DWORD		dwErrorCode;
	INT32		nNum;
	tagRoleInfo	RoleInfo[1];
CMD_END;

CMD_START(NDBC_LoadSimRole)
	DWORD	dwAccountID;
	CHAR	szAccount[X_SHORT_NAME];
CMD_END;

CMD_START(NDBS_LoadSimRole)
	DWORD				dwErrorCode;
	DWORD				dwAccountID;
	INT32				nNum;
	tagAccountCommon	sAccountCommon;
	tagSimRole			SimRole[MAX_ROLENUM_ONEACCOUNT];
CMD_END;

CMD_START(NDBC_CreateRole)	
	DWORD				dwAccountID;
	tagCreateRoleInfo	RoleInfo;
CMD_END;

CMD_START(NDBS_CreateRole)
	DWORD				dwErrorCode;
	DWORD				dwAccountID;
	tagSimRole			SimRole;
	tagRoleInfo			RoleInfo;
CMD_END;

CMD_START(NDBC_DelRole)
	DWORD		dwAccountID;
	DWORD		dwRoleID;
	DWORD		dwTime;
	TCHAR		szRoleName[X_SHORT_NAME];
	CHAR		szIP[X_IP_LEN];
CMD_END;

CMD_START(NDBS_DelRole)
	DWORD		dwErrorCode;
	DWORD		dwRoleID;
	DWORD		dwAccountID;
CMD_END;

CMD_START(NDBC_LoadRole)
	DWORD		dwAccountID;
	DWORD		dwRoleID;
CMD_END;

CMD_START(NDBS_LoadRole) // 注意该消息结构大小需重新计算
	DWORD			dwErrorCode;
	DWORD			dwAccountID;
	DWORD			dwRoleID;
	tagRoleDataLoad	RoleData;
CMD_END;


CMD_START(NDBC_ResumeRole)
CMD_END;

CMD_START(NDBC_SaveRole) // 注意该消息结构大小需重新计算
	DWORD			dwRoleID;
	tagRoleDataSave	RoleData;
CMD_END;

//CMD_START(NDBC_RecordDel) // 删除玩家的buff，功夫，item/equip等. -- 该消息要求先于NDBC_SaveRole
//	DWORD			dwRoleID;
//	tagRecordDel	Record;
//CMD_END;

CMD_START(NDBC_ChangeRoleName)
	DWORD		dwAccountID;
	DWORD		dwRoleID;
	TCHAR		szNewRoleName[X_SHORT_NAME];
CMD_END;

CMD_START(NDBS_ChangeRoleName)
	DWORD		dwErrorCode;
	DWORD		dwAccountID;
	DWORD		dwRoleID;
	TCHAR		szNewRoleName[X_SHORT_NAME];
CMD_END;

CMD_START(NDBC_ChangeRoleGuild)
	DWORD		dwRoleID;
	DWORD		dwGuildID;
CMD_END;

CMD_START(NDBC_InsertFriend)
	DWORD			dwRoleID;
	tagFriendSave	FriendSave;
CMD_END;

//CMD_START(NDBS_InsertFriend)
//	DWORD		dwErrorCode;
//	DWORD		dwRoleID;
//	DWORD		dwFriendID;
//CMD_END;

CMD_START(NDBC_DeleteFriend)
	DWORD		dwRoleID;
	DWORD		dwFriendID;
CMD_END;

CMD_START(NDBC_UpdateFriend)
	DWORD			dwRoleID;
	tagFriendSave	FriendSave;
CMD_END;

CMD_START(NDBC_DeleteFriVal) // 注意：向LoongDB发送友好度相关消息时，应dwRoleID < dwFriendID
	DWORD		dwRoleID;
	DWORD		dwFriendID;
CMD_END;

CMD_START(NDBC_DelAllFriRel)	// 删除角色时，删除数据库里所有相关好友信息
	DWORD		dwRoleID;
CMD_END;

CMD_START(NDBC_DelAllFriValRel) // 删除角色时，删除数据库里所有相关好友度信息
	DWORD		dwRoleID;
CMD_END;

CMD_START(NDBC_InsertFriVal)
	DWORD				dwRoleID;
	tagFriendshipSave	FriendshipSave;
CMD_END;

CMD_START(NDBC_UpdateFriVal)
	DWORD				dwRoleID;
	tagFriendshipSave	FriendshipSave;
CMD_END;

//CMD_START(NDBS_DeleteFriend)
//	DWORD		dwErrorCode;
//	DWORD		dwRoleID;
//	DWORD		dwFriendID;
//CMD_END;

CMD_START(NDBC_InsertBlack)	// 黑名单
	DWORD		dwRoleID;
	DWORD		dwBlackID;
CMD_END;

CMD_START(NDBC_DeleteBlack)
	DWORD		dwRoleID;
	DWORD		dwBlackID;
CMD_END;

CMD_START(NDBC_InsertEnemy)
	DWORD		dwRoleID;
	DWORD		dwEnemyID;
CMD_END;

CMD_START(NDBS_InsertEnemy)
	DWORD		dwErrorCode;
	DWORD		dwRoleID;
	DWORD		dwEnemyID;
CMD_END;

CMD_START(NDBC_DeleteEnemy)
	DWORD		dwRoleID;
	DWORD		dwEnemyID;
CMD_END;

CMD_START(NDBS_DeleteEnemy)
	DWORD		dwErrorCode;
	DWORD		dwRoleID;
	DWORD		dwEnemyID;
CMD_END;

CMD_START(NDBC_LoadBaiBao)	// 百宝袋中有空位时,请求读取新物品消息
	DWORD		dwAccountID;
	INT32		nFreeSpaceSize;	// 百宝袋中空位数目
	INT64		n64Serial;		// 读取指定ID的物品(不指定设为GT_INVALID)
CMD_END;

CMD_START(NDBS_LoadBaiBao)
	DWORD		dwErrorCode;
	DWORD		dwAccountID;
	INT32		nRetNum;		// 读取到的物品(装备)个数
	BYTE		byData[SPACE_BAIBAO_BAG * sizeof(tagEquip)];
CMD_END;

CMD_START(NDBC_LoadBaiBaoYuanBao)
	DWORD		dwAccountID;
CMD_END

CMD_START(NDBS_LoadBaiBaoYuanBao)
	DWORD		dwAccountID;
	INT32		nBaiBaoYuanBao;
CMD_END

CMD_START(NDBC_LoadBaiBaoLog)	// 请求读取百宝袋历史记录
	DWORD		dwAccountID;
CMD_END

CMD_START(NDBS_LoadBaiBaoLog)
	DWORD		dwErrorCode;
	DWORD		dwAccountID;
	INT32		nLogNum;		// 读取到的记录条数
	BYTE		byData[1];		// Log记录数据
CMD_END

CMD_START(NDBC_AddBaiBaoLog)	// 请求追加一条记录
	DWORD				dwAccountID;
	tagBaiBaoRecord		LogInfo;
CMD_END

CMD_START(NDBC_LoadItemNeedLog)
CMD_END;

CMD_START(NDBS_LoadItemNeedLog)
	DWORD		dwErrorCode;
	INT32		nItemNum;
	DWORD		dwTypeID[MAX_ITEM_NEEDLOG_NUM];
CMD_END;

CMD_START(NDBC_LoongWorldInitOK)		// loongworld向loongDB发送的初始化完成的确认消息
CMD_END

CMD_START(NDBS_LoongWorldInitOK)
CMD_END

CMD_START(NDBC_ClearRoleDressMdID)
	DWORD		dwRoleID;
	WORD		wNewVal;
CMD_END

// 读取物品&装备(压力测试用)
CMD_START(NDBC_TestLoadRoleItem)
	DWORD		dwAccountID;
	DWORD		dwRoleID;
CMD_END

CMD_START(NDBC_TestLoadRole)
	DWORD		dwAccountID;
	DWORD		dwRoleID;
CMD_END

//	开启宝箱计数相关
CMD_START(NDBC_UpdateTreasureSum)
DWORD		dwRoleID;
INT			nSum;
CMD_END

CMD_START(NDBS_UpdateTreasureSum)
	DWORD		dwRoleID;
	INT			nSum;
CMD_END

// 保存更名信息
CMD_START(NDBC_SaveRenameInfo)
	DWORD		dwAccountID;
	DWORD		dwRoleID;
	TCHAR		szName[X_SHORT_NAME];
CMD_END

// 保存玩家经验
CMD_START(NDBC_UpdateRoleAtt)
	DWORD		dwRoleID;
	INT			nType;
	INT			nValue;
CMD_END

// 保存玩家属性点
CMD_START(NDBC_UpdateRoleAttPoint)
	DWORD		dwRoleID;
	INT			nAttPointAdd[X_ERA_ATTA_NUM];
	INT			nAttPointLeft;
CMD_END

// 保存玩家天资点
CMD_START(NDBC_UpdateRoleTalentPoint)
	DWORD		dwRoleID;
	tagRoleTalent	Talent;
	INT				nIndex;
	INT				nTalentPoint;
CMD_END

// 保存玩家属性点
CMD_START(NDBC_UpdateRoleItemTransport)
	DWORD		dwRoleID;
	DWORD		dwMapID;
	float		fX;
	float		fZ;
	float		fY;
CMD_END

#pragma pack(pop)
