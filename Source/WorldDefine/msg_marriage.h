//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: msg_marriage.h
// author: 
// actor:
// data: 2009-09-11
// last:
// brief: 玩家婚姻系统相关消息
//-----------------------------------------------------------------------------
#pragma once

#include "msg_common_errorcode.h"

#pragma pack(push, 1)

//----------------------------------------------------------------------------
// 广播类型
//----------------------------------------------------------------------------
enum EMBroadcastType
{
	//###（男方）与###（女方）正式结为夫妻，祝他们白头携老，永结同心！
	E_Broadcast_GetMarriage					= 0,	//结婚

	//###（男性名称）与###（女性名称）的婚礼现在开始！
	E_Broadcast_CelebrateWedding			= 1,	//举行婚礼


	E_Broadcast_Pledge						= 2,	//誓言
};

//----------------------------------------------------------------------------
// 错误码枚举 婚姻相关
//----------------------------------------------------------------------------
enum
{
	E_Marriage_Accept						= E_Success,	// 对方同意
	E_Marriage_Refuse						= 1,	// 对方拒绝		
	E_Marriage_SameSex						= 2,	// 性别相同
	E_Marriage_NotEnough_FriendShip			= 3,	// 友好度不够
	E_Marriage_Applicant_AlreadyMarried		= 4,	// 申请者已婚 结婚时的处理
	E_Marriage_BeApplicant_AlreadyMarried	= 5,	// 被申请者已婚
	E_Marriage_OutOfRange					= 6,	// 双方应在NPC月老附近[10格子之内]
	E_Marriage_NotEnough_Money				= 7,	// 申请者必须拥有66金。
	E_Marriage_NotInSame_Map				= 8,	// 不在同一区域
	E_Marriage_NotAnswer					= 9,	// 对方没有响应
	E_Marriage_LackItem						= 10,	// 缺少必要物品
	E_Marriage_NotMerried					= 11,	// 未婚 离婚时的处理
	E_Marriage_NotEnough_BagSpace			= 12,   // 背包空间不够


    E_Marriage_ErrEnd
};


//----------------------------------------------------------------------------
// 婚礼相关
//----------------------------------------------------------------------------
enum EWeddingStep
{
	ES_Wedding_None							= 0,	//空
	ES_Wedding_Pledge						= 1,	//誓言
	ES_Wedding_Worship						= 2,	//对拜
	ES_Wedding_Kiss							= 3,	//对吻
	ES_Wedding_Gift							= 4,	//发红包
	ES_Wedding_AroundTown					= 5,	//游城
};

enum
{
	E_Wedding_Success					= E_Success,	// 同意 成功
	E_Wedding_Fail						= 1,			// 错误 失败
	E_Wedding_BagNotEnoughSpace			= 2,			// 背包空间不足
	E_Wedding_GiftUseUp					= 3,			// 红包用完了
	E_Wedding_OutOfRange				= 4,			// 夫妻和主婚人应在NPC月老附近[10格子之内]
	E_Wedding_AlreadyWedded				= 5,			// 已经举行过婚礼了
	E_Wedding_HavingWedding				= 6,			// 有人在结婚
	E_Wedding_NotMerried				= 7,			// 申请婚礼者未婚
	E_Wedding_NotInSame_Map				= 8,			// 不在同一区域
};

//----------------------------------------------------------------------------
// 婚姻关系管同步管理相关,注: 配偶id作为人物的一个属性
//                          初始化时请参见
//                          tagNS_GetRoleInitState_Att
//                          及 tagNS_GetRemoteRoleState
//----------------------------------------------------------------------------
// 远程玩家婚姻关系发生变化同步给九宫格玩家
CMD_START(NS_RemoteChangeSpouse)
    DWORD   dwRoleID;       // 远程玩家id
    DWORD   dwLoverID;      // 爱人对应的roleid 没结婚则为GT_INVALID
    BOOL    bHaveWedding;   // 是否举行过婚礼
CMD_END

//----------------------------------------------------------------------------
// 婚礼相关
//----------------------------------------------------------------------------

//请求婚礼
CMD_START(NC_CelebrateWeddingReq)	
	DWORD dwOfficiatorNameCrc; //主婚人名字crc
	DWORD dwNpcID;
CMD_END;

//服务器转发给 主婚人
CMD_START(NS_CelebrateWeddingReq)	
	DWORD dwOfficiatorNameCrc; //主婚人名字crc
CMD_END;

//主婚人的反馈
CMD_START(NC_CelebrateWeddingReqRes)	
	DWORD dwErrorCode; //
CMD_END;

//将主婚人的返回 转发给申请人
CMD_START(NS_CelebrateWeddingReqRes)	
	DWORD	dwErrorCode; //对婚礼请求的反馈
CMD_END


//婚礼进行状态改变请求
CMD_START(NC_UpdateWeddingStepReq)		
EWeddingStep eWeddingStep;
CMD_END;

//服务器同步婚礼状态用
CMD_START(NS_UpdateWeddingStepReq)		
EWeddingStep eWeddingStep;
CMD_END;

//婚礼进行状态改变请求的反馈
CMD_START(NS_UpdateWeddingStepReqRes)		
DWORD	dwErrorCode;
CMD_END;

//请求发红包
CMD_START(NC_GiveGiftReq)				
DWORD	dwDstID; //消息目标id
CMD_END;

//对发红包人的反馈
CMD_START(NS_GiveGiftReqRes)
DWORD	dwSrcID; //消息发起者ID
DWORD	dwErrorCode;
CMD_END;


//----------------------------------------------------------------------------
// 结婚消息结构
//----------------------------------------------------------------------------
CMD_START(NC_GetMarriageReq)		// 玩家请求结婚
DWORD	dwDstID;	// 消息目标ID
DWORD	dwNpcID;
CMD_END;

CMD_START(NS_GetMarriageReq)		// 服务器转发给目标玩家
DWORD	dwSrcID;	// 消息发起者ID
CMD_END;

CMD_START(NC_GetMarriageReqRes)	// 目标玩家对请求结婚的反馈
DWORD	dwErrorCode;
DWORD	dwDstID;
CMD_END;

CMD_START(NS_GetMarriageReqRes)	// 服务器转发给目标玩家结婚请求反馈
DWORD	dwErrorCode;
DWORD	dwSrcID;
DWORD	dwDstID;
CMD_END;

//----------------------------------------------------------------------------
// 离婚消息结构
//----------------------------------------------------------------------------
CMD_START(NC_BreakMarriageReq)		// 玩家请求离婚
DWORD	dwDstID;	// 消息目标ID
CMD_END;

CMD_START(NS_BreakMarriageReq)		// 服务器转发给目标玩家
DWORD	dwSrcID;	// 消息发起者ID
CMD_END;

CMD_START(NC_BreakMarriageReqRes)	// 目标玩家对请求离婚的反馈
DWORD	dwErrorCode;
DWORD	dwDstID;
CMD_END;

CMD_START(NS_BreakMarriageReqRes)	// 服务器转发给目标玩家离婚请求反馈
DWORD	dwErrorCode;
DWORD	dwSrcID;
DWORD	dwDstID;
CMD_END;

//----------------------------------------------------------------------------
#pragma pack(pop)