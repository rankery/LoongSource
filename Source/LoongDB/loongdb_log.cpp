//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: loongdb_log.cpp
// author: Sxg
// actor:
// data: 2009-03-24
// last:
// brief: log相关处理 -- db层
//-----------------------------------------------------------------------------
#include "StdAfx.h"

#include "LoongDB.h"
#include "../ServerDefine/log_cmdid_define.h"
//-----------------------------------------------------------------------------
// log表名称定义
//-----------------------------------------------------------------------------
LPCSTR TableName_LogMoney				= "log_money";
LPCSTR TableName_LogYuanBao				= "log_yuanbao";
LPCSTR TableName_LogExVolume			= "log_exvolume";
LPCSTR TableName_LogTimeStat			= "log_time_stat";
LPCSTR TableName_LogItem				= "log_item";
LPCSTR TableName_LogMallSell			= "log_mallsell";
LPCSTR TableName_LogMallSellPack		= "log_mallsell_pack";
LPCSTR TableName_LogItemTimes			= "log_itemtimes";
LPCSTR TableName_LogFund				= "log_fund";
LPCSTR TableName_LogMaterial			= "log_material";
LPCSTR TableName_LogReputation			= "log_reputation";
LPCSTR TableName_LogGMCmd				= "log_gmcmd";
LPCSTR TableName_LogPet					= "log_pet";

//-----------------------------------------------------------------------------
// 创建log表通用方法
//-----------------------------------------------------------------------------
BOOL CLoongDB::CreateLogTable(LPCSTR strTable, LPCSTR strPostfix)
{
	// 获取流
	Beton::MyStream* pStream = m_pDBLog->GetStream();
	ASSERT(P_VALID(pStream));

	pStream->Clear();
	pStream->FillString("CREATE TABLE IF NOT EXISTS ");
	pStream->FillString(strTable);
	pStream->FillString(strPostfix);
	pStream->FillString(" LIKE ");
	pStream->FillString(strTable);

	if(!m_pDBLog->Execute(pStream))
	{
		// 向监控发消息

		IMSG(_T("\n\n\nLog File<%s"), m_pUtil->Unicode8ToUnicode(strTable));
		IMSG(_T("%s> create failed!\n\n\n"), m_pUtil->Unicode8ToUnicode(strPostfix));

		// 释放流
		m_pDBLog->ReturnStream(pStream);

		return FALSE;
	}

	IMSG(_T("Log File<%s"), m_pUtil->Unicode8ToUnicode(strTable));
	IMSG(_T("%s> create successful!\n"), m_pUtil->Unicode8ToUnicode(strPostfix));

	// 释放流
	m_pDBLog->ReturnStream(pStream);

	return TRUE;
}

//-----------------------------------------------------------------------------
// 金钱记录
//-----------------------------------------------------------------------------
BOOL CLoongDB::CreateLogMoneyTable(LPCSTR strPostfix)
{
	return CreateLogTable(TableName_LogMoney, strPostfix);
}

VOID CLoongDB::InsertMoneyLog(LPCSTR szTablePostfix, LPVOID pData)
{
	UpdateLog(szTablePostfix, pData, (&CLoongDB::FormatInsertMoneyLog));
}

VOID CLoongDB::FormatInsertMoneyLog(Beton::MyStream *pStream, LPCSTR szTablePostfix, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagLogSilver);

	pStream->SetInsert(TableName_LogMoney, szTablePostfix);

	pStream->FillString("AccountID=") << p->dwAccountID;
	pStream->FillString(",RoleID=") << p->dwRoleID;
	pStream->FillString(",CmdID=") << p->dwCmdID;
	pStream->FillString(",ConType=") << p->n8LogConType;
	pStream->FillString(",Num=") << p->n64Silver;
	pStream->FillString(",TotalNum=") << p->n64TotalSilver;
	pStream->FillString(",RoleIDRel=") << p->dwRoleIDRel;
	pStream->FillString(",Time=now()");
}

//-----------------------------------------------------------------------------
// 元宝记录
//-----------------------------------------------------------------------------
BOOL CLoongDB::CreateLogYuanBaoTable(LPCSTR strPostfix)
{
	return CreateLogTable(TableName_LogYuanBao, strPostfix);
}

VOID CLoongDB::InsertYuanBaoLog(LPCSTR szTablePostfix, LPVOID pData)
{
	UpdateLog(szTablePostfix, pData, (&CLoongDB::FormatInsertYuanBaoLog));
}

