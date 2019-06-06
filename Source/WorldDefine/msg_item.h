//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: msg_item.h
// author: 
// actor:
// data: 2008-07-21
// last:
// brief: 客户端和服务器端间消息 -- 物品/装备相关
//-----------------------------------------------------------------------------
#pragma once

#include "msg_common_errorcode.h"
#include "ItemDefine.h"
#include "AvatarAtt.h"

#pragma pack(push, 1)
//----------------------------------------------------------------------------
// 错误码枚举
//----------------------------------------------------------------------------
enum
{
	// 该部分放到msg_common_errorcode.h中，以便各模块通用
	//E_Item_LevelLimit				= 9,		// 等级不满足要求
	//E_Item_SexLimit					= 10,		// 性别不满足要求
	//E_Item_ClassLimit				= 11,		// 职业不满足要求
	//E_Item_TalentLimit				= 12,		// 天资不满足要求
	//E_Item_ZoneRepLimit				= 13,		// 氏族声望不满足要求
	//E_Item_OtherRepLimit			= 14,		// 其他声望不满足要求
	//E_Item_RoleStatusLimit			= 15,		// 玩家状态(战斗，非战斗)不满足要求
	//E_Item_AttA_Limit				= 16,		// 一级属性不满足

	E_Equip_OnFailed				= 100,		// 装备失败
	E_Equip_NotRing					= 101,		// 装备不是ring
	E_Equip_InvalidPos				= 102,		// 装备栏位与装备类型要求栏位不符
	E_Equip_NotRingPos				= 103,		// 装备栏位中移动的目标位置不是ring栏位
	E_Equip_SamePos					= 104,		// 目标位置与当前位置同
	E_Equip_NotIdentify				= 105,		// 装备没有鉴定
	E_Equip_WeaponPos_Overlap		= 106,		// 武器位置重复
	E_Equip_Idendtifed				= 107,		// 武器已鉴定过
	E_Equip_Guard_DeadPunish		= 108,		// 装备具有死亡守护特殊属性，角色死亡不掉落
	E_Equip_Lock					= 109,		// 锁定物品
	E_Equip_VocationLimit			= 110,		// 该职业不能装备该装备

	E_BagPsd_Exist					= 150,		// 已设密码
	E_BagPsd_NoExist				= 151,		// 未设密码
	E_BagPsd_SafeCode_Error			= 152,		// 安全码错误
	E_BagPsd_Error					= 153,		// 密码错误
	E_BagPsd_OK						= 154,		// 密码正确

    E_Dye_OtherItem                 = 170,      // 只能对时装进行染色
    E_Dye_Exist                     = 171,      // 该时装已为该颜色，不需染色
    E_Dye_LvLimit                   = 172,      // 染料等级不足
	E_Dye_NotExist					= 173,		// 装备或者染料不存在

	E_GoldStone_Success				= 180,		// 点金石操作成功
	E_GoldStone_SrcItemNotExist		= 181,		// 点金石原料不存在
	E_GoldStone_NoConfigFile		= 182,		// 点金石服务器端配置文件不存在
	E_GoldStone_CreateDestItemFail	= 183,		// 创建物品失败
	E_GoldStone_MissingMatch		= 184,		// 待点化物品类型与点金石不匹配
	E_GoldStone_OtherError			= 185,		// 其他未知错误
};

//----------------------------------------------------------------------------
// 装备穿戴相关消息(注意结构中字段顺序最好保证内存对齐)
//----------------------------------------------------------------------------
CMD_START(NC_Equip)	// 装备
	EEquipPos	ePosDst;		// 装备部位(注意戒指装备部位，客户端需做判断处理)
	INT64		n64Serial;		// 装备64位序列号
CMD_END

CMD_START(NS_Equip) // 反回装备结果，相关影响由其他消息处理
	DWORD		dwErrorCode;
	INT64		n64Serial;		// 装备64位序列号
CMD_END

CMD_START(NC_Unequip)
	INT16		n16PosDst;		// 待放入背包中位置
	INT16		n16Dummy;		// 对齐用
	INT64		n64Serial;		// 装备64位序列号
CMD_END

CMD_START(NS_Unequip)
	DWORD		dwErrorCode;
	INT64		n64Serial;		// 装备64位序列号
CMD_END

CMD_START(NC_SwapWeapon)
CMD_END

CMD_START(NS_SwapWeapon)
	DWORD		dwErrorCode;
CMD_END

CMD_START(NS_AvatarEquipChange)	// 武器外观变化
	DWORD			dwRoleID;
	tagEquipDisplay	EquipDisplay;	// 装备外观
CMD_END

CMD_START(NS_EquipEffectChange)	// 武器挂载效果变化
	DWORD			dwRoleID;
	BYTE			byDisplayPos;	// 模型位置
	BYTE			byEquipEffect;	// 挂载效果
CMD_END

CMD_START(NC_IdentifyEquip)	// 武器鉴定
	INT64		n64SerialReel;	// 鉴定卷轴
	INT64		n64SerialEquip;	// 装备
CMD_END

CMD_START(NS_IdentifyEquip)	// 武器鉴定
	DWORD		dwErrorCode;
	INT64		n64Serial;
