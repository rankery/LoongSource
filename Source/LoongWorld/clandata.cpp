//-----------------------------------------------------------------------------
//!\file clandata.cpp
//!\author xlguo
//!
//!\date 2009-02-19
//! last 
//!
//!\brief 氏族信息
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "clandata.h"
#include "role.h"
#include "famehall.h"
#include "../WorldDefine/role_att.h"
#include "../WorldDefine/msg_currency.h"

const INT32 ClanData::ArrNRepLvlMin[ERL_NUM + 1]	= { -46800, -10800, -3600, 0, 3600, 10800, 22800, 40800, 76800, 148800, 328800};
const INT32 ClanData::ArrNConUpLim[ERL_NUM]			= { 0, 0, 5000, 5000, 10000, 20000, 40000, 70000, 120000, 200000 };
const INT8	ClanData::MAX_ACT_COUNT					= 1;				// 默认激活珍宝次数
const INT32	ClanData::REP_FULL_VALUE				= 328800;			// 声望满值

ClanData::ClanData()
{
	for (INT32 clantype = ECLT_BEGIN; clantype < ECLT_END; ++clantype)
	{
		m_nContribution[clantype]	= 0;
		m_n8ActCount[clantype]		= 0;
		m_nReputation[clantype]		= 0;
		OnRepChange(ECLanType(clantype));
		OnClanConChange(ECLanType(clantype));
	}
	m_dwtLastResetTime = g_world.GetWorldTime();
}

void ClanData::Init(const BYTE*& pData, Role *pRole)
{
	m_bChanged = FALSE;
	ASSERT( P_VALID(pRole) );
	m_pRole = pRole;
	MTRANS_POINTER(pRepData, pData, tagDBReputeData);
	ASSERT( P_VALID(pRepData) );
	for (INT32 clantype = ECLT_BEGIN; clantype < ECLT_END; ++clantype)
	{
		m_nContribution[clantype]	= pRepData->m_nContribution[clantype];
		m_n8ActCount[clantype]		= pRepData->m_n8ActCount[clantype];
		m_nReputation[clantype]		= pRepData->m_nReputation[clantype];
		ASSERT( m_nReputation[clantype] <= ClanData::REP_FULL_VALUE);
		OnActCountChange(ECLanType(clantype), FALSE);
		OnRepChange(ECLanType(clantype));
		OnClanConChange(ECLanType(clantype));
	}
	m_dwtLastResetTime = pRepData->m_dwtLastResetTime;
	m_u16FameMask = pRepData->m_u16FameMask;

	pData = pData + sizeof(tagDBReputeData);

	// 检查声望重置更新
	g_fameHall.RoleRepUpdate(m_pRole, ECLT_NULL);
}
void ClanData::Save2DB( IN LPVOID pData, OUT LPVOID &pOutPointer, BOOL& bChg )
{
	bChg = TRUE;
	if(FALSE == m_bChanged)
	{	
		bChg = FALSE;
		return;
	}
		
	m_bChanged = FALSE;

	MTRANS_POINTER(pRepData, pData, tagDBReputeData);
	
	ASSERT( P_VALID(pRepData) );
	for (INT32 clantype = ECLT_BEGIN; clantype < ECLT_END; ++clantype)
	{
		pRepData->m_nContribution[clantype]		= m_nContribution[clantype];
		pRepData->m_n8ActCount[clantype]		= m_n8ActCount[clantype];
		pRepData->m_nReputation[clantype]		= m_nReputation[clantype];
	}
	pRepData->m_u16FameMask = m_u16FameMask;
	pRepData->m_dwtLastResetTime = m_dwtLastResetTime;
	pOutPointer = reinterpret_cast<BYTE*>(pData) + sizeof(tagDBReputeData);
}

BOOL ClanData::ResetReputation( tagDWORDTime dwtResetTime, ECLanType eClanType, EReputationLevel eRepLvl )
{
	ASSERT( JDG_VALID(ECLT, eClanType) );

	// 等级判断
	if (RepGetLvl(eClanType) < eRepLvl)
	{
		return FALSE;
	}

	// 时间判断
	if (dwtResetTime <= GetRepRstTimeStamp())
	{
		return FALSE;
	}

	// 需要重置
	RepSetVal(eClanType, ArrNRepLvlMin[eRepLvl]);

	return TRUE;
}
VOID ClanData::ActCountSetVal( INT8 n8ActCount, ECLanType clantype )
{
	ASSERT( JDG_VALID(ECLT, clantype));
	if (n8ActCount > MAX_ACT_COUNT)
	{
		n8ActCount = MAX_ACT_COUNT;
	}
	m_n8ActCount[clantype] = n8ActCount;
	OnActCountChange(clantype);
}
INT8 ClanData::ActCountGetVal( ECLanType clantype )
{
	ASSERT( JDG_VALID(ECLT, clantype));
		
	return m_n8ActCount[clantype];
}

