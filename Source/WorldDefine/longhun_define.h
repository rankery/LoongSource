//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: longhun_define.h
// author: 
// actor:
// data: 2009-03-13
// last: 
// brief: 龙魂相关定义
//-----------------------------------------------------------------------------
#pragma once



#include "container_define.h"
//-----------------------------------------------------------------------------
// 常量
//-----------------------------------------------------------------------------
const INT X_LONGHUN_EFFECT_POS_NUM		= 2;		// 指定部件能够激活的部件的龙魂属性个数

//-----------------------------------------------------------------------------
// 指定部件的龙魂属性由哪些部件激活
//-----------------------------------------------------------------------------
struct tagLongHunActivePos
{
	EEquipPos		eOuter;
	EEquipPos		eInner[2];
};

const tagLongHunActivePos g_sLongHunActivePos[EEP_MaxEquip] = 
{
	{EEP_Wrist,		EEP_Wrist,		EEP_Feet},		// 0, 头部
	{EEP_Null,		EEP_Null,		EEP_Null},		// 1, 面部
	{EEP_Head,		EEP_Head,		EEP_Wrist},		// 2, 上身
	{EEP_Body,		EEP_Body,		EEP_Head},		// 3, 下身
	{EEP_Finger2,	EEP_Finger2,	EEP_Waist},		// 4, 背部
	{EEP_Feet,		EEP_Feet,		EEP_Legs},		// 5, 腕部
	{EEP_Legs,		EEP_Legs,		EEP_Body},		// 6, 足部

	{EEP_Neck,		EEP_Neck,		EEP_Back},		// 7, 戒指
	{EEP_Waist,		EEP_Waist,		EEP_Finger1},	// 8, 戒指
	{EEP_Finger1,	EEP_Finger1,	EEP_Neck},		// 9, 腰饰
	{EEP_Back,		EEP_Back,		EEP_Finger2},	// 10, 项链

	{EEP_LeftHand,	EEP_LeftHand,	EEP_RightHand},	// 11, 主手武器
	{EEP_RightHand,	EEP_RightHand,	EEP_LeftHand}	// 12, 副手武器
};

//-----------------------------------------------------------------------------
// 指定部件能够激活哪些部件的龙魂属性
//-----------------------------------------------------------------------------
struct tagLongHunCanActivePos
{
	EEquipPos		eEquipPos[X_LONGHUN_EFFECT_POS_NUM];

	tagLongHunCanActivePos()
	{
		this->eEquipPos[0] = EEP_Null;
		this->eEquipPos[1] = EEP_Null;
	}
	
	VOID Set(EEquipPos eEquipPos)
	{
		if(EEP_Null == this->eEquipPos[0])
		{
			this->eEquipPos[0] = eEquipPos;
			return;
		}

		this->eEquipPos[1] = eEquipPos;
	}
};

extern tagLongHunCanActivePos g_sLongHunCanActivePos[EEP_MaxEquip];


//-----------------------------------------------------------------------------
// 龙魂能力类型
//-----------------------------------------------------------------------------
enum ELongHunType
{
	ELHT_Start		= 0,

	ELHT_Outer		= 0,	// 表龙魂
	ELHT_Inner		= 1,	// 里龙魂

	ELHT_End		= 1,
};

const INT32 X_LONGHUN_TYPE_NUM = ELHT_End - ELHT_Start + 1;


enum ELongHunTypeEx
{
	ELHTE_Null			= -1,

	ELHTE_Start			= 0,

	ELHTE_PowerGather	= 0,	// 聚力
	ELHTE_PowerBurst	= 1,	// 破力
	ELHTE_CriticalHit	= 2,	// 必杀

	ELHTE_End			= 2,
};

const INT X_LONGHUN_TYPEEX_NUM = ELHTE_End - ELHTE_Start + 1;

//-----------------------------------------------------------------------------
// 武器生产龙魂能力时，不同部位对应可能生成的龙魂能力类型
//-----------------------------------------------------------------------------
const ELongHunTypeEx g_eEquipGenLongHunTypeEx[EEP_MaxEquip] = 
{
	ELHTE_PowerGather,		// 0, 头部
	ELHTE_Null,				// 1, 面部
	ELHTE_PowerGather,		// 2, 上身
	ELHTE_PowerGather,		// 3, 下身
	ELHTE_PowerBurst,		// 4, 背部
	ELHTE_PowerGather,		// 5, 腕部
	ELHTE_PowerGather,		// 6, 足部

	ELHTE_PowerBurst,		// 7, 戒指
	ELHTE_PowerBurst,		// 8, 戒指
	ELHTE_PowerBurst,		// 9, 腰饰
	ELHTE_PowerBurst,		// 10, 项链

	ELHTE_CriticalHit,		// 11, 主手武器
	ELHTE_CriticalHit		// 12, 副手武器
};

//-----------------------------------------------------------------------------
// 龙魂能力原型(静态属性)
//-----------------------------------------------------------------------------
struct tagLongHunProto
{
	DWORD			dwID;								// id
	ELongHunType	eType;								// 表里
	ELongHunTypeEx	eTypeEx;							// 破力，聚力，必杀
	DWORD			dwTriggerID;						// trigger id
	DWORD			dwBuffID1;							// buff id
	DWORD			dwBuffID2;							// buff id
	bool			bQltyCanGen[X_EQUIP_QUALITY_NUM];	// 指定品级是否可随出：白，黄，绿，蓝，橙
	BYTE			byWuXingActive[X_WUXING_TYPE_NUM];	// 激活条件：金，木，水，火，土
};

//------------------------------------------------------------------------------
// 龙魂能力原型（静态属性，客户端版）
//------------------------------------------------------------------------------
struct tagLongHunProtoClient 
	: public tagLongHunProto
{
	TCHAR			szName[X_SHORT_NAME];				// 名称
	TCHAR			szDesc[X_LONG_NAME];				// 描述
};