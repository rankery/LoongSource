#pragma once
#include "fatigue_define.h"
#pragma pack(push,1)

//-------------------------------------------------------------------------
// login与中心激活服务器的防沉迷消息
//-------------------------------------------------------------------------
CMD_START(NC_LOGIN_CM)
	DWORD	dwSectionID;		// 大区编号
CMD_END

CMD_START(NC_ANTIQUERYSTATE)
	CHAR	szAccountName[16];	// 帐号名
CMD_END

CMD_START(NC_ANTIENTHRALL)
	DWORD	dwAccountNum;		// 帐号数目
	CHAR	szAccountName[1];	// 帐号名
CMD_END

CMD_START(NS_ANTIENTHRALL)
	CHAR	szAccountName[16];	// 帐号名
	BYTE	byColor;			// 状态
	DWORD	dwAccOLSeconds;		// 累计在线时间秒
CMD_END

//-------------------------------------------------------------------------
// login 与world的防沉迷消息
//-------------------------------------------------------------------------
CMD_START(NLW_FatigueNotify)
	INT				nNum;
	tagFatigueNotify	notify[1];
CMD_END

#pragma pack(pop)