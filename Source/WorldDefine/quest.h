//-----------------------------------------------------------------------------
//!\file quest.h
//!
//!\date 2008-08-19
//!
//!\brief 客户端与服务器之间发送任务系统相关的消息
//-----------------------------------------------------------------------------

#pragma once

#include "msg_common_errorcode.h"

#pragma pack(push, 1)

//任务接取失败原因
enum
{
	E_QUEST_NOT_EXIST								= 1,		//任务不存在
	E_QUEST_NPC_NOT_EXIST							= 2,		//任务NPC不存在
	E_QUEST_NPC_TOO_FAR								= 3,		//任务NPC与玩家距离太远
	E_QUEST_FAILED_ADD 								= 4,        //增加任务失败
	E_QUEST_FATIGUEGUARD							= 5,		//防沉迷保护
	
	E_CanTakeQuest_FAILED_LOW_LEVEL					= 11,		//没有足够的等级
	E_CanTakeQuest_FAILED_LOW_REP					= 12,		//没有足够的声望
	E_CanTakeQuest_ALREADY_DONE						= 13,		//已经完成了
	E_CanTakeQuest_ALREADY_TAKE						= 14,		//已经接取了
	E_CanTakeQuest_FAILED_MISSING_ITEMS				= 15,		//没有任务所需要的物品
	E_CanTakeQuest_FAILED_MISSING_SKILLS			= 16,		//没有任务所需要的技能
	E_CanTakeQuest_FAILED_MISSING_Att				= 17,		//没有任务所需要的人物属性
	E_CanTakeQuest_FAILED_WRONG_SEX					= 18,       //任务所需要的性别
	E_CanTakeQuest_FAILED_WRONG_CLASS				= 19,       //任务所需要的职业
	E_CanTakeQuest_FAILED_NOT_ENOUGH_MONEY			= 20,       //没有任务所需要的金钱
	E_CanTakeQuest_FAILED_QUEST_NUM_FULL			= 21,		//任务满了，不能再接
	E_CanTakeQuest_FAILED_WRONG_MAP 				= 22, 		//地图不对
	E_CanTakeQuest_FAILED_PREV 						= 23,      	//任务前序不对
	E_CanTakeQuest_FAILED_NEXT 						= 24,      	//任务后续不对
	E_CanTakeQuest_FAILED_WRONG_TIME 				= 25,       //任务时间不对
	E_CanTakeQuest_FAILED_WRONG_TIMES 				= 26,       //任务接取次数不对
	E_CanTakeQuest_FAILED_MISS_ITEM_OF_QUEST 		= 27,       //没有任务物品
	E_CanTakeQuest_FAILED_WRONG_TRIGGER 			= 28,       //任务触发器不对
	E_CanTakeQuest_FAILED_SCRIPT					= 29,		//脚本判断失败，客户端不提供错误提示，服务器直接发送
	E_CanTakeQuest_FAILED_BAG_FULL					= 30,		//背包没有地方，放置任务物品

	E_CanCompleteQuest_FAILED_MISSING_Creature		= 31,		//没有任务完成所需要的生物
	E_CanCompleteQuest_FAILED_MISSING_NPC			= 32,		//没有任务完成所需要的NPC
	E_CanCompleteQuest_FAILED_MISSING_ITEMS			= 33,		//没有任务完成所需要的物品
	E_CanCompleteQuest_FAILEDENOUGH_MONEY			= 34,		//没有任务完成所需要的金钱
	E_CanCompleteQuest_BAG_FULL						= 35,		//背包没有地方，放置奖励物品
	E_CanCompleteQuest_FALIED_SCRIPT				= 36,		//脚本判断失败，客户端不提供错误提示，服务器直接发送

	E_CanTakeQuest_Tomorrow							= 37,		//每天只能完成一次
	E_CanTakeQuest_Week								= 38,		//只能在星期×完成
	E_CanTakeQuest_Class							= 39,		//职业不符合

	E_CanUpdateQuest_Fail							= 40,		//不能更新
};


