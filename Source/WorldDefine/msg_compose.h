//-----------------------------------------------------------------------------
//!\file msg_compose.h
//!
//!\date 2008-09-25
//!
//!\brief 客户端与服务器之间关于合成，分解，强化的消息
//-----------------------------------------------------------------------------
#pragma once

#include "msg_common_errorcode.h"
#include "compose_define.h"

#pragma pack(push, 1)

//----------------------------------------------------------------------------
// 错误码枚举
//----------------------------------------------------------------------------
enum 
{
	//共有错误码
	E_Compose_NPC_Not_Exist = 1, 				//NPC不存在
	E_Compose_Formula_Not_Exist = 2, 			//配方不存在
	E_Compose_Stuff_Not_Enough = 3, 			//材料数量不够
	E_Compose_Stuff_Formula_Not_Match = 4, 		//材料和配方不匹配
	E_Compose_NPC_Distance = 5,					//NPC距离太远
	
	//合成强化返回的三种结果,是根据合成公式算的
	E_Compose_Consolidate_Success = 11, 		//成功
	E_Compose_Consolidate_Lose = 12, 			//失败
	E_Compose_Consolidate_Perfect = 13, 		//完美

	
	//合成错误码
	E_Compose_Stuff_Not_Exist = 21, 			//合成材料不存在
	E_Compose_Type_Not_Exist = 22,				//合成类型不存在
	E_Compose_Skill_Not_Exist = 23,				//玩家没有合成技能
	E_Compose_FormulaNotMatch = 24,				//合成配方不匹配
	E_Compose_NPCCanNotCompose = 25,			//不是合成NPC
	E_Compose_Vitality_Inadequate = 26,			//玩家活力值不足
	E_Compose_NotEnough_Money = 27,				//玩家金钱不够
	E_Compose_Bag_Full = 28,					//背包已满
	
	//分解错误码
	E_Decomposition_Item_Not_Exist = 31, 		//分解物品不存在

	//强化错误码
	E_Consolidate_Equip_Not_Exist = 41, 		//强化装备不存在
	E_Consolidate_Type_Not_Exist = 42,			//强化类型不存在
	E_Consolidate_NPCCanNotPosy	= 43,			//不是铭纹NPC
	E_Consolidate_NotEquipment = 44,			//强化的物品不是装备
	E_Consolidate_BeyondPosyTime,
	E_Consolidate_FormulaNotMatch,				//强化配方不匹配
	E_Consolidate_NotEnough_Money,				//玩家金钱不够
	E_Consolidate_NotEnough_Stuff,				//玩家材料不够
	E_Consolidate_EquipCanNotPosy,				//该属性不能被铭纹
	E_Consolidate_ValConsume_Inadequate,		//装备潜力值不足

	E_Compose_NPCCanNotEngrave,					//不是镌刻NPC
	E_Consolidate_EquipCanNotEngrave,			//该属性不能被镌刻
	E_Consolidate_BeyondEngraveTime,			//超过镌刻次数

	E_Consolidate_Gem_Not_Exit,					//宝石不存在
	E_Consolidate_Gem_Full,						//装备已镶满宝石
	E_Consolidate_EquipCanNotInlay,				//该宝石不能被镶嵌
	E_Consolidate_Gem_Not_Hole,					//装备无孔

	E_Consolidate_Brand_Not_Exit,				//印记不存在
	E_Consolidate_Brand_TopLevel,				//印记等级达到上限
	E_Consolidate_EquipCanNotBrand,				//该装备不能被印记

	E_Consolidate_LoongSoul_Not_Exit,			//龙魂石不存在
	E_Consolidate_LoongSoul_TopLevel,			//龙魂等级达到上限
	E_Consolidate_EquipCanNotLoongSoul,			//该装备不能被龙附

	E_Consolidate_NPCCanNotQuench,				//不是淬火NPC
	E_Consolidate_WXValueInadequate,			//原属性值不足
	E_Consolidate_False,						//淬火失误

	E_Compose_Quality_Not_Match,				//点化只限于黄色及以上装备
	E_Compose_Not_Fashion,						//面具及时装不可点化或分解
	E_Compose_Equip_Lock,						//已锁定的装备不可点化或分解
	E_Compose_Equip_Time_Limit,					//有时间限制的装备不可点化或分解
	E_Compose_Equip_Not_identify,				//未鉴定的装备不可点化或分解
	E_Compose_Equip_Level_Inadequate,			//装备等级不够
	E_Compose_Equip_Quality_Inadequate,			//装备品质不够
	E_Compose_Equip_Type_Inadequate,			//装备类型不满足配方要求

	E_Consolidate_Chisel_Not_Exit,				//凿石不存在
	E_Consolidate_Equip_CanNot_Chisel,			//装备不能被开凿
	E_Consolidate_Equip_Hole_Full,				//镶嵌孔数量已为5个，不可进行开凿
	E_Consolidate_Chisel_Not_Match,				//凿石等级小于所开凿的装备等级，不可进行开凿
	E_Consolidate_NotIdentified,				//未鉴定装备不可开凿

	E_Raise_PotVal_Not_Needing,					// 已经满了，不需要在提升潜力值
	E_Raise_PotVal_Times_Zero,					// 提升次数为零

	// Jason v1.3.1 2009-12-21 宝石拆除errorid
	E_Gem_NPCCanNotRemoveGem,					//不是宝石拆除NPC

};