CMD_END

CMD_START(NS_EquipChange)
	INT64			n64Serial;
	tagEquipSpec	equipSpec;
CMD_END

CMD_START(NS_SuitEffect)	// 套装特效改变
	DWORD		dwRoleID;
	DWORD		dwSuitEffectID;
CMD_END

CMD_START(NS_SuitNum)		// 套装个数改变
	DWORD		dwSuitID;
	INT8		n8Num;
	INT8		n8Dummy[3];
CMD_END

//----------------------------------------------------------------------------
// 物品位置发生变化
//----------------------------------------------------------------------------
CMD_START(NC_ItemPosChange) // 物品位置移动 -- 同一个容器内
	EItemConType	eConType;		// 容器类型
	INT64			n64Serial;		// 移动位置的物品
	INT16			n16Num;			// 移动物品的个数(0表示全部移动)
	INT16			n16PosDst;		// 物品移动的目标位置
	DWORD			dwNPCID;		// 职能NPC ID(现在只有仓库使用)
CMD_END

CMD_START(NS_ItemPosChange) // 物品位置移动 -- 同一个容器内
	EItemConType	eConType;		// 容器类型
	INT64			n64Serial1;		// 移动位置的物品1
	INT64			n64Serial2;		// 受影响物品2, 当为GT_INVALID时, 表示将物品1移动到一个空位
	INT16			n16PosDst1;		// 物品1移动的目标位置
	INT16			n16PosDst2;		// 物品2移动的目标位置
	INT16			n16Num1;		// 物品1最终个数 -- 当该值为0时,需将该物品删除,服务器不另发消息
	INT16			n16Num2;		// 物品2最终个数
	BOOL			bCreateItem;	// n64Serial2是否为新创建的物品堆 -- 为新建堆时，需客户端处理，服务器不另发消息
CMD_END

CMD_START(NC_ItemPosChangeEx) // 物品位置移动 -- 不同容器内
	EItemConType	eConTypeSrc;	// 源容器类型
	INT64			n64Serial1;		// 移动位置的物品1
	EItemConType	eConTypeDst;	// 目的容器类型
	INT16			n16PosDst;		// 物品1移动的目标位置,为GT_INVALID时表示没有指定目标位置
	INT16			n16Dummy;
	DWORD			dwNPCID;		// 职能NPC ID(现在只有仓库使用)
CMD_END

CMD_START(NS_ItemPosChangeEx) // 物品位置移动 -- 不同容器内
	EItemConType	eConTypeSrc1;	// 容器类型 -- n64Serial1的原容器
	EItemConType	eConTypeSrc2;	// 容器类型 -- n64Serial2的原容器
	EItemConType	eConTypeDst1;	// 容器类型 -- n64Serial1的目标容器
	EItemConType	eConTypeDst2;	// 容器类型 -- n64Serial2的目标容器
	INT16			n16PosDst1;		// 物品1移动的目标位置
	INT16			n16PosDst2;		// 物品2移动的目标位置
	INT64			n64Serial1;		// 移动位置的物品1, 当为GT_INVALID时, 表示物品2移动到一个空位
	INT64			n64Serial2;		// 移动位置的物品2, 当为GT_INVALID时, 表示物品1移动到一个空位
	INT16			n16Num1;		// 物品1最终个数 -- 当该值为0时,需将该物品删除,服务器不另发消息
	INT16			n16Num2;		// 物品2最终个数
CMD_END

CMD_START(NC_ItemReorder)	// 行囊整理
	EItemConType	eContainerType;	// 容器类型
	DWORD			dwNPCID;		// GT_INVALID时，为仓库整理；否则为背包整理
	INT16			n16ItemNum;		// 物品个数
	INT16			n16Index[1];	// 排序前物品在容器中的位置，当前数组下标为排序后顺序
CMD_END

CMD_START(NS_ItemReorder)
	DWORD			dwErrorCode;
	EItemConType	eConType;		// 容器类型
	INT16			n16ItemNum;		// 物品个数
	INT16			n16OldIndex[1];	// 当前数组下标为排序后顺序
CMD_END

CMD_START(NC_ItemReorderEx)	// 行囊整理
	EItemConType	eContainerType;	// 容器类型
	DWORD			dwNPCID;		// GT_INVALID时，为仓库整理；否则为背包整理
	INT16			n16ItemNum;		// 物品个数
	BYTE			byData[1];		// tagItemOrder
CMD_END

CMD_START(NS_ItemReorderEx)
	DWORD			dwErrorCode;
	EItemConType	eConType;		// 容器类型
	INT16			n16ItemNum;		// 物品个数
	BYTE			byData[1];		// tagItemOrder
CMD_END


//----------------------------------------------------------------------------
// 物品位置发生变化
//----------------------------------------------------------------------------
CMD_START(NC_GroundItem)
	INT64			n64Serial;
CMD_END

CMD_START(NS_ItemAdd)
//	DWORD			dwErrorCode;
	INT64			n64Serial;
	EItemConType	eConType;
	INT16			n16Index;		// 容器中位置下标
	INT16			n16Num;			// 如果是可堆叠物品,则为该堆总数
	bool			bOverlap;		// 重叠物品是否放入已有堆，如果true，需考虑释放内存
