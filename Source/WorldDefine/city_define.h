//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: city_define.h
// author: Sunnee
// actor:
// data: 2009-08-11
// last:
// brief: 城市系统定义
//-----------------------------------------------------------------------------
#pragma once
#include "msg_common_errorcode.h"

#pragma pack(push, 1)

//-----------------------------------------------------------------------------
// 宏
//-----------------------------------------------------------------------------
#define MCityOutput(base, prolificacy, level)		((base) * (1.0f + ((prolificacy) - 500) / 1000.0f) * (1 + (level) / 5))

#define CITY_DEVELOP_GOLD_CARD				3300003
#define CITY_DEVELOP_SILVER_CARD			3300002
#define CITY_DEVELOP_COPPER_CARD			3300001

//生产力提升
#define CITY_DEVELOP_GOLD_ADD				15
#define CITY_DEVELOP_SILVER_ADD				5
#define CITY_DEVELOP_COPPER_ADD				2

//-----------------------------------------------------------------------------
// 常量
//-----------------------------------------------------------------------------
const INT32 MAX_CITY_BUFF_SIZE				= 40 * 1000;		// 城市系统最大缓存大小

const INT32 MAX_CITY_PROLIFICACY			= 1000;				// 城市最大生产力
const INT32 CITY_PRODUCE_COST				= 50;				// 城市产出消耗生产力

const INT32 MAX_CITY_DEFENCE				= 10000;			// 城市最大防御度

const INT32 MAX_CITY_ITEM_OUTPUT			= 5;				// 最大物品产出种类

const INT32 MAX_CITY_ATT_VALUE				= 99999999;			// 城市最大物资数量
const INT32 MAX_CITY_TAXRATE				= 10;				// 城市最大税率
const INT32 MODIFY_CITY_TAXRATE_CYCLE		= 2 * 24 * 60 * 60;	// 税率变更周期(2天)

const INT32 MAX_CITY_EUDEMONTALLY			= 1000;				// 最大守护神契合度

const INT32 MAX_CITY_ATTACK_FORCE			= 5;				// 城战进攻联盟帮派数量上限
const INT32 MAX_CITY_DEFENCE_FORCE			= 4;				// 城战防御联盟帮派数量上限

const INT32 EXAMINE_CITY_ATT_COST			= 100000;			// 查看城市属性金钱花费

//-----------------------------------------------------------------------------
// 枚举
//-----------------------------------------------------------------------------
enum ECityBroadCastType
{
	ECBC_ChangeHolder		= 0,		//全服消息广播：“###（帮派名称）获得###（禁城名称）的统治权！”

	ECBC_ChangeTaxRate		= 1,		//设置成功后，系统全世界广播：“###（城市名称）的税率变更为###。”

	ECBC_ConfirmAttack		= 2,		//系统全服消息广播：“本次城战的各参与方已确定！请各位可在城战信息界面中查询！”



	ECBC_ReadyForAttack		= 3,		//被确认拥有城战攻击权的各个帮派，全帮派成员提示：“帮派获得了对###（禁城名称）的攻击权！请各位帮众积极备战！”

	ECBC_ReadyForDefence	= 4,		//被确认的帮派全帮派成员提示：“本帮已被同意参与###（禁城名称）的城战，将作为防御方协助防守，请各位成员积极备战”

	ECBC_AttackSignup		= 5,		//全体帮派提示：“帮派已对###（禁城名称）进行攻击报名！”

	ECBC_DefenceSignup		= 6,		//全体帮派提示：“帮派已对###（禁城名称）进行协助防御报名！”

	ECBC_UpgradeProlificacy	= 7,		//同时全帮派成员消息：“###（角色名称）主动交纳####（道具名称），###（城市名称）生产力提升###点。”

	ECBC_RequestSignup		= 8,		//###（帮主名称）帮主决定申请管理###（城市名称），统治权由全体帮派成员对应该城市氏族类型的氏族声望之和决定，结果将于周六0点进行计算！。
};


// 错误码
enum
{
	E_City_Success						= E_Success,		// 成功

