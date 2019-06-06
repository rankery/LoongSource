//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: msg_mall.h
// author: 
// actor:
// data: 2009-02-1
// last:
// brief: 客户端和服务器端间消息 -- 商城商品
//-----------------------------------------------------------------------------
#pragma once
#pragma pack(push, 1)

#include "msg_common_errorcode.h"
#include "mall_define.h"

//----------------------------------------------------------------------------
// 错误码枚举
//----------------------------------------------------------------------------
enum
{
	//E_Mall_				= ,	// 
	E_Mall_Close						= 1,	// 商城关闭

	E_Mall_Item_NotEnough				= 10,	// 物品个数不足购买个数
	E_Mall_Pack_NotEnough				= 11,	// 礼品包个数不足购买个数

	E_Mall_ID_Error						= 12,	// ID不正确
	E_Mall_YuanBao_Error				= 13,	// 元宝单价不正确
	E_Mall_Free_NotExist				= 14,	// 不存在免费物品
	E_Mall_ExVolume_Error				= 15,	// 赠点单价不正确
	E_Mall_Exchange_NotAllowable		= 16,	// 商品不可兑换

	E_Mall_CreateItem_Failed			= 20,	// 商品创建失败
	E_Mall_CreatePres_Failed			= 21,	// 赠品创建失败

	E_Mall_Free_AlreadyGet				= 25,	// 免费物品已领取

	// 元宝交易
	E_Trade_BagYB_NotEnough				= 40,	// 背包中元宝不够
	E_Trade_BagSilver_NotEnough			= 41,	// 背包中金钱不够
	E_Trade_AccountYB_NotEnough			= 42,	// 账户中元宝不够
	E_Trade_AccountSilver_NotEnough		= 43,	// 账户中金钱不够
	E_Trade_SellOrder_Exit				= 44,	// 只能提交一份元宝出售订单
	E_Trade_BuyOrder_Exit				= 45,	// 只能提交一份元宝收购订单
	E_Trade_Tax_NotEnough				= 46,	// 交易手续费不足

	// 帮派团购
	E_GroupPurchase_NotMember			= 50,	// 不是本帮派成员
	E_GroupPurchase_ItemNotAllowable	= 51,	// 该商品不可进行团购
	E_GroupPurchase_NotInGuild			= 52,	// 玩家没有加入帮派
	E_GroupPurchase_AlreadyInitiate		= 53,	// 已经对该物品发起过团购
	E_GroupPurchase_IsInitiate			= 54,	// 不可响应自己发起的团购
	E_GroupPurchase_AlreadyRespond		= 55,	// 已经响应过该团购
	E_GroupPurchase_AlreadyEnd			= 56,	// 团购已经结束
	E_GroupPurchase_NoInfo				= 57,	// 团购信息不存在
	E_GroupPurchase_ServerBusy			= 58,	// 服务器忙
};

//-----------------------------------------------------------------------------
// 传输用结构
//-----------------------------------------------------------------------------
struct tagSimGPInfo
{
	DWORD			dwGuildID;
	DWORD			dwRoleID;
	DWORD			dwMallID;
	INT32			nPrice;				//团购价格
	INT16			nParticipatorNum;	//响应人数
	INT16			nRequireNum;		//团购所需人数
	DWORD			dwRemainTime;		//剩余时间
};

//----------------------------------------------------------------------------
// 消息
//----------------------------------------------------------------------------

//--同步数据
CMD_START(NS_OpenMall)
	DWORD		dwErrorCode;
	DWORD		dwMallTime;
CMD_END

CMD_START(NC_MallGet)					//向服务器询问是否可以打开商城
CMD_END	

CMD_START(NS_MallItem)
	INT			nItemNum;
	BYTE		byData[1];				// tagMallItemProto
CMD_END

CMD_START(NS_MallFreeItem)
	tagMallFreeItem		freeItem;
CMD_END

CMD_START(NS_MallPack)
	INT			nPackNum;
	BYTE		byData[1];
CMD_END

//--更新
CMD_START(NC_MallUpdate)
	DWORD		dwMallTime;
CMD_END

CMD_START(NS_MallUpdate)
	DWORD		 dwMallTime;
CMD_END

CMD_START(NS_MallUpdateItem)
	INT			nItemNum;
	BYTE		byData[1];
CMD_END

CMD_START(NS_MallUpdatePack)
	INT			nItemNum;
	BYTE		byData[1];
