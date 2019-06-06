//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: msg_currency.h
// author: 
// actor:
// data: 2008-11-06
// last:
// brief: 货币消息
//-----------------------------------------------------------------------------
#pragma once

#include "msg_common_errorcode.h"

#pragma pack(push, 1)
//----------------------------------------------------------------------------
// 错误码枚举
//----------------------------------------------------------------------------
enum
{

};

//----------------------------------------------------------------------------
// 消息结构
//----------------------------------------------------------------------------
CMD_START(NS_BagSilver)
	INT64		n64CurSilver;		// 当前总的金钱
	INT64		n64ChangeSilver;	// 变化的金钱(正: 获得; 负: 失去)
CMD_END;

CMD_START(NS_WareSilver)
	INT64		n64CurSilver;		// 当前总的金钱
	INT64		n64ChangeSilver;	// 变化的金钱(正: 获得; 负: 失去)
CMD_END;

CMD_START(NS_BagYuanBao)
	INT32		nCurYuanBao;		// 当前总的元宝
	INT32		nChangeYuanBao;		// 变化的元宝
CMD_END

CMD_START(NS_BaiBaoYuanBao)
	INT32		nCurYuanBao;		// 当前总的元宝
	INT32		nChangeYuanBao;		// 变化的元宝
CMD_END

CMD_START(NS_ExchangeVolume)
	INT32		nCurExVolume;		// 当前总的赠点
	INT32		nChangeExVolume;	// 变化的赠点
CMD_END

CMD_START(NS_ClanCon)
	BYTE		byClanType;			// 氏族类型
	INT32		nCurClanCon;		// 当前总的贡献
	INT32		nChangeClanCon;		// 变化的贡献
CMD_END

//----------------------------------------------------------------------------
#pragma pack(pop)