//-----------------------------------------------------------------------------
//	是否可以接取此任务
//-----------------------------------------------------------------------------
CMD_START(NC_NPCAcceptQuest)
	DWORD		dwNPCID;		// 接取任务NPC唯一ID 
	UINT16		u16QuestID;		// 任务ID
CMD_END

//-----------------------------------------------------------------------------
//	触发器接取任务
//-----------------------------------------------------------------------------
CMD_START(NC_TriggerAcceptQuest)
	UINT16		u16QuestID;		// 任务ID
	DWORD		dwTriggerID;	// 触发器ID
CMD_END

CMD_START(NS_AcceptQuest)
	UINT16		u16QuestID;		// 任务ID
	DWORD		dwErrorCode;	// 错误码
CMD_END

CMD_START(NS_AddQuest)
	UINT16		u16QuestID;							// 任务ID
	INT16		n16ItemCount[QUEST_ITEMS_COUNT];	// 物品任务追踪数量
	tagQuestDynamicTarget	DynamicTarget;			// 动态目标任务才会发给客户端
CMD_END

//-----------------------------------------------------------------------------
//	是否可以完成此任务
//-----------------------------------------------------------------------------
CMD_START(NC_CompleteQuest)
	DWORD		dwNPCID;					// 完成任务NPC唯一ID 
	UINT16		u16QuestID;					// 任务ID
	INT32		nRewChoicesItemIndex;		// 可选奖励物品序号
CMD_END

CMD_START(NS_CompleteQuest)
	UINT16		u16QuestID;					// 任务ID
	DWORD		dwErrorCode;				// 错误码
CMD_END

//-----------------------------------------------------------------------------
//	是否可以删除任务
//-----------------------------------------------------------------------------
CMD_START(NC_DeleteQuest)
	UINT16		u16QuestID;					// 删除任务ID
CMD_END

CMD_START(NS_DeleteQuest)
	UINT16		u16QuestID;					// 任务ID
	DWORD		dwErrorCode;				// 错误码
CMD_END

//-----------------------------------------------------------------------------
//	更新任务NPC对话数据
//-----------------------------------------------------------------------------
CMD_START(NC_UpdateQuestNPCTalk)
	DWORD		dwNPCID;						// 更新任务NPC唯一ID 
	UINT16		u16QuestID;						// 更新任务ID
CMD_END

//-----------------------------------------------------------------------------
//	更新杀怪任务数据
//-----------------------------------------------------------------------------
CMD_START(NS_QuestUpdateKillCreature)
	UINT16		u16QuestID;						// 任务ID
	INT			nCreatureIndex;					// 怪物序号
	INT16		n16NewKillNum;					// 更新过后的杀怪计数
CMD_END

//-----------------------------------------------------------------------------
//	更新物品任务数据
//-----------------------------------------------------------------------------
CMD_START(NS_QuestUpdateItem)
	UINT16		u16QuestID;						// 任务ID
	INT			nItemIndex;						// 物品序号
	INT16		n16NewItemNum;					// 更新过后的物品计数
CMD_END

//-----------------------------------------------------------------------------
// 更新NPC对话任务数据
//-----------------------------------------------------------------------------
CMD_START(NS_QuestUpdateNPCTalk)
	UINT16		u16QuestID;						// 任务ID
	DWORD		dwErrorCode;					// 错误码
	INT			nIndex;							// 某个索引
CMD_END

//-----------------------------------------------------------------------------
// 更新Trigger任务数据
//-----------------------------------------------------------------------------
CMD_START(NS_QuestUpdateTrigger)
	UINT16		u16QuestID;						// 任务ID
	INT			nIndex;							// 某个索引
CMD_END

//-----------------------------------------------------------------------------
// 更新调查地物任务数据
//-----------------------------------------------------------------------------
CMD_START(NS_QuestUpdateInveset)
	UINT16		u16QuestID;
	INT			nIndex;
CMD_END

//-----------------------------------------------------------------------------
// 任务错误屏显信息
//-----------------------------------------------------------------------------
CMD_START(NS_QuestFailedMsg)
	UINT16		u16QuestID;						// 任务ID
	TCHAR		szMsg[1];						// 错误屏显信息
CMD_END

#pragma pack(pop)