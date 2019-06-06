//-----------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: skill.h
// author: Aslan
// actor:
// data: 2008-9-10
// last:
// brief: 技能
//-----------------------------------------------------------------------------------------
#pragma once

#include "skill_buff.h"
#include "skill_trigger.h"

//-----------------------------------------------------------------------------------------
// 技能等级改变方式
//-----------------------------------------------------------------------------------------
enum ESkillLevelChange
{
	ESLC_Learn = 0,		// 投放
	ESLC_Self = 1,		// 固定
	ESLC_Temp = 2,		// 临时
};

//-----------------------------------------------------------------------------------------
// 其它技能对该技能的影响
//-----------------------------------------------------------------------------------------
struct tagSkillMod
{
	// 技能属性影响
	FLOAT			fOPDistMod;								// 作用距离加成
	FLOAT			fOPRadiusMod;							// 作用范围加成
	INT				nPrepareTimeMod;						// 起手时间加成（毫秒）
	INT				nCoolDownMod;							// 冷却时间加成
	INT				nEnmityMod;								// 技能仇恨加成
	INT				nEnmityModPct;							// 技能仇恨百分比加成
	INT				nHitMod;								// 技能命中率加成
	INT				nHitModPct;								// 技能命中率百分比加成
	INT				nCritMod;								// 技能致命率加成
	INT				nCritModPct;							// 技能致命率百分比加成

	// 伤害
	INT				nChannelDmgMod[MAX_CHANNEL_TIMES];		// 管道伤害每一次的伤害量加成
	INT				nChannelDmgModPct[MAX_CHANNEL_TIMES];	// 管道伤害每一次的伤害量的百分比加成

	// 状态
	DWORD			dwBuffTypeID[MAX_BUFF_PER_SKILL];		// 技能所带buff
	DWORD			dwTriggerID[MAX_BUFF_PER_SKILL];		// 技能所带的buff触发器

	// 消耗
	INT				nSkillCostMod[ESCT_End];				// 技能消耗加成
	INT				nSkillCostModPct[ESCT_End];				// 技能消耗百分比加成

	// 人物属性影响
	mutable TMap<ERoleAttribute, INT>	mapRoleAttMod;		// 其它技能对该技能造成的人物属性影响的加成
	mutable TMap<ERoleAttribute, INT>	mapRoleAttModPct;	// 其它技能对该技能造成的人物属性影响的百分比加成

	tagSkillMod()
	{
		fOPDistMod = 0.0f;
		fOPRadiusMod = 0.0f;
		nPrepareTimeMod = 0;
		nCoolDownMod = 0;
		nEnmityMod = 0;
		nEnmityModPct = 0;
		nHitMod = 0;
		nHitModPct = 0;
		nCritMod = 0;
		nCritModPct = 0;

		memset(dwBuffTypeID, 0xFF, sizeof(dwBuffTypeID));
		memset(dwTriggerID, 0xFF, sizeof(dwTriggerID));
		ZeroMemory(nChannelDmgMod, sizeof(nChannelDmgMod));
		ZeroMemory(nChannelDmgModPct, sizeof(nChannelDmgModPct));
		ZeroMemory(nSkillCostMod, sizeof(nSkillCostMod));
		ZeroMemory(nSkillCostModPct, sizeof(nSkillCostModPct));
	}
};

class SkillScript;

//--------------------------------------------------------------------------------------------
// 技能类
//--------------------------------------------------------------------------------------------
class Skill
{
public:
	//----------------------------------------------------------------------------------------
	// Constructor
	//----------------------------------------------------------------------------------------
	Skill();
	Skill(DWORD dwID, INT nSelfLevel, INT nLearnLevel, INT nCoolDown, INT nProficiency);
	~Skill()	{ SAFE_DEL(m_pMod); }

	//----------------------------------------------------------------------------------------
	// 一些通用函数
	//----------------------------------------------------------------------------------------
	static DWORD			GetIDFromTypeID(DWORD dwTypeID)		{ return dwTypeID / 100; }
	static INT				GetLevelFromTypeID(DWORD dwTypeID)	{ return dwTypeID % 100; }
	static DWORD			CreateTypeID(DWORD dwID, INT nLevel){ return dwID * 100 + nLevel; }

