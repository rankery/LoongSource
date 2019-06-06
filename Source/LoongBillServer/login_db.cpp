//-----------------------------------------------------------------------------
//!\file login_db.h
//!\author songg
//!
//!\date 2009-08-31
//! last 2009-08-31
//!
//!\brief login服务器数据库
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"

#include "login_db.h"
#include "../ServerDefine/bill_define.h"
//------------------------------------------------------------------------------
// 全局变量定义
//------------------------------------------------------------------------------
LoginDB g_LoginDB;
//-----------------------------------------------------------------------------
// 初始化
//-----------------------------------------------------------------------------
BOOL LoginDB::Init()
{
	TObjRef<VarContainer> pVar = "BillConfig";

	// 初始化数据库
	BOOL bRet = m_DB.Init(pVar->GetString(_T("db_host"), _T("login")), 
		pVar->GetString(_T("db_user"), _T("login")),
		pVar->GetString(_T("db_psd"), _T("login")), 
		pVar->GetString(_T("db_name"), _T("login")),
		pVar->GetInt(_T("db_port"), _T("login")));


	return bRet;
}

//------------------------------------------------------------------------------
// 销毁
//------------------------------------------------------------------------------
VOID LoginDB::Destroy()
{

}

//------------------------------------------------------------------------------
// 更新
//------------------------------------------------------------------------------
VOID LoginDB::Update()
{
	// 重连
	if( IsConLost() )
		Reconnect();
}

//------------------------------------------------------------------------------
// 重连
//------------------------------------------------------------------------------
VOID LoginDB::Reconnect()
{
	m_DB.Reconnect();
}
//-------------------------------------------------------------------------
// 插入玩家记录
//-------------------------------------------------------------------------
BOOL LoginDB::InsertAccout(LPCSTR szAccountName, LPCSTR szPsd, LPCSTR szMiBao, BOOL bGuard)
{
	if( !P_VALID(szAccountName) || !P_VALID(szPsd) ) return FALSE;

	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	// 申请连接
	Connection* pCon = m_DB.GetFreeConnection();

	pStream->SetInsert("account");
	pStream->FillString("name='").FillString(szAccountName, pCon).FillString("',");
	pStream->FillString("psd='").FillString(szPsd, pCon).FillString("',");
	pStream->FillString("mibao='").FillBlob(szMiBao, MIBAO_LEN, pCon).FillString("',");
	pStream->FillString("guard=") << bGuard;

	// 释放连接
	m_DB.ReturnConnection(pCon);

	// 查询
	BOOL bRet = m_DB.Execute(pStream);

	// 释放流
	m_DB.ReturnStream(pStream);

	return bRet;
}

//-------------------------------------------------------------------------------------
// 得到玩家的ID
//-------------------------------------------------------------------------------------
DWORD LoginDB::GetAccountID(const LPCSTR szAccountName)
{
	if( IsConLost() ) return GT_INVALID;		// 断开连接

	MyStream* pStream  = m_DB.GetStream();
	if( !P_VALID(pStream) ) return GT_INVALID;

	Connection* pCon = m_DB.GetFreeConnection();

	pStream->SetSelect("account", "id");
	pStream->SetWhere();
	pStream->FillString("name='").FillString(szAccountName, pCon).FillString("'");

	m_DB.ReturnConnection(pCon);

	QueryResult* pResult = m_DB.Query(pStream);

	m_DB.ReturnStream(pStream);

	if( !P_VALID(pResult) ) return GT_INVALID;
	if(  1 != pResult->GetRowCount() )
	{
		m_DB.FreeQueryResult(pResult);
		return GT_INVALID;
	}

	DWORD dwAccountID = (*pResult)[0].GetDword();

	m_DB.FreeQueryResult(pResult);

	return dwAccountID;
}
//-------------------------------------------------------------------------------------------------------
// 锁表 -- 锁多个参数为"t1,t2,t3"
//-------------------------------------------------------------------------------------------------------
BOOL LoginDB::LockTables(LPCSTR szTableNames)
{
	if( IsConLost() ) return FALSE;		// 断开连接

	MyStream* pStream  = m_DB.GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	pStream->Clear();

	pStream->FillString("Lock Tables ");
	pStream->FillString(szTableNames);
	pStream->FillString(" WRITE");

	// 查询
	BOOL bRet = m_DB.Execute(pStream);

	// 释放流
	m_DB.ReturnStream(pStream);

	return bRet;
}
//-------------------------------------------------------------------------------------------------------
// 解锁
//-------------------------------------------------------------------------------------------------------
BOOL LoginDB::UnlockTables()
{
	if( IsConLost() ) return FALSE;		// 断开连接

	MyStream* pStream  = m_DB.GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	pStream->Clear();

	pStream->FillString("Unlock Tables");

	// 查询
	BOOL bRet = m_DB.Execute(pStream);

	// 释放流
	m_DB.ReturnStream(pStream);

	return bRet;
}