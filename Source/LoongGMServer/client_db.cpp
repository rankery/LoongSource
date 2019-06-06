//-----------------------------------------------------------------------------
//!\file	client_db.cpp
//!\brief	客户端数据库管理类
//!
//!\date	2009-04-20
//! last	2009-04-20
//!\author	zhangrong
//!
//! Copyright (c) 1985-2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"

#include "../ServerDefine/rt_define.h"

#include "client_db.h"
#include "server.h"

//-----------------------------------------------------------------------------
// Construct
//-----------------------------------------------------------------------------
 ClientDB::ClientDB() : m_DB()
{

}

//-----------------------------------------------------------------------------
// 初始化
//-----------------------------------------------------------------------------
BOOL ClientDB::Init(LPCTSTR strHost, LPCTSTR strUser, LPCTSTR strPassword, LPCTSTR strDatabase, INT nPort)
{
	BOOL bRet = m_DB.Init(strHost, strUser, strPassword, strDatabase, nPort);
	return bRet;
}

//------------------------------------------------------------------------------
// 销毁
//------------------------------------------------------------------------------
VOID ClientDB::Destroy()
{

}

//-------------------------------------------------------------------------------
// 查询客户端数据
//-------------------------------------------------------------------------------
BOOL ClientDB::DBQueryClientData(LPCTSTR szName, DWORD dwPsdCrc, DWORD& dwClientID, INT& nPrivilege)
{
	// 获取一个Stream
	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	// 获取一条连接
	Connection* pCon = m_DB.GetFreeConnection();

	// 设置sql语句
	pStream->SetSelect("account", "id,privilege");
	pStream->SetWhere();
	pStream->FillString("name='").FillString(szName, pCon);
	pStream->FillString("' and psd=") << dwPsdCrc;


	// 进行查询
	QueryResult* pResult = m_DB.Query(pStream);


	// 查询结果为空
	if( !P_VALID(pResult) || pResult->GetRowCount() <= 0) return FALSE;

	// 设置查询结果
	dwClientID = (*pResult)[0].GetDword();
	nPrivilege = (*pResult)[1].GetInt();

	// 释放 Result
	m_DB.FreeQueryResult(pResult);

	// 归还连接
	m_DB.ReturnConnection(pCon);

	// 释放Stream
	m_DB.ReturnStream(pStream);

	return TRUE;
}

//-------------------------------------------------------------------------------
// 查询客户端数据
//-------------------------------------------------------------------------------
BOOL ClientDB::DBQueryAccountPrivilege(LPCTSTR szName, INT& nPrivilege)
{
	// 获取一个Stream
	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	// 获取一条连接
	Connection* pCon = m_DB.GetFreeConnection();

	// 设置sql语句
	pStream->SetSelect("account", "privilege");
	pStream->SetWhere();
	pStream->FillString("name='").FillString(szName, pCon).FillString("'");


	// 进行查询
	QueryResult* pResult = m_DB.Query(pStream);


	// 查询结果为空
	if( !P_VALID(pResult) || pResult->GetRowCount() <= 0) return FALSE;

	// 设置查询结果
	nPrivilege = (*pResult)[0].GetInt();

	// 释放 Result
	m_DB.FreeQueryResult(pResult);

	// 归还连接
	m_DB.ReturnConnection(pCon);

	// 释放Stream
	m_DB.ReturnStream(pStream);

	return TRUE;
}

//---------------------------------------------------------------------------------
// 创建客户端帐号
//---------------------------------------------------------------------------------
BOOL ClientDB::DBCreateGameMaster( LPCTSTR szName, DWORD dwPsdCrc, INT nPrivilege )
{
	if( !P_VALID(szName) || !P_VALID(dwPsdCrc) )
		return FALSE;

	// 获取一个Stream
	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	// 获取一条连接
	Connection* pCon = m_DB.GetFreeConnection();

	// 设置sql语句
	pStream->SetInsert("account");
	pStream->FillString("name='").FillString(szName, pCon);
	pStream->FillString("',psd=") << dwPsdCrc;
	pStream->FillString(",privilege=") << nPrivilege;

	// 归还连接
	m_DB.ReturnConnection(pCon);

	// 进行操作
	BOOL bRet = m_DB.Execute(pStream);

	// 释放Stream
	m_DB.ReturnStream(pStream);

	return bRet;
}