VOID CLoongDB::FormatInsertYuanBaoLog(Beton::MyStream *pStream, LPCSTR szTablePostfix, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagLogYuanBao);

	pStream->SetInsert(TableName_LogYuanBao, szTablePostfix);

	pStream->FillString("AccountID=") << p->dwAccountID;
	pStream->FillString(",RoleID=") << p->dwRoleID;
	pStream->FillString(",CmdID=") << p->dwCmdID;
	pStream->FillString(",ConType=") << p->n8LogConType;
	pStream->FillString(",Num=") << p->nYuanBao;
	pStream->FillString(",TotalNum=") << p->nTotalYuanBao;
	pStream->FillString(",Time=now()");
}

//-----------------------------------------------------------------------------
// 赠点记录
//-----------------------------------------------------------------------------
BOOL CLoongDB::CreateLogExVolumeTable(LPCSTR strPostfix)
{
	return CreateLogTable(TableName_LogExVolume, strPostfix);
}

VOID CLoongDB::InsertExVolumeLog(LPCSTR szTablePostfix, LPVOID pData)
{
	UpdateLog(szTablePostfix, pData, (&CLoongDB::FormatInsertExVolumeLog));
}

VOID CLoongDB::FormatInsertExVolumeLog(Beton::MyStream *pStream, LPCSTR szTablePostfix, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagLogExVolume);

	pStream->SetInsert(TableName_LogExVolume, szTablePostfix);

	pStream->FillString("AccountID=") << p->dwAccountID;
	pStream->FillString(",RoleID=") << p->dwRoleID;
	pStream->FillString(",CmdID=") << p->dwCmdID;
	pStream->FillString(",Num=") << p->nExVolume;
	pStream->FillString(",TotalNum=") << p->nTotalExVolume;
	pStream->FillString(",Time=now()");
}

//-----------------------------------------------------------------------------
// 定时统计记录
//-----------------------------------------------------------------------------
BOOL CLoongDB::CreateLogTimeStatTable(LPCSTR strPostfix)
{
	return CreateLogTable(TableName_LogTimeStat, strPostfix);
}

VOID CLoongDB::InsertTimeStatLog(LPCSTR szTablePostfix, LPVOID pData)
{
	UpdateLog(szTablePostfix, pData, (&CLoongDB::FormatInsertTimeStatLog));
}

VOID CLoongDB::FormatInsertTimeStatLog(Beton::MyStream *pStream, LPCSTR szTablePostfix, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagLogTimeStat);

	pStream->SetInsert(TableName_LogTimeStat, szTablePostfix);

	pStream->FillString("AccountID=") << p->dwAccountID;
	pStream->FillString(",RoleID=") << p->dwRoleID;
	pStream->FillString(",BagSilver=") << p->n64BagSilver;
	pStream->FillString(",WareSilver=") << p->n64WareSilver;
	pStream->FillString(",BagYB=") << p->nBagYuanBao;
	pStream->FillString(",BaiBaoYB=") << p->nBaiBaoYuanBao;
	pStream->FillString(",ExVolume=") << p->nExVolume;
	pStream->FillString(",Time=now()");
}

//-----------------------------------------------------------------------------
// 物品变化记录
//-----------------------------------------------------------------------------
BOOL CLoongDB::CreateLogItemTable(LPCSTR strPostfix)
{
	return CreateLogTable(TableName_LogItem, strPostfix);
}

VOID CLoongDB::InsertItemLog(LPCSTR szTablePostfix, LPVOID pData)
{
	UpdateLog(szTablePostfix, pData, (&CLoongDB::FormatInsertItemLog));
}

VOID CLoongDB::FormatInsertItemLog(Beton::MyStream *pStream, LPCSTR szTablePostfix, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagLogItem);

	pStream->SetInsert(TableName_LogItem, szTablePostfix);

	pStream->FillString("RoleID=") << p->dwRoleID;
	pStream->FillString(",CmdID=") << p->dwCmdID;
	pStream->FillString(",TypeID=") << p->dwTypeID;
	pStream->FillString(",OptNum=") << p->n16OptNum;

	pStream->FillString(",Serial1=") << p->n64Serial1;
	pStream->FillString(",ConType1=") << p->n8ConType1;
	pStream->FillString(",ResNum1=") << p->n16ResNum1;

	pStream->FillString(",Serial2=") << p->n64Serial2;
	pStream->FillString(",ConType2=") << p->n8ConType2;
	pStream->FillString(",ResNum2=") << p->n16ResNum2;

	pStream->FillString(",RoleIDRel=") << p->dwRoleIDRel;

	pStream->FillString(",Time=now()");
}