	E_City_Not_BeOccupied				= 1,		// 城市没有被占领
	E_City_Guild_NotMatch				= 2,		// 玩家帮派信息不符
	E_City_Item_NotEnough				= 3,		// 玩家物品不符合要求
	E_City_Power_NotEnough				= 4,		// 玩家权限不足
	E_City_Time_Limit					= 5,		// 时间限制
	E_City_Taxation_NotEnough			= 6,		// 税金不足
	E_City_Prolificacy_LackItem			= 7,		// 缺少物品

	E_City_SignUp_ForceError			= 11,		// 报名阵营错误
	E_City_SignUp_LevelLimit			= 12,		// 报名等级限制
	E_City_SignUp_HolderConflict		= 13,		// 统治帮派不能报名
	E_City_SignUp_InsideCityConflict	= 14,		// 城市内冲突
	E_City_SignUp_InterCityConflict		= 15,		// 城市间冲突

	E_City_SignUp_Fund_NotEnough		= 16,		// 报名资金不足
	E_City_SignUp_Hold_CityMax			= 17,		// 占领城市数量达到上限
	E_City_SignUp_Force_Full			= 18,		// 城战势力中帮派数量已满
	E_City_SignUp_Already_Confirm		= 19,		// 报名已经得到承认
	E_City_SignUp_Info_Error			= 20,		// 报名信息错误
	E_City_SignUp_Minfare_Limite		= 21,		// 报名费最低值限制
};

// 城市属性类型
enum ECityAttType
{
	ECAT_CityID,			// 城市ID
	ECAT_ClanType,			// 氏族类型
	ECAT_Holder,			// 统治帮派ID
	ECAT_Defence,			// 防御
	ECAT_EudemonTally,		// 守护神契合度
	ECAT_TaxRate,			// 税率
	ECAT_TaxRateTime,		// 税率更新时间
	ECAT_Taxation,			// 税金
	ECAT_Prolificacy,		// 生产力
	ECAT_FundOutput,		// 资金产出
	ECAT_MaterialOutput,	// 资材产出
	ECAT_ItemOutput,		// 物品产出

	ECAT_RequestSignUp,		// 申请管理报名
	ECAT_AttackSignUp,		// 攻方报名
	ECAT_DefenceSignUp,		// 守方报名

	ECAT_AttackSignUpNum,		// 攻方报名数
	ECAT_DefenceSignUpNum,		// 守方报名数
	ECAT_DefenceConfirmNum,		// 守方已经确认的数

	ECAT_AttackSignUpGuildID,	// 进攻方报名帮派id
	ECAT_DefenceSignUpGuildID,	// 防守方报名帮派id

	ECAT_ConfirmDefenceGuildID,	// 防守方被确认的帮派id


	ECAT_End
};

// 势力类型
enum ECityForceType
{
	ECFT_Attack,		// 攻击联盟
	ECFT_Defence,		// 防守联盟
	ECFT_Request,		// 首次申请

	ECFT_End
};

// 城门类型
enum ECityGateType
{
	ECGT_Broken,		// 残破木门
	ECGT_RoundLog,		// 圆木城门
	ECGT_Massive,		// 厚重木门
	ECGT_Copper,		// 铜皮木门
	ECGT_Megalith,		// 巨石城门
	ECGT_BlackIron,		// 黑铁城门
	ECGT_DarkIron,		// 玄铁城门
	ECGT_Eternity,		// 不朽城门

	ECGT_End
};

const INT32 CITY_GATE_DEFENCE[ECGT_End] = 
{
	0,		// 残破木门
	1000,	// 圆木城门
	3000,	// 厚重木门
	4000,	// 铜皮木门
	6000,	// 巨石城门
	7000,	// 黑铁城门
	8000,	// 玄铁城门
	9000,	// 不朽城门
};

// 生产力提升类型
enum EProlificacyUpgrade
{
	EPU_Copper,			// 城市发展铜令
	EPU_Silver,			// 城市发展银令
	EPU_Gold,			// 城市发展金令

	EPU_End
};

// 税金转化类型
enum ETaxTransformType
{
	ETTT_GuildFund,		// 帮派资金
	ETTT_PersonalSilver,// 个人金钱

	ETTT_End
};

// 城战状态
enum ECityStruggleState
{
	ECSGS_Start				=	0,

	ECSGS_UnStruggle		=	0,			// 非城战
	ECSGS_Struggle			=	1,			// 城战中

	ECSGS_End				=	ECSGS_Struggle,
};

