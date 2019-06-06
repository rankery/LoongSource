//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: msg_log.h
// author: 
// actor:
// data: 2009-03-23
// last:
// brief: log相关
//-----------------------------------------------------------------------------
#pragma once

#pragma pack(push, 1)

#include "log_cmdid_define.h"
//-----------------------------------------------------------------------------

CMD_START(NDBC_LogSilver)		// 金钱变化
	tagLogSilver	sLogSilver;
CMD_END

CMD_START(NDBC_LogYuanBao)		// 元宝变化
	tagLogYuanBao	sLogYuanBao;
CMD_END

CMD_START(NDBC_LogExVolume)		// 赠点变化
	tagLogExVolume	sLogExVolume;
CMD_END

CMD_START(NDBC_LogTimeStat)		// 定时统计
	tagLogTimeStat	sLogTimeStat;
CMD_END

CMD_START(NDBC_LogItem)			// 物品log
	tagLogItem		sLogItem;
CMD_END

CMD_START(NDBC_LogItemTimes)	// 物品log
	tagLogItemTims	sLogItemTimes;
CMD_END

CMD_START(NDBC_LogMallSell)		// 商城出售物品log
	tagLogMallSell	sLogMallSell;
CMD_END

CMD_START(NDBC_LogMallSellPack)		// 商城出售礼品包log
	tagLogMallSellPack	sLogMallSellPack;
CMD_END

//-----------------------------------------------------------------------------
// 帮派LOG记录
//-----------------------------------------------------------------------------
CMD_START(NDBC_LogFund)
tagLogFund			sLogFund;
CMD_END

CMD_START(NDBC_LogMaterial)
tagLogMaterial		sLogMaterial;
CMD_END

CMD_START(NDBC_LogReputation)
tagLogReputation	sLogReputation;
CMD_END

//-----------------------------------------------------------------------------
// 帮派LOG记录
//-----------------------------------------------------------------------------
CMD_START(NDBC_LogGMCmd)
	tagLogGMCmd		sLogGMCmd;
CMD_END

//-----------------------------------------------------------------------------
// 宠物LOG记录
//-----------------------------------------------------------------------------
CMD_START(NDBC_LogPet)
	tagLogPet	sLogPet;
CMD_END

//-----------------------------------------------------------------------------
#pragma pack(pop)