	//----------------------------------------------------------------------------------------
	// 初始化，更新，保存和消息
	//----------------------------------------------------------------------------------------
	BOOL					Init(DWORD dwID, INT nSelfLevel, INT nLearnLevel, INT nCoolDown, INT nProficiency);
	VOID					Update();
	VOID					InitSkillSave(OUT tagSkillSave* pSkillSave);
	VOID					GenerateMsgInfo(tagSkillMsgInfo* const pMsgInfo);
	BOOL					IsNeedSave()			{ return m_bNeedSave;}
	VOID					SetSaved()				{ m_bNeedSave = FALSE;}

	//----------------------------------------------------------------------------------------
	// 各种Get
	//----------------------------------------------------------------------------------------
	DWORD					GetID()					{ return m_dwID; }
	DWORD					GetTypeID()				{ return m_pProto->dwID; }
	INT						GetLevel()				{ return m_nLevel; }
	INT						GetLearnLevel()			{ return m_nLearnLevel; }
	INT						GetSelfLevel()			{ return m_nSelfLevel; }
	INT						GetTempAddLevel()		{ return m_nTempAddLevel; }
	INT						GetProficiency()		{ return m_nProficiency; }
	INT						GetMaxLevel()			{ return m_pProto->nMaxLevel; }
	INT						GetMaxLearnLevel()		{ return m_pProto->nMaxLearnLevel; }
	INT						GetCoolDownCountDown()	{ return m_nCoolDownCountDown; }
	ESkillType				GetType()				{ return m_pProto->eType; }
	ESkillTypeEx			GetTypeEx()				{ return (ESkillTypeEx)m_pProto->nType2; }
	ESkillTypeEx2			GetTypeEx2()			{ return (ESkillTypeEx2)m_pProto->nType3; }
	ESkillUseType			GetUseType()			{ return m_pProto->eUseType; }
	ESkillDmgType			GetDmgType()			{ return m_pProto->eDmgType; }
	ESkillDistType			GetDistType()			{ return m_pProto->eDistType; }
	ESkillTargetType		GetTargetType()			{ return m_pProto->eTargetType; }
	DWORD					GetTargetSkillID()		{ return m_pProto->dwTargetSkillID; }
	DWORD					GetTargetBuffID()		{ return m_pProto->dwTargetBuffID; }
	DWORD					GetTargetTriggerID()	{ return m_pProto->dwTargetTriggerID; }
	ESkillOPType			GetOPType()				{ return m_pProto->eOPType; }
	INT						GetDmgTimes()			{ return m_pProto->nDmgTimes; }
	ETalentType				GetTalentType()			{ return m_pProto->eTalentType; }
	INT						GetLevelUpExp()			{ return m_pProto->nLevelUpExp; }
	INT						GetLevelUpType()		{ return m_pProto->eLevelUpType; }

	INT						GetDmg(INT nIndex);
	INT						GetDmgTime(INT nIndex);
	FLOAT					GetOPDist();
	FLOAT					GetOPRadius();
	INT						GetPrepareTime();
	INT						GetCoolDown();
	FLOAT					GetHit();
	FLOAT					GetCrit();
	INT						GetCost(ESkillCostType eCostType);
	VOID					GetCostItem(DWORD& dwItemID, INT& nNum);
	DWORD					GetBuffTypeID(INT nIndex);
	DWORD					GetTriggerID(INT nIndex);
	INT						GetEnmity();
	const SkillScript*		GetSkillScript()		{ return m_pScript; }

	const tagSkillProto*	GetProto()				{ return m_pProto; }
	const tagSkillMod*		GetMod()				{ return m_pMod; }
	const Unit*				GetOwner()				{ return m_pOwner; }