//-----------------------------------------------------------------------------
// 结构
//-----------------------------------------------------------------------------

// 城市税率
struct tagCityTaxRate
{
	BYTE byCityID;
	BYTE byTaxRate;
};

// 城战报名结构
struct tagCitySignUp
{
	DWORD				dwGuildID;		// 报名帮派ID
	ECityForceType		eCityForceType;	// 报名参与势力
	INT32				nFare;			// 报名费(当联盟为防御时代表是否经过承认)
};

// 城市物品产出结构
struct tagCityItemOutput 
{
	DWORD				dwTypeID;		// 物品TypeID
	INT					nItemNum;		// 产出数量
	INT32				nLimit;			// 生产力限制
};
struct tagWarTime
{
	DWORD	WarWeekDay		: 3;	// 0 - 6    0为周日
	DWORD	BegHour			: 5;	// 0 - 23
	DWORD	BegMinutemin	: 6;	// 0 - 59
	DWORD	EndHour			: 5;	// 0 - 23
	DWORD	EndMinutemin	: 6;	// 0 - 59

	//--------------------------------------------------------------------------------
	// operator =
	//--------------------------------------------------------------------------------
	tagWarTime& operator = (const tagWarTime& time)
	{ 
		WarWeekDay=time.WarWeekDay;
		BegHour=time.BegHour; 
		BegMinutemin=time.BegMinutemin;
		EndHour=time.EndHour;
		EndMinutemin=time.EndMinutemin;
		return *this;
	}

	//--------------------------------------------------------------------------------
	// operator =
	//--------------------------------------------------------------------------------
	tagWarTime& operator = (DWORD dwTime)
	{
		return (*this = *(tagWarTime*)&dwTime);
	}
	//--------------------------------------------------------------------------------
	// copy constructor
	//--------------------------------------------------------------------------------
	tagWarTime(const tagWarTime& time)
	{
		*this = time;
	}

	//--------------------------------------------------------------------------------
	// constructor
	//--------------------------------------------------------------------------------
	tagWarTime()
	{
		memset(this,0,sizeof(*this));
	};
};

// 城市属性结构 -- Proto
struct tagCityProto 
{
	DWORD				dwCityID;						// 城市ID
	ECLanType			eClanType;						// 氏族类型
	DWORD				dwShellID;						// 卫城ID
	DWORD				dwKernelID;						// 禁城ID
	INT32				nFundOutput;					// 资金产出
	INT32				nMaterialOutput;				// 资材产出
	DWORD				dwMinSigupFare;					// 最低报名费用
	DWORD				dwDefaultDefence;				// 默认防守值
	DWORD				dwActivityID;					// 对应的活动id
	tagCityItemOutput	sItemOutput[MAX_CITY_ITEM_OUTPUT];	// 物品产出
};

// 城市属性结构 -- DB
struct tagCityAtt2DB
{
	DWORD				dwCityID;		// 城市ID
	DWORD				dwGuildID;		// 统治帮派ID
	INT32				nDefence;		// 防御度
	INT32				nEudemonTally;	// 守护神契合度
	INT32				nTaxRate;		// 税率
	DWORD				dwTaxRateTime;	// 最近一次更新税率的时间
	INT32				nTaxation;		// 税金
	INT32				nProlificacy;	// 生产力
};

// 城市属性结构 -- Client
struct tagCityAtt2Client : public tagCityAtt2DB
{
	ECityGateType		eCityGateType;		// 城门类型
	INT32				nFundOutputBase;	// 基础资金产出
	INT32				nFundOutputCur;		// 当前资金产出
	INT32				nMaterialOutputBase;// 基础资材产出
	INT32				nMaterialOutputCur;	// 当前资材产出
	INT					nItemOutputType;	// 物品产出种类
	tagCityItemOutput	sItemData[1];		// 物品数据
};

// 城市单一属性结构
struct tagCityAppointedAtt 
{
	ECityAttType		eType;
	INT					nDataNum;
	BYTE				byData[1];
};

//---------------------------------------------------------------------------------
// 角色城战复活属性结构
//---------------------------------------------------------------------------------
struct tagStruggleRevive
{
	ECityForceType		eForceType;		// 进攻或防御联盟
	INT					nDeadTick;		// 角色死亡时城战的tick
};

//-----------------------------------------------------------------------------
#pragma pack(pop)