BOOL ClanData::ActCountDec(ECLanType clantype)
{
	ASSERT( JDG_VALID(ECLT, clantype));
	if (m_n8ActCount[clantype] <= 0)
	{
		return FALSE;
	}
	else
	{
		--m_n8ActCount[clantype];
		OnActCountChange(clantype);
		return TRUE;
	}
}

INT32 ClanData::ClanConGetVal(ECLanType eClanType) const 
{	
	ASSERT( JDG_VALID(ECLT, eClanType));
	return m_nContribution[eClanType];
}
INT32 ClanData::ClanConGetMaxVal(ECLanType eClanType) const 
{
	ASSERT( JDG_VALID(ECLT, eClanType)); 
	EReputationLevel repLvl = m_eRepLevel[eClanType];
	ASSERT( JDG_VALID(ERL, repLvl));

	return ArrNConUpLim[repLvl];
}
INT32 ClanData::ClanConDec( INT32 nNeed, ECLanType eClanType, BOOL bSend /*= FALSE*/ )
{
	ASSERT( JDG_VALID(ECLT, eClanType) ); 
	ASSERT( nNeed >= 0 );

	INT32 nSrc = m_nContribution[eClanType];
	m_nContribution[eClanType] -= nNeed;
	OnClanConChange(eClanType);
	INT32 nChg = nSrc - m_nContribution[eClanType];
	
	if (bSend)
	{
		tagNS_ClanCon send;
		send.nCurClanCon = m_nContribution[eClanType];
		send.nChangeClanCon = -nChg;
		send.byClanType = eClanType;
		m_pRole->SendMessage(&send, send.dwSize);
	}

	return nChg;
}
INT32 ClanData::ClanConInc( INT32 nIncr, ECLanType eClanType, BOOL bSend /*= FALSE*/ )
{
	ASSERT( JDG_VALID(ECLT, eClanType)); 
	ASSERT( nIncr >= 0 );

	INT32 nSrc = m_nContribution[eClanType];
	m_nContribution[eClanType] += nIncr;
	OnClanConChange(eClanType);
	INT32 nChg = m_nContribution[eClanType] - nSrc;

	if (bSend)
	{
		tagNS_ClanCon send;
		send.nCurClanCon = m_nContribution[eClanType];
		send.nChangeClanCon = nChg;
		send.byClanType = eClanType;
		m_pRole->SendMessage(&send, send.dwSize);
	}

	return nChg;
}

VOID ClanData::RepSetVal( ECLanType eClanType, INT32 nNewRep, BOOL bSend /*= TRUE*/ )
{
	ASSERT( JDG_VALID( ECLT, eClanType) );
	INT32 nOldRepVal = m_nReputation[eClanType];
	m_nReputation[eClanType] = nNewRep;
	OnRepChange(eClanType);
	INT32 nNewRepVal = m_nReputation[eClanType];
	if (bSend)
	{
		tagNS_RoleReputeChange send;
		send.nRepute	= nNewRepVal;
		send.nChange	= nNewRepVal - nOldRepVal;
		send.byClanType	= eClanType;
		m_pRole->SendMessage(&send, send.dwSize);
	}
}
VOID ClanData::RepModVal( ECLanType eClanType, INT32 nMod, BOOL bSend /*= TRUE*/ )
{
	ASSERT( JDG_VALID( ECLT, eClanType) );
	INT32 nOldRepVal = m_nReputation[eClanType];
	m_nReputation[eClanType] += nMod;
	OnRepChange(eClanType);
	INT32 nNewRepVal = m_nReputation[eClanType];

	if (bSend)
	{
		tagNS_RoleReputeChange send;
		send.nRepute	= nNewRepVal;
		send.nChange	= nNewRepVal - nOldRepVal;
		send.byClanType	= eClanType;
		m_pRole->SendMessage(&send, send.dwSize);
	}
}
INT32 ClanData::RepGetVal(ECLanType eClanType)
{
	ASSERT( JDG_VALID( ECLT, eClanType) );
	INT32 nRtv = m_nReputation[eClanType];
	ASSERT( EReputationValid(nRtv) );
	return nRtv;
}
INT16 ClanData::RepGetLvl(ECLanType eClanType)
{
	INT16 nRtv = m_eRepLevel[eClanType];
	ASSERT( JDG_VALID( ERL, nRtv) );
	return nRtv;
}

