//-----------------------------------------------------------------------------
//!\file msg_yuanbao.h
//!
//!\date 2009-3-11
//!
//!\brief 元宝交易
//-----------------------------------------------------------------------------
#pragma once

#pragma pack(push)
#pragma pack(1)

#include "../WorldDefine/currency_define.h"
#include "../WorldDefine/mall_define.h"

CMD_START(NDBC_LoadAllYBAccount)				// 读取所有玩家的元宝交易账户
CMD_END

CMD_START(NDBS_LoadAllYBAccount)
	INT				nNum;
	tagYBAccount	Account[1];
CMD_END

CMD_START(NDBC_InsertAccount)					// 创建一个元宝交易账户
	DWORD			dwRoleID;
CMD_END;

CMD_START(NDBC_SynAccountYB)					// 同步元宝交易账户元宝数
	DWORD			dwRoleID;
	INT				nYuanBao;
CMD_END

CMD_START(NDBC_SynAccountSilver)				// 同步元宝交易账户金钱数
	DWORD			dwRoleID;
	tagMoney		Money;
CMD_END

CMD_START(NDBC_SynSubmitSell)					// 同步元宝交易账户出售订单的状态
	DWORD			dwRoleID;
	bool			bSellOrder;
CMD_END

CMD_START(NDBC_SynSubmitBuy)					// 同步元宝交易账户收购订单的状态
	DWORD			dwRoleID;
	bool			bBuyOrder;					
CMD_END

CMD_START(NDBC_LoadAllOrder)					// 读取所有玩家的元宝交易账单
CMD_END

CMD_START(NDBS_LoadAllOrder)
	INT				nNum;
	DWORD			dwMaxOrderID;
	tagYuanBaoOrder YBOrder[1];
CMD_END

CMD_START(NDBC_InsertOrder)						// 创建元宝交易订单
	tagYBOrder		YBOrder;					
CMD_END

CMD_START(NDBC_CompleteOrder)
	tagCompleteOrder	CompleteOrder;
CMD_END

CMD_START(NDBC_RefreshOrder)
	tagRefresh			RefreshOrder;
CMD_END

CMD_START(NDBC_GetRoleYBOrder)
	DWORD			dwRoleID;
CMD_END

CMD_START(NDBS_GetRoleYBOrder)
	INT				nNum;
	tagYuanBaoOrder YBOrder[1];
CMD_END

CMD_START(NDBC_DeleteYBOrder)					// 删除数据库中时间超过七天的所有订单
	DWORD			dwDeleteTime;
CMD_END

#pragma pack(pop)