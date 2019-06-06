//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: att_res.h
// author: Sxg
// actor:
// data: 2008-05-28
// last:
// brief: 资源管理器定义
//-----------------------------------------------------------------------------
#pragma once

#include "../ServerDefine/role_data_define.h"
#include "../WorldDefine/ItemDefine.h"
#include "../WorldDefine/skill_define.h"
#include "../WorldDefine/buff_define.h"
#include "../WorldDefine/trigger_define.h"
#include "../WorldDefine/compose_define.h"
#include "../WorldDefine/MapAttDefine.h"
#include "../WorldDefine/mall_define.h"
#include "../WorldDefine/variable_len.h"
#include "../WorldDefine/guild_define.h"
#include "../WorldDefine/stall_define.h"
#include "../ServerDefine/pet_define.h"
#include "../ServerDefine/consolidate_define.h"
#include "../ServerDefine/title_define.h"
#include "../ServerDefine/instance_define.h"
#include "../ServerDefine/att_res_define.h"
#include "../WorldDefine/pet_define.h"
#include "../ServerDefine/guild_define.h"
#include "famehall_part.h"
#include "../ServerDefine/item_define.h"


struct	tagMotionProto;
struct	tagPetProto;
struct	tagPetSkillProto;
struct	tagPetEquipProto;
struct	tagPetGatherProto;
struct	tagPetWuXingProto;
struct	tagVNBEquipProto;
struct	tagVNBGiftProto;
struct	tagItemProto;
struct	tagShopProto;
struct	tagDakProto;
struct	tagSuitProto;
struct	tagMallItemProto;
struct	tagMallPackProto;
struct	tagSSpawnPointProto;
struct	tagGuildFacilities;
struct	tagGuildUpgradeNeed;
struct	tagCofCSellGood; 
struct	tagCofCBuyGood; 

class	Quest;
//-----------------------------------------------------------------------------
class AttRes
{
public:
	typedef VOID* (WINAPI* Fun_p)(LPVOID, LPCTSTR, INT32);

public:
	BOOL Init();
	VOID Destroy();

public: // 上层确保互斥 -- 最好是在地图线程的上层线程执行
	// 重新加载指定属性文件
	BOOL ReloadItemProto();

	// 重新加载商城数据
	BOOL ReloadMallProto();

	// 重新加载需要记log的物品id -- item_needlog表
	// 发消息tagNDBC_LoadItemNeedLog即可

public:
	// 根据TypeID重新设置是否记录log
	VOID ResetItemLog(DWORD dwTypeID[], INT32 nNum);

public:
	//-------------------------------------------------------------------------------------------
	// 升级
	//-------------------------------------------------------------------------------------------
	const tagLevelUpEffect* GetLevelUpEffect(INT32 nLevel) const { return &m_LevelUpEffect[nLevel]; }

	//-------------------------------------------------------------------------------------------
	// 默认值
	//-------------------------------------------------------------------------------------------
	INT GetAttDefRole(INT nIndex) { ASSERT( nIndex > ERA_Null && nIndex < ERA_End ); return m_AttDefMinMax[nIndex].nDefRole; }
	INT GetAttDefCreature(INT nIndex) { ASSERT( nIndex > ERA_Null && nIndex < ERA_End ); return m_AttDefMinMax[nIndex].nDefCreature; }
	INT GetAttMin(INT nIndex) { ASSERT( nIndex > ERA_Null && nIndex < ERA_End ); return m_AttDefMinMax[nIndex].nMin; }
	INT GetAttMax(INT nIndex) { ASSERT( nIndex > ERA_Null && nIndex < ERA_End ); return m_AttDefMinMax[nIndex].nMax; }

	//-------------------------------------------------------------------------------------------
	// 过滤词表
	//-------------------------------------------------------------------------------------------
	std::vector<tstring>* GetNameFilterWords() 	{ return &m_vectNameFilter; }
	//std::vector<tstring>* GetChatFilterWords() 	{ return &m_vectChatFilter; }

	//-------------------------------------------------------------------------------------------
	// 不同语言版本名称长度
	//-------------------------------------------------------------------------------------------
	const tagVariableLen& GetVariableLen() const { return m_VarLen; }

	//-------------------------------------------------------------------------------
	// 获取权限表
	//-------------------------------------------------------------------------------
	const tagGuildPower& GetGuildPower(EGuildMemberPos eGuildPos) const
	{
		return m_GuildPowerDefault[eGuildPos];
	}

	const tagGuildKick& GetGuildKick(EGuildMemberPos eGuildPos) const
	{
		return m_GuildKickDefault[eGuildPos];
	}

	const tagGuildAppoint& GetGuildAppoint(EGuildMemberPos eGuildPos) const
	{
		return m_GuildAppointDefault[eGuildPos];
	}

	//-------------------------------------------------------------------------------
	// 获取帮派设施升级需求信息
	//-------------------------------------------------------------------------------
	BOOL GetGuildUpgradeBaseInfo(BYTE eType, BYTE byLevel, OUT tagGuildFacilities& sInfo);
	BOOL GetGuildUpgradeItemInfo(BYTE eType, BYTE byLevel, OUT tagGuildFacilities& sInfo);

	//-------------------------------------------------------------------------------
	// 获取帮派事务信息
	//-------------------------------------------------------------------------------
	const tagGuildAffairInfo* GetGuildAffairInfo(DWORD dwBuffID)	{ return m_GuildAffairInfo.Peek(dwBuffID); }

	//-------------------------------------------------------------------------------
	// 获取帮派技能信息
	//-------------------------------------------------------------------------------
	BOOL GetGuildSkillInfo(DWORD dwSkillID, INT nLevel, tagGuildSkill& sGuildSkill);
	BOOL LoadGuildSkillInfo(TMap<DWORD, tagGuildSkill*>& mapGuildSkill);
	const tagGuildSkill* GetGuildSkillProto(DWORD dwSkillID)		{ return m_GuildSkillInfo.Peek(dwSkillID); }

