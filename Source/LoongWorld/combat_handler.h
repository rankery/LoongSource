//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: combat_handler.h
// author: Aslan
// actor:
// data: 2008-09-25
// last:
// brief: 战斗系统管理器
//-----------------------------------------------------------------------------
#pragma once

class Unit;

class CombatHandler
{
public:
	enum ETargetEffectFlag
	{
		ETEF_Hited		=	0x0001,		// 命中
		ETEF_Block		=	0x0002,		// 格挡
		ETEF_Crited		=	0x0004,		// 暴击
	};

	enum EInterruptType
	{
		EIT_Null		=	-1,			// 无
		EIT_Move		=	0,			// 移动打断
		EIT_Skill		=	1,			// 技能
	};

public:
	//----------------------------------------------------------------------------
	// Constructor
	//----------------------------------------------------------------------------
	CombatHandler();

	//----------------------------------------------------------------------------
	// 初始化、开始，更新和结束
	//----------------------------------------------------------------------------
	VOID	Init(Unit* pOwner);
	INT		UseSkill(DWORD dwSkillID, DWORD dwTargetUnitID, DWORD dwSerial);
	INT		UseItem(INT64 n64ItemID, DWORD dwTargetUnitID, DWORD dwSerial, DWORD &dwTypeID, bool& bImmediate);
	VOID	Update();
	VOID	End();

	//----------------------------------------------------------------------------
	// 取消技能使用
	//----------------------------------------------------------------------------
	VOID	CancelSkillUse(DWORD dwSkillID);

	//----------------------------------------------------------------------------
	// 取消物品使用
	//----------------------------------------------------------------------------
	VOID	CancelItemUse(INT64 n64ItemSerial);

	//----------------------------------------------------------------------------
	// 打断起手
	//----------------------------------------------------------------------------
	BOOL	InterruptPrepare(EInterruptType eType, BOOL bOrdinary, BOOL bForce=FALSE);

	//----------------------------------------------------------------------------
	// 打断释放
	//----------------------------------------------------------------------------
	BOOL	InterruptOperate(EInterruptType eType, DWORD dwMisc, BOOL bForce=FALSE);

	//----------------------------------------------------------------------------
	// 各种mod
	//----------------------------------------------------------------------------
	VOID	ModSkillPrepareModPct(INT nModPct);
	VOID	ModTargetArmorLeftPct(INT nModPct);

	//----------------------------------------------------------------------------
	// 各种Get
	//----------------------------------------------------------------------------
	BOOL	IsValid()					{ return IsUseSkill() || IsUseItem(); }
	BOOL	IsUseSkill()				{ return P_VALID(m_dwSkillID); }
	BOOL	IsUseItem()					{ return P_VALID(m_n64ItemID); }
	BOOL	IsPreparing()				{ return IsSkillPreparing() || IsItemPreparing(); }
	BOOL	IsOperating()				{ return IsSkillOperating() || IsItemOperating(); }
	INT		GetSkillPrepareCountDown()	{ return m_nSkillPrepareCountDown; }
	INT		GetItemPrepareCountDown()	{ return m_nItemPrepareCountDown; }
	DWORD	GetTargetUnitID()			{ return m_dwTargetUnitID; }
	DWORD	GetSkillID()				{ return m_dwSkillID; }
	FLOAT	GetTargetArmor(Unit &target);

	//----------------------------------------------------------------------------
	// 技能使用判断
	//----------------------------------------------------------------------------
	INT		CanCastSkill(Skill* pSkill, DWORD dwTargetUnitID);

	INT		CheckSkillAbility(Skill* pSkill);							// 测试技能能力
	INT		CheckOwnerLimitSkill();											// 测试技能所有者限制
	INT		CheckSkillLimit(Skill* pSkill);								// 测试技能本身限制
	INT		CheckTargetLimit(Skill* pSkill, DWORD dwTargetUnitID);		// 测试目标限制
	INT		CheckCostLimit(Skill* pSkill);								// 测试消耗限制
	INT		CheckVocationLimit(Skill* pSkill);							// 测试职业限制
	INT		CheckTargetLogicLimit(Skill* pSkill, Unit* pTarget);		// 检测技能和目标的
	INT		CheckMapLimit(Skill* pSkill);								// 检测地图限制

	BOOL	CheckSkillConflict(Skill* pSkill);							// 检查技能使用的冲突