//-----------------------------------------------------------------------------
// 物品使用次数变化记录
//-----------------------------------------------------------------------------
BOOL CLoongDB::CreateLogItemTimesTable(LPCSTR strPostfix)
{
	return CreateLogTable(TableName_LogItemTimes, strPostfix);
}

VOID CLoongDB::InsertItemTimesLog(LPCSTR szTablePostfix, LPVOID pData)
{
	UpdateLog(szTablePostfix, pData, (&CLoongDB::FormatInsertItemTimesLog));
}

VOID CLoongDB::FormatInsertItemTimesLog(Beton::MyStream *pStream, LPCSTR szTablePostfix, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagLogItemTims);

	pStream->SetInsert(TableName_LogItemTimes, szTablePostfix);

	pStream->FillString("RoleID=") << p->dwRoleID;
	pStream->FillString(",CmdID=") << p->dwCmdID;
	pStream->FillString(",Serial=") << p->n64Serial;
	pStream->FillString(",TypeID=") << p->dwTypeID;
	pStream->FillString(",ConType=") << p->n8ConType;
	pStream->FillString(",UsedTimes=") << p->nUsedTimes;
	pStream->FillString(",MaxUseTimes=") << p->nMaxUseTimes;

	pStream->FillString(",Time=now()");
}

//-----------------------------------------------------------------------------
// 商城出售商品记录
//-----------------------------------------------------------------------------
BOOL CLoongDB::CreateLogMallSellTable(LPCSTR strPostfix)
{
	return CreateLogTable(TableName_LogMallSell, strPostfix);
}

VOID CLoongDB::InsertMallSellLog(LPCSTR szTablePostfix, LPVOID pData)
{
	UpdateLog(szTablePostfix, pData, (&CLoongDB::FormatInsertMallSellLog));
}

VOID CLoongDB::FormatInsertMallSellLog(Beton::MyStream *pStream, LPCSTR szTablePostfix, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagLogMallSell);

	pStream->SetInsert(TableName_LogMallSell, szTablePostfix);

	pStream->FillString("CmdID=") << p->dwCmdID;
	pStream->FillString(",Serial=") << p->n64Serial;
	pStream->FillString(",TypeID=") << p->dwTypeID;
	pStream->FillString(",RoleID=") << p->dwRoleID;
	pStream->FillString(",Num=") << p->n16NumSell;
	pStream->FillString(",ExistTime=") << p->dwExistTime;
	pStream->FillString(",MaxUseTimes=") << p->nMaxUseTimes;
	pStream->FillString(",CostYB=") << p->nCostYuanBao;
	pStream->FillString(",CostExVolume=") << p->nCostExVolume;

	DwordTime2DataTime(m_szTime, sizeof(m_szTime), p->dwFstGainTime);
	pStream->FillString(",FstGainTime='").FillString(m_szTime);
	pStream->FillString("'");

	pStream->FillString(",Time=now()");
}

//-----------------------------------------------------------------------------
// 商城出售礼品包记录
//-----------------------------------------------------------------------------
BOOL CLoongDB::CreateLogMallSellPackTable(LPCSTR strPostfix)
{
	return CreateLogTable(TableName_LogMallSellPack, strPostfix);
}

VOID CLoongDB::InsertMallSellPackLog(LPCSTR szTablePostfix, LPVOID pData)
{
	UpdateLog(szTablePostfix, pData, (&CLoongDB::FormatInsertMallSellPackLog));
}

VOID CLoongDB::FormatInsertMallSellPackLog(Beton::MyStream *pStream, LPCSTR szTablePostfix, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagLogMallSellPack);

	pStream->SetInsert(TableName_LogMallSellPack, szTablePostfix);

	pStream->FillString("PackID=") << p->dwPackID;
	pStream->FillString(",BuyID=") << p->dwBuyRoleID;
	pStream->FillString(",ToID=") << p->dwToRoleID;
	pStream->FillString(",CostYB=") << p->nCostYuanBao;

	pStream->FillString(",Time=now()");
}

//-----------------------------------------------------------------------------
// 帮派资金记录
//-----------------------------------------------------------------------------
BOOL CLoongDB::CreateLogFundTable(LPCSTR strPostfix)
{
	return CreateLogTable(TableName_LogFund, strPostfix);
}

VOID CLoongDB::InsertFundLog(LPCSTR szTablePostfix, LPVOID pData)
{
	UpdateLog(szTablePostfix, pData, (&CLoongDB::FormatInsertFundLog));
}

