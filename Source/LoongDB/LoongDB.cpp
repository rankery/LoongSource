//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: LoongDB.cpp
// author: Sxg
// actor:
// data: 2008-05-12
// last:
// brief: 数据库操作应用层实现
//-------------------------------------------------------------------------------------------------------
#include "StdAfx.h"
#include "../WorldDefine/RoleDefine.h"
#include "../ServerDefine/role_data.h"
#include "../ServerDefine/msg_mall.h"
#include "../ServerDefine/base_define.h"
#include "../ServerDefine/msg_leftmsg.h"
#include "../ServerDefine/item_define.h"
#include "LoongDB.h"
#include "../WorldDefine/msg_external_links.h"

//-------------------------------------------------------------------------------------------------------
// 构造函数
//-------------------------------------------------------------------------------------------------------
CLoongDB::CLoongDB()
{
	m_pDBLoong = new Beton::DataBase;
	m_pDBLog = new Beton::DataBase;

	m_bInitOK = FALSE;

	m_dwMaxPetID	= MIN_PET_ID;
	m_nPetNum		= 0;

	m_dwMaxRoleID	= 0;
	m_nRoleNum		= 0;
	m_nLoadNumLim	= 0;

	m_dwReadTimes	= 0;
	m_dwWriteTimes	= 0;
}

//-------------------------------------------------------------------------------------------------------
// 析构函数
//-------------------------------------------------------------------------------------------------------
CLoongDB::~CLoongDB()
{
	SAFE_DEL(m_pDBLoong);
	SAFE_DEL(m_pDBLog);
}


//-------------------------------------------------------------------------------------------------------
// 读取配置文件,并初始化数据库连接
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::Init(LPCTSTR szIniFileName)
{
	// 创建容器,并载入配置文件
	CreateObj("LoongDBVar", "VarContainer");
	TObjRef<VarContainer> pVar = "LoongDBVar";
	if(!P_VALID(pVar))
	{
		ERR(_T("Create LoongDBVar(VarContainer) obj failed!"));
		return FALSE;
	}
	pVar->Load("VirtualFileSys", szIniFileName);

	// 创建并初始化游戏数据库引擎
	/*if(!m_pDBLoong->Init(_T("172.17.1.201"), _T("root"), _T("kungfu"), _T("loong_base"), 3306))*/
	if(!m_pDBLoong->Init(pVar->GetString(_T("host_name db_game")),
						pVar->GetString(_T("user_name db_game")), 
						pVar->GetString(_T("password db_game")), 
						pVar->GetString(_T("db_name db_game")), 
						(INT)pVar->GetDword(_T("port db_game"))))
	{
		ERR(_T("Game Database initialize failed! Please check profile..."));
		SAFE_DEL(m_pDBLoong);
		return FALSE;
	}

	// 创建并初始化log数据库引擎
	if(!m_pDBLog->Init(pVar->GetString(_T("host_name db_log")), 
						pVar->GetString(_T("user_name db_log")), 
						pVar->GetString(_T("password db_log")), 
						pVar->GetString(_T("db_name db_log")), 
						(INT)pVar->GetDword(_T("port db_log"))))
	{
		ERR(_T("Log Database initialize failed! Please check profile..."));
		SAFE_DEL(m_pDBLoong);
		SAFE_DEL(m_pDBLog);
		return FALSE;
	}

	// 清空容器
	pVar->Clear();
	// 销毁容器
	KillObj("LoongDBVar");

	GetRoleCountAndMaxRoleID(m_nRoleNum, m_dwMaxRoleID);
	GetYBAccountNum(m_nAccountNum);
	GetYBOrderNum(m_nOrderNum);
	GetActivityNum(m_nActivityNum);
	GetMaxOrderIndex(m_dwMaxOrderIndex);	
	GetPetCountAndMaxPetID(m_nPetNum, m_dwMaxPetID);
	GetGPInfoNumAndSize(m_nGroupPurchaseInfoNum, m_nGroupPurchaseInfoSize);

	m_dwReadTimes	= 0;
	m_dwWriteTimes	= 0;

	m_bInitOK = TRUE;

	return TRUE;
}

//-------------------------------------------------------------------------------------------------------
// 资源回收函数
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::Destroy()
{
	if(m_bInitOK)
	{
		m_dwReadTimes	= 0;
		m_dwWriteTimes	= 0;
		m_bInitOK		= FALSE;

		m_pDBLoong->ShutDown();
		m_pDBLog->ShutDown();

		return;
	}
}

