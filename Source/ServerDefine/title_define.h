//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: title_define.h
// author: 
// actor:
// data: 2009-01-20
// last:
// brief: 服务器用称号相关结构
//-----------------------------------------------------------------------------

#pragma once
#pragma pack(push, 1)



//-----------------------------------------------------------------------------
// 称号数目
//-----------------------------------------------------------------------------
const UINT16 MAX_TITLE_NUM			= 80;

//-----------------------------------------------------------------------------
// 称号ID是否有效
//-----------------------------------------------------------------------------
#define TITLEID_VALID( id ) ((id) >= 0 && (id) < MAX_TITLE_NUM)

//-----------------------------------------------------------------------------
// 触发事件
//-----------------------------------------------------------------------------
enum EnumTitleEvent
{	
	ETE_NULL						=-1,
	ETE_BEGIN						=0 ,	

	ETE_KILL_MONSTER				=0 ,	// 击杀怪物
	ETE_KILL_NPC					=1 ,	// 击杀NPC
	ETE_KILL_BOSS					=2 ,	// 击杀Boss
	ETE_KILL_ROLE					=3 ,	// 击杀角色
	ETE_QUEST_COMPLETE				=4 ,	// 完成任务
	ETE_QUEST_FAILED				=5 ,	// 任务失败 *
	ETE_ROLE_DIE					=6 ,	// 角色死亡 *
	ETE_ROLE_KILLEDBYROLE			=7 ,	// 角色被其他角色击杀 *
	ETE_COMPOSITE_EQUIP_SUCCESS		=8 ,	// 合成装备成功 *
	ETE_COMPOSITE_ITEM_SUCCESS		=9 ,	// 合成物品成功 *
	ETE_STRENGTHEN_EQUIP_SUCCESS	=10,	// 强化装备成功 *
	ETE_STRENGTHEN_EQUIP_FAILED		=11,	// 强化装备失败 *
	ETE_STRENGTHEN_EQUIP_PERFECT	=12,	// 强化装备完美 *
	ETE_IDENTIFY_BLUE_EQUIP			=13,	// 鉴定出蓝色或蓝色以上品级
	ETE_STALL						=14,	// 摆摊 *
	ETE_GATHER						=15,	// 采集、收获
	ETE_SHOUT						=16,	// 使用传音
	ETE_ROLE_TRANSACTION_SUCCESS	=17,	// 与角色成功交易
	ETE_CREATE_FACTION				=18,	// 建立帮派 *
	ETE_BE_CASTLLAN					=19,	// 成为城主 *
	ETE_FRIEND_MAKE					=20,	// 加为好友
	ETE_MARRY_JOIN					=21,	// 结婚友好度
	ETE_MARRY_SEPARATION			=22,	// 离婚 * todo
	ETE_USE_ITEM					=23,	// 角色使用道具
	ETE_USE_SKILL					=24,	// 角色使用技能
	ETE_JOIN_FAMEHALL				=25,	// 角色进入某个名人堂 *
	ETE_ROLE_LEVEL					=26,	// 角色达到级别
	ETE_REPUTE_CHANGE				=27,	// 声望变化 *
	ETE_FRIEND_VALUE				=28,	// 友好度变化

	ETE_COMPOSITE_SKILL_SUCCESS		= 29,	// 合成成功
	ETE_STRENGTHEN_SKILL_SUCCESS	= 30,	// 强化技能
	ETE_TRIGGER						= 31,	// 触发器
	ETE_SKILL_LEVEL					= 32,	// 技能升级
	ETE_ROLE_ATT					= 33,	// 角色属性值
	ETE_BAG_ITEM					= 34,	// 角色行囊装入物品 
	ETE_SCRIPT						= 35,	// 脚本获得
	ETE_MAPTRIGGER					= 36,	// 场景触发器 * todo
	ETE_CLEARPOINT					= 37,	// 洗点 * todo
	ETE_CLEARTALENT					= 38,	// 洗天资 * todo
	ETE_ONLINETIME					= 39,	// 上限累计时间 * todo
	ETE_CLOTHCOLOR					= 40,	// 时装染色 * todo
	ETE_MARRY						= 41,	// 结婚 * todo

	ETE_MAX_EVENTNUM				,	// 事件数目
	ETE_END								=ETE_MAX_EVENTNUM,
};

#define ETE_VALID( ete ) ( (ete) >= ETE_BEGIN && (ete) < ETE_END )
//-----------------------------------------------------------------------------
// 称号条件类型
//-----------------------------------------------------------------------------
enum EnumConditionType
{
	ECT_COUNT						= 0,	// 计数条件
	ECT_VALUE						= 1,	// 阈值条件
	ECT_CHECK						= 2,	// 特殊检查条件
};

