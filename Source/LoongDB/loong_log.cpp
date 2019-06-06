//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: loong_log.cpp
// author: Sxg
// actor:
// data: 2009-03-24
// last:
// brief: log相关处理 -- 消息处理
//-----------------------------------------------------------------------------
#include "StdAfx.h"

#include "Loong.h"
#include "LoongDB.h"
#include "../ServerDefine/msg_log.h"

//-----------------------------------------------------------------------------
// 创建新表
//-----------------------------------------------------------------------------
VOID CLoong::CreateCurDataLogTable()
{	
	IMSG(_T("\n\n"));

	CHAR szTime[8] = {'\0'};
	tagDWORDTime dwTime = GetCurrentDWORDTime();
	sprintf_s(szTime, sizeof(szTime), "_%02d%02d", dwTime.month, dwTime.day);

	// 创建新表
	if(m_pDB->CreateLogMoneyTable(szTime)
		&& m_pDB->CreateLogYuanBaoTable(szTime)
		&& m_pDB->CreateLogExVolumeTable(szTime)
		&& m_pDB->CreateLogTimeStatTable(szTime)
		&& m_pDB->CreateLogItemTable(szTime)
		&& m_pDB->CreateLogItemTimesTable(szTime)
		&& m_pDB->CreateLogMallSellTable(szTime)
		&& m_pDB->CreateLogMallSellPackTable(szTime)
		&& m_pDB->CreateLogFundTable(szTime)
		&& m_pDB->CreateLogMaterialTable(szTime)
		&& m_pDB->CreateLogReputationTable(szTime)
		&& m_pDB->CreateLogGMCmd(szTime)
		&& m_pDB->CreateLogPet(szTime))
	{
		m_dwLogTime = dwTime;
		sprintf_s(m_szLogFilePostfix, sizeof(m_szLogFilePostfix), szTime);
	}

	IMSG(_T("\n\n"));
}

//-----------------------------------------------------------------------------
// 金钱
//-----------------------------------------------------------------------------
DWORD CLoong::LogSilver(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_LogSilver);

	m_pDB->InsertMoneyLog(m_szLogFilePostfix, &p->sLogSilver);

	return 0;
}

//-----------------------------------------------------------------------------
// 元宝
//-----------------------------------------------------------------------------
DWORD CLoong::LogYuanBao(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_LogYuanBao);

	m_pDB->InsertYuanBaoLog(m_szLogFilePostfix, &p->sLogYuanBao);

	return 0;
}

//-----------------------------------------------------------------------------
// 赠点
//-----------------------------------------------------------------------------
DWORD CLoong::LogExVolume(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_LogExVolume);

	m_pDB->InsertExVolumeLog(m_szLogFilePostfix, &p->sLogExVolume);

	return 0;
}

//-----------------------------------------------------------------------------
// 定时统计
//-----------------------------------------------------------------------------
DWORD CLoong::LogTimeStat(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_LogTimeStat);

	m_pDB->InsertTimeStatLog(m_szLogFilePostfix, &p->sLogTimeStat);

	return 0;
}

//-----------------------------------------------------------------------------
// 物品log
//-----------------------------------------------------------------------------
DWORD CLoong::LogItem(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_LogItem);

	m_pDB->InsertItemLog(m_szLogFilePostfix, &p->sLogItem);

	return 0;
}

//-----------------------------------------------------------------------------
// 商城出售log
//-----------------------------------------------------------------------------
DWORD CLoong::LogMallSell(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_LogMallSell);

	m_pDB->InsertMallSellLog(m_szLogFilePostfix, &p->sLogMallSell);

	return 0;
}

//-----------------------------------------------------------------------------
// 商城出售礼品包log
//-----------------------------------------------------------------------------
DWORD CLoong::LogMallSellPack(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_LogMallSellPack);

	m_pDB->InsertMallSellPackLog(m_szLogFilePostfix, &p->sLogMallSellPack);

	return 0;
}

//-----------------------------------------------------------------------------
// 商城出售log
//-----------------------------------------------------------------------------
DWORD CLoong::LogItemTimes(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_LogItemTimes);

	m_pDB->InsertItemTimesLog(m_szLogFilePostfix, &p->sLogItemTimes);

	return 0;
}

//-----------------------------------------------------------------------------
// 帮派资金log
//-----------------------------------------------------------------------------
DWORD CLoong::LogFund(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_LogFund);

	m_pDB->InsertFundLog(m_szLogFilePostfix, &p->sLogFund);

	return 0;
}

//-----------------------------------------------------------------------------
// 帮派资材log
//-----------------------------------------------------------------------------
DWORD CLoong::LogMaterial(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_LogMaterial);

	m_pDB->InsertMaterialLog(m_szLogFilePostfix, &p->sLogMaterial);

	return 0;
}

//-----------------------------------------------------------------------------
// 帮派声望log
//-----------------------------------------------------------------------------
DWORD CLoong::LogReputation(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_LogReputation);

	m_pDB->InsertReputationLog(m_szLogFilePostfix, &p->sLogReputation);

	return 0;
}

DWORD CLoong::LogGMCmd(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_LogGMCmd);

	m_pDB->InsertGMCmdLog(m_szLogFilePostfix, &p->sLogGMCmd);

	return 0;
}

DWORD CLoong::LogPet(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_LogPet);

	m_pDB->InsertPetLog(m_szLogFilePostfix, &p->sLogPet);

	return 0;
}