//-------------------------------------------------------------------------------------------------------
// 获取roledata记录条数和最大RoleID
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::GetRoleCountAndMaxRoleID(INT32 &nCount, DWORD &dwMaxRoleID)
{
	nCount = 0;
	dwMaxRoleID = 0;
	
	// 获取流
	Beton::MyStream* pStream = m_pDBLoong->GetStream();

	// 格式化
	pStream->SetSelect("roledata", "count(RoleID),max(RoleID)");

	// 查询数据库
	Beton::QueryResult* pResult = m_pDBLoong->Query(pStream);
	if(P_VALID(pResult) && pResult->GetRowCount() > 0)
	{
		nCount = (*pResult)[0].GetInt();
		dwMaxRoleID = (*pResult)[1].GetDword();
	}

	++m_dwReadTimes;

	// 释放流和结果
	m_pDBLoong->ReturnStream(pStream);
	m_pDBLoong->FreeQueryResult(pResult);
}

//-------------------------------------------------------------------------------------------------------
// 获取roledata记录条数和最大RoleID
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::GetMaxMinSerial(INT64 &n64Max, INT64 &n64Min)
{
	INT64 n64MaxSerial = MIN_ITEM_SERIAL_INTERNAL;
	INT64 n64MinSerial = MAX_ITEM_SERIAL_OUTTER;

	n64Max = n64MaxSerial;
	n64Min = n64MinSerial;

	GetMaxMinSerial("item", n64MaxSerial, n64MinSerial);
	n64Max = max(n64MaxSerial, n64Max);
	n64Min = min(n64MinSerial, n64Min);

	GetMaxMinSerial("item_baibao", n64MaxSerial, n64MinSerial);
	n64Max = max(n64MaxSerial, n64Max);
	n64Min = min(n64MinSerial, n64Min);

	GetMaxMinSerial("item_del", n64MaxSerial, n64MinSerial);
	n64Max = max(n64MaxSerial, n64Max);
	n64Min = min(n64MinSerial, n64Min);
}


//-------------------------------------------------------------------------------------------------------
// 获取roledata记录条数和最大RoleID
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::GetMaxMinSerial(const char* pTable, INT64 &n64Max, INT64 &n64Min)
{
	// 获取流
	Beton::MyStream* pStream = m_pDBLoong->GetStream();

	// 格式化
	pStream->SetSelect(pTable, "max(SerialNum),min(SerialNum)");
	pStream->SetWhere();
	pStream->FillString("SerialNum>") << 0;

	// 查询数据库
	Beton::QueryResult* pResult = m_pDBLoong->Query(pStream);
	if(P_VALID(pResult) && pResult->GetRowCount() > 0)
	{
		n64Max = (*pResult)[0].GetLong();
		n64Min = (*pResult)[1].GetLong();
	}

	++m_dwReadTimes;

	// 释放流和结果
	m_pDBLoong->ReturnStream(pStream);
	m_pDBLoong->FreeQueryResult(pResult);
}

//-------------------------------------------------------------------------------------------------------
// 创建角色
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::CreateRole(OUT DWORD &dwNewRoleID, DWORD dwAccountID, 
						  const tagCreateRoleInfo &RoleInfo, DWORD &dwRoleNameCrc)
{
	// 获取流和连接
	Beton::MyStream* pStream = m_pDBLoong->GetStream();
	Beton::Connection* pCon = m_pDBLoong->GetFreeConnection();

	// 格式化角色数据
	pStream->SetInsert("roledata");
	pStream->FillString("AccountID=") << dwAccountID;
	pStream->FillString(",RoleID=") << m_dwMaxRoleID + 1;
	pStream->FillString(",RoleName='").FillString(RoleInfo.szRoleName, pCon);
	
	// 转成小写计算CRC32
	_tcscpy_s(m_szRoleName, sizeof(m_szRoleName), RoleInfo.szRoleName);
	_tcslwr(m_szRoleName);
	dwRoleNameCrc = m_pUtil->Crc32(m_szRoleName);
	pStream->FillString("',RoleNameCrc=") << dwRoleNameCrc;

	pStream->FillString(",Sex=") << RoleInfo.Avatar.bySex;
	pStream->FillString(",HairModelID=") << RoleInfo.Avatar.wHairMdlID;
	pStream->FillString(",HairColorID=") << RoleInfo.Avatar.wHairTexID;
	pStream->FillString(",FaceModelID=") << RoleInfo.Avatar.wFaceMdlID;
	pStream->FillString(",FaceDetailID=") << RoleInfo.Avatar.wFaceDetailTexID;
	pStream->FillString(",DressModelID=") << RoleInfo.Avatar.wDressMdlID;

	pStream->FillString(",MapID=") << RoleInfo.dwMapID;
	pStream->FillString(",X=") << RoleInfo.fX;			// 坐标
	pStream->FillString(",Y=") << RoleInfo.fY;
	pStream->FillString(",Z=") << RoleInfo.fZ;
	pStream->FillString(",FaceX=") << RoleInfo.fFaceX;	// 朝向
	pStream->FillString(",FaceY=") << RoleInfo.fFaceY;
	pStream->FillString(",FaceZ=") << RoleInfo.fFaceZ;

	pStream->FillString(",Level=") << RoleInfo.byLevel;

	// LoongDB设置时间
	if(DwordTime2DataTime(m_szTime, sizeof(m_szTime), RoleInfo.CreateTime))
	{
		pStream->FillString(",CreateTime='").FillString(m_szTime);
		pStream->FillString("'");
	}
	else
	{
		IMSG(_T("Error: Func DwordTime2DataTime() run error in CLoongDB::CreateRole()!!!!!\r\n"));
		ASSERT(0);
	}

	// 插入到数据库
	BOOL bRet = m_pDBLoong->Execute(pStream);

	// 释放流和连接
	m_pDBLoong->ReturnConnection(pCon);
	m_pDBLoong->ReturnStream(pStream);

	if(bRet)
	{
		++m_dwMaxRoleID;
		++m_nRoleNum;
		dwNewRoleID = m_dwMaxRoleID;
	}

	++m_dwWriteTimes;

	return bRet;
}

