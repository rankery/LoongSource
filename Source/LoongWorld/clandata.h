//-----------------------------------------------------------------------------
//!\file clandata.h
//!\author xlguo
//!
//!\date 2009-02-19
//! last 
//!
//!\brief 氏族信息数据
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "../ServerDefine/clandata_define.h"



class ClanData
{
public:
	static const INT8	MAX_ACT_COUNT;						// 默认激活珍宝次数
	static const INT32	REP_FULL_VALUE;						// 声望满值
	static const INT32	ArrNRepLvlMin[ERL_NUM + 1];			// 声望阶段下限
	static const INT32	ArrNConUpLim[ERL_NUM];				// 贡献阶段上限

public:// 用于存取数据库
	ClanData();
	void	Init(const BYTE*& pData, Role *pRole);
	void	Save2DB(IN LPVOID pData, OUT LPVOID &pOutPointer, BOOL& bChg);

public:// 用于名人堂
	BOOL	ResetReputation(tagDWORDTime dwtResetTime, ECLanType eClanType, EReputationLevel eRepLvl);
	VOID	SetRepRstTimeStamp(tagDWORDTime dwtRepRstTime){	m_dwtLastResetTime = dwtRepRstTime; m_bChanged = TRUE;}
	tagDWORDTime	GetRepRstTimeStamp(){return m_dwtLastResetTime;}
	VOID	ActCountSetVal(INT8 n8ActCount, ECLanType clantype);
	INT8	ActCountGetVal(ECLanType clantype);
	BOOL	ActCountDec(ECLanType clantype);
	VOID	SetFame(ECLanType eClanType, BOOL bFame = TRUE);
	BOOL	IsFame(ECLanType eClanType) const	{ ASSERT(JDG_VALID(ECLT, eClanType)); return m_u16FameMask & (1<<eClanType);	}

public:// 用于消费贡献
	INT32	ClanConGetVal(ECLanType eClanType) const ;
	INT32	ClanConGetMaxVal(ECLanType eClanType) const ;
	INT32	ClanConDec(INT32 nNeed, ECLanType eClanType, BOOL bSend = FALSE);
	INT32	ClanConInc(INT32 nIncr, ECLanType eClanType, BOOL bSend = FALSE);

public:// 用于改变声望
	VOID	RepSetVal(ECLanType eClanType, INT32 nNewRep, BOOL bSend = TRUE);
	VOID	RepModVal(ECLanType eClanType, INT32 nMod, BOOL bSend = TRUE);
	INT32	RepGetVal(ECLanType eClanType);
	INT16	RepGetLvl(ECLanType eClanType);

private:
	VOID	OnRepChange(ECLanType eClanType);
	VOID	OnClanConChange(ECLanType eClanType);
	VOID	OnActCountChange(ECLanType eClanType, BOOL bSend = TRUE);
	BOOL	EReputationValid(INT32 nReputation);
	EReputationLevel GetRepLvl(INT32 nReputation);

private:
	Role*				m_pRole;							// 角色
	EReputationLevel	m_eRepLevel[ECLT_NUM];				// 声望级别

	INT8				m_n8ActCount[ECLT_NUM];				// 剩余激活次数
	INT32				m_nReputation[ECLT_NUM];			// 声望值
	INT32				m_nContribution[ECLT_NUM];			// 贡献值
	tagDWORDTime		m_dwtLastResetTime;					// 上次声望重置时间
	UINT16				m_u16FameMask;						// 名人堂成员

	BOOL				m_bChanged;							// 是否有改变
private:
	static VOID Prepare(tagDBReputeData* pdbData);

public:// 用于测试
	static BOOL StaticTest();
	BOOL DynamicTest(INT nTestNO , ECLanType eClanType, INT nVal, BOOL bSend = TRUE);

};