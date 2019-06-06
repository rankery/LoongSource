//-----------------------------------------------------------------------------
// filename: msg_famehall.h
// author: 
// actor:
// data: 2009-02-25
// last: 
// brief: 名人堂服务器消息格式
//-----------------------------------------------------------------------------
#pragma once
#include "../ServerDefine/famehall_define.h"
#include "../WorldDefine/reputation.h"

#pragma pack(push, 1)

//-----------------------------------------------------------------------------
// 更新声望重置时间戳
//-----------------------------------------------------------------------------
CMD_START(NDBC_UpDateRepRstTime)
	DWORD	dwRepRstTimeStamp;
	BYTE	byClanType;		
CMD_END;


//-----------------------------------------------------------------------------
// 获得声望排名
//-----------------------------------------------------------------------------
CMD_START(NDBC_GetRepRankList)
	BYTE	byClanType;	
CMD_END;

CMD_START(NDBS_GetRepRankList)
	BYTE			byClanType;
	DWORD			dwErrorCode;
	INT16			nRecNum;
	tagRepRankData	repRank[MAX_REP_RANK_NUM];
CMD_END;

//-----------------------------------------------------------------------------
// 名人堂初始化
//-----------------------------------------------------------------------------
CMD_START(NDBC_FameHallInitStart)
CMD_END;

//-----------------------------------------------------------------------------
// 声望重置时间戳
//-----------------------------------------------------------------------------
CMD_START(NDBS_GetRepResetTimestamp)
	DWORD	dwResetTime[ECLT_NUM];	// tagDWORDTime
CMD_END;

//-----------------------------------------------------------------------------
// 获得已激活珍宝
//-----------------------------------------------------------------------------
CMD_START(NDBS_GetActTreasureList)
	INT16			nRecNum[ECLT_NUM];
	tagTreasureData	treasureData[MAX_ACT_TREASURE_NUM];
CMD_END;

//-----------------------------------------------------------------------------
// 获得获得进入名人堂快照
//-----------------------------------------------------------------------------
CMD_START(NDBS_GetFameHallEnterSnap)
	INT16					nRecNum[ECLT_NUM];
	tagFameHallEnterSnap	enterSnap[MAX_ENTER_FAMEHALL_SNAP_NUM * ECLT_NUM];		// tagFameHallEnterSnap
CMD_END;

//-----------------------------------------------------------------------------
// 进入名人堂
//-----------------------------------------------------------------------------
CMD_START(NDBC_InsertFameHallSnap)
	BYTE	byClanType;				// 氏族类型
	DWORD	dwRoleID;				// 角色id
	DWORD	dwEnterNameID;			// 进入名人堂是名称id
	DWORD	dwEnterTime;			// tagDWORDTime
CMD_END;


//-----------------------------------------------------------------------------
// 激活珍宝
//-----------------------------------------------------------------------------
CMD_START(NDBC_InsertActClanTreasure)
	BYTE	byClanType;				// 氏族类型
	UINT16	u16TreasureID;			// 珍宝关联的物品id
	DWORD	dwRoleID;				// 激活珍宝的人物id
	DWORD	dwActNameID;			// 激活珍宝时人物名id
	DWORD	dwActTime;				// 激活时间
CMD_END;


#pragma pack(pop)