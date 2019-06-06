//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: pet_define.h
// author: 
// actor:
// data: 2009-05-07
// last:
// brief: 服务器用宠物相关结构
//-----------------------------------------------------------------------------
#pragma once
#include "../WorldDefine/pet_define.h"
#include "../WorldDefine/ItemDefine.h"
#pragma pack(push, 1)

const INT MAX_PETSOUL_NUM		= 5;	// 最多拥有宠物数量
const INT MAX_CALLED_PET_NUM	= 1;	// 最多召唤宠物数量

const INT SPIRIT_RESUME_COUNT	= 2 * TICK_PER_SECOND * 60;// 灵力回复计数


//! 服务器用宠物属性
// enum EPetAtt
// {
// 	//独立出来
// 	// 	EPA_CurExp
// 	// 	EPA_Level
// 	// 	EPA_MaxSkillNum
// 
// 	EPA_Null				= -1,
// 	EPA_Begin				= 0,
// 
// 	// 	一级属性
// 	// 	影响二级属性的属性
// 
// 	EPA_1_Begin				= 0,
// 
// 	EPA_Speed				= 0,
// 	EPA_Endurance			= 1,
// 	EPA_Spirit				= 2,
// 
// 	EPA_1_End				= 3,
// 
// 	// 	二级属性
// 	// 	被一级属性影响的属性
// 	EPA_2_Begin				= 3,
// 
// 	EPA_MountSpeedAdd		= 3,
// 	EPA_SavetimeAdd			= 4,
// 	EPA_GatherNumAdd		= 5,
// 	EPA_FindTreasurePct		= 6,
// 	EPA_Energy				= 7,	// SaveDB
// 	EPA_MaxEnergy			= 8,
// 	EPA_RoleAttAddPct		= 9,
// 
// 	EPA_2_End				= 10,
// 
// 	// 	固有属性
// 	// 	只可以加成的属性
// 	EPA_ExpTimes			= 10,
// 	EPA_IQ					= 11,	// SaveDB
// 	EPA_Quality				= 12,	// SaveDB
// 	EPA_BagGrid				= 13,
// 	EPA_Hungry				= 14,	// SaveDB
// 	EPA_RemainPoints		= 15,	// SaveDB
// 	EPA_MountSpeed			= 16,
// 
// 	EPA_End					,
// };

// 洗点：		除了EPA_Quality其他所有
// 品质影响：	EPA_Aptitude，EPA_Potential，EPA_SpiritMax
// 等级影响：	EPA_MountSpeed，EPA_SpiritMax，EPA_TalentCount



enum EPetAtt
{
	EPA_Null				=-1,
	EPA_Begin				= 0,

	EPA_Quality				= 0,	//品质	1~5品，决定灵兽的资质、天赋技能数量、天赋计数	天赋技能共有五种，用于额外产出收益，促进灵力消耗
	EPA_Aptitude			= 1,	//资质	决定灵兽的灵力成长，决定初始潜能	取值1~100，具体取决于品质
	EPA_Spirit				= 2,	//灵力	随着等级的成长而成长，使用技能时需要消耗该值；休息时可自动恢复	分当前灵力值、最大灵力值：
	EPA_TalentCount			= 3,	//天赋计数	
	EPA_Potential			= 4,	//潜能	学习技能时消耗该值，类似装备潜力值	初始潜能=资质×10
	EPA_WuXingEnergy		= 5,	//五行力	当灵兽使用技能时，可以储存该值，在施展天赋技能时消耗该值	技能对应的五行值在宠物技能表中填写
	EPA_MountNum			= 6,	//骑乘人数
	EPA_AddOn_Begin			= 7,

