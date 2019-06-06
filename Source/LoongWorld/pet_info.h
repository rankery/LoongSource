//-----------------------------------------------------------------------------
//!\file pet_info.h
//!\author xlguo
//!
//!\date 2009-04-03
//! last 
//!
//!\brief 宠物数据
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once
/*
//----------------------------------------------------------------------------
// 宠物技能状态
//----------------------------------------------------------------------------
enum EPetSkillStat
{
	EPSS_Using		= 0,
	EPSS_CanUse		= 1,
	EPSS_CanNotUse	= 2,
	EPSS_Passive	= 3,
};
const INT	MAX_SKILL_MOD_ATT	= 3;
//----------------------------------------------------------------------------
// 宠物技能原型
//----------------------------------------------------------------------------
struct tagPetSkillProto
{
	UINT16	u16PetSkillID;
	INT		AttIndexs[MAX_SKILL_MOD_ATT];
	INT		AttMods[MAX_SKILL_MOD_ATT];
	DWORD	dwScriptID;
};

//----------------------------------------------------------------------------
// 宠物技能
//----------------------------------------------------------------------------
class PetSkill
{
public:
	BOOL	SetOwner(Pet* pPet);

	EPetSkillStat	GetStatus();
	BOOL	CanCast();
	VOID	Cast();
	BOOL	Skill();

private:
	tagPetSkillProto*	pProto;
	PetSoul*			pPetSoul;
	DWORD				dwItemID;
};

//! 宠物整型数值属性枚举
enum EPetAttribute
{   
	EPA_Begin		= 0,
	EPA_Quality		= EPA_A_BEGIN,	//资质	
	EPA_Iq			,	//智商

	EPA_A_BEGIN		= EPA_Begin,
	EPA_Speed		,	//速度(1-999)
	EPA_Endurance	,	//耐力(1-999)
	EPA_Spirit		,	//灵力(1-999)
	EPA_A_END		,
	EPA_A_NUM		= EPA_A_END - EPA_A_BEGIN,

	EPA_B_BEGIN		= EPA_A_END,
	EPA_MaxSkillNum	,	//最大可学技能数
	EPA_CurSkillNum	,	//当前可学技能数
	EPA_MaxBagCap	,	//最大背包格子数量
	EPA_CurBagCap	,	//当前背包格子数量
	EPA_MaxMagic	,	//最大能力,宠物施放技能消耗
	EPA_CurMagic	,	//当前能力,宠物施放技能消耗
	EPA_RidingSpeed	,	//骑乘速度
	EPA_B_END		,
	EPA_B_NUM		= EPA_B_END - EPA_B_BEGIN,

	EPA_S_BEGIN		= EPA_B_END,
	EPA_Loyal		,	//忠诚度
	EPA_Hungry		,	//饥饿度
	EPA_S_END		,
	EPA_S_NUM		= EPA_S_END - EPA_S_BEGIN,

	EPA_End			= EPA_S_END,
	EPA_NUM			= EPA_End - EPA_Begin,
}

#define MTransEPAttA2Index(EPAAttA)		((EPAAttA) - EPA_A_BEGIN)
#define MTransIndex2EPAttA(Index)		((Index) + EPA_A_BEGIN)
*/

//----------------------------------------------------------------------------
// 宠物灵魂，宠物带中的宠物
//----------------------------------------------------------------------------
// class PetSoul
// {
// 	// 宠物属性
// private:
// 	Pet*	m_pEntity;		// 宠物实体
// 	Role*	m_pMasterRole;	// 主人
// 
// 	DWORD			m_dwNameID;	//宠物名字id
// 	INT				m_nLevel;	//当前等级(0-200)
// 	INT				m_nExp;		//当前经验2^37
// 	EPetGrowthStage	m_eStage;	//生长阶段
// 	EPetState		m_eState;	//宠物状态
// 
// 	INT		m_nPotentialPt;	//潜能点
// 	INT		m_PotentialPts[EPA_A_NUM];
// 
// 	INT		m_nAttBase[EPA_NUM];	// 基础（包括投点）
// 	INT		m_nAttAddOn[EPA_NUM];	// 装备加成
// 	INT		m_nAttCur[EPA_NUM];		// 
// 
// public:
// 	VOID	ModAttValue(INT32 nIndex)
// 	{
// 		// 若为召唤状态更新宠物实体
// 		if (!P_VALID(m_pEntity)) return;
// 	}
// 
// public:
// 	// 学习技能
// 	BOOL	LearnSkill(DWORD dwSkillID, INT64 n64ItemID);
// 	// 设置技能
// 	BOOL	SetSkill();
// 
// 	// 装备
// 	BOOL	MountEquip(PetEquip* pPetEquip);
// 	BOOL	UnLoadEquip(UINT16 u16PetEquipID);
// 
// public:
// 	// 发送所有技能概要消息
// 	VOID	SendAllPetSkillBrief();
// 	// 发送所有技能详细消息
// 	VOID	SendAllPetSkillDetail();
// 	// 发送所有装备概要消息
// 	VOID	SendAllPetEquipBrief();
// 	// 发送所有装备详细消息
// 	VOID	SendAllPetEquipDetail();
// 
// public:
// 	// 初始化宠物数据
// 	BOOL	Init(PVOID pData);
// 
// 	// 宠物自身更新（忠诚度，饥饿度，技能等等）
// 	VOID	Update()
// 	{
// 		// 更新所有技能
// 		// 召唤状态更新跟踪主人
// 	}
// };