//-------------------------------------------------------------------------------------------------------
// 恢复角色
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::ResumeRole(DWORD dwRoleID)
{
	//?? 目前不提供支持
	return TRUE;
}

//-------------------------------------------------------------------------------------------------------
// 备份角色相关数据//??其它相关表数据
//-------------------------------------------------------------------------------------------------------
DWORD CLoongDB::BackupRole(DWORD dwAccountID, DWORD dwRoleID)
{
	DWORD dwErrorCode;

	// 备份roledata表中相关数据
	dwErrorCode = BackupRoleData(dwAccountID, dwRoleID);
	if(E_Success != dwErrorCode)
	{
		return dwErrorCode;
	}

	// 备份item表中相关数据
	dwErrorCode = BackupItem(dwRoleID);
	if(E_Success != dwErrorCode)
	{
		return dwErrorCode;
	}

	return dwErrorCode;
}


//********************************** 备份数据(部分表，删除角色用) ***************************************

//-------------------------------------------------------------------------------------------------------
// 备份角色属性信息
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::BackupRoleData(DWORD dwAccountID, DWORD dwRoleID)
{
	BOOL bRet = TRUE;
	
	// 获取流
	Beton::MyStream* pStream = m_pDBLoong->GetStream();
	ASSERT(P_VALID(pStream));

	// 格式化数据库操作语句
	pStream->Clear();
	pStream->FillString("insert into roledata_del select * from roledata where AccountID=") << dwAccountID;
	pStream->FillString(" and RoleID=") << dwRoleID;

	// 执行
	bRet = m_pDBLoong->Execute(pStream);

	// 释放流
	m_pDBLoong->ReturnStream(pStream);

	++m_dwWriteTimes;

	return bRet;
}

//-------------------------------------------------------------------------------------------------------
// 备份角色物品信息
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::BackupItem(DWORD dwRoleID)
{
	BOOL bRet = TRUE;
	
	// 获取流
	Beton::MyStream* pStream = m_pDBLoong->GetStream();
	ASSERT(P_VALID(pStream));

	// 格式化数据库操作语句
	pStream->Clear();
	pStream->FillString("insert into item_del select * from item where OwnerID=") << dwRoleID;

	// 执行
	bRet = m_pDBLoong->Execute(pStream);
	
	// 释放流
	m_pDBLoong->ReturnStream(pStream);

	++m_dwWriteTimes;

	return bRet;
}


//************************************* 删除角色所有相关数据 ********************************************

//-------------------------------------------------------------------------------------------------------
// 删除角色//??其它相关表数据
//-------------------------------------------------------------------------------------------------------
DWORD CLoongDB::DelRole(DWORD dwAccountID, DWORD dwRoleID)
{
	if(DelRoleData(dwAccountID, dwRoleID))
	{
		// 删除buff或物品若失败, 不做处理//??
		DelRoleBuff(dwRoleID);
		DelRoleItem(dwRoleID);
	}
	else
	{
		return E_DBDelete_RoleFailed;
	}

	return E_Success;
}

//-------------------------------------------------------------------------------------------------------
// 删除角色属性信息
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::DelRoleData(DWORD dwAccountID, DWORD dwRoleID)
{
	BOOL bRet = TRUE;

	// 获取流
	Beton::MyStream* pStream = m_pDBLoong->GetStream();
	ASSERT(P_VALID(pStream));

	// 格式化数据库操作语句
	pStream->SetDelete("roledata");
	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;
	pStream->FillString(" and AccountID=") << dwAccountID;

	// 执行
	bRet = m_pDBLoong->Execute(pStream);

	// 释放流
	m_pDBLoong->ReturnStream(pStream);

	++m_dwWriteTimes;

	return bRet;
}