VOID ClanData::OnRepChange(ECLanType eClanType)
{
	m_bChanged = TRUE;
	// 修正声望值
	if (m_nReputation[eClanType] < ArrNRepLvlMin[ERL_BEGIN])
	{
		m_nReputation[eClanType] = ArrNRepLvlMin[ERL_BEGIN];
	}
	else if (m_nReputation[eClanType] >= ArrNRepLvlMin[ERL_END])
	{
		m_nReputation[eClanType] = ArrNRepLvlMin[ERL_END];
	}

	// 修正级别
	EReputationLevel erl = GetRepLvl(m_nReputation[eClanType]);
	if ( ERL_NULL == erl )
	{
		ASSERT(0);
	}
	else if (erl != m_eRepLevel[eClanType])
	{
		m_eRepLevel[eClanType] = erl;
	}
	// tbd: 修正贡献
	
	// 尝试进入名人堂
	if (RepGetVal(eClanType) >= ENTER_FAMEHALL_REP_LIM)
	{
		g_fameHall.TryEnterFameHall(m_pRole, eClanType);
	}	
}
VOID ClanData::OnClanConChange(ECLanType eClanType)
{
	m_bChanged = TRUE;

	BOOL bUpperLimit = FALSE;

	// 修正贡献值
	if (m_nContribution[eClanType] >= ArrNConUpLim[ m_eRepLevel[eClanType] ])
	{
		m_nContribution[eClanType] = ArrNConUpLim[ m_eRepLevel[eClanType] ];
		bUpperLimit = TRUE;
	}
	else if (m_nContribution[eClanType] < ArrNConUpLim[ ERL_BEGIN ])
	{
		m_nContribution[eClanType] = ArrNConUpLim[ ERL_BEGIN ];
	}

	if(bUpperLimit)
	{
		tagNS_ClanConUpperLimit send;
		send.eClanType = eClanType;
		m_pRole->SendMessage(&send, send.dwSize);
	}
}
VOID ClanData::OnActCountChange( ECLanType eClanType, BOOL bSend /*= TRUE*/ )
{
	m_bChanged = TRUE;
	if (m_n8ActCount[eClanType] >= MAX_ACT_COUNT)
	{
		m_n8ActCount[eClanType] = MAX_ACT_COUNT;
	}
	else if (m_n8ActCount[eClanType] < 0)
	{
		m_n8ActCount[eClanType] = 0;
	}
	if( bSend )
	{
		tagNS_TreasureActCountChange send;
		send.nActCount	= static_cast<BYTE>(m_n8ActCount[eClanType]);
		send.eClanType	= eClanType;
		m_pRole->SendMessage(&send, send.dwSize);
	}
}
BOOL ClanData::EReputationValid(INT32 nReputation)
{
	if (nReputation < ArrNRepLvlMin[ERL_BEGIN] || nReputation > ArrNRepLvlMin[ERL_END])
	{
		return FALSE;
	}
	return TRUE;
}

