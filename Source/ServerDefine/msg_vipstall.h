//-----------------------------------------------------------------------------
// filename: msg_vipstall.h
// author: 
// actor:
// data: 2009-06-15
// last: 
// brief: VIP摊位服务器消息格式
//-----------------------------------------------------------------------------
#pragma once

#include "../WorldDefine/stall_define.h"

#pragma pack(push, 1)

struct tagVipStall2DB
{
	DWORD	dwOwnerID;
	INT32	nRemainTime;
	BYTE	byStallIndex;

	tagVipStall2DB()
	{
		dwOwnerID		= GT_INVALID;
		nRemainTime		= GT_INVALID;
		byStallIndex	= GT_INVALID;
	}
};

CMD_START(NDBC_GetAllVIPStallInfo)
CMD_END

CMD_START(NDBS_GetAllVIPStallInfo)
	DWORD			dwErrorCode;
	tagVipStall2DB	VipStallInfo[VIP_STALL_MAX_NUM];
CMD_END

CMD_START(NDBC_ApplyVIPStall)
	tagVipStall2DB	VipStallInfo;
CMD_END

CMD_START(NDBC_UpdateVIPStall)
CMD_END

#pragma pack(pop)