	//-------------------------------------------------------------------------------
	// 获取帮派跑商信息
	//-------------------------------------------------------------------------------
	const tagCommerceInfo*		GetGuildCommerceInfo(INT nLevel);
	const tagCommodityProto*	GetCommodityProto(DWORD dwGoodID)	{ return m_GuildCommodityProto.Peek(dwGoodID); }
	const tagCofCSPProto*		GetCofCSPProto(DWORD dwCofCID)		{ return m_CofCSPProto.Peek(dwCofCID); }
	const FLOAT					GetCofCProfit(DWORD dwDstID, DWORD dwSrcID);
	BOOL LoadCofCGoodInfo(DWORD dwCofCID, TMap<DWORD, tagCofCSellGood*>& mapGoodSell,
		TMap<DWORD, tagCofCBuyGood*>& mapGoodBuy);
	
	//-------------------------------------------------------------------------------------------
	// 物品
	//-------------------------------------------------------------------------------------------
	tagItemProto* GetItemProto(DWORD dwTypeID)			{ return m_mapItemProto.Peek(dwTypeID); }
	//-------------------------------------------------------------------------------------------
	// 装备
	//-------------------------------------------------------------------------------------------
	tagEquipProto* GetEquipProto(DWORD dwTypeID)		{ return m_mapEquipProto.Peek(dwTypeID); }
	//-------------------------------------------------------------------------------------------
	// 精华(宝石,印记,龙魂等)
	//-------------------------------------------------------------------------------------------
	tagGemProto* GetGemProto(DWORD dwTypeID)			{ return m_mapGemProto.Peek(dwTypeID); }
	//-------------------------------------------------------------------------------------------
	// 商店
	//-------------------------------------------------------------------------------------------
	tagShopProto* GetShopProto(DWORD dwShopID)			{ return m_mapShopProto.Peek(dwShopID); }

	//-------------------------------------------------------------------------------------------
	// 商场商品, 礼品包及免费物品
	//-------------------------------------------------------------------------------------------
	const tagMallItemProto* GetMallItemProto(DWORD dwID){ return m_mapMallItemProto.Peek(dwID); }
	const tagMallPackProto* GetMallPackProto(DWORD dwID){ return m_mapMallPackProto.Peek(dwID); }
	const tagMallFreeItem*	GetMallFreeProto()			{ return &m_MallFreeItemProto; }
	
	TMap<DWORD, tagMallItemProto*>& GetMallItem()		 { return m_mapMallItemProto; }
	TMap<DWORD, tagMallPackProto*>& GetMallPack()		 { return m_mapMallPackProto; }

	INT	GetMallItemNum()								{ return m_mapMallItemProto.Size(); }
	INT GetMallPackNum()								{ return m_mapMallPackProto.Size(); }

	//-------------------------------------------------------------------------------------------
	// 驿站
	//-------------------------------------------------------------------------------------------
	tagDakProto* GetDakProto(DWORD dwDakID)				{ return m_mapDakProto.Peek(dwDakID); }
	//-------------------------------------------------------------------------------------------
	// 装备品级鉴定几率
	//-------------------------------------------------------------------------------------------
	tagEquipQltyPct* GetEquipQltyPct(DWORD dwTypeID)	{ return m_mapEquipQltyPct.Peek(dwTypeID); }
	//-------------------------------------------------------------------------------------------
	// 套装静态属性
	//-------------------------------------------------------------------------------------------
	const tagSuitProto* GetSuitProto(DWORD dwSuitID)	{ return m_mapSuitProto.Peek(dwSuitID); }
	//-------------------------------------------------------------------------------------------
	// 品级装备属性参数
	//-------------------------------------------------------------------------------------------
	const tagEquipQltyEffect* GetEquipQltyEffect(INT32 nQlty) const { return &m_EquipQltyEffect[nQlty]; }

	//-------------------------------------------------------------------------------------------
	// 龙魂能力
	//-------------------------------------------------------------------------------------------
	const tagLongHunProto* GetLongHunProto(DWORD dwID) { return m_mapLongHunProto.Peek(dwID); }
	
	const std::vector<DWORD>& GetLongHunSpecVect(INT nType, INT nTypeEx, INT nQlty) const
	{
		return m_vectLongHun[nType][nTypeEx][nQlty];
	}

	//-------------------------------------------------------------------------------------------
	// 时装品级对生成影响参数
	//-------------------------------------------------------------------------------------------
	const tagFashionGen* GetFashionQltyEffect(INT32 nQlty) const { return &m_FashionGen[nQlty]; }

	//-------------------------------------------------------------------------------------------
	// 时装颜色生成概率参数
	//-------------------------------------------------------------------------------------------
	const tagFashionColorPct* GetFashionColorPct(INT32 nQlty) const { return &m_FashionColorPct[nQlty]; }
	
	//-------------------------------------------------------------------------------------------
	// 得到满足要求所有物品的指针列表
	//-------------------------------------------------------------------------------------------
	TList<tagItemProto*> GetItemProtoList();
	//-------------------------------------------------------------------------------------------
	// 得到铭纹属性对应的装备是否可强化
	//-------------------------------------------------------------------------------------------
	BOOL IsPosyPos(EPosyAtt ePosyAtt, EEquipPos eEquipPos);
	//-------------------------------------------------------------------------------------------
	// 得到镌刻属性对应的装备是否可强化
	//-------------------------------------------------------------------------------------------
	BOOL IsEngravePos(EEngraveAtt ePosyAtt, EEquipPos eEquipPos);