	BOOL					IsAffected()			{ return P_VALID(m_pMod); }
	BOOL					IsActive()				{ return ESUT_Active == GetUseType(); }
	BOOL					IsPassive()				{ return ESUT_Passive == GetUseType(); }
	BOOL					IsOrdAttackSkill()		{ return ESDGT_Ordinary == GetDmgType(); }
	BOOL					IsExAttackSkill()		{ ESkillDmgType eType = GetDmgType(); return ( ESDGT_Bleeding == eType || ESDGT_Brunt == eType || ESDGT_Bang == eType || ESDGT_Ordinary == eType ); }
	BOOL					IsInAttackSkill()		{ ESkillDmgType eType = GetDmgType(); return ( ESDGT_Poison == eType || ESDGT_Thinker == eType || ESDGT_Injury == eType ); }
	BOOL					IsStuntSkill()			{ return ESDGT_Stunt == GetDmgType(); }
	BOOL					IsMelee()				{ return ESDT_Melee == GetDistType(); }
	BOOL					IsRanged()				{ return ESDT_Ranged == GetDistType(); }
	BOOL					IsInner()				{ return ESDT_Inner == GetDistType(); }
	BOOL					IsFriendly()			{ return m_pProto->bFriendly; }
	BOOL					IsHostile()				{ return m_pProto->bHostile; }
	BOOL					IsInDependent()			{ return m_pProto->bIndependent; }
	BOOL					IsMoveable()			{ return m_pProto->bMoveable; }
	BOOL					IsCanModAtt();

	//--------------------------------------------------------------------------------
	// 各种Set
	//--------------------------------------------------------------------------------
	VOID					SetOwner(Unit* pOwner)		{ m_pOwner = pOwner; }
	VOID					AddProficiency(INT nValue)	{ m_nProficiency += nValue; m_bNeedSave = TRUE;}
	VOID					SetProficiency(INT nValue)	{ m_nProficiency = nValue; m_bNeedSave = TRUE;}

	//--------------------------------------------------------------------------------
	// 技能自身加成
	//--------------------------------------------------------------------------------
	BOOL					SetMod(const tagSkillProto* pProto);
	BOOL					UnSetMod(const tagSkillProto* pProto);

	//--------------------------------------------------------------------------------
	// 技能属性对人物属性影响
	//--------------------------------------------------------------------------------
	VOID					SetOwnerMod();
	VOID					UnsetOwnerMod();

	//---------------------------------------------------------------------------------
	// 技能升级，降级
	//---------------------------------------------------------------------------------
	VOID					IncLevel(ESkillLevelChange eType, INT nInc=1);
	VOID					DecLevel(ESkillLevelChange eType, INT nDec=1);

	//--------------------------------------------------------------------------------
	// 冷却
	//--------------------------------------------------------------------------------
	VOID					StartCoolDown()		{ m_nCoolDownCountDown = GetCoolDown(); m_bNeedSave = TRUE;}
	VOID					ClearCoolDown()		{ m_nCoolDownCountDown = 0; m_bNeedSave = TRUE;}
	BOOL					CountDownCoolDown();	

private:
	//--------------------------------------------------------------------------
	// 增加某些技能buff
	//--------------------------------------------------------------------------
	VOID SetSkillBuff(const tagSkillProto* pProto);

	//--------------------------------------------------------------------------
	// 撤销某些技能buff
	//--------------------------------------------------------------------------
	VOID UnSetSkillBuff(const tagSkillProto* pProto);

	//--------------------------------------------------------------------------
	// 注册技能本身的触发器关联
	//--------------------------------------------------------------------------
	VOID RegisterTriggerEvent();

	//--------------------------------------------------------------------------
	// 反注册技能本身的触发器关联
	//--------------------------------------------------------------------------
	VOID UnRegisterTriggerEvent();


private:
	DWORD					m_dwID;						// 技能ID（大ID）
	INT						m_nLevel;					// 技能当前等级（永久等级+投放等级+临时等级）

	INT						m_nLearnLevel;				// 技能当前投放等级
	INT						m_nSelfLevel;				// 技能永久等级
	INT						m_nTempAddLevel;			// 临时加上的等级						
	INT						m_nProficiency;				// 技能熟练度