CMD_END


//--购买
CMD_START(NC_MallBuyItem)
	DWORD		dwTypeID;				// 商城商品ID
	INT			nPrice;					// 商品价格(玩家购买时的单价)
	INT16		n16BuyNum;				// 购买个数
	INT			nIndexInServer;			// 在服务器中的位置			
CMD_END

CMD_START(NS_MallBuyItem)
	DWORD		dwErrorCode;
CMD_END

CMD_START(NC_MallBuyPack)
	DWORD		dwTypeID;				// 商城商品ID
	INT			nPrice;					// 商品价格(玩家购买时的单价)
	INT16		n16BuyNum;				// 购买个数
	INT			nIndexInServer;		// 在服务器中的位置			
CMD_END

CMD_START(NS_MallBuyPack)
	DWORD		dwErrorCode;
CMD_END


//--赠送
CMD_START(NC_MallPresentItem)
	DWORD		dwTypeID;				// 商城商品ID
	INT			nPrice;					// 商品价格(玩家购买时的单价)
	DWORD		dwRoleTgtID;			// 要送的玩家ID
	INT16		n16BuyNum;				// 购买个数
	INT			nIndexInServer;		// 在服务器中的位置		
	TCHAR		szLeave[Max_LeaveWord_Length];
CMD_END

CMD_START(NS_MallPresentItem)
	DWORD		dwErrorCode;
CMD_END

CMD_START(NC_MallPresentPack)
	DWORD		dwTypeID;				// 商城商品ID
	INT			nPrice;					// 商品价格(玩家购买时的单价)
	DWORD		dwRoleTgtID;			// 要送的玩家ID
	INT16		n16BuyNum;				// 购买个数
	INT			nIndexInServer;		// 在服务器中的位置		
	TCHAR		szLeave[Max_LeaveWord_Length];
CMD_END

CMD_START(NS_MallPresentPack)
	DWORD		dwErrorCode;
CMD_END

//--免费领取
CMD_START(NC_MallFreeGetItem)
	DWORD		dwTypeID;
CMD_END

CMD_START(NS_MallFreeGetItem)
	DWORD		dwErrorCode;
CMD_END

//--帮派团购
CMD_START(NC_LaunchGroupPurchase)
	DWORD		dwTypeID;				// 商城商品ID
	INT			nPrice;					// 商品价格(玩家购买时的单价)
	BYTE		byScope;				// 团购规模
	INT			nIndexInServer;			// 在服务器中的位置
CMD_END

CMD_START(NS_LaunchGroupPurchase)
	DWORD		dwErrorCode;
CMD_END

CMD_START(NC_RespondGroupPurchase)
	DWORD		dwGuildID;				// 团购信息所属帮派
	DWORD		dwTypeID;				// 商城商品ID
	DWORD		dwRoleID;				// 发起者ID
	INT			nPrice;					// 商品价格(团购总价格)
CMD_END

CMD_START(NS_RespondGroupPurchase)
	DWORD		dwErrorCode;
	INT64		nGroupPurchaseKey;		// 团购信息键值(RoleID<<32 | MallID)
CMD_END

CMD_START(NC_GetSimGPInfo)
CMD_END

CMD_START(NS_GetSimGPInfo)
	DWORD				dwErrorCode;
	INT32				nGroupPurchase;		// 帮派团购指数
	INT					nGPInfoNum;			// 团购信息数量
	tagSimGPInfo		simData[1];			// 回馈的团购信息
CMD_END

CMD_START(NC_GetParticipators)
	DWORD		dwGuildID;				// 团购信息所属帮派
	DWORD		dwTypeID;				// 商城商品ID
	DWORD		dwRoleID;				// 发起者ID	
CMD_END

CMD_START(NS_Getparticipators)
	DWORD		dwErrorCode;
	INT64		nGroupPurchaseKey;		// 团购信息键值(RoleID<<32 | MallID)
	INT			nParticipatorNum;		// 响应者数量
	DWORD		dwParticipators[1];		// 响应者列表
CMD_END

enum ERespondBroadCast
{
	ERespondBroadCast_NULL		= 0,
	ERespondBroadCast_Launch,
	ERespondBroadCast_Success,
	ERespondBroadCast_Lose,
};

CMD_START(NS_RespondBroadCast)
	ERespondBroadCast	eType;
	DWORD				dwRoleID;
	DWORD				dwTypeID;			//物品TypeID