//--------------------------------------------------------------------------------------
// 删除客户端帐号
//--------------------------------------------------------------------------------------
BOOL ClientDB::DBDeleteGameMaster(LPCTSTR szName)
{
	if( !P_VALID(szName) ) return FALSE;

	// 获取一个Stream
	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	// 获取一条连接
	Connection* pCon = m_DB.GetFreeConnection();

	// 设置sql语句
	pStream->SetDelete("account");
	pStream->SetWhere();
	pStream->FillString("name='").FillString(szName, pCon).FillString("'");

	// 归还连接
	m_DB.ReturnConnection(pCon);

	// 进行操作
	BOOL bRet = m_DB.Execute(pStream);

	// 释放Stream
	m_DB.ReturnStream(pStream);

	return bRet;
}

//----------------------------------------------------------------------------------------
// 更改客户端帐号权限
//----------------------------------------------------------------------------------------
BOOL ClientDB::DBChangeGMPrivilege(LPCTSTR szName, INT nNewPrivilege)
{
	if( !P_VALID(szName) ) return FALSE;

	// 获取一个Stream
	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	// 获取一条连接
	Connection* pCon = m_DB.GetFreeConnection();

	// 设置sql语句
	pStream->SetUpdate("account");
	pStream->FillString("privilege=") << nNewPrivilege;
	pStream->SetWhere();
	pStream->FillString("name='").FillString(szName, pCon).FillString("'");

	// 归还连接
	m_DB.ReturnConnection(pCon);

	// 进行操作
	BOOL bRet = m_DB.Execute(pStream);

	// 释放Stream
	m_DB.ReturnStream(pStream);

	return bRet;
}

//----------------------------------------------------------------------------------------
// 获得某个消息在某个权限值下的操作权限
//----------------------------------------------------------------------------------------
BOOL ClientDB::DBQueryPrivilegeInfo(TMap<DWORD, DWORD>& mapPrivilege)
{
	// 进行操作
	QueryResult* pResult = m_DB.Query("select MsgName,pri_1,pri_2,pri_3,pri_4,pri_5,pri_6,pri_7,pri_8,pri_9 from privilege");
	if( !P_VALID(pResult) || pResult->GetRowCount() <= 0) return FALSE;

	INT nCount = pResult->GetRowCount();

	for(INT n = 0; n < nCount; ++n)
	{
		// 得到消息名
		const char* pMsgName = (*pResult)[0].GetString();

		// 得到各个权限标志
		
		BOOL bPri1 = (*pResult)[1].GetBool();
		BOOL bPri2 = (*pResult)[2].GetBool();
		BOOL bPri3 = (*pResult)[3].GetBool();
		BOOL bPri4 = (*pResult)[4].GetBool();
		BOOL bPri5 = (*pResult)[5].GetBool();
		BOOL bPri6 = (*pResult)[6].GetBool();
		BOOL bPri7 = (*pResult)[7].GetBool();
		BOOL bPri8 = (*pResult)[8].GetBool();
		BOOL bPri9 = (*pResult)[9].GetBool();

		// 计算消息Crc
		DWORD dwMsgID = IUTIL->Crc32(pMsgName);

		// 组成权限位组
		DWORD dwPrivilegeFlag = 0;
		if( bPri1 )		dwPrivilegeFlag |= (1 << EP_Pri_GM_D);
		if( bPri2 )		dwPrivilegeFlag |= (1 << EP_Pri_GM_C);
		if( bPri3 )		dwPrivilegeFlag |= (1 << EP_Pri_GM_B);
		if( bPri4 )		dwPrivilegeFlag |= (1 << EP_Pri_GM_A);
		if( bPri5 )		dwPrivilegeFlag |= (1 << EP_Pri_GM_S);
		if( bPri6 )		dwPrivilegeFlag |= (1 << EP_Pri_Rt_B);
		if( bPri7 )		dwPrivilegeFlag |= (1 << EP_Pri_Rt_A);
		if( bPri8 )		dwPrivilegeFlag |= (1 << EP_Pri_Rt_C);
		if( bPri9 )		dwPrivilegeFlag |= (1 << EP_Pri_Rt_D);

		// 写进权限列表
		mapPrivilege.Add(dwMsgID, dwPrivilegeFlag);

		// 下一行
		pResult->NextRow();
	}

	return TRUE;
}

BOOL ClientDB::DBGMExist( LPCTSTR szName )
{
	// 获取一个Stream
	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	// 获取一条连接
	Connection* pCon = m_DB.GetFreeConnection();

	// 设置sql语句
	pStream->SetSelect("account", "*");
	pStream->SetWhere();
	pStream->FillString("name='").FillString(szName, pCon).FillString("'");



	// 进行操作
	QueryResult* pResult = m_DB.Query(pStream);

	BOOL bRtv = (P_VALID(pResult) && pResult->GetRowCount() > 0);

	// 归还连接
	m_DB.ReturnConnection(pCon);

	// 释放Stream
	m_DB.ReturnStream(pStream);

	return bRtv;
}

