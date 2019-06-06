//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: msg_mall.h
// author: 
// actor:
// data: 2009-02-9
// last:
// brief: 客户端和服务器端间消息 -- 挖宝开宝
//-----------------------------------------------------------------------------
#pragma once
#pragma pack(push, 1)

#include "msg_common_errorcode.h"
#define MAX_CHEST_NUM 16

//普通藏宝图任务提示
enum ENormalTreasureMap 
{
	ENTM_NULL		= 1,
	ENTM_Nothing,					//一无所获
	ENTM_GainMoney,					//金钱奖励
	ENTM_ConfrontTrap,				//遭遇陷阱
	ENTM_Billiken,					//福神下凡
	ENTM_GainOlMap,					//获得古老藏宝图
	ENTM_GainKey,					//获得钥匙
	ENTM_GainOldChest,				//获得旧箱子
	ENTM_DriftBottle,				//漂流瓶
	ENTM_GainLoongSquamaChest,		//获得龙鳞宝箱
	ENTM_BadOldSeal,				//挖坏上古封印
	ENTM_Plutus,					//财神下凡
};

//古老藏宝图任务提示
enum EOldTreasureMap
{
	EOTM_NULL			=1,
	EOTM_Billiken,					//福神下凡
	EOTM_GainLoongSquamaChest,		//获得龙鳞宝箱
	EOTM_GainChest,					//获得箱子
	EOTM_GainLoongChest,			//获得玄龙宝箱
	EOTM_GainFourTreasures,			//获得文房四宝
	EOTM_GainKey,					//获得钥匙
	EOTM_GainRareTreaMap,			//获得绝世藏宝图
	EOTM_Plutus,					//挖到财神爷
	EOTM_DungeonEnter,				//发现密境入口
	EOTM_BadLoongPulse,				//掘断龙脉
	EOTM_BadOldSeal,				//挖坏上古封印
	EOTM_AwakenGods,				//唤醒上古之灵
};

//绝世藏宝图任务提示
enum ERareTreasureMap
{
	ERTM_NULL			= 1,
	ERTM_GainMoney,					//金钱奖励
	ERTM_GainRareStuff,				//获得稀有材料若干
	ERTM_GainDelicacyChest,			//获得精致的箱子
	ERTM_GainLoongChest,			//获得玄龙宝箱
	ERTM_GainLoongSquamaChest,		//获得龙鳞宝箱
	ERTM_GodsOfPractice,			//神之历练
	ERTM_MonsterAttackCity,			//开放怪物攻城活动
	ERTM_DungeonEnter,				//发现密境入口
	ERTM_AwakenGods,				//唤醒上古之灵
};

CMD_START(NC_DiaBao)
	DWORD				dwPlayerID;				//挖宝者ID
	DWORD				dwItemID;				//宝箱ID
	DWORD				dwItemTypdID;			//宝箱的TypeID
CMD_END

CMD_START(NS_DiaBao)
	DWORD				dwErrorCode;			//错误码
	Vector3				v3Pos;					//宝箱的位置(宝箱消失后,播放特效)
CMD_END

//普通藏宝图任务消息
CMD_START(NS_NormalTreasureMap)
	ENormalTreasureMap	eType;					//普通藏宝图任务提示
CMD_END

//古老藏宝图任务消息
CMD_START(NS_OldTreasureMap)
	EOldTreasureMap		eType;					//古老藏宝图任务提示
CMD_END

//绝世藏宝图任务消息
CMD_START(NS_RareTreasureMap)
	ERareTreasureMap	eType;					//绝世藏宝图任务提示
CMD_END

//进入奇遇FB
CMD_START(NC_EnterAdventureFB)
CMD_END

CMD_START(NS_EnterAdventureFB)
CMD_END

//历练
CMD_START(NC_Experience)
CMD_END

CMD_START(NS_Experience)
CMD_END

//--------------------------------------------------------------------------------
//以下为开宝箱
//--------------------------------------------------------------------------------
enum EChestType
{
	EChestType_NULL			= 0,
	EChestType_LoongSquamaChest,		//龙鳞宝箱
	EChestType_WackeLoongChest,			//玄龙宝箱
	EChestType_GodLargessChest,			//天神赐福宝箱
	EChestType_DestinyBoxChest,			//命运宝箱
};

CMD_START(NC_TreasureChest)
	INT64			n64ChestID;			//宝箱(龙鳞宝箱或玄龙宝箱)
	INT64			n64KeyID;			//钥匙
CMD_END

CMD_START(NS_TreasureChest)
	DWORD			dwChestTypeID;				//宝箱的TypdID
	DWORD			dwTypeID[MAX_CHEST_NUM];	//物品ID
	INT				nNum[MAX_CHEST_NUM];		//开出的数量
	DWORD			dwErrorCode;
CMD_END


CMD_START(NC_StopTreasureChest)
	INT64			n64ChestID;			//宝箱
	INT64			n64KeyID;			//钥匙
CMD_END

CMD_START(NS_StopTreasureChest);
	DWORD			dwTypeID;			//开出的物品
	INT				nNum;				//开出的数量
	DWORD			dwErrorCode;
CMD_END

CMD_START(NC_AgainTreasureChest)
	INT64			n64ChestID;			//宝箱
	INT64			n64KeyID;			//钥匙
CMD_END

CMD_START(NS_AgainTreasureChest)
	BYTE			byDestroy;			//毁坏为1,没毁坏为0
	DWORD			dwErrorCode;
CMD_END

CMD_START(NC_ChestGetItem)
	DWORD			dwTypeID;
CMD_END

CMD_START(NS_ChestGetItem)
	DWORD			dwErrorCode;
CMD_END

#pragma pack(pop)