	INT						m_nCoolDownCountDown;		// 技能冷却

	const SkillScript*		m_pScript;					// 技能脚本
	const tagSkillProto*	m_pProto;					// 技能静态属性
	mutable tagSkillMod*	m_pMod;						// 技能属性影响（如果没有任何技能影响它，该值为NULL）
	Unit*					m_pOwner;					// 技能所有者

	BOOL					m_bNeedSave;				// 需要保存
};

//---------------------------------------------------------------------------------
// 构造函数
//---------------------------------------------------------------------------------
inline Skill::Skill() 
:	m_dwID(GT_INVALID), m_nLevel(GT_INVALID), m_nLearnLevel(GT_INVALID), 
	m_nSelfLevel(GT_INVALID), m_nTempAddLevel(GT_INVALID), m_nCoolDownCountDown(GT_INVALID),
	m_pProto(NULL), m_pMod(NULL), m_pOwner(NULL),m_nProficiency(GT_INVALID), m_pScript(NULL), m_bNeedSave(FALSE)
{
}

inline Skill::Skill(DWORD dwID, INT nSelfLevel, INT nLearnLevel, INT nCoolDown, INT nProficiency)
:	m_dwID(GT_INVALID), m_nLevel(GT_INVALID), m_nLearnLevel(GT_INVALID), 
	m_nSelfLevel(GT_INVALID), m_nTempAddLevel(GT_INVALID), m_nCoolDownCountDown(GT_INVALID),
	m_pProto(NULL), m_pMod(NULL), m_pOwner(NULL),m_nProficiency(GT_INVALID), m_pScript(NULL)
{
	Init(dwID, nSelfLevel, nLearnLevel, nCoolDown, nProficiency);
}

//---------------------------------------------------------------------------------
// 技能升级（mod不需要改变，但该技能原有buff要特殊处理一下）
//---------------------------------------------------------------------------------
inline VOID Skill::IncLevel(ESkillLevelChange eType, INT nInc)
{
	ASSERT( nInc > 0 );

	if( ESLC_Learn == eType )		m_nLearnLevel += nInc;
	else if( ESLC_Self == eType )	m_nSelfLevel += nInc;
	else if( ESLC_Temp == eType )	m_nTempAddLevel += nInc;

	INT nNewLevel = min(m_pProto->nMaxLevel, m_nLearnLevel + m_nSelfLevel + m_nTempAddLevel);
	if( nNewLevel == m_nLevel ) return;

	const tagSkillProto* pNewProto = g_attRes.GetSkillProto(CreateTypeID(m_dwID, nNewLevel));
	ASSERT( P_VALID(pNewProto) );

	// 去掉该等级的原有buff
	UnSetSkillBuff(m_pProto);

	m_nLevel = nNewLevel;
	m_pProto = pNewProto;

	// 设置新等级的buff
	SetSkillBuff(m_pProto);
}

//-----------------------------------------------------------------------------------------------
// 技能降级（mod不需要改变，但该技能原有buff要特殊处理一下）
//-----------------------------------------------------------------------------------------------
inline VOID Skill::DecLevel(ESkillLevelChange eType, INT nDec)
{
	ASSERT( nDec > 0 );

	if( ESLC_Learn == eType )		{ m_nLearnLevel -= nDec;	ASSERT(m_nLearnLevel >= 0); }
	else if( ESLC_Self == eType )	{ m_nSelfLevel -= nDec;		ASSERT(m_nSelfLevel >= 0); }
	else if( ESLC_Temp == eType )	{ m_nTempAddLevel -= nDec;	ASSERT( m_nTempAddLevel >= 0 ); }

	INT nNewLevel = min(m_pProto->nMaxLevel, m_nLearnLevel + m_nSelfLevel + m_nTempAddLevel);
	if( nNewLevel == m_nLevel ) return;

	const tagSkillProto* pNewProto = g_attRes.GetSkillProto(CreateTypeID(m_dwID, nNewLevel));
	ASSERT( P_VALID(pNewProto) );

	// 去掉原来等级的buff
	UnSetSkillBuff(m_pProto);

	m_nLevel = nNewLevel;
	m_pProto = pNewProto;

	// 设置新等级的buff
	SetSkillBuff(m_pProto);
}

