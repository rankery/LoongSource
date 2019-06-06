//-----------------------------------------------------------------------------
// filename: msg_strength.h
// author: 
// actor:
// data: 2009-09-09
// last: 
// brief: 实力系统相关消息格式
//-----------------------------------------------------------------------------
#pragma once
#include "../WorldDefine/msg_strength.h"
#include "strength_define.h"

//-----------------------------------------------------------------------------
// 发自：world 发往：db server 请求初始化实力排行榜
//-----------------------------------------------------------------------------
CMD_START(NDBC_InitStrengthRankings)
CMD_END;

//-----------------------------------------------------------------------------
// 发自：db server 发往：world 将实力排行榜发给world
//-----------------------------------------------------------------------------
CMD_START(NDBS_StrengthRankings)
	INT	nSum;														// 总记录条数，也就是排行榜人数，最大500
	tagStrengthRoleNode		Rankings[X_STRENGTH_RANKINGS_LEN];		// 按名次发送各角色的信息
CMD_END;

CMD_START(NDBC_UpdateRankings)
	ERankingType	eType;
CMD_END;

CMD_START(NDBS_UpdateRankings)
	DWORD dwErrorCode;
CMD_END;