//-------------------------------------------------------------------------------------------------------
// 删除角色buff信息
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::DelRoleBuff(DWORD dwRoleID)
{
	BOOL bRet = TRUE;

	// 获取流
	Beton::MyStream* pStream = m_pDBLoong->GetStream();
	ASSERT(P_VALID(pStream));

	// 格式化数据库操作语句
	pStream->SetDelete("buff");
	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;

	// 执行
	bRet = m_pDBLoong->Execute(pStream);

	// 释放流
	m_pDBLoong->ReturnStream(pStream);

	++m_dwWriteTimes;

	return bRet;
}

//-------------------------------------------------------------------------------------------------------
// 删除角色物品信息
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::DelRoleItem(DWORD dwRoleID)
{
	BOOL bRet = TRUE;

	// 获取流
	Beton::MyStream* pStream = m_pDBLoong->GetStream();
	ASSERT(P_VALID(pStream));

	// 格式化数据库操作语句
	pStream->SetDelete("item");
	pStream->SetWhere();
	pStream->FillString("OwnerID=") << dwRoleID;
	pStream->FillString(" and (ContainerTypeID!=") << EICT_RoleWare;
	pStream->FillString(" or (ContainerTypeID=") << EICT_RoleWare;
	pStream->FillString(" and Bind=") << EBS_Bind;
	pStream->FillString("))");

	// 执行
	bRet = m_pDBLoong->Execute(pStream);

	// 释放流
	m_pDBLoong->ReturnStream(pStream);

	++m_dwWriteTimes;

	return bRet;
}


//-------------------------------------------------------------------------------------------------------
// 记录删除角色时的角色名称，Crc，时间及IP等
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::RecordInfoDelRole(DWORD dwAccountID, DWORD dwRoleID, LPCTSTR szRoleName, LPCSTR szIP)
{
	BOOL bRet = TRUE;

	// 获取流和连接
	Beton::MyStream* pStream = m_pDBLoong->GetStream();
	Beton::Connection* pCon = m_pDBLoong->GetFreeConnection();
	ASSERT(P_VALID(pStream));
	ASSERT(P_VALID(pCon));
	
	// 记录删除操作相关信息
	pStream->SetInsert("role_del");
	pStream->FillString("AccountID=") << dwAccountID;
	pStream->FillString(",RoleName='").FillString(szRoleName, pCon);
	pStream->FillString("',RoleID=") << dwRoleID;
	pStream->FillString(",IP='").FillString(szIP);
	pStream->FillString("',DeleteTime=now()");

	// 执行
	bRet = m_pDBLoong->Execute(pStream);

	// 释放流和连接
	m_pDBLoong->ReturnConnection(pCon);
	m_pDBLoong->ReturnStream(pStream);

	++m_dwWriteTimes;

	return bRet;
}

//-------------------------------------------------------------------------------------------------------
// 物品冷却
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::ReplaceItemCDTime(DWORD dwRoleID, LPVOID pData, INT32 nNum, OUT LPVOID *ppOutPointer)
{
	BOOL bRet = TRUE;

	// 获取流和连接
	Beton::MyStream* pStream = m_pDBLoong->GetStream();
	Beton::Connection* pCon = m_pDBLoong->GetFreeConnection();

	*ppOutPointer = pData;

	// 格式化数据
	pStream->SetReplace("item_cdtime");
	pStream->FillString("RoleID=") << dwRoleID;
	if(nNum > 0)
	{
		pStream->FillString(",CDTime='");
		pStream->FillBlob(pData, sizeof(tagCDTime) * nNum, pCon);
		pStream->FillString("'");

		*ppOutPointer = (BYTE*)pData + sizeof(tagCDTime) * nNum;
	}

	// 执行
	bRet = m_pDBLoong->Execute(pStream);

	// 释放流和连接
	m_pDBLoong->ReturnConnection(pCon);
	m_pDBLoong->ReturnStream(pStream);

	++m_dwWriteTimes;

	return bRet;
}

//-------------------------------------------------------------------------------------------------------
// 获取YBAccount记录的数量
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::GetYBAccountNum(INT32 &nAccountNum)
{
	nAccountNum = 0;

	// 获取流
	Beton::MyStream* pStream = m_pDBLoong->GetStream();

	// 格式化
	pStream->SetSelect("ybaccount", "count(RoleID)");

	// 查询数据库
	Beton::QueryResult* pResult = m_pDBLoong->Query(pStream);
	if(P_VALID(pResult) && pResult->GetRowCount() > 0)
	{
		nAccountNum = (*pResult)[0].GetInt();
	}

	++m_dwReadTimes;

	// 释放流和结果
	m_pDBLoong->ReturnStream(pStream);
	m_pDBLoong->FreeQueryResult(pResult);
}

