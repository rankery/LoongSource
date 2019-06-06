//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: msg_bill_s.h
// author: 
// actor:
// data: 2009-06-09
// last:
// brief: 直充服务器与游戏世界的借口
//-----------------------------------------------------------------------------
#pragma once

#include "../WorldDefine/msg_common_errorcode.h"
#include "../ServerDefine/bill_define.h"

//-----------------------------------------------------------------------------
// 游戏世界登陆
//-----------------------------------------------------------------------------
CMD_START(NWB_Login)
	DWORD		dwWorldID;
CMD_END

CMD_START(NBW_Login)
	DWORD		dwErrorCode;
CMD_END

//-----------------------------------------------------------------------------
// 告知游戏世界取百宝袋
//-----------------------------------------------------------------------------
CMD_START(NBW_SafePickBaiBao)
	INT64		n64Serial;
	DWORD		dwAccountID;
	EBillType	eBillType;
	INT32		nCurYuanBao;
	DWORD		dwIncYuanbao;
CMD_END