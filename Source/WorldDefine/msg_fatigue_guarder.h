//-----------------------------------------------------------------------------
// filename: msg_fatigue_guarder.h
// author: client/server hyu/xlguo
// actor:
// data: 2009-02-05
// last: 2009-02-05
// brief: 防沉迷
//-----------------------------------------------------------------------------
#pragma once
#pragma pack(push, 1)

enum 
{
	E_FatigueLimit_None		= 0,
	E_FatigueLimit_Quest	= 1,
	E_FatigueLimit_Stall	= 2,
};

CMD_START(NS_FatigueLimit)
	BYTE	byLimitCode;		// 防沉迷类型
CMD_END

//----------------------------------------------------------------------------
// 获得目前防沉迷信息
//----------------------------------------------------------------------------
CMD_START(NC_GetFatigueInfo)
CMD_END

CMD_START(NS_GetFatigueInfo)
	BYTE			byState;		// 当前状态
	DWORD			dwCurOLSec;		// 当前累计上线时间
CMD_END

#pragma pack(pop)