//-------------------------------------------------------------------------------------------------------
// 获取yuanbaoorder的委托订单记录的数量
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::GetYBOrderNum(INT32 &nOrderNum)
{
	nOrderNum = 0;

	// 获取流
	Beton::MyStream* pStream = m_pDBLoong->GetStream();

	// 格式化
	pStream->SetSelect("yuanbaoorder", "count(dwID)");
	pStream->SetWhere();
	pStream->FillString("OrderMode=") << 0;

	// 查询数据库
	Beton::QueryResult* pResult = m_pDBLoong->Query(pStream);
	if(P_VALID(pResult) && pResult->GetRowCount() > 0)
	{
		nOrderNum = (*pResult)[0].GetInt();
	}

	++m_dwReadTimes;

	// 释放流和结果
	m_pDBLoong->ReturnStream(pStream);
	m_pDBLoong->FreeQueryResult(pResult);
}

//-------------------------------------------------------------------------------------------------------
// 获取一个玩家yuanbaoorder记录的数量
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::GetRoleYBOrderNum(DWORD dwRoleID, INT32 &nOrderNum)
{
	nOrderNum = 0;

	// 获取流
	Beton::MyStream* pStream = m_pDBLoong->GetStream();

	// 格式化
	pStream->SetSelect("yuanbaoorder", "count(dwID)");
	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;

	// 查询数据库
	Beton::QueryResult* pResult = m_pDBLoong->Query(pStream);
	if(P_VALID(pResult) && pResult->GetRowCount() > 0)
	{
		nOrderNum = (*pResult)[0].GetInt();
	}

	++m_dwReadTimes;

	// 释放流和结果
	m_pDBLoong->ReturnStream(pStream);
	m_pDBLoong->FreeQueryResult(pResult);
}

//-------------------------------------------------------------------------------------------------------
// 获得固定活动数量
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::GetActivityNum(INT32 &nActivityNum)
{
	nActivityNum = 0;

	// 获取流
	Beton::MyStream* pStream = m_pDBLoong->GetStream();

	// 格式化
	pStream->SetSelect("activity", "count(dwID)");

	// 查询数据库
	Beton::QueryResult* pResult = m_pDBLoong->Query(pStream);
	if(P_VALID(pResult) && pResult->GetRowCount() > 0)
	{
		nActivityNum = (*pResult)[0].GetInt();
	}

	++m_dwReadTimes;

	// 释放流和结果
	m_pDBLoong->ReturnStream(pStream);
	m_pDBLoong->FreeQueryResult(pResult);
}

//-------------------------------------------------------------------------------------------------------
// 获取一个玩家yuanbaoorder记录最大Index
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::GetMaxOrderIndex( DWORD& dwMaxIndex )
{
	dwMaxIndex	= 0;

	// 获取流
	Beton::MyStream* pStream = m_pDBLoong->GetStream();

	// 格式化
	pStream->SetSelect("yuanbaoorder", "max(dwID)");

	// 查询数据库
	Beton::QueryResult* pResult = m_pDBLoong->Query(pStream);
	if(P_VALID(pResult) && pResult->GetRowCount() > 0)
	{
		dwMaxIndex = (*pResult)[0].GetDword();
	}

	++m_dwReadTimes;

	// 释放流和结果
	m_pDBLoong->ReturnStream(pStream);
	m_pDBLoong->FreeQueryResult(pResult);
}

VOID CLoongDB::GetPetCountAndMaxPetID( INT32 &nCount, DWORD &dwMaxPetID )
{
	nCount = 0;
	dwMaxPetID = MIN_PET_ID;

	// 获取流
	Beton::MyStream* pStream = m_pDBLoong->GetStream();

	// 格式化
	pStream->SetSelect("pet_data", "count(pet_id),max(pet_id)");

	// 查询数据库
	Beton::QueryResult* pResult = m_pDBLoong->Query(pStream);
	if(P_VALID(pResult) && pResult->GetRowCount() > 0)
	{
		nCount = (*pResult)[0].GetInt();
		dwMaxPetID = (*pResult)[1].GetDword();
		if (!IS_PET(dwMaxPetID))
		{
			dwMaxPetID = MIN_PET_ID;
		}
	}

	++m_dwReadTimes;

	// 释放流和结果
	m_pDBLoong->ReturnStream(pStream);
	m_pDBLoong->FreeQueryResult(pResult);
}