CMD_END

//--兑换
CMD_START(NC_MallItemExchange)
	DWORD		dwMallID;				// 商城商品ID
	INT			nPrice;					// 单个物品所需赠点数量
	INT16		n16BuyNum;				// 兑换个数
	INT			nIndexInServer;		// 在服务器中的位置		
CMD_END

CMD_START(NS_MallItemExchange)
	DWORD		dwErrorCode;
CMD_END

CMD_START(NC_MallPackExchange)
	DWORD		dwMallID;				// 商城商品ID
	INT			nPrice;					// 所需赠点数量
	INT			nIndexInServer;		// 在服务器中的位置	
CMD_END

CMD_START(NS_MallPackExchange)
	DWORD		dwErrorCode;
CMD_END

//----------------------------------------------------------------------------
// 元宝交易
//----------------------------------------------------------------------------
CMD_START(NS_SynAccoutYB)				// 同步交易账户的元宝数
	INT			nYuanBao;
CMD_END

CMD_START(NS_SynAccoutSilver)			// 同步交易账户的金钱数
	INT64		nSilver;
CMD_END

CMD_START(NS_SynAccout)					// 同步交易账户
	INT			nYuanBao;
	INT			nSilver;
CMD_END


CMD_START(NC_SaveYB2Account)			// 玩家向交易账户存元宝
	INT			nNum;					// 元宝数量
CMD_END

CMD_START(NS_SaveYB2Account)
	DWORD		dwErrorCode;
CMD_END

CMD_START(NC_SaveSilver2Account)		// 玩家向交易账户存金钱
	INT64		nNum;
CMD_END

CMD_START(NS_SaveSilver2Account)
	DWORD		dwErrorCode;
CMD_END

CMD_START(NC_DepositAccountYB)			// 从交易账户提取元宝
	INT			nNum;
CMD_END

CMD_START(NS_DepositAccountYB)
	DWORD		dwErrorCode;
CMD_END

CMD_START(NC_DepositAccountSilver)		// 从交易账户提取金钱
	INT64		nNum;
CMD_END				

CMD_START(NS_DepositAccountSilver)
	DWORD		dwErrorCode;
CMD_END

CMD_START(NC_SynYBTradeInfo)			// 获取所有元宝交易价格的列表和交易账户信息
CMD_END

CMD_START(NS_GetSellPriceList)
	INT				nListNum;
	tagYBPriceList	YBPriceList[1];
CMD_END

CMD_START(NS_GetBuyPriceList)
	INT				nListNum;
	tagYBPriceList	YBPriceList[1];
CMD_END

CMD_START(NS_GetYBAccount)				// 获得元宝交易账户
	DWORD			dwRoleID;			// 玩家ID
	INT				nYuanBao;			// 元宝数量
	INT64			nSilver;			// 金钱数量
	BOOL			bSellOrder;			// 是否有出售订单
	BOOL			bBuyOrder;			// 是否有收购订单
CMD_END

CMD_START(NS_SynSellPriceList)			// 同步出售价格列表
	INT				nPrice;
	INT				nNum;
CMD_END

CMD_START(NS_SynBuyPriceList)			// 同步购买价格列表
	INT				nPrice;
	INT				nNum;
CMD_END

CMD_START(NC_SubmitSellOrder)			// 提交元宝出售订单
	INT				nNum;				// 元宝数量
	INT				nPrice;				// 单价
CMD_END

CMD_START(NS_SubmitSellOrder)
	DWORD			dwErrorCode;
CMD_END

CMD_START(NC_SubmitBuyOrder)			// 提交元宝收购订单
	INT				nNum;
	INT				nPrice;
CMD_END

CMD_START(NS_SubmitBuyOrder)
	DWORD			dwErrorCode;
CMD_END

CMD_START(NC_GetYuanbaoOrder)           // 获得玩家订单
CMD_END

CMD_START(NS_GetYuanbaoOrder)
    INT             nNum;               // 订单数量
    tagYuanBaoOrder Orders[1];
CMD_END

CMD_START(NC_DeleteOrder)			    // 删除订单
	DWORD			dwOrderID;			// 订单ID
	EYBOTYPE		eYBOType;			// 订单类型
CMD_END

CMD_START(NS_DeleteOrder)
	DWORD			dwErrorCode;
CMD_END

#pragma pack(pop)