	//----------------------------------------------------------------------------
	// 计算技能效果
	//----------------------------------------------------------------------------
	DWORD	CalculateSkillEffect(Skill* pSkill, Unit* pTarget);
	VOID	CalSkillTargetList();
	BOOL	CalculateHit(Skill* pSkill, Unit* pTarget);
	BOOL	CalculateBlock(Skill* pSkill, Unit* pTarget);
	BOOL	CalculateCritRate(Skill* pSkill, Unit* pTarget);
	FLOAT	CalculateCritAmount(Skill* pSkill, Unit* pTarget);	
	VOID	CalculateDmg(Skill* pSkill, Unit* pTarget);
	VOID	CalculateCost(Skill* pSkill);

	//----------------------------------------------------------------------------
	// 物品使用判断
	//----------------------------------------------------------------------------
	INT		CanUseItem(tagItem* pItem);
	INT		CheckItemAbility(tagItem* pItem);							// 检测物品本身
	INT		CheckOwnerLimitItem();										// 检测自身
	INT		CheckRoleProtoLimit(tagItem* pItem);						// 检测人物属性限制
	INT		CheckRoleStateLimit(tagItem* pItem);						// 检测人物状态限制
	INT		CheckRoleVocationLimit(tagItem* pItem);						// 检测人物职业限制
	INT		CheckMapLimit(tagItem* pItem);								// 检测地图限制
	BOOL	CheckItemConflict(tagItem* pItem);

	//-----------------------------------------------------------------------------
	// 计算物品效果
	//-----------------------------------------------------------------------------
	VOID	CalUseItemTargetList();
	
private:
	//-----------------------------------------------------------------------------
	// 各种Get
	//-----------------------------------------------------------------------------
	bool	IsSkillPreparing()			{ return m_bSkillPreparing; }
	bool	IsSkillOperating()			{ return m_bSkillOperating; }
	bool	IsItemPreparing()			{ return m_bItemPreparing; }
	bool	IsItemOperating()			{ return m_bItemOperating; }

	//-----------------------------------------------------------------------------
	// Mod底层调用函数
	//-----------------------------------------------------------------------------
	VOID	ModPct(IN OUT FLOAT &fDstPct, IN INT nModPct);

	//-----------------------------------------------------------------------------
	// 更新操作
	//-----------------------------------------------------------------------------
	VOID	UpdateSkillPrepare();
	VOID	UpdateSkillOperate();
	VOID	UpdateItemPrepare();
	VOID	UpdateItemOperate();

	//-----------------------------------------------------------------------------
	// 结束操作
	//-----------------------------------------------------------------------------
	VOID	EndUseSkill();
	VOID	EndUseItem();

	//----------------------------------------------------------------------------
	// 伤害公式的系数计算
	//----------------------------------------------------------------------------
	FLOAT	CalBaseDmg(Skill* pSkill);
	FLOAT	CalAttackDefenceCoef(Skill* pSkill, Unit* pTarget);
	FLOAT	CalMoraleCoef(Unit* pTarget);
	FLOAT	CalDerateCoef(Skill* pSkill, Unit* pTarget);
	FLOAT	CalInjuryCoef();
	FLOAT	CalLevelCoef(Skill* pSkill, Unit* pTarget);

private:
	Unit*			m_pOwner;					// 发起者

	DWORD			m_dwSkillID;				// 发动的技能ID
	INT64			m_n64ItemID;				// 使用物品64位ID
	DWORD			m_dwTargetUnitID;			// 技能发动的主目标ID
	DWORD			m_dwTargetEffectFlag;		// 主目标的作用效果
	DWORD			m_dwSkillSerial;			// 技能攻击序列号
	DWORD			m_dwItemSerial;				// 物品使用序列号
	DWORD			m_dwTargetUnitIDItem;		// 使用物品的目标ID

	bool			m_bSkillPreparing;			// 技能是否在起手
	bool			m_bItemPreparing;			// 物品正在起手
	bool			m_bSkillOperating;			// 技能是否在发动
	bool			m_bItemOperating;			// 物品是否在发动

	INT				m_nSkillPrepareCountDown;	// 技能起手倒计时（毫秒）
	INT				m_nItemPrepareCountDown;	// 物品起手倒计时（毫秒）

	FLOAT			m_fSkillPrepareModPct;		// 技能起手时间影响百分比
	FLOAT			m_fTargetArmorLeftPct;		// 目标护甲削弱后剩余百分比（1.0f - 削弱百分比）