//-------------------------------------------------------------------------------------------------------
// 获取所有帮派团购信息的数量
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::GetGPInfoNumAndSize( INT32 &nGPInfoNum, INT32 &nDataSize )
{
	nGPInfoNum	= 0;
	nDataSize	= 0;

	// 获取流
	Beton::MyStream* pStream = m_pDBLoong->GetStream();

	// 格式化
	pStream->SetSelect("group_purchase", "ParticipatorList");

	// 查询数据库
	Beton::QueryResult* pResult = m_pDBLoong->Query(pStream);
	if(P_VALID(pResult) && pResult->GetRowCount() > 0)
	{
		nGPInfoNum = pResult->GetRowCount();
		nDataSize = (sizeof(tagGPInfo)-sizeof(DWORD)) * nGPInfoNum;

		for (int i=0; i<nGPInfoNum; i++)
		{
			nDataSize += (*pResult)[0].GetLen();

			pResult->NextRow();
		}
	}

	++m_dwReadTimes;

	// 释放流和结果
	m_pDBLoong->ReturnStream(pStream);
	m_pDBLoong->FreeQueryResult(pResult);
}

BOOL CLoongDB::LoadLeftMsg( DWORD dwRoleID, PVOID pLeftMsg, DWORD &dwSize )
{
	INT nMsgNum = 0;
	PVOID pData = pLeftMsg;
	BOOL bRtv = Load(pData, &nMsgNum, GT_INVALID, dwRoleID, &CLoongDB::FormatLoadLeftMsg, &CLoongDB::ProcResLoadLeftMsg);
	if (bRtv)
	{
		dwSize = (BYTE*)pData - (BYTE*)pLeftMsg;
	}
	return bRtv;
}

VOID CLoongDB::FormatLoadLeftMsg( Beton::MyStream *pStream, DWORD dwDumy, DWORD dwRoleID )
{
	pStream->Clear();
	pStream->FillString("select msg_data from left_msg where roleid=") << dwRoleID;
	pStream->FillString(" order by msg_id asc");
}

VOID CLoongDB::ProcResLoadLeftMsg( OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes )
{
	if(!P_VALID(pRes) || pRes->GetRowCount() <= 0)
		return;

	INT nRowNum = pRes->GetRowCount();

	INT i;

	for (i=0; i<nRowNum; ++i)
	{
		UINT uLen = (*pRes)[0].GetLen();

		BOOL bRtv = (*pRes)[0].GetBlob(pData, uLen);

		if (!bRtv)
			continue;

		pData = (BYTE*) pData + uLen;

		pRes->NextRow();
	}

	if (P_VALID(pNum))
	{
		*pNum = i;
	}
}

BOOL CLoongDB::DelLeftMsg( DWORD dwRoleID, DWORD dwDateTime )
{
	return Update(dwRoleID, (LPVOID)&dwDateTime, 1, sizeof(DWORD), &CLoongDB::FormatDelLeftMsg, NULL);
}

VOID CLoongDB::FormatDelLeftMsg( Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pDumy )
{
	DWORD dwDateTime = *((DWORD*)pDumy);
	pStream->Clear();
	pStream->FillString("delete from left_msg where roleid=") << dwRoleID;
	pStream->FillString(" and msg_id=") << dwDateTime;
}

BOOL CLoongDB::InsertLeftMsg( DWORD dwRoleID, DWORD dwDateTime, PVOID pLeftMsg )
{
	return Update(dwRoleID, pLeftMsg, 1, 0,&CLoongDB::FormatInsertLeftMsg, NULL); 
}

VOID CLoongDB::FormatInsertLeftMsg( Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData )
{
	MTRANS_POINTER(pMsgInsert, pData, tagDBLeftMsg);
	Beton::Connection* con = m_pDBLoong->GetFreeConnection();
	pStream->Clear();
	pStream->FillString("insert into left_msg set roleid=") << pMsgInsert->dwRoleID;
	pStream->FillString(", msg_id=")<< pMsgInsert->dwDateTime;
	tagNetCmd* pCmd = (tagNetCmd*)(pMsgInsert->byData);
	pStream->FillString(", msg_data='");
	pStream->FillBlob(pCmd, pCmd->dwSize, con);
	pStream->FillString("'");
	m_pDBLoong->ReturnConnection(con);
}

VOID CLoongDB::FormatClearLeftMsg( Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData )
{
	pStream->Clear();
	pStream->FillString("delete from left_msg where roleid=")<<dwRoleID;
}

BOOL CLoongDB::ClearLeftMsg( DWORD dwRoleID )
{
	return Update(dwRoleID, NULL, 1, 0, &CLoongDB::FormatClearLeftMsg);
}

BOOL CLoongDB::LoadLeftMsgIndexes( DWORD* pIndexes, DWORD dwRoleID, INT &nNum )
{
	LPVOID pData = (LPVOID)pIndexes;
	return Load(pData, &nNum, GT_INVALID, dwRoleID, &CLoongDB::FormatLoadLeftMsgIndexes, &CLoongDB::ProcResLoadLeftMsgIndexes);
}