//-----------------------------------------------------------------------------
// 条件检查返回值
//-----------------------------------------------------------------------------
enum EnumCheckResult
{
	ECR_ACTIVE						= 0,	// 激活
	ECR_COUNTDOWN					= 1,	// 减少
	ECR_NOAFECT						= 2,	// 无影响
};

struct tagTitleProto;


//-----------------------------------------------------------------------------
// 称号条件基类
//-----------------------------------------------------------------------------
class Condition
{
public:
	Condition(UINT16 u16TitleID, DWORD dwPara1, DWORD dwPara2)
		:m_u16TitleID(u16TitleID), m_dwPara1(dwPara1), m_dwPara2(dwPara2){}
	const tagTitleProto* GetTitleProto();
	bool IsCountCond() const;
	EnumCheckResult Check(DWORD dwArg1, DWORD dwArg2) {	return DoCheck(dwArg1, dwArg2);	}
	void SetPara2(DWORD dwPara2)	{	m_dwPara2 = dwPara2; }
	DWORD GetPara2() const {return m_dwPara2;}
	DWORD GetPara1() const {return m_dwPara1;}
protected:
//	Role* GetRole() const {return m_pRole;}
private:
	virtual EnumCheckResult DoCheck(DWORD dwArg1, DWORD dwArg2) = 0;

protected:
	DWORD	m_dwPara1;
	DWORD	m_dwPara2;
private:
	UINT16	m_u16TitleID;
//	Role*	m_pRole;
};


//-----------------------------------------------------------------------------
// 计数条件
//-----------------------------------------------------------------------------
class CountCondition: public Condition
{
public:
	CountCondition(UINT16 u16TitleID, DWORD dwPara1, DWORD dwPara2)
		:Condition(u16TitleID, dwPara1, dwPara2){}

private:
	virtual EnumCheckResult DoCheck(DWORD dwSubType, DWORD dwArg2)
	{
		if ( GT_VALID(m_dwPara1) && dwSubType != m_dwPara1) return ECR_NOAFECT; 
		if ((--m_dwPara2) <= 0)
		{
			return ECR_ACTIVE;
		}
		else
		{
			return ECR_COUNTDOWN;
		}
	}
};

//-----------------------------------------------------------------------------
// 阈值条件
//-----------------------------------------------------------------------------
class ValueCondition: public Condition
{
public:
	ValueCondition(UINT16 u16TitleID, DWORD dwPara1, DWORD dwPara2)
		:Condition(u16TitleID, dwPara1, dwPara2){}
private:
	virtual EnumCheckResult DoCheck(DWORD dwSubType, DWORD dwValue)
	{
		if ( GT_VALID(m_dwPara1) && dwSubType != m_dwPara1) return ECR_NOAFECT; 
		if (dwValue >= m_dwPara2)
		{
			return ECR_ACTIVE;
		}
		else
		{
			return ECR_NOAFECT; 
		}
	}
};

//-----------------------------------------------------------------------------
// 复杂检测条件
//-----------------------------------------------------------------------------
class CheckCondition:public Condition
{
public:
	CheckCondition(UINT16 u16TitleID, DWORD dwPara1, DWORD dwPara2)
		:Condition(u16TitleID, dwPara1, dwPara2){}
private:
	virtual EnumCheckResult DoCheck(DWORD dwArg1, DWORD dwArg2)
	{
//		Role* pRole = GetRole();

		return ECR_NOAFECT;//pRole->CheckCondition();
	}
};

//-----------------------------------------------------------------------------
// 称号原型
//-----------------------------------------------------------------------------
struct tagTitleProto
{
	enum
	{
		EVENTSNUM	= 1,
	};

	UINT16				m_u16ID;
	DWORD				m_dwBuffID;

	EnumConditionType 	m_CondType;
	DWORD				m_dwPara1;
	DWORD				m_dwPara2;

	EnumTitleEvent		m_Events[EVENTSNUM];

	// 生成条件
	Condition* MakeCond(DWORD dwPara1, DWORD dwPara2) const 
	{
		dwPara1 = GT_VALID(dwPara1) ? dwPara1 : m_dwPara1;
		dwPara2 = GT_VALID(dwPara2) ? dwPara2 : m_dwPara2;
		switch (m_CondType)
		{
		case ECT_COUNT:
			return new CountCondition(m_u16ID, dwPara1, dwPara2);
			break;
		case ECT_VALUE:
			return new ValueCondition(m_u16ID, dwPara1, dwPara2);
			break;
		case ECT_CHECK:
			return new CheckCondition(m_u16ID, dwPara1, dwPara2);
			break;
		default:
			ASSERT(0);
			return NULL;
			break;
		}
	}
};

#pragma pack(pop)