	//-------------------------------------------------------------------------------------------
	// 技能
	//-------------------------------------------------------------------------------------------
	const tagSkillProto* GetSkillProto(DWORD dwTypeID) { return m_mapSkillProto.Peek(dwTypeID); }

	//-------------------------------------------------------------------------------------------
	// 状态
	//-------------------------------------------------------------------------------------------
	const tagBuffProto* GetBuffProto(DWORD dwTypeID) { return m_mapBuffProto.Peek(dwTypeID); }

	//-------------------------------------------------------------------------------------------
	// 触发器
	//-------------------------------------------------------------------------------------------
	const tagTriggerProto* GetTriggerProto(DWORD dwTriggerID) { return m_mapTriggerProto.Peek(dwTriggerID); }

	//-------------------------------------------------------------------------------------------
	// 某个技能是否能被其它技能影响
	//-------------------------------------------------------------------------------------------
	BOOL CanBeModified(DWORD dwSkillID) { return m_mapSkillModify.IsExist(dwSkillID); }

	//-------------------------------------------------------------------------------------------
	// 得到能影响某个技能的技能列表
	//-------------------------------------------------------------------------------------------
	tagSkillModify* GetSkillModifier(DWORD dwSkillID) { return m_mapSkillModify.Peek(dwSkillID); }

	//-------------------------------------------------------------------------------------------
	// 怪物
	//-------------------------------------------------------------------------------------------
	const tagCreatureProto* GetCreatureProto(DWORD dwTypeID) { return m_mapCreatureProto.Peek(dwTypeID); }

	//-------------------------------------------------------------------------------------------
	// 怪物AI
	//-------------------------------------------------------------------------------------------
	const tagCreatureAI* GetCreatureAI(DWORD dwAIID) { return m_mapCreatureAI.Peek(dwAIID); }

	//-------------------------------------------------------------------------------------------
	// 非随机副本地图刷怪点
	//-------------------------------------------------------------------------------------------
	const tagSSpawnPointProto *GetSSpawnPointProto(DWORD dwSpawnPointID) { return m_mapSSpawnPoint.Peek(dwSpawnPointID); }

	//-------------------------------------------------------------------------------------------
	// 副本随机刷怪点
	//-------------------------------------------------------------------------------------------
	const tagRandSpawnPointInfo* GetSpawnPointProto(DWORD dwSpawnPoint) { return m_mapSpawnPointProto.Peek(dwSpawnPoint); }

	//-------------------------------------------------------------------------------------------
	// 副本随机刷怪点
	//-------------------------------------------------------------------------------------------
	const tagInstance*	GetInstanceProto(DWORD dwMapID)	{ return m_mapInstanceProto.Peek(dwMapID); }

	//-------------------------------------------------------------------------------------------
	// 副本中不能使用的物品
	//-------------------------------------------------------------------------------------------
	const tagInstanceItem* GetInstanceItem(DWORD dwMapID) { return m_mapInstanceItem.Peek(dwMapID); }

	//-------------------------------------------------------------------------------------------
	// 副本中不能使用的技能
	//-------------------------------------------------------------------------------------------
	const tagInstanceSkill* GetInstanceSkill(DWORD dwMapID) { return m_mapInstanceSkill.Peek(dwMapID); }

	//-------------------------------------------------------------------------------------------
	// 副本随机刷怪点等级映射表
	//-------------------------------------------------------------------------------------------
	const tagLevelMapping* GetLevelMapping(INT nLevel) { return m_mapLevelMapping.Peek(nLevel); }

	//-------------------------------------------------------------------------------------------
	// 铭纹
	//-------------------------------------------------------------------------------------------
	const tagPosyProtoSer* GetPosyProto(DWORD dwPosyID) { return m_mapPosyProto.Peek(dwPosyID); }

	//-------------------------------------------------------------------------------------------
	// 镌刻
	//-------------------------------------------------------------------------------------------
	const tagEngraveProtoSer* GetEngraveProto(DWORD dwEngraveID) { return m_mapEngraveProto.Peek(dwEngraveID); }

	//-------------------------------------------------------------------------------------------
	// 镶嵌,烙印,龙附
	//-------------------------------------------------------------------------------------------
	const tagConsolidateItem* GetConsolidateProto(DWORD dwTypeID) { return m_mapConsolidateProto.Peek(dwTypeID); }

	//-------------------------------------------------------------------------------------------
	// 合成
	//-------------------------------------------------------------------------------------------
	const tagProduceProtoSer* GetProduceProto(DWORD dwFormulaID) { return m_mapProduceProto.Peek(dwFormulaID); }

	//-------------------------------------------------------------------------------------------
	// 点化,装备分解
	//-------------------------------------------------------------------------------------------
	const tagDeComposeProtoSer* GetDeComposeProto(DWORD dwFormulaID) { return m_mapDeComposeProto.Peek(dwFormulaID); }

	//-------------------------------------------------------------------------------------------
	// 淬火
	//-------------------------------------------------------------------------------------------
	const tagQuenchProtoSer *GetQuenchProto(DWORD dwFormulaID) { return m_mapQuenchProto.Peek(dwFormulaID); }

	//-------------------------------------------------------------------------------------------
	// 称号
	//-------------------------------------------------------------------------------------------
	const tagTitleProto *GetTitleProto(UINT16 u16TitleID) { return &m_TitleProto[u16TitleID]; }

	//-------------------------------------------------------------------------------------------
	// 氏族珍宝
	//-------------------------------------------------------------------------------------------
	const tagClanTreasureProto *GetClanTreasureProto(UINT16 u16TreasureID) { return &m_ClanTreasureProto[u16TreasureID]; }

	VOID GetRandVNBEquipProto(std::list<tagVNBEquipProto*>& listEquips)	
	{	
		m_mapVNBEquipProto.ExportAllValue(listEquips);
	}

