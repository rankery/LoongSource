/*******************************

服务器客户端关于实力的消息

*******************************/

#pragma once

#include "msg_common_errorcode.h"
#include "role_info.h"

#pragma pack(push, 1)

enum ERankingType
{
	ERT_Strength,	// 实力值
	ERT_Level,		// 角色等级
	ERT_Money,		// 金钱
	ERT_Fame		// 声望
};
struct tagRankingInfo
{	
	INT				nCurRanking;		// 当前排名
	INT				nRankingChange;		// 排名变化
	INT64			n64Value;			// 值
	tagRoleIDName	sIDName;			// 角色ID和姓名
};

//-----------------------------------------------------------
// 获取服务器实力排名
//-----------------------------------------------------------
CMD_START(NC_GetRankingInfo)
	INT				nPage;				// 申请排名的页数，由服务器自己计算，nPage从1开始
	ERankingType	eType;				// 要申请排行的类型
CMD_END;

//-----------------------------------------------------------
// 服务器反馈回来的信息
//-----------------------------------------------------------
CMD_START(NS_GetRankingInfo)
	INT				nPage;				// 申请排名的页数
	INT				nLocalPlayerRank;	// 本地玩家排名
	ERankingType	eType;				// 排名类型
	tagRankingInfo	sData[20];			// 获取的排名信息
CMD_END;

//-----------------------------------------------------------
// 请求获取自己的实力值
//-----------------------------------------------------------
CMD_START(NC_GetSelfStrength)
CMD_END;

//-----------------------------------------------------------
// 获取自己的实力值
//-----------------------------------------------------------
CMD_START(NS_GetSelfStrength)
	DWORD		dwSelfStrength;			// 当前玩家实力值
	DWORD		dwHero;					// 英雄值
	DWORD		dwSmart;				// 韬略值
	DWORD		dwChivalrous;			// 侠义值
	DWORD		dwWarfare;				// 征战值
CMD_END

#pragma pack(pop)
