//-------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: msg_vip_netbar.h
// author: xlguo
// actor:
// data: 2009-8-3
// last:
// brief: 金牌网吧服务器消息
//-------------------------------------------------------------------------------
#pragma once
#include "vip_netbar_define.h"
#pragma pack(push,1)

//-------------------------------------------------------------------------
// 获取账号历史数据
//-------------------------------------------------------------------------
CMD_START(NDBC_GetVNBData)
	DWORD dwDate;
CMD_END

CMD_START(NDBS_GetVNBData)
	tagDBVNBPlayers players;	
CMD_END

//-------------------------------------------------------------------------
// 更新金牌网吧玩家
//-------------------------------------------------------------------------
CMD_START(NDBC_UpdateVNBPlayer)
	DWORD	dwAccountID;
	DWORD	dwLoginTime;
CMD_END

#pragma pack(pop)