VOID CLoongDB::FormatLoadLeftMsgIndexes( Beton::MyStream *pStream, DWORD dwDumy, DWORD dwRoleID )
{
	pStream->FillString("select msg_id from left_msg where roleid=") << dwRoleID;
	pStream->FillString(" order by msg_id asc");
}

VOID CLoongDB::ProcResLoadLeftMsgIndexes( OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes )
{
	if (!P_VALID(pRes))
		return;

	DWORD* pIndexes = (DWORD*)pData;

	INT nNum = pRes->GetRowCount();
	INT i;
	for (i=0; i<nNum; ++i)
	{
		pIndexes[i] = (*pRes)[0].GetDword();
		pRes->NextRow();
	}
	if (P_VALID(pNum))
	{
		*pNum = i;
	}
}

BOOL CLoongDB::UpdateRoleAtt( DWORD dwRoleID, PVOID pData )
{
	return Update(dwRoleID, pData, 1, sizeof(tagNDBC_UpdateRoleAtt), &CLoongDB::FormateUpdateRoleAtt);
}

BOOL CLoongDB::UpdateRoleAttPoint( DWORD dwRoleID, LPVOID pMsg )
{
	return Update(dwRoleID, pMsg, 1, sizeof(tagNDBC_UpdateRoleAttPoint), &CLoongDB::FormateUpdateRoleAttPoint);
}

BOOL CLoongDB::UpdateRoleTalentPoint( DWORD dwRoleID, LPVOID pMsg )
{
	return Update(dwRoleID, pMsg, 1, sizeof(tagNDBC_UpdateRoleTalentPoint), &CLoongDB::FormateUpdateRoleTalentPoint);
}
BOOL CLoongDB::UpdateRoleItemTranport( DWORD dwRoleID, LPVOID pMsg )
{
	return Update(dwRoleID, pMsg, 1, sizeof(tagNDBC_UpdateRoleItemTransport), &CLoongDB::FormateUpdateRoleItemTransport);
}
VOID CLoongDB::FormateUpdateRoleAtt( Beton::MyStream* pStream, DWORD dwRoleID, PVOID pData )
{
	MTRANS_POINTER(pRecv, pData, tagNDBC_UpdateRoleAtt);

	pStream->Clear();
	pStream->SetUpdate("roledata");
	switch(pRecv->nType)
	{
	case ERTSA_Exp:
		pStream->FillString("ExpCurLevel");
		break;
	case ERTSA_Level:
		pStream->FillString("Level");
		break;
	default:
		ASSERT(0);
		break;
	}
	pStream->FillString("=") << pRecv->nValue;
	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;
}

VOID CLoongDB::FormateUpdateRoleAttPoint( Beton::MyStream* pStream, DWORD dwRoleID, PVOID pData )
{
	MTRANS_POINTER(pRecv, pData, tagNDBC_UpdateRoleAttPoint);

	pStream->Clear();
	pStream->SetUpdate("roledata");

	pStream->FillString("AttPtAvail=") << pRecv->nAttPointLeft;

	pStream->FillString(",PhysiqueAdded=") << pRecv->nAttPointAdd[0];
	pStream->FillString(",StrengthAdded=") << pRecv->nAttPointAdd[1];
	pStream->FillString(",PneumaAdded=") << pRecv->nAttPointAdd[2];
	pStream->FillString(",InnerforceAdded=") << pRecv->nAttPointAdd[3];
	pStream->FillString(",TechniqueAdded=") << pRecv->nAttPointAdd[4];
	pStream->FillString(",AgilityAdded=") << pRecv->nAttPointAdd[5];

	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;
}