	const tagVNBGiftProto*	GetRandVNBGiftProto()	
	{	
		tagVNBGiftProto* pProto = NULL;
		DWORD dwId = GT_INVALID;
		if (!m_mapVNBGiftProto.RandPeek(dwId, pProto) || !P_VALID(pProto))
		{
			return NULL;
		}
		else
		{
			return pProto;
		}
	}

	const tagCreatureAI* RandGetCreatureAI()
	{
		DWORD dwAIID = GT_INVALID;
		tagCreatureAI* pAI = NULL;

		m_mapCreatureAI.RandPeek(dwAIID, pAI);

		return pAI;
	}
	const tagCreatureAI* RandGetCreatureAIInGroup(DWORD dwGroupID)
	{
		TList<DWORD>* pList = m_mapCreatureAIGroup.Peek(dwGroupID);
		if( P_VALID(pList) && pList->Size() > 0 )
		{
			DWORD dwAIID = GT_INVALID;
			pList->RandPeek(dwAIID);

			return m_mapCreatureAI.Peek(dwAIID);
		}
		else
		{
			return NULL;
		}
	}

	//-------------------------------------------------------------------------------------------
	// 获得宠物属性默认最小最大值
	//-------------------------------------------------------------------------------------------
// 	INT GetPetDef(INT nPetAtt) { ASSERT(IS_EPA(nPetAtt));	return m_nPetAttDefMinMax[nPetAtt].nDef;	}
// 	INT GetPetMin(INT nPetAtt) { ASSERT(IS_EPA(nPetAtt));	return m_nPetAttDefMinMax[nPetAtt].nMin;	}
// 	INT GetPetMax(INT nPetAtt) { ASSERT(IS_EPA(nPetAtt));	return m_nPetAttDefMinMax[nPetAtt].nMax;	}

	//-------------------------------------------------------------------------------------------
	// 获得宠物原型
	//-------------------------------------------------------------------------------------------
	const tagPetProto* GetPetProto(DWORD dwPetTypeID) { return m_mapPetProto.Peek(dwPetTypeID); }

	//-------------------------------------------------------------------------------------------
	// 获得宠物升级原型
	//-------------------------------------------------------------------------------------------
	const tagPetLvlUpProto* GetPetLvlUpProto(DWORD dwVLevel) { return m_mapPetLvlUpProto.Peek(dwVLevel); }

	//-------------------------------------------------------------------------------------------
	// 获得宠物升级原型
	//-------------------------------------------------------------------------------------------
	const tagPetLvlUpItemProto* GetPetLvlUpItemProto(DWORD dwTypeID) { return m_mapPetLvlUpItemProto.Peek(dwTypeID); }

	//-------------------------------------------------------------------------------------------
	// 获得宠物技能原型
	//-------------------------------------------------------------------------------------------
	const tagPetSkillProto* GetPetSkillProto(DWORD dwPetSkillTypeID) { return m_mapPetSkillProto.Peek(dwPetSkillTypeID); }

	//-------------------------------------------------------------------------------------------
	// 获得宠物装备原型
	//-------------------------------------------------------------------------------------------
	const tagPetEquipProto* GetPetEquipProto(DWORD dwPetEquipTypeID) { return m_mapPetEquipProto.Peek(dwPetEquipTypeID); }

	//-------------------------------------------------------------------------------------------
	// 获得宠物五行凝结原型
	//-------------------------------------------------------------------------------------------
	const tagPetWuXingProto* GetPetWuXingProto(DWORD dwPetWuXingTypeID) { return m_mapPetWuXingProto.Peek(dwPetWuXingTypeID); }

	//-------------------------------------------------------------------------------------------
	// 获得宠物技能数组
	//-------------------------------------------------------------------------------------------
	const std::list<DWORD>& GetPetNormalSkillList(INT nPetLevel) { return m_PetLevelSkillVec[nPetLevel - 1]; }

	//-------------------------------------------------------------------------------------------
	// VIP摊位信息
	//-------------------------------------------------------------------------------------------
	const INT32 GetVIPStallRent(INT nIndex) { return m_nVIPStallRent[nIndex]; }

	//-------------------------------------------------------------------------------------------
	// 获得动作原型
	//-------------------------------------------------------------------------------------------
	const tagMotionProto* GetMotionProto(DWORD dwTypeID) { return m_mapMotionProto.Peek(dwTypeID); }

private:
	//-------------------------------------------------------------------------------------------
	// 设置初始化某个资源的一条记录的处理函数
	//-------------------------------------------------------------------------------------------
	VOID SetInitOneRes(Fun_p pFun) { m_pFun = pFun; }

	//-------------------------------------------------------------------------------------------
	// 初始化资源的模版函数 -- 存入map中
	//-------------------------------------------------------------------------------------------
	template<class K, class T> 
	BOOL LoadResMap(TMap<K, T*> &mapRes, LPCTSTR szFileName, LPCTSTR szFileName2 = NULL);

	//-------------------------------------------------------------------------------------------
	// 释放资源的模版函数
	//-------------------------------------------------------------------------------------------
	template<class K, class T> VOID FreeResMap(TMap<K, T*> &mapRes);

	//-------------------------------------------------------------------------------------------
	// 初始化资源的模版函数 -- 存入array中
	//-------------------------------------------------------------------------------------------
	template<class T>
	BOOL LoadResArray(T *arrayRes, INT32 nIndexStart, INT32 nIndexEnd, LPCTSTR szFileName);

private:
	//-------------------------------------------------------------------------------------------
	// 初始化过滤词表
	//-------------------------------------------------------------------------------------------
	BOOL InitFilterWords(OUT vector<tstring>& vectFilterWords, LPCTSTR szFileName);