CMD_END

CMD_START(NS_NewItemAdd)
	tagItem			Item;
CMD_END

CMD_START(NS_NewEquipAdd)
	tagEquip		Equip;
CMD_END

CMD_START(NC_ItemRemove)
	EItemConType	eConType;
	INT64			n64Serial;
CMD_END

CMD_START(NS_ItemRemove)
//	DWORD			dwErrorCode;
	EItemConType	eConType;
	INT64			n64Serial;
	INT16			n16Index;
	INT16			n16Num;			// 如果是可堆叠物品,则为剩余个数,为0表示全部删除
CMD_END

CMD_START(NS_ItemCDUpdate)
	DWORD			dwTypeID;
	DWORD			dwCDTime;
CMD_END;


//----------------------------------------------------------------------------
// 行囊加密相关
//----------------------------------------------------------------------------
CMD_START(NC_SetBagPsd)		// 设置密码
	DWORD	dwBagPsdCrc;	// 欲设置的行囊密码crc值
CMD_END;

//CMD_START(NS_SetBagPsd)
//	DWORD	dwErrorCode;
//CMD_END;

CMD_START(NC_UnsetBagPsd)	// 取消密码
	DWORD	dwSafeCodeCrc;	// 安全码的crc值
CMD_END;

//CMD_START(NS_UnsetBagPsd)
//	DWORD	dwErrorCode;
//CMD_END;

CMD_START(NC_OldBagPsd)		// 旧密码
	DWORD	dwOldBagPsdCrc;
CMD_END;

//CMD_START(NS_OldBagPsd)
//	DWORD	dwErrorCode;
//CMD_END;

CMD_START(NC_ResetBagPsd)	// 修改密码
	DWORD	dwOldBagPsdCrc;
	DWORD	dwNewBagPsdCrc;
CMD_END;

//CMD_START(NS_ResetBagPsd)
//	DWORD	dwErrorCode;
//CMD_END;

CMD_START(NC_OpenBagPsd)	// 若加密，打开背包时需先发送该消息
	DWORD	dwBagPsdCrc;
CMD_END;

//CMD_START(NS_OpenBagPsd)
//	DWORD	dwErrorCode;
//CMD_END;

CMD_START(NS_BagPsd)		// 行囊密码相关消息反馈
	DWORD	dwErrorCode;
	DWORD	dwRoleStateEx;	// 只需玩家自己知道的状态
CMD_END;


//----------------------------------------------------------------------------
// 主手武器崭新度更新
//----------------------------------------------------------------------------
CMD_START(NS_RWNewessChange)
	INT32	nAttackTimes;		// 主手武器当前使用次数
CMD_END

//----------------------------------------------------------------------------
// 龙魂能力
//----------------------------------------------------------------------------
CMD_START(NS_LongHunOn)		// 指定部位装备的龙魂能力已激活
	INT16	n16EquipPos;
CMD_END

CMD_START(NS_LongHunOff)	// 指定部位装备的龙魂能力未激活
	INT16	n16EquipPos;
CMD_END

//----------------------------------------------------------------------------
// 使用染料为时装上色相关
//----------------------------------------------------------------------------
CMD_START(NC_DyeFashion)
    INT64   n64DyeSN;       // 染料64位id
    INT64   n64EquipSerial; // 时装装备64位id
CMD_END

CMD_START(NS_DyeFashion)
    DWORD   dwErrCode;      // 错误码
CMD_END

//----------------------------------------------------------------------------
// 百宝袋记录相关
//----------------------------------------------------------------------------
CMD_START(NC_InitBaiBaoRecord)	// 向服务器发送初始化百宝袋记录请求
CMD_END

CMD_START(NS_InitBaiBaoRecord)	// 服务器返回所有的消息
	INT16	n16Num;				// 记录数量
	BYTE	byData[1];			// tagBaiBaoRecord，长度计算取n16Size，不要用sizeof
CMD_END

CMD_START(NS_SingleBaiBaoRecord)// 玩家在线时，服务器发送单条百宝袋记录
	tagBaiBaoRecord	sRecord;	// 单条记录
CMD_END

//----------------------------------------------------------------------------
// 打开随身仓库
//----------------------------------------------------------------------------
CMD_START(NC_WareOpen)
	INT64	n64ItemID;
CMD_END

CMD_START(NS_WareOpen)
	DWORD	dwErrorCode;
CMD_END

//-----------------------------------------------------------------------------
//	点金石
//-----------------------------------------------------------------------------
CMD_START(NC_BeGoldStone)
	INT64		n64SrcItemID;	// 点金石
	INT64		n64DstItemID;	// 点金石要操作的物品
CMD_END
CMD_START(NS_BeGoldStone)
	DWORD		dwErrCode;		// 点金石操作结果
CMD_END

//-----------------------------------------------------------------------------
//	物品绑定
//-----------------------------------------------------------------------------
CMD_START(NS_ItemBind)
	EItemConType	eConType;
	INT64			n64Serial;
CMD_END

#pragma pack(pop)