VOID CLoongDB::FormateUpdateRoleTalentPoint( Beton::MyStream* pStream, DWORD dwRoleID, PVOID pData )
{
	MTRANS_POINTER(pRecv, pData, tagNDBC_UpdateRoleTalentPoint);
	pStream->Clear();
	pStream->SetUpdate("roledata");

	std::stringstream str;

	str << "TalentType" << pRecv->nIndex+1 << "=" << pRecv->Talent.eType;
	str << ",TalentVal" << pRecv->nIndex+1 << "=" << pRecv->Talent.nPoint;
	pStream->FillString(str.str().c_str());

	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;
}
VOID CLoongDB::FormateUpdateRoleItemTransport( Beton::MyStream* pStream, DWORD dwRoleID, PVOID pData )
{
	MTRANS_POINTER(pRecv, pData, tagNDBC_UpdateRoleItemTransport);
	pStream->Clear();
	pStream->SetUpdate("roledata");
	pStream->FillString("ItemTransportMapID=") << pRecv->dwMapID;
	pStream->FillString(",ItemTransportX=") << pRecv->fX;
	pStream->FillString(",ItemTransportZ=") << pRecv->fZ;
	pStream->FillString(",ItemTransportY=") << pRecv->fY;
	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;
}
INT32 CLoongDB::TestDBReadFunction()
{
	// 获取流
	Beton::MyStream* pStream = m_pDBLoong->GetStream();

	// 格式化
	pStream->SetSelect("roledata", "*");
	pStream->SetWhere();
	pStream->FillString("RoleID=") << m_dwMaxRoleID;

	// 查询数据库
	Beton::QueryResult* pResult = m_pDBLoong->Query(pStream);
	if(P_VALID(pResult) && pResult->GetRowCount() > 0)
	{

	}
	else
	{
		// 释放流和结果
		m_pDBLoong->ReturnStream(pStream);
		m_pDBLoong->FreeQueryResult(pResult);
		return E_SystemError;
	}

	// 释放流和结果
	m_pDBLoong->ReturnStream(pStream);
	m_pDBLoong->FreeQueryResult(pResult);

	return E_Success;
}

BOOL CLoongDB::LoadRoleItemSerial( DWORD dwRoleID, PVOID &pData, INT &nNum, DWORD dwConType )
{
	return Load(pData, &nNum, dwConType, dwRoleID, &CLoongDB::FormatLoadRoleItemSerials, &CLoongDB::ProcResLoadRoleItemSerials);
}

// Jason 外部链接 v1.3.1
BOOL CLoongDB::LoadExtLinks(tagExternalLink *& links,int & num)
{
	BOOL bRet = TRUE;
	num = 0;
	do 
	{
		Beton::MyStream* pStream = m_pDBLoong->GetStream();
		//ASSERT(P_VALID(pStream));
		if( !P_VALID(pStream) )
			break;

		pStream->Clear();

		pStream->FillString  ("select LinkName,LinkURL from external_links order by ID desc");

		// 查询数据库
		Beton::QueryResult* pResult = m_pDBLoong->Query(pStream);
		if(!P_VALID(pResult))
		{
			bRet = FALSE;
			m_pDBLoong->ReturnStream(pStream);
			break;
		}

		num = pResult->GetRowCount();
		if (num > 0)
		{
			void * p = g_MemPoolSafe.Alloc  ( sizeof(tagExternalLink) * num );
			tagExternalLink * pLink = (tagExternalLink *)p;
			links = pLink;
			int l = 8;

			LPCTSTR pStr = NULL;
			
			Util util;
			num = num > 6 ? 6 : num;
			for(int i = 0; i < num ; ++i)
			{
				pStr = util.Unicode8ToUnicode  ((*pResult)[0].GetString());
				if( _tcslen(pStr) > 8 - 1 )
					l = 8 - 1;
				else
					l = _tcslen(pStr);
				_tcsncpy(pLink[i].linkName , pStr, l) ;
				pStr = util.Unicode8ToUnicode  ((*pResult)[1].GetString());
				if( _tcslen(pStr) > X_LONG_NAME - 1 )
					l = X_LONG_NAME - 1;
				else
					l = _tcslen(pStr);
				_tcsncpy(pLink[i].linkValue , pStr, l) ;

				pResult->NextRow  ();
			}
		}
		else
		{
			bRet = FALSE;
		}

		// 释放流和结果
		m_pDBLoong->ReturnStream(pStream);
		m_pDBLoong->FreeQueryResult(pResult);

	} while (0);

	return bRet;
}

VOID CLoongDB::FormatLoadRoleItemSerials( Beton::MyStream *pStream, DWORD dwMisc, DWORD dwRoleID )
{
	DWORD dwConType = dwMisc & 0xff;
	DWORD dwBind	= (dwMisc >> 16) & 0xff;

	pStream->Clear();
	pStream->SetSelect("item", "SerialNum, TypeID");
	pStream->SetWhere();
	pStream->FillString("OwnerID=") << dwRoleID;
	pStream->FillString(" and ContainerTypeID=") << dwConType;
	pStream->FillString(" and Bind=") << dwBind;

}

VOID CLoongDB::ProcResLoadRoleItemSerials( OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes )
{
	if (!P_VALID(pRes))
		return;

	tagItemData* pItemData = (tagItemData*)pData;

	INT nNum = pRes->GetRowCount();
	INT i = 0;
	for (i=0; i<nNum; ++i)
	{
		pItemData[i].n64Serial = (*pRes)[0].GetLong();
		pItemData[i].dwTypeID = (*pRes)[1].GetDword();

		pRes->NextRow();
	}

	if (P_VALID(pNum))
	{
		*pNum = i;
	}	
}