	INT				m_nSkillOperateTime;		// 技能操作的时间，用于间隔计算伤害（毫秒）
	INT				m_nSkillCurDmgIndex;		// 当前要计算第几次伤害

	TList<DWORD>	m_listTargetID;				// 技能目标列表
	TList<DWORD>	m_listHitedTarget;			// 技能命中的目标
	TList<DWORD>	m_listDodgedTarget;			// 技能闪避的目标
	TList<DWORD>	m_listBlockedTarget;		// 技能格挡的目标
	TList<DWORD>	m_listCritedTarget;			// 技能暴击的目标
};

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
inline CombatHandler::CombatHandler()
: m_pOwner(NULL), m_dwSkillID(GT_INVALID), m_n64ItemID(GT_INVALID), m_dwTargetUnitID(GT_INVALID),
  m_dwTargetEffectFlag(0), m_dwSkillSerial(0), m_dwItemSerial(0), m_bSkillPreparing(false), m_bItemPreparing(false),
  m_bSkillOperating(false), m_bItemOperating(false), m_nSkillPrepareCountDown(0), m_nItemPrepareCountDown(0),
  m_fSkillPrepareModPct(1.0f), m_fTargetArmorLeftPct(1.0f), m_nSkillOperateTime(0), m_nSkillCurDmgIndex(0)
{
}

//-----------------------------------------------------------------------------
// 当前技能初始化
//-----------------------------------------------------------------------------
inline VOID CombatHandler::Init(Unit* pOwner)
{
	m_pOwner	=	pOwner;
}

//------------------------------------------------------------------------------------------
// 结束使用技能
//------------------------------------------------------------------------------------------
inline VOID CombatHandler::EndUseSkill()
{
	m_dwSkillID					=	GT_INVALID;
	m_dwTargetUnitID			=	GT_INVALID;
	m_dwTargetEffectFlag		=	0;
	m_dwSkillSerial				=	0;

	m_bSkillPreparing			=	false;
	m_bSkillOperating			=	false;

	m_nSkillPrepareCountDown	=	0;
	m_nSkillOperateTime			=	0;
	m_nSkillCurDmgIndex			=	0;
}

//-------------------------------------------------------------------------------------------
// 结束使用物品
//-------------------------------------------------------------------------------------------
inline VOID CombatHandler::EndUseItem()
{
	m_n64ItemID					=	GT_INVALID;
	m_dwItemSerial				=	0;
	m_bItemPreparing			=	false;
	m_bItemOperating			=	false;
	m_nItemPrepareCountDown		=	0;
}

//--------------------------------------------------------------------------------------------
// 结束战斗系统所有动作
//--------------------------------------------------------------------------------------------
inline VOID CombatHandler::End()
{
	EndUseSkill();
	EndUseItem();
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
inline VOID CombatHandler::Update()
{
	// 如果正在使用技能
	if( IsUseSkill() )
	{
		if( IsSkillPreparing() )		UpdateSkillPrepare();
		else if( IsSkillOperating() )	UpdateSkillOperate();
		else							EndUseSkill();
	}

	// 如果正在使用物品
	if( IsUseItem() )
	{
		if( IsItemPreparing() )			UpdateItemPrepare();
		else if( IsItemOperating() )	UpdateItemOperate();
		else							EndUseItem();
	}
}

//-----------------------------------------------------------------------------
// Mod底层调用函数
//-----------------------------------------------------------------------------
inline VOID CombatHandler::ModPct(IN OUT FLOAT &fDstPct, IN INT nModPct)
{
	fDstPct += (FLOAT)nModPct / 10000.0f;

	if(fDstPct < 0.0f)
	{
		// 当百分比变为负值时，无法进行反向操作
		ASSERT(fDstPct >= 0.0f);
		fDstPct = 0.0f;
	}
}

//----------------------------------------------------------------------------
// 设置起手时间影响百分比
//----------------------------------------------------------------------------
inline VOID CombatHandler::ModSkillPrepareModPct(INT nModPct)
{
	ModPct(m_fSkillPrepareModPct, nModPct);
}

//----------------------------------------------------------------------------
// 设置对目标护甲影响百分比
//----------------------------------------------------------------------------
inline VOID CombatHandler::ModTargetArmorLeftPct(INT nModPct)
{
	ModPct(m_fTargetArmorLeftPct, nModPct);
}
