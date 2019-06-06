//------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: loongdb_role.cpp
// author: Sxg
// actor:
// data: 2009-05-04
// last:
// brief: 数据库操作应用层实现 -- 角色表(roledata)操作相关
//------------------------------------------------------------------------------
#include "StdAfx.h"

#include "LoongDB.h"
//-------------------------------------------------------------------------------------------------------
// 修改角色名称
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::ChangeRoleName(DWORD dwAccountID, DWORD dwRoleID, LPCTSTR szNewRoleName)
{
	BOOL bRet = TRUE;

	// 获取流和连接
	Beton::MyStream* pStream = m_pDBLoong->GetStream();
	Beton::Connection* pCon = m_pDBLoong->GetFreeConnection();

	// 格式化数据
	pStream->SetUpdate("roledata");
	pStream->FillString("RoleName='").FillString(szNewRoleName, pCon);
	pStream->FillString("',RoleNameCrc=") << m_pUtil->Crc32(szNewRoleName);
	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;
	pStream->FillString(" and AccountID=") << dwAccountID;

	// 执行
	bRet = m_pDBLoong->Execute(pStream);

	// 释放流和连接
	m_pDBLoong->ReturnConnection(pCon);
	m_pDBLoong->ReturnStream(pStream);

	++m_dwWriteTimes;

	return bRet;
}

//-------------------------------------------------------------------------------------------------------
// 修改角色帮派id
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::ChangeRoleGuild(DWORD dwRoleID, DWORD dwGuildID)
{
	return Update(dwRoleID, &dwGuildID, 1, 0, &CLoongDB::FormatUpdateChangeRoleGuild);
}

VOID CLoongDB::FormatUpdateChangeRoleGuild(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	pStream->SetUpdate("roledata");
	pStream->FillString("GuildID=") << *(DWORD*)pData;

	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;
}

//-------------------------------------------------------------------------------------------------------
// 修改角色dressMdID
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::ChangeRoleDressMdID(DWORD dwRoleID, WORD wValue)
{
	return Update(dwRoleID, &wValue, 1, 0, &CLoongDB::FormatUpdateChangeRoleDressMdID);
}

VOID CLoongDB::FormatUpdateChangeRoleDressMdID(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	pStream->SetUpdate("roledata");
	pStream->FillString("DressModelID=") << *(WORD*)pData;

	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;
}

//-------------------------------------------------------------------------------------------------------
// 设置角色删除标志位
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::SetRoleRemoveFlag(DWORD dwRoleID, DWORD dwTime)
{
	return Update(dwRoleID, &dwTime, 1, 0, &CLoongDB::FormatUpdateSetRoleRemoveFlag);
}

VOID CLoongDB::FormatUpdateSetRoleRemoveFlag(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pTime)
{
	pStream->SetUpdate("roledata");
	pStream->FillString("RemoveFlag=") << 1;
	pStream->FillString(",RemoveTime=") << *(DWORD*)pTime;

	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;
}