//---------------------------------------------------------------------------------------------
// 冷却倒计时
//---------------------------------------------------------------------------------------------
inline BOOL Skill::CountDownCoolDown()
{
	m_nCoolDownCountDown -= TICK_TIME;

	if( m_nCoolDownCountDown <= 0 )
	{
		m_nCoolDownCountDown = 0;
		m_bNeedSave = TRUE;
		return TRUE;
	}
	// 大于5秒钟需要保存
	else if(m_nCoolDownCountDown % (TICK_TIME*5) == 0)
		m_bNeedSave = TRUE;

	return FALSE;
}

//---------------------------------------------------------------------------------------------
// 是否是可以影响人物属性的技能
//---------------------------------------------------------------------------------------------
inline BOOL Skill::IsCanModAtt()
{
	ESkillTargetType eTargetType = m_pProto->eTargetType;
	return IsPassive() && ESTT_Skill != eTargetType && ESTT_Buff != eTargetType && ESTT_Trigger != eTargetType;
}

//---------------------------------------------------------------------------------
// 得到某一个索引的伤害值
//---------------------------------------------------------------------------------
inline INT Skill::GetDmg(INT nIndex)
{
	if( nIndex < 0 && nIndex >= MAX_CHANNEL_TIMES ) return 0;

	INT nDmg = m_pProto->nChannelDmg[nIndex];

	if( P_VALID(m_pMod) )
	{
		nDmg = nDmg + m_pMod->nChannelDmgMod[nIndex] + 
			INT((FLOAT)nDmg * ((FLOAT)m_pMod->nChannelDmgModPct[nIndex] / 10000.0f));
	}

	return nDmg;
}

//---------------------------------------------------------------------------------
// 得到某一个索引对应的伤害时间（毫秒）
//---------------------------------------------------------------------------------
inline INT Skill::GetDmgTime(INT nIndex)
{
	ASSERT( nIndex >=0 && nIndex < MAX_CHANNEL_TIMES );
	return m_pProto->nChannelTime[nIndex];
}

//---------------------------------------------------------------------------------
// 得到攻击距离
//---------------------------------------------------------------------------------
inline FLOAT Skill::GetOPDist()
{
	return m_pProto->fOPDist + ( P_VALID(m_pMod) ? m_pMod->fOPDistMod : 0 );
}

//---------------------------------------------------------------------------------
// 得到攻击半径
//---------------------------------------------------------------------------------
inline FLOAT Skill::GetOPRadius()
{
	return m_pProto->fOPRadius + ( P_VALID(m_pMod) ? m_pMod->fOPRadiusMod : 0 );
}

//----------------------------------------------------------------------------------
// 得到起手时间
//----------------------------------------------------------------------------------
inline INT Skill::GetPrepareTime()
{
	return m_pProto->nPrepareTime + ( P_VALID(m_pMod) ? m_pMod->nPrepareTimeMod : 0 );
}

//----------------------------------------------------------------------------------
// 得到冷却时间
//----------------------------------------------------------------------------------
inline INT Skill::GetCoolDown()
{
	return m_pProto->nCoolDown + ( P_VALID(m_pMod) ? m_pMod->nCoolDownMod : 0 );
}

//----------------------------------------------------------------------------------
// 得到技能命中率
//----------------------------------------------------------------------------------
inline FLOAT Skill::GetHit()
{
	INT nHit = m_pProto->nHit;

	if( P_VALID(m_pMod) )
	{
		nHit = nHit + m_pMod->nHitMod + INT((FLOAT)nHit * ((FLOAT)m_pMod->nHitModPct / 10000.0f));
	}
	return (FLOAT)nHit / 10000.0f;
}