	//-------------------------------------------------------------------------------------------
	// 从脚本管理器中获取变量长度
	//-------------------------------------------------------------------------------------------
	VOID InitVarLen();

private:
	//-------------------------------------------------------------------------------------------
	// 初始化物品的一条记录
	//-------------------------------------------------------------------------------------------
	VOID* InitOneItemProto(OUT LPVOID pProtoType, IN LPCTSTR szField, INT32 nDummy);

	//-------------------------------------------------------------------------------------------
	// 初始化装备的一条记录
	//-------------------------------------------------------------------------------------------
	VOID* InitOneEquipProto(OUT LPVOID pProtoType, IN LPCTSTR szField, INT32 nDummy);

	//-------------------------------------------------------------------------------------------
	// 初始化宝石等的一条记录
	//-------------------------------------------------------------------------------------------
	VOID* InitOneGemProto(OUT LPVOID pProtoType, IN LPCTSTR szField, INT32 nDummy);

	//-------------------------------------------------------------------------------------------
	// 初始化套装的一条记录
	//-------------------------------------------------------------------------------------------
	VOID* InitOneSuitProto(OUT LPVOID pProtoType, IN LPCTSTR szField, INT32 nDummy);

	//-------------------------------------------------------------------------------------------
	// 初始化一类装备品级鉴定几率
	//-------------------------------------------------------------------------------------------
	VOID* InitOneEquipQltyPct(OUT LPVOID pProtoType, IN LPCTSTR szField, INT32 nDummy);

	//-------------------------------------------------------------------------------------------
	// 初始化龙魂能力表
	//-------------------------------------------------------------------------------------------
	VOID* InitOneLongHunProto(OUT LPVOID pProtoType, IN LPCTSTR szField, INT32 nDummy);

	//-------------------------------------------------------------------------------------------
	// 初始化商店
	//-------------------------------------------------------------------------------------------
	VOID* InitOneShopProto(OUT LPVOID pProtoType, IN LPCTSTR szField, INT32 nDummy);

	//-------------------------------------------------------------------------------------------
	// 初始化商场商品及礼品包
	//-------------------------------------------------------------------------------------------
	VOID  InitOneMallItemProtoBase(OUT LPVOID pProtoType, IN LPCTSTR szField);
	VOID* InitOneMallItemProto(OUT LPVOID pProtoType, IN LPCTSTR szField, INT32 nDummy);
	VOID* InitOneMallPackProto(OUT LPVOID pProtoType, IN LPCTSTR szField, INT32 nDummy);

	//-------------------------------------------------------------------------------------------
	// 初始化驿站
	//-------------------------------------------------------------------------------------------
	VOID* InitOneDakProto(OUT LPVOID pProtoType, IN LPCTSTR szField, INT32 nDummy);

	//-------------------------------------------------------------------------------------------
	// 初始化技能的一条记录
	//-------------------------------------------------------------------------------------------
    VOID* InitOneSkillProto(OUT LPVOID pProtoType, IN LPCTSTR szField, INT32 nDummy);

	//-------------------------------------------------------------------------------------------
	// 初始化buff的一条记录
	//-------------------------------------------------------------------------------------------
	VOID* InitOneBuffProto(OUT LPVOID pProtoType, IN LPCTSTR szField, INT32 nDummy);

	//-------------------------------------------------------------------------------------------
	// 初始化trigger的一条记录
	//-------------------------------------------------------------------------------------------
	VOID* InitOneTriggerProto(OUT LPVOID pProtoType, IN LPCTSTR szField, INT32 nDummy);

	//-------------------------------------------------------------------------------------------
	// 初始化Creature的一条记录
	//-------------------------------------------------------------------------------------------
	VOID* InitOneCreatureProto(OUT LPVOID pProtoType, IN LPCTSTR szField, INT32 nDummy);

	//-------------------------------------------------------------------------------------------
	// 初始化Creature_ai的一条记录
	//-------------------------------------------------------------------------------------------
	VOID* InitOneCreatureAIProto(OUT LPVOID pProtoType, IN LPCTSTR szField, INT nDummy);

	//--------------------------------------------------------------------------------------------
	// 初始化铭纹的一条记录
	//--------------------------------------------------------------------------------------------
	VOID* InitOnePosyProto(OUT LPVOID pProtoType, LPCTSTR szField, INT nDummy);

	//--------------------------------------------------------------------------------------------
	// 初始化镌刻的一条记录
	//--------------------------------------------------------------------------------------------
	VOID* InitOneEngraveProto(OUT LPVOID pProtoType, LPCTSTR szField, INT nDummy);

	//--------------------------------------------------------------------------------------------
	// 初始化镶嵌,烙印,龙附的一条记录
	//--------------------------------------------------------------------------------------------
	VOID* InitOneConsolidateProto(OUT LPVOID pProtoType, LPCTSTR szField, INT nDummy);

	//--------------------------------------------------------------------------------------------
	// 初始化生活技能的一条记录
	//--------------------------------------------------------------------------------------------
	VOID* InitOneProduceProto(OUT LPVOID nProtoType, LPCTSTR szField, INT nDummy);

	//--------------------------------------------------------------------------------------------
	// 点火,通用分解
	//--------------------------------------------------------------------------------------------
	VOID* InitOneDeComposeProto(OUT LPVOID nProtoType, LPCTSTR szField, INT nDummy);

	//--------------------------------------------------------------------------------------------
	// 淬火
	//--------------------------------------------------------------------------------------------
	VOID* InitOneQuenchProto(OUT LPVOID nProtoType, LPCTSTR szField, INT nDummy);

	//--------------------------------------------------------------------------------------------
	// 副本中不能使用的物品
	//--------------------------------------------------------------------------------------------
	VOID* InitOneInsItemProto(OUT LPVOID nProtoType, LPCTSTR szField, INT nDummy);