//-----------------------------------------------------------------------------
//	合成消息
//-----------------------------------------------------------------------------
CMD_START(NC_Produce)
	DWORD 				dwNPCID; 		//NPCID
	DWORD				dwSkillID;		//技能ID
	INT64				n64ItemID;		//物品ID
	DWORD 				dwFormulaID;  	//配方ID
	INT64				n64IMID;		//IMid
	INT64				n64StuffID[1]; 	//材料 数量变长
CMD_END

CMD_START(NS_Produce)
	DWORD				dwErrorCode;	//返回码
CMD_END

//-----------------------------------------------------------------------------
//	分解消息 属于点化技能中的一个分支
//-----------------------------------------------------------------------------
CMD_START(NC_Decomposition)
	DWORD				dwNPCID; 		//NPCID	
	DWORD				dwSkillID;		//技能ID
	INT64				n64ItemID;		//物品ID
	DWORD				dwFormulaID;	//配方ID
	INT64				n64IMID;		//IMid
	INT64				n64Item; 		//被分解物品64位ID
CMD_END

CMD_START(NS_Decomposition)
	DWORD				dwErrorCode;	//返回码
CMD_END

//-----------------------------------------------------------------------------
//	强化消息
//-----------------------------------------------------------------------------
CMD_START(NC_ConsolidatePosy)
	DWORD 				dwNPCID; 		//NPCID
	DWORD 				dwFormulaID;  	//配方ID
	INT64 				n64ItemID; 		//要被强化物品的64位ID
	INT64				n64IMID;		//IMid
	INT64				n64StuffID[1]; 	//材料 数量变长	
CMD_END

CMD_START(NS_ConsolidatePosy)
	DWORD				dwErrorCode;	//返回码
CMD_END

CMD_START(NC_ConsolidateEngrave)
	DWORD				dwNPCID;		//NPCID
	DWORD				dwFormulaID;	//配方ID
	INT64				n64ItemID;		//要被强化物品的64位ID
	INT64				n64IMID;		//IMID
	INT64				n64StuffID[1];	//材料 
CMD_END

CMD_START(NS_ConsolidateEngrave)
	DWORD				dwErrorCode;	//返回码
CMD_END

CMD_START(NC_ConsolidateQuench)
	DWORD				dwNPCID;		//NPCID
	DWORD				dwFormulaID;	//配方ID
	INT64				n64ItemID;		//要被强化物品的64位ID
	INT64				n64IMID;		//IMID
	INT64				n64StuffID[1];	//材料 
CMD_END

CMD_START(NS_ConsolidateQuench)
	DWORD				dwErrorCode;	//返回码
CMD_END

//-----------------------------------------------------------------------------
//	镶嵌、烙印、龙附
//-----------------------------------------------------------------------------
CMD_START(NC_Inlay)
	INT64				n64SrcItemID;	//宝石的64ID
	INT64				n64DstItemID;	//装备的64ID
CMD_END

CMD_START(NS_Inlay)
	DWORD				dwErrorCode;	//返回码
CMD_END

CMD_START(NC_Brand)
	INT64				n64SrcItemID;	//印记的64ID
	INT64				n64DstItemID;	//装备的64ID
	INT64				n64IMID;		//保底符64ID 不使用:GT_INVALID
CMD_END

CMD_START(NS_Brand)
	DWORD				dwErrorCode;	//返回码
CMD_END

CMD_START(NC_LoongSoul)
	INT64				n64SrcItemID;	//龙魂石64ID
	INT64				n64DstItemID;	//装备的64ID
CMD_END

CMD_START(NS_LoongSoul)
	DWORD				dwErrorCode;	//返回码
CMD_END

CMD_START(NC_Chisel)	
	INT64				n64SrcItemID;	//装备的64ID
	INT64				n64StuffID;		//凿石的64ID
CMD_END

CMD_START(NS_Chisel)
	DWORD				dwErrorCode;	//返回码
CMD_END

//-----------------------------------------------------------------------------
//	提升装备潜力值
//-----------------------------------------------------------------------------

CMD_START(NC_AddPot)
	INT64				n64SrcItemID;	//道具的64ID
	INT64				n64DstItemID;	//装备的64ID
	INT64				n64IMID;		// 保底符64ID 不使用:GT_INVALID
CMD_END

CMD_START(NS_AddPot)
	DWORD				dwErrorCode;	//返回码 0：成功； 1：完美； 2：失败 
	DWORD				dwPotValue;		// 增加的潜力值
CMD_END

// Jason v1.3.1 2009-12-21 宝石拆解相关协议
CMD_START( NC_GetGemRemovalInfo )
	DWORD 				dwNPCID; 		// NPCID
	INT64				n64EquipID;		// 装备序列id
CMD_END

CMD_START( NS_GetGemRemovalInfo )
	DWORD				dwErrCode;		// 
	INT16				n16GemNum;		// 宝石个数，最多5颗宝石
	DWORD				dwGemIDs[MAX_EQUIPHOLE_NUM];	// 宝石typeids，0/GT_INVALID表示没有宝石
CMD_END

CMD_START( NC_GemRemoval )
	DWORD 				dwNPCID; 		// NPCID
	INT64				n64EquipID;		// 装备序列id
	INT16				n16GemNum;
	INT64				n64GemRomovalSign[MAX_EQUIPHOLE_NUM];	// 宝石拆除符id
CMD_END

CMD_START( NS_GemRemoval )
	DWORD				dwErrorCode;
CMD_END


#pragma pack(pop)
		




