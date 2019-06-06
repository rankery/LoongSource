//-----------------------------------------------------------------------------
//!\file msg_stall.h
//!
//!\date 2008-12-26
//!
//!\brief 摆摊消息
//-----------------------------------------------------------------------------
#pragma once

#include "stall_define.h"

#pragma pack(push, 1)
//-----------------------------------------------------------------------------
// 摆摊者发送的消息
//-----------------------------------------------------------------------------
CMD_START(NC_StallStart)			// 打开摆摊界面
CMD_END

CMD_START(NS_StallStart)
	DWORD	dwErrorCode;
CMD_END

CMD_START(NC_StallSetGoods)			// 商品上架
	INT64	n64Serial;
	INT64	n64UnitPrice;
	BYTE	byIndex;				// 货架上的索引，从0开始
CMD_END

CMD_START(NS_StallSetGoods)
	DWORD	dwErrorCode;
CMD_END

CMD_START(NC_StallUnsetGoods)		// 商品下架
	BYTE	byIndex;
CMD_END

CMD_START(NS_StallUnsetGoods)
	DWORD	dwErrorCode;
CMD_END

CMD_START(NC_StallSetTitle)			// 设置标题
	TCHAR	szData[STALL_MAX_TITLE_NUM];// 标题
CMD_END

CMD_START(NS_StallSetTitle)
	DWORD	dwErrorCode;
CMD_END

CMD_START(NC_StallSetAdText)		// 设置广告
	TCHAR	szData[STALL_MAX_ADTEXT_NUM];// 广告
CMD_END

CMD_START(NS_StallSetAdText)
	DWORD	dwErrorCode;
CMD_END

CMD_START(NC_StallSetAdFlag)		// 设置广告广播标志
	bool	bFlag;					// 1表示广播；0表示不广播
CMD_END

CMD_START(NS_StallSetAdFlag)
	DWORD	dwErrorCode;
CMD_END

CMD_START(NC_StallSetFinish)		// 完成摆摊设置，告诉周围玩家
CMD_END

CMD_START(NS_StallSetFinish)
	DWORD	dwErrorCode;
	DWORD	dwRoleID;				// 摆摊者ID
	BYTE	byStallLevel;			// 摊位模型等级
CMD_END

CMD_START(NC_StallClose)			// 收摊
CMD_END

CMD_START(NS_StallClose)
	DWORD	dwErrorCode;
	DWORD	dwStallRoleID;			// 收摊者ID
CMD_END

CMD_START(NS_StallBuyRefresh)
	DWORD	dwStallRoleID;			// 摆摊者ID
	INT16	n16Num;					// 更新后摊位上商品个数
	BYTE	byIndex;				// 摊位上商品的位置索引
CMD_END

CMD_START(NS_StallSetRefresh)		// 商品上架广播
	DWORD	dwStallRoleID;			// 摆摊者ID
	BYTE	byIndex;				// 摊位上商品的位置索引
CMD_END

CMD_START(NS_StallUnsetRefresh)		// 商品下架广播
	DWORD	dwStallRoleID;			// 摆摊者ID
	BYTE	byIndex;
CMD_END

//-----------------------------------------------------------------------------
// 购买者发送的消息
//-----------------------------------------------------------------------------
CMD_START(NC_StallGet)
	DWORD	dwStallRoleID;			// 摆摊者ID
CMD_END

CMD_START(NS_StallGet)
	DWORD	dwErrorCode;
	DWORD	dwStallRoleID;			// 摆摊者ID
	BYTE	byNum;					// 商品个数
	BYTE	byData[1];				// tagMsgStallGoods
CMD_END

CMD_START(NC_StallGetTitle)
	DWORD	dwStallRoleID;			// 摆摊者ID
CMD_END

CMD_START(NS_StallGetTitle)
	DWORD	dwErrorCode;
	DWORD	dwStallRoleID;			// 摆摊者ID
	TCHAR	szTitle[STALL_MAX_TITLE_NUM];
CMD_END

CMD_START(NC_StallBuy)				// 购买商品
	INT64	n64Serial;				// 要购买的商品64位ID
	INT64	n64UnitPrice;			// 商品单价(检查商品是否有过更新用)
	DWORD	dwStallRoleID;			// 摆摊者ID
	INT16	n16Num;					// 商品个数
	BYTE	byIndex;				// 所在摊位的格子索引(从0开始)
CMD_END

CMD_START(NS_StallBuy)
	DWORD	dwErrorCode;
CMD_END

CMD_START(NC_StallGetSpec)			// 获取指定摊位上的商品
	DWORD	dwStallRoleID;			// 摆摊者ID
	BYTE	byIndex;				// 摊位上商品的位置索引
CMD_END

CMD_START(NS_StallGetSpec)
	DWORD	dwErrorCode;
	DWORD	dwStallRoleID;			// 摆摊者ID
	BYTE	byData[1];				// tagMsgStallGoods
CMD_END

//-----------------------------------------------------------------------------
// VIP摊位消息
//-----------------------------------------------------------------------------
CMD_START(NC_GetAllVIPStallInfo)
CMD_END

CMD_START(NS_GetAllVIPStallInfo)
	DWORD		dwErrorCode;
	DWORD		dwRequestTime;
	tagVIPStall	VIPStallInfo[VIP_STALL_MAX_NUM];
CMD_END

CMD_START(NC_UpdateVIPStallInfo)
	DWORD		dwRequestTime;		// VIP摊位信息更新时间
CMD_END

CMD_START(NS_UpdateVIPStallInfo)
	DWORD		dwErrorCode;
	DWORD		dwRequestTime;		// VIP摊位信息更新时间
	INT32		nInfoNum;			// 更新摊位信息条目数
	tagVIPStall	VIPStallInfo[1];	// 更新信息
CMD_END

CMD_START(NC_ApplyVIPStall)
	INT32		nRent;				// 摊位租金
	BYTE		byIndex;			// 摊位序号
CMD_END

CMD_START(NS_ApplyVIPStall)
	DWORD		dwErrorCode;
CMD_END

CMD_START(NC_SpecVIPStallGet)
	BYTE		byIndex;			// VIP摊位序号
CMD_END

CMD_START(NS_SpecVIPStallGet)
	DWORD		dwErrorCode;
CMD_END

CMD_START(NC_VIPStallBuy)
	INT64	n64Serial;				// 要购买的商品64位ID
	INT64	n64UnitPrice;			// 商品单价(检查商品是否有过更新用)
	DWORD	dwStallRoleID;			// 摆摊者ID
	INT16	n16Num;					// 商品个数
	BYTE	byIndex;				// 所在摊位的格子索引(从0开始)
CMD_END

CMD_START(NS_VIPStallBuy)
	DWORD		dwErrorCode;
CMD_END

//-----------------------------------------------------------------------------
#pragma pack(pop)