	EPA_SpiritMax			= 7,	//最大灵力值=500＋资质×（阶×10＋等）
	EPA_TalentCountMax		= 8,	//天赋计数最大值	用于当天的天赋技能使用次数计数，每天重置；当计数达到最大值时，不可使用天赋技能	品质白=1，品质黄=2，品质绿=3，品质蓝=4，品质橙=5 天赋计数=阶＋品质×3
	EPA_BagGrid				= 9,	//行囊格数	使用相关技能时，灵兽能运载物品的数量	默认4，装备加成2~16
	EPA_DeliveryConsume		=10,	//快递耗时	使用快递技能时，灵兽工作的时间缩放	默认0，装备加成-？~0：ms，对应技能表“工作时间”（降低5秒，填-5000）
	EPA_SellConsume			=11,	//贩卖耗时	使用贩卖技能时，灵兽工作的时间缩放	默认0，装备加成-？~0：ms，对应技能表“工作时间”
	EPA_StorageConsume		=12,	//存放耗时	使用存放技能时，灵兽工作的时间缩放	默认0，装备加成-？~0：ms，对应技能表“工作时间”
	EPA_GatherConsume		=13,	//采集耗时	使用采集技能时，灵兽工作时间缩放	默认0，装备加成-？~0：ms，对应技能表“工作时间”
	EPA_TrainResume			=14,	//训练恢复	灵兽的自动训练技能的恢复时间缩放	默认0，装备加成-？~0：ms，对应技能表“恢复时间”
	EPA_SpiritRate			=15,	//灵力消耗	(0-1 0-10000)使用技能时的灵力消耗缩放比	默认100%，装备加成-100%~0%，对应技能表energy_cost
	EPA_ExpRate				=16,	//经验加成	(0-1 0-10000)获得经验时的缩放比	默认100%，装备加成0%~400%
	EPA_PickUpResume		=17,	//拾取恢复	降低灵兽拾取的恢复时间	默认0，装备加成-20~-1
	EPA_MedicineSaving		=18,	//吃药节省	(0-1 0-10000)宠物每次喂药时可能不消耗该药品	默认0%，装备加成1%~5%
	EPA_StrengthEffect		=19,	//强身效果	(0-1 0-10000)提升强身类技能的效果	默认100%，装备加成0%~400%
	EPA_WuXingConsume		=20,	//五行力消耗	(0-1 0-10000)当使用天赋技能时，降低该技能对五行力的需求	默认100%，装备加成-50%~0%，对应技能表“五行力消耗”
	EPA_MountSpeed			=21,	//骑乘速度	(0-1 0-10000)灵兽被驾御时的移动速度	默认白30%，黄40%，绿50%，蓝60%，橙70% 装备加成0%~100%	受阶影响
	EPA_MountNumMax			=22,	//骑乘人数
	
	EPA_AddOn_End			=23,

	EPA_End					=23,
};

const INT EPA_NUM		= EPA_End - EPA_Begin;


#define IS_EPA(n)		( (n) >= EPA_Begin && (n) < EPA_End )
#define IS_EPA_ADDON(n)	( (n) >= EPA_AddOn_Begin && (n) < EPA_AddOn_End )

#define EPATOEPAAO(n)	( INT((n) - EPA_AddOn_Begin) )

// 宠物属性默认值
struct tagPetDefMinMax
{
	INT		nDef;
	INT		nMin;
	INT		nMax;
};

//-------------------------------------------------------------------------------------------------------
// 宠物灵魂创建数据
//-------------------------------------------------------------------------------------------------------
struct tagDBPetSoulCreate
{
	TCHAR	tszName[X_SHORT_NAME];	// 宠物名称
	INT		nQuality;				// 品质
	INT		nAptitude;				// 资质
	INT		nPotential;				// 当前潜能 
	INT		nSpirte;				// 当前灵力
//	BOOL	bMountable;				// 能否骑乘

	DWORD	dwProtoID;				// 原型ID
	DWORD	dwMasterID;				// 主人ID
	tagDBPetSoulCreate()	{ZeroMemory(this, sizeof(*this));}
};

//-------------------------------------------------------------------------------------------------------
// 宠物灵魂数据
//-------------------------------------------------------------------------------------------------------
struct tagDBPetAtt
{
	DWORD	dwPetID;				// 宠物ID
	TCHAR	tszName[X_SHORT_NAME];	// 宠物名称
	DWORD	dwProtoID;				// 原型ID
	DWORD	dwMasterID;				// 主人ID

	INT		nQuality;				// 品质
	INT		nAptitude;				// 资质
	INT		nPotential;				// 当前潜能 
	INT		nSpirit;				// 当前灵力
	INT		nWuXingEnergy;			// 五行力
	INT		nTalentCount;			// 天赋计数
	BYTE	byPetState;				// 灵兽状态

	INT		nExpCur;				// 当前经验
	INT		nStep;					// 阶
	INT		nGrade;					// 等
	
	BOOL	bLocked;				// 是否锁定

	tagDBPetAtt()	{ZeroMemory(this, sizeof(*this));}
};

//-------------------------------------------------------------------------------------------------------
// 宠物技能数据
//-------------------------------------------------------------------------------------------------------
struct tagDBPetSkill
{
	DWORD	dwTypeID;				// 原型id
	INT		nPara1;					// 冷却计时
	INT		nPara2;					// 使用计时
	tagDBPetSkill()	{ZeroMemory(this, sizeof(*this));}
};

//-------------------------------------------------------------------------------------------------------
// 宠物加载数据
//-------------------------------------------------------------------------------------------------------
struct tagDBPet
{
	tagDBPetAtt		petAtt;			// 宠物属性
	INT				nPetSkillNum;	// 宠物技能
	INT				nPetEquipNum;	// 宠物装备
	BYTE			byData[1];		// tagDBPetSkill and tagItem
	tagDBPet()	{ZeroMemory(this, sizeof(*this));}
	INT	GetSize()
	{
		if (0 == nPetSkillNum && 0 == nPetEquipNum)
			return sizeof(*this);
		else
			return sizeof(*this) - 1 + sizeof(tagDBPetSkill) * nPetSkillNum + sizeof(tagItem) * nPetEquipNum;
	}
};


#pragma pack(pop)