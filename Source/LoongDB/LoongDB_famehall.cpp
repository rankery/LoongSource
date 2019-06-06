//-----------------------------------------------------------------------------
//!\file LoongDB_famehall.cpp
//!\author xlguo
//!
//!\date 2009-03-13
//! last 
//!
//!\brief 名人堂数据库处理
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "LoongDB.h"
#include "../ServerDefine/msg_famehall.h"

BOOL CLoongDB::LoadActTreasures( LPVOID& pData , INT& nTotalNum)
{
	Load(pData, &nTotalNum, GT_INVALID, GT_INVALID, &CLoongDB::FormatLoadActTreasures, &CLoongDB::ProcResLoadActTreasures);
	return TRUE;
}

VOID CLoongDB::FormatLoadActTreasures(Beton::MyStream *pStream, DWORD nClanType, DWORD dwRoleID)
{
	pStream->Clear();
	pStream->SetSelect("famehall_act_treasure", "ClanType,TreasureID,ActNameID");
	pStream->FillString(" order by ClanType asc,ActTime asc;");
}

VOID CLoongDB::ProcResLoadActTreasures( OUT LPVOID& pData, OUT INT32 *pNum, Beton::QueryResult *pRes )
{

	MTRANS_POINTER(p, pData, tagNDBS_GetActTreasureList);
	ASSERT_P_VALID(p);
	ASSERT_P_VALID(pNum);
	*pNum = pRes->GetRowCount();
	ASSERT(*pNum > 0);

	INT nClanType = ECLT_BEGIN;
	INT nIndex = 0;

	do 
	{
		nClanType = (*pRes)[0].GetDword();
		++(p->nRecNum[nClanType]);
		p->treasureData[nIndex].n16TreasureID	= (*pRes)[1].GetShort();
		p->treasureData[nIndex].dwNamePrefixID	= (*pRes)[2].GetDword();
		++nIndex;
	} while (pRes->NextRow());

	ASSERT(*pNum == nIndex);
}


BOOL CLoongDB::LoadFameHallSnaps(LPVOID& pData, INT& nTotalNum)
{
	Load(pData, &nTotalNum, GT_INVALID, GT_INVALID, &CLoongDB::FormatLoadFameSnaps, &CLoongDB::ProcResLoadFameSnaps);
	return TRUE;
}
VOID CLoongDB::FormatLoadFameSnaps(Beton::MyStream *pStream, DWORD dwClanType, DWORD dwRoleID)
{
	pStream->Clear();
	pStream->SetSelect("famehall_entersnap", "ClanType,RoleID,EnterNameID,EnterTime");
	pStream->FillString(" order by ClanType asc,EnterTime asc");

}
VOID CLoongDB::ProcResLoadFameSnaps(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(p, pData, tagNDBS_GetFameHallEnterSnap);

	ASSERT(pNum != NULL);
	*pNum = pRes->GetRowCount();

	INT nIndex = 0;
	INT nClanType = ECLT_BEGIN;

	do 
	{
		nClanType = (*pRes)[0].GetDword();
		++(p->nRecNum[nClanType]);

		p->enterSnap[nIndex].dwRoleID		= (*pRes)[1].GetDword();
		p->enterSnap[nIndex].dwEnterNameID	= (*pRes)[2].GetDword();
		p->enterSnap[nIndex].dwtEnterTime	= (*pRes)[3].GetDword();
		++nIndex;
	} while (pRes->NextRow());

	ASSERT(*pNum == nIndex);

}


BOOL CLoongDB::LoadRepRstTimeStamp(LPVOID& pData)
{
	MTRANS_POINTER(p, pData, tagNDBS_GetRepResetTimestamp);
	PVOID pOut = p->dwResetTime;
	INT nNum = 0;
	return Load(pOut, &nNum, GT_INVALID, GT_INVALID, &CLoongDB::FormatLoadRstTimeStamps, &CLoongDB::ProcResLoadRstTimeStamps);
}
VOID CLoongDB::FormatLoadRstTimeStamps(Beton::MyStream *pStream, DWORD dwClanType, DWORD dwRoleID)
{
	pStream->Clear();
	pStream->SetSelect("famehall_rep_rst_timestamp", "ClanType,ResetTimeStamp");
}
VOID CLoongDB::ProcResLoadRstTimeStamps(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pDwt, pData, tagDWORDTime);
	ASSERT(P_VALID(pNum));
	*pNum = pRes->GetRowCount();
	do 
	{
		INT nClanType = (*pRes)[0].GetDword();
		ASSERT(JDG_VALID(ECLT, nClanType));
		pDwt[nClanType] = (*pRes)[1].GetDword();
	} while (pRes->NextRow());

}