	//--------------------------------------------------------------------------------------------
	// 副本中不能使用的技能
	//--------------------------------------------------------------------------------------------
	VOID* InitOneInsSkillProto(OUT LPVOID nProtoType, LPCTSTR szField, INT nDummy);

	//--------------------------------------------------------------------------------------------
	// 副本随机刷怪点
	//--------------------------------------------------------------------------------------------
	VOID* InitOneSpawnPointProto(OUT LPVOID nProtoType, LPCTSTR szField, INT nDummy);

	//--------------------------------------------------------------------------------------------
	// 副本动态刷怪点等级映射表
	//--------------------------------------------------------------------------------------------
	VOID* InitOneLevelMapping(OUT LPVOID nProtoType, LPCTSTR szField, INT nDummy);

	//--------------------------------------------------------------------------------------------
	// 初始化副本静态数据
	//--------------------------------------------------------------------------------------------
	VOID* InitOneInstanceProto(OUT LPVOID nProtoType, LPCTSTR szField, INT nDummy);

	//--------------------------------------------------------------------------------------------
	// 初始化非副本刷怪点原型
	//--------------------------------------------------------------------------------------------
	VOID* InitOneSSpawnPointProto(OUT LPVOID nProtoType, LPCTSTR szField, INT nDummy);

	//--------------------------------------------------------------------------------------------
	// 初始化宠物属性默认最小最大值
	//--------------------------------------------------------------------------------------------
//	VOID InitPetAttDefMinMax(OUT LPVOID pArray, IN LPCTSTR szField, INT32 nIndex);

	//--------------------------------------------------------------------------------------------
	// 初始化宠物原型
	//--------------------------------------------------------------------------------------------
	VOID* InitOnePetProto(OUT LPVOID nProtoType, LPCTSTR szField, INT nDummy);

	//--------------------------------------------------------------------------------------------
	// 初始化宠物升级原型
	//--------------------------------------------------------------------------------------------
	VOID* InitOnePetLvlUpProto(OUT LPVOID nProtoType, LPCTSTR szField, INT nDummy);

	//--------------------------------------------------------------------------------------------
	// 初始化宠物升级物品原型
	//--------------------------------------------------------------------------------------------
	VOID* InitOnePetLvlUpItemProto(OUT LPVOID nProtoType, LPCTSTR szField, INT nDummy);

	//--------------------------------------------------------------------------------------------
	// 初始化宠物技能原型
	//--------------------------------------------------------------------------------------------
	VOID* InitOnePetSkillProto(OUT LPVOID nProtoType, LPCTSTR szField, INT nDummy);

	//--------------------------------------------------------------------------------------------
	// 初始化宠物装备原型
	//--------------------------------------------------------------------------------------------
	VOID* InitOnePetEquipProto(OUT LPVOID nProtoType, LPCTSTR szField, INT nDummy);

	//--------------------------------------------------------------------------------------------
	// 初始化宠物五行凝结表原型
	//--------------------------------------------------------------------------------------------
	VOID* InitOnePetWuXingProto(OUT LPVOID nProtoType, LPCTSTR szField, INT nDummy);

	//--------------------------------------------------------------------------------------------
	// 初始化金牌网吧礼品
	//--------------------------------------------------------------------------------------------
	VOID* InitOneVNBGiftProto(OUT LPVOID nProtoType, LPCTSTR szField, INT nDummy);

	//--------------------------------------------------------------------------------------------
	// 初始化金牌网吧礼品
	//--------------------------------------------------------------------------------------------
	VOID* InitOneVNBEquipProto(OUT LPVOID nProtoType, LPCTSTR szField, INT nDummy);

	//--------------------------------------------------------------------------------------------
	// 初始化宠物采集表
	//--------------------------------------------------------------------------------------------
	VOID* InitOnePetGatherProto(OUT LPVOID nProtoType, LPCTSTR szField, INT nDummy);
	//--------------------------------------------------------------------------------------------
	// 初始化宠物技能原型
	//--------------------------------------------------------------------------------------------
	VOID InitPetSkillsVec();

	//--------------------------------------------------------------------------------------------
	// 初始化帮派设施升级需求信息
	//--------------------------------------------------------------------------------------------
	VOID* InitOneGuildUpgradeProto(OUT LPVOID nProtoType, LPCTSTR szField, INT nDummy);

	//--------------------------------------------------------------------------------------------
	// 初始化帮派事务信息
	//--------------------------------------------------------------------------------------------
	VOID* InitOneGuildAffairProto(OUT LPVOID nProtoType, LPCTSTR szField, INT nDummy);

	//--------------------------------------------------------------------------------------------
	// 初始化帮派技能信息
	//--------------------------------------------------------------------------------------------
	VOID* InitOneGuildSkillProto(OUT LPVOID nProtoType, LPCTSTR szField, INT nDummy);

	//--------------------------------------------------------------------------------------------
	// 初始化帮派跑商信息
	//--------------------------------------------------------------------------------------------
	VOID* InitOneGuildCommerceProto(OUT LPVOID nProtoType, LPCTSTR szField, INT nDummy);
	VOID* InitOneCommodityProto(OUT LPVOID nProtoType, LPCTSTR szField, INT nDummy);
	VOID* InitOneCofCProto(OUT LPVOID nProtoType, LPCTSTR szField, INT nDummy);
	VOID* InitOneCofCSPProto(OUT LPVOID nProtoType, LPCTSTR szField, INT nDummy);

	//--------------------------------------------------------------------------------------------
	// 初始化个性动作
	//--------------------------------------------------------------------------------------------
	VOID* InitOneMotionProto(OUT LPVOID nProtoType, LPCTSTR szField, INT nDummy);

private:
	//-------------------------------------------------------------------------------------------
	// 加载人物升级相关影响静态数据
	//-------------------------------------------------------------------------------------------
	VOID InitRoleAttLevelUp(OUT LPVOID pArray, IN LPCTSTR szField, INT32 nIndex);

