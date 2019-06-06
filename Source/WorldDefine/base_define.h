//-----------------------------------------------------------------------------
//!\file base_define.h
//!\author Lyp / Fred
//!
//!\date 2004-08-05
//! last 2004-09-17
//!
//!\brief 基本定义
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

// 所有名字的长度,包括人名、怪物名、商店名等
#define X_LONG_NAME				256
#define X_HUGE_NAME				512
#define X_SHORT_NAME			32
#define X_IP_LEN				24
#define X_PET_NAME              7
#define INT64					__int64

CONST INT	TICK_TIME				= 200;					// 每Tick 200 ms
CONST INT	TICK_PER_SECOND			= (1000/TICK_TIME);		// 每秒多少Tick

//--------------------------------------------------------------------------------
// 各种游戏对象的ID范围
//--------------------------------------------------------------------------------
#define		MIN_PLAYER_ID			1
#define		MAX_PLAYER_ID			2000000000
#define		IS_PLAYER(ID)			( (ID) >= MIN_PLAYER_ID && (ID) <= MAX_PLAYER_ID )

#define		MIN_CREATURE_ID			2000000001
#define		MAX_CREATURE_ID			3000000000

#define		IS_CREATURE(ID)			( (ID) >= MIN_CREATURE_ID && (ID) <= MAX_CREATURE_ID )

#define		MIN_NORMAL_CREATURE_ID	2000000001
#define		MAX_NORMAL_CREATURE_ID	2900000000//2800000000
#define		IS_NORMAL_CREATURE(ID)	( (ID) >= MIN_NORMAL_CREATURE_ID && (ID) <= MAX_NORMAL_CREATURE_ID )

#define		MIN_DOOR_ID				2900000001
#define		MAX_DOOR_ID				2901000000
#define		IS_DOOR(ID)				( (ID) >= MIN_DOOR_ID && (ID) <= MAX_DOOR_ID )

#define		MIN_PET_ID				3000000001//2800000001
#define		MAX_PET_ID				4000000000//2900000000
#define		IS_PET(ID)				( (ID) >= MIN_PET_ID && (ID) <= MAX_PET_ID )

#define		MIN_ITEM_ID				1
#define		MAX_ITEM_ID				7999999

#define		MIN_EQUIP_ID			8000000

//------------------------------------------------------------------------
// 金钱typeid
//------------------------------------------------------------------------
const DWORD		TYPE_ID_MONEY		= -1 - 10;// 也许不会被其它用到


//----------------------------------------------------------------
// 判断是否为装备
//----------------------------------------------------------------
#define		MIsEquipment(dwTypeID)		( (dwTypeID) >= MIN_EQUIP_ID )

//----------------------------------------------------------------
// 判断是否为任务物品
//----------------------------------------------------------------
#define		MIsQuestItem(pItemProto)			( EITE_Quest == (pItemProto)->eTypeEx )

////----------------------------------------------------------------
//// 判断装备是否已鉴定
////----------------------------------------------------------------
//#define MIsIdentified(pEquip)			(pEquip->equipSpec.byQuality != (BYTE)EIQ_Null)
//
////----------------------------------------------------------------
//// 判断装备是否为ring
////----------------------------------------------------------------
//#define MIsRing(pEquip)				(EEP_Finger1 == ((tagEquipProto*)pEquip->pProtoType)->eEquipPos)
//#define MIsWeapon(pEquip)			(pEquip->pProtoType->eTySpe == EIT_Weapon)