//-------------------------------------------------------------------------------------------------------
// 读取氏族声望排名
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::LoadRepRank(BYTE byClanType, PVOID pData, INT32 &nNum)
{
	PVOID pOut = pData;
	BOOL bRtv = Load(pOut, &nNum, byClanType, GT_INVALID, &CLoongDB::FormatLoadRepRank, &CLoongDB::ProcResLoadRepRank);
	ASSERT(nNum * sizeof(tagRepRankData) == reinterpret_cast<BYTE *>(pOut) - reinterpret_cast<BYTE *>(pData));
	return bRtv;
}


//-------------------------------------------------------------------------------------------------------
// 读取声望排名SQL语句格式化
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatLoadRepRank(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID)
{
	static const char* ColNameTable[ECLT_NUM] = {	"RepXuanYuan",	"RepShenNong",	"RepFuXi",	"RepSanMiao",
		"RepJiuLi",		"RepYueZhi",	"RepNvWa",	"RepGongGong"
	};
	BYTE byClanType = static_cast<BYTE>(dwAccountID);
	ASSERT(JDG_VALID(ECLT, byClanType));
	const char* szColName = ColNameTable[byClanType];

	std::string strquery;
	strquery.append("RoleID,");
	strquery.append(szColName);

	pStream->SetSelect("clan_data", strquery.c_str());

	pStream->SetWhere();
	pStream->FillString(szColName).FillString(">=0 order by ").FillString(szColName).FillString(" desc limit ") << MAX_REP_RANK_NUM;
}


//-------------------------------------------------------------------------------------------------------
// 读取声望排名结果检查及保存
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::ProcResLoadRepRank(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes)
{
	ASSERT(P_VALID(pData));
	MTRANS_POINTER(p, pData, tagRepRankData);

	ASSERT(P_VALID(pNum));
	*pNum = pRes->GetRowCount();
	ASSERT(*pNum <= MAX_REP_RANK_NUM);

	for (INT nIndex = 0; nIndex < *pNum; ++nIndex)
	{
		p[nIndex].dwRoleID		= (*pRes)[0].GetDword();
		p[nIndex].dwReputValue	= (*pRes)[1].GetInt();

		pRes->NextRow();
	}
	pData = reinterpret_cast<BYTE *>(pData) + sizeof(tagRepRankData) * (*pNum);
}

//-------------------------------------------------------------------------------------------------------
// 插入进入名人堂快照
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::InsertFameHallEnterSnap(PVOID pData)
{	
	return Update(GT_INVALID, pData, 1, sizeof(tagNDBC_InsertFameHallSnap), &CLoongDB::FormatInsertFameHallEnterSnap);
}

VOID CLoongDB::FormatInsertFameHallEnterSnap(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagNDBC_InsertFameHallSnap);
	pStream->Clear();
	pStream->SetInsert("famehall_entersnap");
	pStream->FillString("RoleID=") << p->dwRoleID;
	pStream->FillString(",EnterNameID=") << p->dwEnterNameID;
	pStream->FillString(",ClanType=") << p->byClanType;
	pStream->FillString(",EnterTime=") << p->dwEnterTime;
}

//-------------------------------------------------------------------------------------------------------
// 插入激活氏族珍宝
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::InsertActTreasure(PVOID pData)
{
	return Update(GT_INVALID,pData, 1, sizeof(tagNDBC_InsertActClanTreasure), &CLoongDB::FormatInsertActTreasure );
}

VOID CLoongDB::FormatInsertActTreasure(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagNDBC_InsertActClanTreasure);
	pStream->Clear();
	pStream->SetInsert("famehall_act_treasure");
	pStream->FillString("TreasureID=") << p->u16TreasureID;
	pStream->FillString(",ActNameID=") << p->dwActNameID;
	pStream->FillString(",RoleID=") << p->dwRoleID;
	pStream->FillString(",ClanType=") << p->byClanType;
	pStream->FillString(",ActTime=") << p->dwActTime;
}

//-------------------------------------------------------------------------------------------------------
// 更新声望重置时间
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::UpdateRepRstTime(BYTE byClanType, DWORD dwRstTime)
{
	return Update(byClanType, &dwRstTime, 1, sizeof(DWORD), &CLoongDB::FormatUpdateRepRstTime);
}

VOID CLoongDB::FormatUpdateRepRstTime(Beton::MyStream *pStream, DWORD dwClanType, LPVOID pData)
{
	MTRANS_POINTER(p, pData, DWORD);
	pStream->Clear();
	pStream->SetInsert("famehall_rep_rst_timestamp");
	pStream->FillString("ClanType=") << dwClanType;
	pStream->FillString(",ResetTimeStamp=") << *p;
	pStream->FillString(" ON DUPLICATE KEY UPDATE");
	pStream->FillString(" ResetTimeStamp=") << *p;
}