	//-------------------------------------------------------------------------------------------
	// 加载人物怪物属性的默认值，最小值和最大值
	//-------------------------------------------------------------------------------------------
	VOID InitAttDefMinMax(OUT LPVOID pArray, IN LPCTSTR szField, INT32 nIndex);

	//-------------------------------------------------------------------------------------------
	// 加载品级装备属性参数
	//-------------------------------------------------------------------------------------------
	VOID InitEquipQltyEffect(OUT LPVOID pArray, IN LPCTSTR szField, INT32 nIndex);

	//-------------------------------------------------------------------------------------------
	// 加载时装生成参数
	//-------------------------------------------------------------------------------------------
	VOID InitFashionQltyEffect(OUT LPVOID pArray, IN LPCTSTR szField, INT32 nIndex);

	//-------------------------------------------------------------------------------------------
	// 加载时装生成时颜色概率
	//-------------------------------------------------------------------------------------------
	VOID InitFashionColorPct(OUT LPVOID pArray, IN LPCTSTR szField, INT32 nIndex);

	//-------------------------------------------------------------------------------------------
	// 可铭纹装备部位的一条记录
	//-------------------------------------------------------------------------------------------
	VOID InitOnePosyPosProto(OUT LPVOID pArray, IN LPCTSTR szField, INT32 nIndex);

	//-------------------------------------------------------------------------------------------
	// 称号的一条记录
	//-------------------------------------------------------------------------------------------
	VOID InitOneTitleProto(OUT LPVOID pArray, IN LPCTSTR szField, INT32 nIndex);

	//-------------------------------------------------------------------------------------------
	// 氏族珍宝的一条记录
	//-------------------------------------------------------------------------------------------
	VOID InitOneClanTreasureProto(OUT LPVOID pArray, IN LPCTSTR szField, INT32 nIndex);

	//-------------------------------------------------------------------------------------------
	// 初始化商城免费物品(仅有一个)
	//-------------------------------------------------------------------------------------------
	VOID InitOneMallFreeItemProto(OUT LPVOID pArray, IN LPCTSTR szField, INT32 nIndex);

	//-------------------------------------------------------------------------------------------
	// 初始化帮派某个职位权限
	//-------------------------------------------------------------------------------------------
	VOID InitOnePosGuildPower(OUT LPVOID pArray, IN LPCTSTR szField, INT32 nIndex);
	VOID InitOnePosGuildKick(OUT LPVOID pArray, IN LPCTSTR szField, INT32 nIndex);
	VOID InitOnePosGuildAppoint(OUT LPVOID pArray, IN LPCTSTR szField, INT32 nIndex);

	//-------------------------------------------------------------------------------------------
	// 初始化VIP摊位
	//-------------------------------------------------------------------------------------------
	VOID InitOneVIPStallProto(OUT LPVOID pArray, IN LPCTSTR szField, INT32 nIndex);

private:
	//-------------------------------------------------------------------------------------------
	// 加载影响某一技能，某一状态，某一触发器的所有技能列表
	//-------------------------------------------------------------------------------------------
	VOID LoadModifyMap();

	//-------------------------------------------------------------------------------------------
	// 怪物AI分组
	//-------------------------------------------------------------------------------------------
	VOID GroupCreatureAI();

	//-------------------------------------------------------------------------------------------
	// 初始化龙魂能力辅助数组
	//-------------------------------------------------------------------------------------------
	VOID InitLongHunAss();
	VOID InitLongHunCanActivePos();

private:
	TSFPTrunk<AttRes>		m_Trunk;
	TObjRef<Util>			m_pUtil;
	TObjRef<VarContainer>	m_pVar;
	Fun_p					m_pFun;

	std::vector<tstring>				m_vectNameFilter;
	//std::vector<tstring>				m_vectChatFilter;

	tagVariableLen						m_VarLen;
	
	TMap<DWORD, tagItemProto*>			m_mapItemProto;
	TMap<DWORD, tagEquipProto*>			m_mapEquipProto;

	TMap<DWORD, tagGemProto*>			m_mapGemProto;
	TMap<DWORD, tagEquipQltyPct*>		m_mapEquipQltyPct;

	TMap<DWORD, tagLongHunProto*>		m_mapLongHunProto;
	std::vector<DWORD>					m_vectLongHun[X_LONGHUN_TYPE_NUM][X_LONGHUN_TYPEEX_NUM][X_EQUIP_QUALITY_NUM]; // 随龙魂能力辅助结构

	TMap<DWORD, tagShopProto*>			m_mapShopProto;
	TMap<DWORD, tagDakProto*>			m_mapDakProto;

	TMap<DWORD, tagMallItemProto*>		m_mapMallItemProto;
	TMap<DWORD, tagMallPackProto*>		m_mapMallPackProto;
	tagMallFreeItem						m_MallFreeItemProto;

	TMap<DWORD, tagSuitProto*>			m_mapSuitProto;

    TMap<DWORD, tagSkillProto*>			m_mapSkillProto;
	TMap<DWORD, tagBuffProto*>			m_mapBuffProto;
	TMap<DWORD, tagTriggerProto*>		m_mapTriggerProto;

	TMap<DWORD, tagSkillModify*>		m_mapSkillModify;

	TMap<DWORD, tagCreatureProto*>		m_mapCreatureProto;
	TMap<DWORD, tagCreatureAI*>			m_mapCreatureAI;
	TMap<DWORD, TList<DWORD>*>			m_mapCreatureAIGroup;

	// Jason 2009-12-7 神龙赐福配置文件信息
	TMap<int,	tagConfigLessingOfLoong*>	m_mapConfigInfoOfLessingLoong;

