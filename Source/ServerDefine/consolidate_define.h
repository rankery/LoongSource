//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: consolidate_define.h
// author: 
// actor:
// data: 2008-06-04
// last:
// brief: 服务器用装备强化相关结构
//-----------------------------------------------------------------------------
#include "../ServerDefine/base_define.h"
#include "../WorldDefine/compose_define.h"

#pragma once

#pragma pack(push, 1)

// 强化类型
enum EConsolidateTypeSer
{
	ECTS_Posy		= 0,	// 铭纹
	ECTS_Engrave	= 1,	// 镌刻
	ECTS_Inlay		= 2,	// 镶嵌
	ECTS_Brand		= 3,	// 烙印
	ECTS_LoongSoul	= 4,	// 龙附
	ECTS_Quench		= 5,	// 淬火
	ECTS_Produce	= 6,	// 生产
	ECTS_Decompose  = 8,	// 点化分解
	ECTS_RaisePotVal = 9,	// 提升潜力值 Jason 2009-12-6
};

// 需要计算装备光晕的强化类型
//enum EConsolidateFlare
//{
//	ECF_Posy = 0,			// 铭纹
//	ECF_Engrave = 1,		// 镌刻
//	ECF_Inlay = 2,			// 镶嵌
//	ECF_Brand = 3,			// 烙印
//	ECF_LoongSoul = 4,		// 龙附
//};

// 铭纹属性转换
#define ConsolidatePosyAttConvert(eRoleState, ePosyState)\
		{												 \
		eRoleState = ERA_Null;							 \
		switch(ePosyState)							     \
			{										     \
		case EPosyAtt_MaxHP:							 \
			eRoleState = ERA_MaxHP;					     \
		break;										     \
		case EPosyAtt_MaxMP:							 \
			eRoleState = ERA_MaxMP;					     \
		break;										     \
		case EPosyAtt_ExAttack:							 \
			eRoleState = ERA_ExAttack;					 \
		break;										     \
		case EPosyAtt_ExDefense:						 \
			eRoleState = ERA_ExDefense;					 \
		break;										     \
		case EPosyAtt_InAttack:							 \
			eRoleState = ERA_InAttack;					 \
		break;										     \
		case EPosyAtt_InDefense:						 \
			eRoleState = ERA_InDefense;					 \
		break;										     \
			}											 \
		}

//铭纹结构体
struct tagPosyProtoSer
{
	DWORD				dwID;					// 配方ID
	TCHAR				szName[X_SHORT_NAME];	// 配方名称
	EPosyAtt			ePosyAtt;				// 铭纹属性
	BYTE				byPosyTimes;			// 铭纹等级
	INT					nSuccessRate;			// 成功率
	INT					nPotValConsume;			// 潜力值消耗
	DWORD				dwMoneyConsume;			// 金钱消耗
	FLOAT 				fcoefficientA; 			// 属性价值公式参数A
	FLOAT 				fcoefficientB; 			// 属性价值公式参数B
	FLOAT 				fcoefficientC; 			// 属性价值公式参数C
	tagConsolidateStuff	ConsolidateStuff[MAX_CONSOLIDATE_STUFF_QUANTITY];   // 强化材料
	INT					nTotleStuff;										// 材料的总数量
	tagPosyProtoSer(){ZeroMemory(this,sizeof(*this));}
};


//镌刻结构体
struct tagEngraveProtoSer
{
	DWORD				dwID;					//配方ID
	TCHAR				szName[X_SHORT_NAME];	//配方名称
	EEngraveAtt			eEngraveAtt;			//镌刻属性
	BYTE				byEngraveTimes;			//镌刻次数
	INT					nSuccessRate;			//成功率
	INT					nPotValConsume;			//潜力值消耗
	DWORD				dwMoneyConsume;			//金钱消耗
	FLOAT 				fcoefficientA; 			//属性价值公式参数A
	FLOAT 				fcoefficientB; 			//属性价值公式参数B
	FLOAT 				fcoefficientC; 			//属性价值公式参数C
	tagConsolidateStuff	ConsolidateStuff[MAX_CONSOLIDATE_STUFF_QUANTITY]; //强化材料
	INT					nTotleStuff;										// 材料的总数量
	tagEngraveProtoSer(){ZeroMemory(this,sizeof(*this));}
};


//生产合成
struct tagProduceProtoSer
{
	DWORD				dwID;					//配方ID
	TCHAR				szName[X_SHORT_NAME];	//配方名称
	EProduceType		eProType;				//生产类型
	EComposeType		eComType;				//合成类型
	EFormulaFrom		eFormFrom;				//配方来源
	INT			  		nFormLvl;				//配方等级
	DWORD				dwProItemTypeID;		//合成的物品
	DWORD				dwProQuantity;			//合成的数量
	INT					nSuccessRate;			//成功率
	DWORD				dwVitalityConsume;		//活力消耗
	DWORD				dwMasterIncre;			//熟练度增加
	DWORD				dwMoneyConsume;			//金钱消耗
	tagProduceSutff		ProduceStuff[MAX_PRODUCE_STUFF_QUANTITY];//生产材料
};

struct tagDeComposeProtoSer
{
	DWORD				dwID;					//配方ID
	EComposeType		eComType;				//合成类型
	BYTE				byLevel;				//装备等级
	BYTE				byQuality;				//装备品级
	EItemType			eType;					//装备类型
	EItemTypeEx			eTypeEx;				//武器类型
	EEquipPos			ePos;					//装备位置
	INT					nFormLvl;				//配方等级
	EFormulaFrom		eFormFrom;				//配方来源
	DWORD				dwVitalityConsume;		//活力消耗
	DWORD				dwMasterIncre;			//熟练度增加
	DWORD				dwMoneyConsume;			//金钱消耗
	tagOutputStuff		OutputStuff[MAX_OUTPUT_QUANTITY];			//输出材料	
};


//淬火结构体
struct tagQuenchProtoSer
{
	DWORD				dwID;					//配方ID
	TCHAR				szName[X_SHORT_NAME];	//配方名称
	tagQuenchAtt		srcQuenchAtt;			//原属性及属性值
	tagQuenchAtt		dstQuenchAtt;			//目标属性及属性值
	DWORD				dwPotValConsume;		//潜力值消耗
	DWORD				dwMoneyConsume;			//金钱消耗
	INT					nSuccessRate;			//成功率
	tagConsolidateStuff	ConsolidateStuff[MAX_CONSOLIDATE_STUFF_QUANTITY]; //强化材料
};


#pragma pack(pop)