//----------------------------------------------------------------------------------
// 得到技能致命率
//----------------------------------------------------------------------------------
inline FLOAT Skill::GetCrit()
{
	INT nCrit = m_pProto->nCrit;

	if( P_VALID(m_pMod) )
	{
		nCrit = nCrit + m_pMod->nCritMod + INT((FLOAT)nCrit * ((FLOAT)m_pMod->nCritModPct / 10000.0f));
	}
	return (FLOAT)nCrit / 10000.0f;
}

//----------------------------------------------------------------------------------
// 得到技能仇恨
//----------------------------------------------------------------------------------
inline INT Skill::GetEnmity()
{
	INT nEnmity = m_pProto->nEnmity;

	if(P_VALID(m_pMod))
	{
		nEnmity = nEnmity + m_pMod->nEnmityMod + INT((FLOAT)nEnmity * ((FLOAT)m_pMod->nEnmityModPct / 10000.0f));
	}
	return nEnmity;
}


//---------------------------------------------------------------------------------------
// 得到物品消耗
//---------------------------------------------------------------------------------------
inline VOID Skill::GetCostItem(DWORD& dwItemID, INT& nNum)
{
	dwItemID = m_pProto->dwCostItemID;
	nNum = m_pProto->nCostItemNum;
}

//---------------------------------------------------------------------------------------
// 得到某个索引对应的BuffID
//---------------------------------------------------------------------------------------
inline DWORD Skill::GetBuffTypeID(INT nIndex)
{
	if( nIndex < 0 || nIndex >= MAX_BUFF_PER_SKILL )
		return GT_INVALID;

	if( P_VALID(m_pMod) )	return m_pMod->dwBuffTypeID[nIndex];
	else					return m_pProto->dwBuffID[nIndex];
}

//---------------------------------------------------------------------------------------
// 得到某个索引对应的TriggerID
//---------------------------------------------------------------------------------------
inline DWORD Skill::GetTriggerID(INT nIndex)
{
	if( nIndex < 0 || nIndex >= MAX_BUFF_PER_SKILL )
		return GT_INVALID;

	if( P_VALID(m_pMod) )	return m_pMod->dwTriggerID[nIndex];
	else					return m_pProto->dwTriggerID[nIndex];
}

//---------------------------------------------------------------------------------------
// 给技能自身添加Buff
//---------------------------------------------------------------------------------------
inline VOID Skill::SetSkillBuff(const tagSkillProto* pProto)
{
	if( !P_VALID(m_pMod) ) return;

	UnRegisterTriggerEvent();

	// 尝试给其添加一个buff
	for(INT n = 0; n < MAX_BUFF_PER_SKILL; n++)
	{
		if( FALSE == GT_VALID(pProto->dwBuffID[n]) )
			continue;

		for(INT m = 0; m < MAX_BUFF_PER_SKILL; m++)
		{
			if( P_VALID(m_pMod->dwBuffTypeID[m]) ) continue;
			m_pMod->dwBuffTypeID[m] = pProto->dwBuffID[n];
			m_pMod->dwTriggerID[m] = pProto->dwTriggerID[n];
			break;
		}
	}

	RegisterTriggerEvent();
}

//---------------------------------------------------------------------------------------
// 撤销技能自身的某些Buff
//---------------------------------------------------------------------------------------
inline VOID Skill::UnSetSkillBuff(const tagSkillProto* pProto)
{
	if( !P_VALID(m_pMod) ) return;

	UnRegisterTriggerEvent();

	for(INT n = 0; n < MAX_BUFF_PER_SKILL; n++)
	{
		if( FALSE == GT_VALID(pProto->dwBuffID[n]) ) continue;

		for(INT m = 0; m < MAX_BUFF_PER_SKILL; m++)
		{
			if( m_pMod->dwBuffTypeID[m] != pProto->dwBuffID[n] )
				continue;

			m_pMod->dwBuffTypeID[m] = GT_INVALID;
			m_pMod->dwTriggerID[m] = GT_INVALID;
			break;
		}
	}

	RegisterTriggerEvent();
}
