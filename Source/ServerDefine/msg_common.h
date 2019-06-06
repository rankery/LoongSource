//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: msg_common.h
// author: 
// actor:
// data: 2009-03-25
// last:
// brief: 账号下角色通用数据相关消息
//-----------------------------------------------------------------------------
#pragma once

#pragma pack(push, 1)
//-----------------------------------------------------------------------------

#include "../WorldDefine/currency_define.h"
//-----------------------------------------------------------------------------
// 消息结构
//-----------------------------------------------------------------------------
CMD_START(NDBC_ChangeBagPsd)
	DWORD		dwAccountID;
	DWORD		dwNewBagPsd;
CMD_END

CMD_START(NDBC_SafeCodeSet)
	DWORD		dwAccountID;
	DWORD		dwSafeCodeCrc;
CMD_END

//CMD_START(NDBS_SafeCodeSet)
//	DWORD		dwErrorCode;
//CMD_END

CMD_START(NDBC_SafeCodeReset)
	DWORD		dwAccountID;
	DWORD		dwTimeReset;
CMD_END

CMD_START(NDBC_SafeCodeResetCancel)
	DWORD		dwAccountID;
CMD_END


CMD_START(NDBC_WareSizeUpdate)
	DWORD		dwAccountID;
	INT16		n16WareSize;
CMD_END

CMD_START(NDBC_WareMoneyUpdate)
	DWORD		dwAccountID;
	INT64		n64WareSilver;
CMD_END

CMD_START(NDBC_BaiBaoYBUpdate)
	DWORD		dwAccountID;
	INT			nBaiBaoYuanBao;
CMD_END


//-----------------------------------------------------------------------------
#pragma pack(pop)