BOOL ClientDB::DBQueryBindIp( LPCTSTR szName, DWORD& dwIP )
{
	// 获取一个Stream
	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	// 获取一条连接
	Connection* pCon = m_DB.GetFreeConnection();

	// 设置sql语句
	pStream->SetSelect("rt_iptable", "ip");
	pStream->SetWhere();
	pStream->FillString("name='").FillString(szName, pCon).FillString("'");

	// 进行操作
	QueryResult* pResult = m_DB.Query(pStream);

	BOOL bRtv = TRUE;
	if (P_VALID(pResult) && pResult->GetRowCount() > 0)
	{
		TObjRef<StreamTransport> pTrans = "StreamTransport";
		dwIP = pTrans->StringIP2IP(const_cast<char *>((*pResult)[0].GetString()));
		if (INADDR_NONE == dwIP)
		{
			bRtv = FALSE;
		}
	}

	// 归还连接
	m_DB.ReturnConnection(pCon);

	// 释放Stream
	m_DB.ReturnStream(pStream);

	return bRtv;	
}

BOOL ClientDB::DBChangeBindIp( LPCTSTR szName, DWORD dwIP )
{
	if( !P_VALID(szName) ) return FALSE;

	// 获取一个Stream
	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	// 获取一条连接
	Connection* pCon = m_DB.GetFreeConnection();

	TObjRef<StreamTransport> pTrans = "StreamTransport";

	// 设置sql语句
	pStream->SetUpdate("rt_iptable");
	pStream->FillString("ip='").FillString(pTrans->IP2String(dwIP), pCon).FillString("'");
	pStream->SetWhere();
	pStream->FillString("name='").FillString(szName, pCon).FillString("'");

	// 进行操作
	BOOL bRet = m_DB.Execute(pStream);

	// 归还连接
	m_DB.ReturnConnection(pCon);

	// 释放Stream
	m_DB.ReturnStream(pStream);

	return bRet;
}

BOOL ClientDB::DBCheckIPBind( LPCTSTR szName, DWORD dwClientIP )
{
	DWORD dwBindIP = INADDR_NONE;
	if (dwClientIP == INADDR_NONE)
	{
		return FALSE;
	}
	else if (!DBQueryBindIp(szName, dwBindIP))
	{
		return FALSE;
	}
	else if(dwClientIP != dwBindIP)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}
BOOL ClientDB::DBReplaceBindIp(LPCTSTR szName, DWORD dwIP)
{
	if( !P_VALID(szName) ) return FALSE;

	// 获取一个Stream
	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	// 获取一条连接
	Connection* pCon = m_DB.GetFreeConnection();

	TObjRef<StreamTransport> pTrans = "StreamTransport";

	// 设置sql语句
	pStream->SetReplace("rt_iptable");
	pStream->FillString("ip='").FillString(pTrans->IP2String(dwIP), pCon).FillString("'");
	pStream->FillString(",name='").FillString(szName, pCon).FillString("'");

	// 进行操作
	BOOL bRet = m_DB.Execute(pStream);

	// 归还连接
	m_DB.ReturnConnection(pCon);

	// 释放Stream
	m_DB.ReturnStream(pStream);

	return bRet;
}

BOOL ClientDB::DBLogGMAction( DWORD dwAccountID, LPCTSTR szName, LPCTSTR szIP, EActionType eActionType, LPCTSTR szDetails )
{
	if( !P_VALID(szName) || !GT_VALID(dwAccountID) )
		return FALSE;

	if (!IsActionValid(eActionType))
		return FALSE;

	// 获取一个Stream
	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	// 获取一条连接
	Connection* pCon = m_DB.GetFreeConnection();

	// 设置sql语句
	pStream->SetInsert("gm_log");
	pStream->FillString("AccountID=") << dwAccountID;
	pStream->FillString(",Name='").FillString(szName, pCon);
	pStream->FillString("',Time=NOW()");
	pStream->FillString(",OperIP='").FillString(szIP, pCon);
	pStream->FillString("',Action=") << eActionType;
	pStream->FillString(",Details='").FillString(szDetails, pCon);
	pStream->FillString("'");

	// 归还连接
	m_DB.ReturnConnection(pCon);

	// 进行操作
	BOOL bRet = m_DB.Execute(pStream);

	// 释放Stream
	m_DB.ReturnStream(pStream);

	return bRet;
}