VOID CLoongDB::FormatInsertFundLog(Beton::MyStream *pStream, LPCSTR szTablePostfix, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagLogFund);

	pStream->SetInsert(TableName_LogFund, szTablePostfix);

	pStream->FillString("guild_id=") << p->dwGuildID;
	pStream->FillString(",account_id=") << p->dwAccountID;
	pStream->FillString(",role_id=") << p->dwRoleID;
	pStream->FillString(",cmd_id=") << p->dwCmdID;
	pStream->FillString(",fund=") << p->nFund;
	pStream->FillString(",total_fund=") << p->nTotalFund;
	pStream->FillString(",time=now()");
}

//-----------------------------------------------------------------------------
// 帮派资材记录
//-----------------------------------------------------------------------------
BOOL CLoongDB::CreateLogMaterialTable(LPCSTR strPostfix)
{
	return CreateLogTable(TableName_LogMaterial, strPostfix);
}

VOID CLoongDB::InsertMaterialLog(LPCSTR szTablePostfix, LPVOID pData)
{
	UpdateLog(szTablePostfix, pData, (&CLoongDB::FormatInsertMaterialLog));
}

VOID CLoongDB::FormatInsertMaterialLog(Beton::MyStream *pStream, LPCSTR szTablePostfix, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagLogMaterial);

	pStream->SetInsert(TableName_LogMaterial, szTablePostfix);

	pStream->FillString("guild_id=") << p->dwGuildID;
	pStream->FillString(",account_id=") << p->dwAccountID;
	pStream->FillString(",role_id=") << p->dwRoleID;
	pStream->FillString(",cmd_id=") << p->dwCmdID;
	pStream->FillString(",material=") << p->nMaterial;
	pStream->FillString(",total_material=") << p->nTotalMaterial;
	pStream->FillString(",time=now()");
}

//-----------------------------------------------------------------------------
// 帮派声望记录
//-----------------------------------------------------------------------------
BOOL CLoongDB::CreateLogReputationTable(LPCSTR strPostfix)
{
	return CreateLogTable(TableName_LogReputation, strPostfix);
}

VOID CLoongDB::InsertReputationLog(LPCSTR szTablePostfix, LPVOID pData)
{
	UpdateLog(szTablePostfix, pData, (&CLoongDB::FormatInsertReputationLog));
}

VOID CLoongDB::FormatInsertReputationLog(Beton::MyStream *pStream, LPCSTR szTablePostfix, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagLogReputation);

	pStream->SetInsert(TableName_LogReputation, szTablePostfix);

	pStream->FillString("guild_id=") << p->dwGuildID;
	pStream->FillString(",account_id=") << p->dwAccountID;
	pStream->FillString(",role_id=") << p->dwRoleID;
	pStream->FillString(",cmd_id=") << p->dwCmdID;
	pStream->FillString(",reputation=") << p->nReputation;
	pStream->FillString(",total_reputation=") << p->nTotalReputation;
	pStream->FillString(",time=now()");
}

// GM命令log相关
BOOL CLoongDB::CreateLogGMCmd(LPCSTR strPostfix)
{
	return CreateLogTable(TableName_LogGMCmd, strPostfix);
}

VOID CLoongDB::InsertGMCmdLog(LPCSTR szTablePostfix, LPVOID pData)
{
	UpdateLog(szTablePostfix, pData, (&CLoongDB::FormatInsertGMCmdLog));
}

VOID CLoongDB::FormatInsertGMCmdLog(Beton::MyStream *pStream, LPCSTR szTablePostfix, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagLogGMCmd);

	pStream->SetInsert(TableName_LogGMCmd, szTablePostfix);

	pStream->FillString("role_id=") << p->dwRoleID;
	pStream->FillString(",gm_cmd='").FillString(p->szGMCmd).FillString("'");
	pStream->FillString(",error_code=") << p->dwErrorCode;
	pStream->FillString(",time=now()");
}

// 宠物log相关
BOOL CLoongDB::CreateLogPet(LPCSTR strPostfix)
{
	return CreateLogTable(TableName_LogPet, strPostfix);
}

VOID CLoongDB::InsertPetLog(LPCSTR szTablePostfix, LPVOID pData)
{
	UpdateLog(szTablePostfix, pData, (&CLoongDB::FormatInsertPetLog));
}

VOID CLoongDB::FormatInsertPetLog(Beton::MyStream *pStream, LPCSTR szTablePostfix, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagLogPet);

	pStream->SetInsert(TableName_LogPet, szTablePostfix);

	pStream->FillString("master_id=") << p->dwMasterID;
	pStream->FillString(",pet_id=") << p->dwPetID;
	pStream->FillString(",cmd_id=") << p->dwCmdID;
}