EReputationLevel ClanData::GetRepLvl(INT32 nReputation)
{
	for (INT nLvlIndex = ERL_BEGIN; nLvlIndex < ERL_END; ++nLvlIndex)
	{
		if (ArrNRepLvlMin[nLvlIndex] <= nReputation && 
			ArrNRepLvlMin[nLvlIndex + 1] > nReputation)
		{
			return EReputationLevel(nLvlIndex);
		}
	}
	if (REP_FULL_VALUE == nReputation)
	{
		return ERL_Legend;
	}
	return ERL_NULL;
}
VOID ClanData::SetFame(ECLanType eClanType, BOOL bFame/* = TRUE*/)
{ 
	ASSERT(JDG_VALID(ECLT, eClanType)); 
	UINT16 u16Mask	= (UINT16)(1<<eClanType);
	if (bFame)
	{
		m_u16FameMask |= u16Mask;
		tagNS_BecomeFame send;
		send.eClanType	= eClanType;
		m_pRole->SendMessage(&send, send.dwSize);
	}
	else
		m_u16FameMask &= ~u16Mask;
}
BOOL ClanData::StaticTest()
{
	// 准备数据
	tagDBReputeData dbData;
	Prepare(&dbData);

	// 测试初始化
	ClanData clandata;
	const BYTE * pData = (const BYTE *)(&dbData);
	clandata.Init(pData, NULL);

	// 测试保存数据
	tagDBReputeData dbDataSave;
	LPVOID ppp;
	BOOL bchg;
	clandata.Save2DB((LPVOID)(&dbDataSave), ppp, bchg);
	
	// 测试声望接口
	INT32 nVal = 1;
	for (INT32 clantype = ECLT_BEGIN; clantype < ECLT_END; ++clantype)
	{
		ECLanType eclantype = ECLanType(clantype);
		clandata.RepSetVal(eclantype, -46800);
		nVal = clandata.RepGetLvl(eclantype);
		clandata.RepModVal(eclantype, 100000);
		nVal = clandata.RepGetLvl(eclantype);
	}

	// 测试贡献接口
	for (INT32 clantype = ECLT_BEGIN; clantype < ECLT_END; ++clantype)
	{
		ECLanType eclantype = ECLanType(clantype);
		nVal = clandata.ClanConGetMaxVal(eclantype);
		nVal = clandata.ClanConGetVal(eclantype);
		clandata.ClanConInc(1000, eclantype);
		clandata.ClanConDec(1000, eclantype);
	}

	// 测试名人堂接口
	for (INT32 clantype = ECLT_BEGIN; clantype < ECLT_END; ++clantype)
	{
		ECLanType eclantype = ECLanType(clantype);
		clandata.ActCountSetVal(1,eclantype);
		nVal = clandata.ActCountGetVal(eclantype);
		nVal = clandata.ActCountDec(eclantype);
		nVal = clandata.ActCountGetVal(eclantype);
	}

	return TRUE;

}

BOOL ClanData::DynamicTest( INT nTestNO , ECLanType eClanType, INT nVal, BOOL bSend /*= TRUE*/ )
{
	if (!JDG_VALID(ECLT,eClanType))
	{
		return FALSE;
	}
	switch(nTestNO)
	{
	case 0:
		{
			INT nMaxConVal = ClanConGetMaxVal(eClanType);
			INT nSrcCon = ClanConGetVal(eClanType);
			ClanConInc(nVal, eClanType, bSend);
			INT nDstCon = ClanConGetVal(eClanType);
		}
		break;
	case 1:
		{
			INT nMaxConVal = ClanConGetMaxVal(eClanType);
			INT nSrcCon = ClanConGetVal(eClanType);
			ClanConDec(nVal, eClanType, bSend);
			INT nDstCon = ClanConGetVal(eClanType);
		}
		break;
	case 2:
		{
			INT nSrcLvl = RepGetLvl(eClanType);
			INT nSrcVal = RepGetVal(eClanType);
			RepModVal(eClanType, nVal, bSend);
			INT nDstLvl = RepGetLvl(eClanType);
			INT nDstVal = RepGetVal(eClanType);
		}
		break;
	case 3:
		{
			INT nSrcLvl = RepGetLvl(eClanType);
			INT nSrcVal = RepGetVal(eClanType);
			RepSetVal(eClanType, nVal, bSend);
			INT nDstLvl = RepGetLvl(eClanType);
			INT nDstVal = RepGetVal(eClanType);
		}
		break;
	case 4:
		{
			INT nActCount = ActCountGetVal(eClanType);
		}
		break;
	case 5:
		{
			BOOL bFameRole = IsFame(eClanType);
			SetFame(eClanType, nVal);
			if (nVal)
			{
				ASSERT(nVal != 0 && IsFame(eClanType));
			}
			else
			{
				ASSERT(nVal == 0 && !IsFame(eClanType));
			}
		}
		break;
	case 6:
		break;
	}
	return TRUE;
}

VOID ClanData::Prepare(tagDBReputeData* pdbData)
{
	for (INT32 clandata = ECLT_BEGIN; clandata < ECLT_END; ++clandata)
	{
		pdbData->m_n8ActCount[clandata]		= 0;
		pdbData->m_nReputation[clandata]		= 0;
		pdbData->m_nContribution[clandata]		= 0;
	}
	pdbData->m_dwtLastResetTime	= g_world.GetWorldTime();
}