//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: activity.h
// author: wjiang
// actor:
// data: 2009-03-27
// last:
// brief: 资源管理器数据结构定义
//-----------------------------------------------------------------------------
#pragma once
#pragma pack(push, 1)

//-----------------------------------------------------------------------------
// 活动类型
//-----------------------------------------------------------------------------
enum EActMode
{
	EAM_Day,				// 每日活动
	EAM_Week,				// 每周活动
	EAM_Month,				// 每月活动
	EAM_Festival,			// 节日活动
};

//-----------------------------------------------------------------------------
// 广播类型
//-----------------------------------------------------------------------------
enum EActBroadMode
{
	EABM_AboutStart,		// 即将开始
	EABM_Start,				// 开始
	EABM_AboutEnd,			// 即将结束
	EABM_End,				// 结束
};

//-----------------------------------------------------------------------------
// 星期几
//-----------------------------------------------------------------------------
enum EWeekday
{
	EW_Sunday = 0,
	EW_Monday,
	EW_Tuesday,
	EW_Wednesday,
	EW_Thursday,
	EW_Friday,
	EW_Saturday,
};

//-----------------------------------------------------------------------------
// 固定活动任务的时间结构（每周的星期几的几点到几点）
//-----------------------------------------------------------------------------
struct tagActTime
{
	EWeekday		eWeekday;					// 星期几

	tagDWORDTime	startTime;					// 开始时间
	tagDWORDTime	endTime;					// 结束时间
	BYTE			byStartBroadH;				// 广播开始的时间（小时）
	BYTE			byStartBroadM;				// 广播开始的时间（分钟）
	BYTE			byStartInterval;			// 广播开始前的时间间隔（分钟）
	BYTE			byEndBroadH;				// 广播结束的时间（小时）
	BYTE			byEndBroadM;				// 广播结束的时间（分钟）
	BYTE			byEndInterval;				// 广播结束前的时间间隔（分钟）
};

struct tagActInfo
{
	DWORD			dwID;						// 活动ID
	TCHAR			szName[X_SHORT_NAME];		// 活动名称
	EActMode		eActMode;					// 活动类型
	tagActTime		ActTime;					// 固定活动任务的时间结构
};

struct tagActivityData
{
	DWORD							dwID;		// 活动ID
	tagScriptData<ESD_Activity>		Data;		// 活动脚本数据
};

CMD_START(NDBC_LoadActivityData)					// 存入活动脚本数据
CMD_END

CMD_START(NDBS_LoadActivityData)					// 读取活动脚本数据
	INT								nNum;
	tagActivityData					Activity[1];
CMD_END

CMD_START(NDBC_SaveActivityData)
	tagActivityData					Activity;
CMD_END

#pragma pack(pop)




