//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: msg_city.h
// author: 
// actor:
// data: 2009-11-09
// last:
// brief: 城市系统消息
//-----------------------------------------------------------------------------
#pragma once

#include "../WorldDefine/city_define.h"
#include "../WorldDefine/msg_common_errorcode.h"

#pragma pack(push, 1)

//----------------------------------------------------------------------------
// 获取城市属性
//----------------------------------------------------------------------------
CMD_START(NC_GetCityInfo)
	DWORD					dwNPCID;
CMD_END

CMD_START(NC_GetCityInfoByCityID)
	DWORD					dwCityID;
CMD_END

CMD_START(NS_GetCityInfo)
	DWORD					dwErrorCode;
	tagCityAtt2Client		sCityAtt;
CMD_END

CMD_START(NC_GetAppointedCityAtt)
	DWORD					dwNPCID;
	ECityAttType			eCityAttType;
CMD_END

CMD_START(NS_GetAppointedCityAtt)
	tagCityAppointedAtt		sCityAtt;
CMD_END

//----------------------------------------------------------------------------
// 获取城战报名信息
//----------------------------------------------------------------------------
CMD_START(NC_GetSigupInfo)
	DWORD					dwNPCID;
CMD_END

CMD_START(NS_GetSigupInfo)
	DWORD					dwErrorCode;
	DWORD					dwCityID;	   //城市id
	DWORD					dwAvgAckMoney; //攻防报名的平均费用
	DWORD					dwGuildID;	   //当前占领该城市的帮派id 如果是npc占领则为GT_INVALID
	tagWarTime				WarTime;	   //城战时间
	INT32					nAckNum;
	INT32					nDefNum;
	INT32					nConfirmNum;   //已确认防守的
	DWORD					dwData[1];
CMD_END

//----------------------------------------------------------------------------
// 守方确认
//----------------------------------------------------------------------------
CMD_START(NC_ConfirmDefenceForce)
	DWORD					dwNPCID;
	DWORD					dwGuildID[MAX_CITY_DEFENCE_FORCE];
CMD_END

CMD_START(NS_ConfirmDefenceForce)
	DWORD					dwErrorCode;
CMD_END

//----------------------------------------------------------------------------
// 设置税率
//----------------------------------------------------------------------------
CMD_START(NC_SetTaxRate)
	DWORD					dwNPCID;
	INT32					nTaxRate;
CMD_END

CMD_START(NS_SetTaxRate)	// 广播
	DWORD					dwErrorCode;
	DWORD					dwCityID;
	INT32					nTaxRate;
CMD_END

//----------------------------------------------------------------------------
// 提取税金
//----------------------------------------------------------------------------
CMD_START(NC_TransformTaxation)
	DWORD					dwNPCID;
	ETaxTransformType		eTransType;
	INT32					nTaxation;
CMD_END

CMD_START(NS_TransformTaxation)
	DWORD					dwErrorCode;
	INT32					nTaxation;		//剩余税金
CMD_END

//----------------------------------------------------------------------------
// 城战报名
//----------------------------------------------------------------------------

//城市有统治帮派的时候
CMD_START(NC_SignupForCitywar)
	ECityForceType			eForce;
	INT32					nFare; //进攻报名时的出价
CMD_END

CMD_START(NS_SignupForCitywar)
	DWORD					dwErrorCode;
CMD_END


//----------------------------------------------------------------------------
// 查看当前城战信息

CMD_START(NC_GetCityWarCity)
CMD_END

CMD_START(NS_GetCityWarCity)
	INT32 nNum;
	DWORD dwCityID[1];
CMD_END


CMD_START(NC_GetCityWarInfo)	
	DWORD dwCityID;
CMD_END

CMD_START(NS_GetCityWarInfo)
	DWORD dwErrorCode;
	DWORD dwAttacks[MAX_CITY_ATTACK_FORCE];			// 攻击方
	DWORD dwDefenses[MAX_CITY_DEFENCE_FORCE + 1];	// 防御方
CMD_END


//----------------------------------------------------------------------------
// 提升生产力
//----------------------------------------------------------------------------

CMD_START(NC_UpgradeProlificacy)
	DWORD dwNPCID;
	EProlificacyUpgrade eType;
CMD_END

CMD_START(NS_UpgradeProlificacy)	
	DWORD dwErrorCode;
CMD_END

#pragma pack(pop)

