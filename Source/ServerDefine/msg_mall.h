//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: msg_mall.h
// author: 
// actor:
// data: 2008-05-25
// last:
// brief: 商城相关
//-----------------------------------------------------------------------------
#pragma once

#pragma pack(push)
#pragma pack(1)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// 枚举
//-----------------------------------------------------------------------------
enum
{
	E_DBLoad_GroupPurchase_Faild		= 1,		// 读取帮派团购信息失败
};

//-----------------------------------------------------------------------------
// 传输用结构
//-----------------------------------------------------------------------------
struct tagGPInfoKey
{
	DWORD			dwGuildID;
	DWORD			dwRoleID;
	DWORD			dwMallID;
};

struct tagGPInfo : public tagGPInfoKey 
{
	INT32			nPrice;				//团购价格
	INT16			nParticipatorNum;	//响应人数
	INT16			nRequireNum;		//团购所需人数
	INT32			nRemainTime;		//剩余时间
	DWORD			dwParticipators[1];	//响应者列表

	INT32 Size()
	{
		return sizeof(tagGPInfo) + (nParticipatorNum-1) * sizeof(DWORD);
	}
};

//-----------------------------------------------------------------------------
// 消息结构
//-----------------------------------------------------------------------------
CMD_START(NDBC_GetAllGPInfo)
CMD_END

CMD_START(NDBS_GetAllGPInfo)
	DWORD			dwErrorCode;
	INT				nGPInfoNum;
	tagGPInfo		GPInfoData[1];
CMD_END

CMD_START(NDBC_AddGPInfo)
	tagGPInfo		GPInfo;
CMD_END

CMD_START(NDBC_RemoveGPInfo)
	tagGPInfoKey	GpInfoKey;
CMD_END

CMD_START(NDBC_RemoveGuildGPInfo)
	DWORD			dwGuildID;
CMD_END

CMD_START(NDBC_UpdateGPInfo)
	tagGPInfoKey	GpInfoKey;
	DWORD			dwNewParticipator;	//新的响应者ID
CMD_END

CMD_START(NDBC_UpdateGPTime)
CMD_END

//-----------------------------------------------------------------------------
#pragma pack(pop)