	tagLevelUpEffect					m_LevelUpEffect[MAX_ROLE_LEVEL+1];		// 下标和等级对应
	tagAttDefMinMax						m_AttDefMinMax[ERA_End];
	tagEquipQltyEffect					m_EquipQltyEffect[X_EQUIP_QUALITY_NUM];
	tagFashionGen						m_FashionGen[X_EQUIP_QUALITY_NUM];
	tagFashionColorPct					m_FashionColorPct[X_EQUIP_QUALITY_NUM];

	// 默认帮派成员权限表
	tagGuildPower						m_GuildPowerDefault[X_GUILD_POS_NUM];
	tagGuildKick						m_GuildKickDefault[X_GUILD_POS_NUM];
	tagGuildAppoint						m_GuildAppointDefault[X_GUILD_POS_NUM];

	// 帮派设施升级所需物品
	TMap<DWORD, tagGuildUpgradeNeed*>	m_GuildUpgradeNeedInfo;

	// 帮派事务
	TMap<DWORD, tagGuildAffairInfo*>	m_GuildAffairInfo;

	// 帮派技能
	TMap<DWORD, tagGuildSkill*>			m_GuildSkillInfo;

	// 帮派跑商
	TMap<DWORD, tagCommerceInfo*>		m_GuildCommerceInfo;
	TMap<DWORD, tagCommodityProto*>		m_GuildCommodityProto;
	TMap<INT64, tagCofCProto*>			m_CofCProto;
	TMap<DWORD, tagCofCSPProto*>		m_CofCSPProto;

	// 可铭纹装备部位 
	tagConsolidatePos					m_PosyPos[EPosyAtt_End][MAX_CONSOLIDATE_POS_QUANTITY];
	TMap<DWORD, tagPosyProtoSer*>		m_mapPosyProto;			// 铭纹静态表
	// 可镌刻装备部位
	tagConsolidatePos					m_EngravePos[EEngraveAtt_End][MAX_CONSOLIDATE_POS_QUANTITY];
	TMap<DWORD, tagEngraveProtoSer*>	m_mapEngraveProto;		// 镌刻静态表
	// 镶嵌,烙印,龙附静态表
	TMap<DWORD, tagConsolidateItem*>	m_mapConsolidateProto;
	// 合成
	TMap<DWORD, tagProduceProtoSer*>	m_mapProduceProto;
	// 淬火
	TMap<DWORD, tagQuenchProtoSer*>		m_mapQuenchProto;
	// 点化,装备分解
	TMap<DWORD, tagDeComposeProtoSer*>	m_mapDeComposeProto;
	// 称号
	tagTitleProto						m_TitleProto[MAX_TITLE_NUM];
	// 氏族珍宝
	tagClanTreasureProto				m_ClanTreasureProto[CLAN_TREASURE_NUM];
	// 副本随机刷怪点
	TMap<DWORD, tagRandSpawnPointInfo*> m_mapSpawnPointProto;
	// 副本静态属性
	TMap<DWORD, tagInstance*>			m_mapInstanceProto;
	// 副本中不能使用的物品
	TMap<DWORD, tagInstanceItem*>		m_mapInstanceItem;
	// 副本中不能使用的技能
	TMap<DWORD, tagInstanceSkill*>		m_mapInstanceSkill;
	// 副本随机刷怪点等级映射表
	TMap<INT, tagLevelMapping*>			m_mapLevelMapping;
	// 非副本刷怪点
	TMap<INT, tagSSpawnPointProto*>		m_mapSSpawnPoint;

	// 宠物原型
	TMap<DWORD, tagPetProto*>			m_mapPetProto;

	// 宠物升级原型
	TMap<DWORD, tagPetLvlUpProto*>		m_mapPetLvlUpProto;

	// 宠物升级原型
	TMap<DWORD, tagPetLvlUpItemProto*>	m_mapPetLvlUpItemProto;

	// 宠物技能原型
	TMap<DWORD, tagPetSkillProto*>		m_mapPetSkillProto;
	
	// 宠物等级对应固定技能id
	std::list<DWORD>					m_PetLevelSkillVec[NUM_PET_VLEVEL];

	// 宠物装备原型
	TMap<DWORD, tagPetEquipProto*>		m_mapPetEquipProto;

	// 宠物采集表
	TMap<DWORD, tagPetGatherProto*>		m_mapPetGatherProto;

	// 宠物五行凝结表
	TMap<DWORD, tagPetWuXingProto*>		m_mapPetWuXingProto;

	TMap<DWORD, tagVNBGiftProto*>		m_mapVNBGiftProto;
	TMap<DWORD, tagVNBEquipProto*>		m_mapVNBEquipProto;

	// 宠物属性值
//	tagPetDefMinMax						m_nPetAttDefMinMax[EPA_NUM];

	// VIP摊位租金(下标与ID对应)
	INT32								m_nVIPStallRent[VIP_STALL_MAX_NUM];

	TMap<DWORD, tagMotionProto*>		m_mapMotionProto;
public:
	// GM命令调用
	TMap<DWORD, tagEquipProto*>			GetEquipMap(){return m_mapEquipProto;}	


	// Jason 2009-12-7 神龙赐福配置文件信息
	tagConfigLessingOfLoong* GetConfigInfoOfLessingLoong(const int & ndx);
private:
	VOID* AttRes::InitLessingLoong(OUT LPVOID nProtoType, LPCTSTR szField, INT nDummy);
	TMap<INT64, tagPhilosophersStoneConfig*>		m_mapPhilosophersStoneConfig;
	VOID* InitPhilosophersStoneConfig(OUT LPVOID nProtoType, LPCTSTR szField, INT nDummy);
public:
	tagPhilosophersStoneConfig* GetConfigInfoOfPhilosophersStone(INT nStoneType,DWORD srcItemTypeID);
};

extern AttRes g_attRes;
