//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: TreasureChest_define.h
// author: twan
// actor:
// data: 2009-07-29
// last:
// brief: 宝箱属性定义
//-----------------------------------------------------------------------------

#pragma once

const INT ROLE_CHSET_RATE		=	100;			// 角色节点
const INT SERVER_CHEST_RATE		=	1000;			// 服务器节点

//--------------------------------------------------------------------------------------------
// 几种掉率枚举
//--------------------------------------------------------------------------------------------
enum ERateType
{
	ERTT_BeginRate			= 0,	// 起始掉率
	ERTT_RoleRate			= 1,	// 角色节点掉率
	ERTT_ServerRate			= 2,	// 服务器节点掉率
	ERTT_NormalRate			= 3,	// 普通掉率
};

//	宝箱内物品定义
struct tagChestItem 
{
	DWORD				dwTypeID;			//物品id
	INT					nTableID;			//在宝箱物品中的id，用此id整除100来确定此物品属于哪种宝箱
	INT					nNum;				//物品开出的数量
	FLOAT				fBeginRate;			//起始掉率
	FLOAT				fRoleRate;			//角色节点掉率
	FLOAT				fServerRate;		//服务器节点掉率
	FLOAT				fNormalRate;		//普通掉率
};

// 开宝箱状态
struct tagChestState
{	
	tagChestItem		ChestItem;		// 玩家开宝箱过程中开出的物品
	INT64				nChestSerial;		// 玩家开宝箱过程中宝箱的serialID
	INT64				nKeySerial;		// 玩家开宝箱过程中钥匙的serialID
	DWORD				dwChestTypeID;	// 宝箱的typeid
	DWORD				dwKeyTypeID;		// 钥匙的typeid	
};