#pragma once
#pragma pack(push, 1)

#include "msg_common_errorcode.h"

CONST	UINT16		DOUBLE_EXP_QUEST_ID		=	10848;		// 双倍经验任务ID
CONST	UINT16		FISHING_QUEST_ID		=	79;			// 钓鱼任务ID

enum ELoongBenediction
{
	ELBD_Success			= 0,			// 成功
	ELBD_Bag_NotEnoughSpace	= 1,			// 背包已经满了
	ELBD_Time_NotArrive		= 2,			// 时间还未到
	ELBD_No_LessingProto	= 3,			// 没有找到赐福原型配置信息
	ELBD_Item_Create_Failed	= 4,			// 物品创建失败
	ELBD_No_Benediction		= 5,			// 没有了
};


//-----------------------------------------------------------------------------
// 防止玩家流失，包括下线提示，神龙赐福
//-----------------------------------------------------------------------------

CMD_START(NC_ExitGamePrompt)
CMD_END

CMD_START(NS_ExitGamePrompt)
	BOOL		bDualExpQuestDo;				//今日是否完成,双倍杀怪经验的任务 id = 10848
	BOOL		bFishQuestDo;					//今日是否完成，钓鱼任务 id = 79
	INT32		nExprence;						//钓鱼一小时获得的经验
CMD_END


//-----------------------------------------------------------------------------
// 神龙赐福
//-----------------------------------------------------------------------------
CMD_START(NC_GetLoongBenediction)			//获取奖励
CMD_END

CMD_START(NS_GetLoongBenediction)
	DWORD		dwErrorCode;
	DWORD		nItemTypeID;				//奖励的物品的ID
	INT			nItemCount;					//物品奖励的个数
	INT32		nExprence;					//奖励的经验的ID
CMD_END

CMD_START(NS_SetNextBenediction)
	BOOL			isEnd;					//神龙赐福是否结束的标志
	DWORD			nextServerTime;			//下一次神龙赐福的时间，剩余秒数
	DWORD			nItemTypeID;			//获得的物品奖励
	INT				nItemCount;				//物品奖励的个数
	INT32			n32Exprence;			//经验奖励
CMD_END

CMD_START(NS_LoongBenedictionInit)			//告诉客户端需要初始化这个模块
CMD_END

#pragma pack(pop)