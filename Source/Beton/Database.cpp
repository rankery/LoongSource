//--------------------------------------------------------------------------------------------------
// File: Database.cpp
// Desc: Database manipulation class
// Time: 2008-05-04
// Auth: Aslan
//
// Copyright (c) 2008 Tengreng All rights reserved.
//---------------------------------------------------------------------------------------------------
#pragma once

#include "StdAfx.h"
#include "Config.h"
#include "Database.h"
#include "MysqlStream.h"
#include "SafeStreamQueue.h"
#include "StreamPool.h"
#include "QueryResult.h"

namespace Beton
{

//----------------------------------------------------------------------------------------------------------------------------
// connection
//----------------------------------------------------------------------------------------------------------------------------
Connection::Connection()
{
	m_Mysql = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------
// constructor
//----------------------------------------------------------------------------------------------------------------------------
DataBase::DataBase() : m_Trunk(this), m_nPort(GT_INVALID), m_fnWarning(NULL), m_nConNum(0), m_Con(NULL), m_bConLost(TRUE),
					   m_hThreadAsynQuery(NULL), m_hThreadCheck(NULL), m_bTerminateAsynQuery(FALSE), m_bTerminateCheck(FALSE), m_pLog(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------------
// destructor
//----------------------------------------------------------------------------------------------------------------------------
DataBase::~DataBase()
{
	ShutDown();
}

//----------------------------------------------------------------------------------------------------------------------------
// 设置数据参数
//----------------------------------------------------------------------------------------------------------------------------
VOID DataBase::SetParam(const char* szHost, const char* szUser, const char* szPassword, const char* szDatabase, INT nPort, INT nConNum)
{
	m_Hostname = std::string(szHost);
	m_Username = std::string(szUser);
	m_Password = std::string(szPassword);
	m_DatabaseName = std::string(szDatabase);
	m_nPort = nPort;
	m_nConNum = nConNum;
}

//----------------------------------------------------------------------------------------------------------------------------
// 设置数据参数
//----------------------------------------------------------------------------------------------------------------------------
VOID DataBase::SetParam(const WCHAR* szHost, const WCHAR* szUser, const WCHAR* szPassword, const WCHAR* szDatabase, INT nPort, INT nConNum)
{
	m_Hostname = std::string(m_pUtil->UnicodeToUnicode8(szHost));
	m_Username = std::string(m_pUtil->UnicodeToUnicode8(szUser));
	m_Password = std::string(m_pUtil->UnicodeToUnicode8(szPassword));
	m_DatabaseName = std::string(m_pUtil->UnicodeToUnicode8(szDatabase));
	m_nPort = nPort;
	m_nConNum = nConNum;
}

//----------------------------------------------------------------------------------------------------------------------------
// 初始化函数
//----------------------------------------------------------------------------------------------------------------------------
BOOL DataBase::Init(LPCTSTR strHost, LPCTSTR strUser, LPCTSTR strPassword, LPCTSTR strDatabase, INT nPort, INT nConNum/*=2*/)
{
	if( nConNum <= 1 ) return FALSE;

	m_pUtil = "Util";
	
	SetParam(strHost, strUser, strPassword, strDatabase, nPort, nConNum);

	return Start();
}

//---------------------------------------------------------------------------------------------------------------
// 重连
//---------------------------------------------------------------------------------------------------------------
BOOL DataBase::Reconnect()
{
	ShutDown();		// 先全体关闭

	return Start();	// 开启
}

//---------------------------------------------------------------------------------------------------------------
// 启动
//---------------------------------------------------------------------------------------------------------------
BOOL DataBase::Start()
{
	// 建立连接
	m_Con = new Connection[m_nConNum];

	for(INT n = 0; n < m_nConNum; n++)
	{
		MYSQL* temp = mysql_init(NULL);
		if( mysql_options(temp, MYSQL_SET_CHARSET_NAME, "utf8") )
		{
			// 加入log
		}

		my_bool my_true = true;
		if( mysql_options(temp, MYSQL_OPT_RECONNECT, &my_true) )
		{
			// 加入log
		}

		if( NULL == mysql_real_connect(temp, m_Hostname.c_str(), m_Username.c_str(), m_Password.c_str(), m_DatabaseName.c_str(), m_nPort, NULL, 0) )
		{
			// 写入日志
			return false;
		}

		m_Con[n].m_Mysql = temp;
	}

	// 根据DB的名字生成log变量名称
	m_pLog = new Log;

	if( !P_VALID(m_pLog) )
	{
		return FALSE;
	}
	m_LogFileName = CreateLogFileName();
	if( FALSE == m_pLog->Create(m_LogFileName.c_str()) )
	{
		return FALSE;
	}

	// 生成异步操作线程
	m_bTerminateAsynQuery = FALSE;
	INT nRet = _beginthreadex(NULL, 0, (THREADPROC)m_Trunk.sfp1(&DataBase::ThreadAsynQuery), NULL, 0, NULL);
	if( 0 == nRet || -1 == nRet )
	{
		return FALSE;
	}
	m_hThreadAsynQuery = (HANDLE)nRet;

	// 生成检测线程
	m_bTerminateCheck = FALSE;
	nRet = _beginthreadex(NULL, 0, (THREADPROC)m_Trunk.sfp1(&DataBase::ThreadCheck), NULL, 0, NULL);
	if( 0 == nRet || -1 == nRet )
	{
		return FALSE;
	}
	m_hThreadCheck = (HANDLE)nRet;

	// 设置连接成功
	m_bConLost = FALSE;

	return TRUE;
}

//---------------------------------------------------------------------------------------------------------------
// 关闭
//---------------------------------------------------------------------------------------------------------------
VOID DataBase::ShutDown()
{
	// 首先等待异步操作线程结束（该线程会做完所有目前缓存中的sql语句，并将stream归还给池,池在析构的时候会释放资源）
	InterlockedExchange((LPLONG)&m_bTerminateAsynQuery, TRUE);
	InterlockedExchange((LPLONG)&m_bTerminateCheck, TRUE);

	WaitForSingleObject(m_hThreadAsynQuery, INFINITE);
	WaitForSingleObject(m_hThreadCheck, INFINITE);

	SAFE_CLOSE_HANDLE(m_hThreadAsynQuery);
	SAFE_CLOSE_HANDLE(m_hThreadCheck);

	// 关闭所有连接
	if( P_VALID(m_Con) )
	{
		for(INT n = 0; n < m_nConNum; n++)
		{
			if( m_Con[n].m_Mysql )
				mysql_close(m_Con[n].m_Mysql);
		}
		SAFE_DEL_ARRAY(m_Con);
	}

	// 关闭log
	if( P_VALID(m_pLog) )
	{
		m_pLog->Close();
		SAFE_DEL(m_pLog);
	}
}

//-----------------------------------------------------------------------------------------------------------------
// 得到一个空闲的连接，对于外部手动获得的连接，在操作完后，必须调用ReturnConnection释放连接
//-----------------------------------------------------------------------------------------------------------------
Connection* DataBase::GetFreeConnection()
{
	DWORD n = 0;
	while(TRUE)
	{
		Connection* con = &m_Con[((n++) % m_nConNum)];
		if( con->m_mutex.TryAcquire() )
			return con;
	}

	// 不可能到这里
	return NULL;
}

//------------------------------------------------------------------------------------------------------------------
// 使用完后释放一个连接
//------------------------------------------------------------------------------------------------------------------
VOID DataBase::ReturnConnection(Connection* con)
{
	if( NULL != con )
		con->m_mutex.Release();
}

//-------------------------------------------------------------------------------------------------------------------
// 不指定连接的阻塞时查询操作
//-------------------------------------------------------------------------------------------------------------------
QueryResult* DataBase::Query(const char* szStr)
{
	if( NULL == szStr ) return NULL;

	// 任意获得一条连接
	Connection* con = GetFreeConnection();

	QueryResult* pRet = NULL;

	if( SendQuery(con, szStr, strlen(szStr)) )
	{
		pRet = StoreQueryResult(con);
	}

	ReturnConnection(con);
	return pRet;
}

QueryResult* DataBase::Query(const MyStream* pStream)
{
	if( NULL == pStream || NULL == pStream->GetBuf() || 0 == pStream->GetBufLen() )
		return NULL;

	// 任意获得一条连接
	Connection* con = GetFreeConnection();

	QueryResult* pRet = NULL;

	if( SendQuery(con, pStream->GetBuf(), pStream->GetBufLen()) )
	{
		pRet = StoreQueryResult(con);
	}

	ReturnConnection(con);
	return pRet;
}

//------------------------------------------------------------------------------------------------------------------------
// 指定连接的阻塞式查询操作
//------------------------------------------------------------------------------------------------------------------------
QueryResult* DataBase::WaitQuery(const char* szStr, Connection* con)
{
	if( NULL == szStr || NULL == con ) return NULL;

	QueryResult* pRet = NULL;

	if( SendQuery(con, szStr, strlen(szStr)) )
	{
		pRet = StoreQueryResult(con);
	}

	return pRet;
}

QueryResult* DataBase::WaitQuery(const MyStream* pStream, Connection* con)
{
	if( NULL == pStream || NULL == pStream->GetBuf() || 0 == pStream->GetBufLen() || NULL == con )
		return NULL;

	QueryResult* pRet = NULL;

	if( SendQuery(con, pStream->GetBuf(), pStream->GetBufLen()) )
	{
		pRet = StoreQueryResult(con);
	}

	return pRet;
}

//-----------------------------------------------------------------------------------------------------------
// 不指定连接的阻塞式数据库执行操作
//-----------------------------------------------------------------------------------------------------------
BOOL DataBase::Execute(const char* szStr)
{
	if( NULL == szStr ) return FALSE;

	// 任意获得一条连接
	Connection* con = GetFreeConnection();

	BOOL bRet = SendQuery(con, szStr, strlen(szStr));

	ReturnConnection(con);
	return bRet;
}

BOOL DataBase::Execute(const MyStream* pStream)
{
	if( NULL == pStream || NULL == pStream->GetBuf() || 0 == pStream->GetBufLen() )
		return FALSE;

	// 任意获得一条连接
	Connection* con = GetFreeConnection();

	BOOL bRet = SendQuery(con, pStream->GetBuf(), pStream->GetBufLen());

	ReturnConnection(con);
	return bRet;
}

//-------------------------------------------------------------------------------------------------------------
// 指定连接的阻塞式数据库执行操作
//--------------------------------------------------------------------------------------------------------------
BOOL DataBase::WaitExecute(const char* szStr, Connection* con)
{
	if( NULL == szStr || NULL == con ) return FALSE;

	return SendQuery(con, szStr, strlen(szStr));

}

BOOL DataBase::WaitExecute(const MyStream* pStream, Connection* con)
{
	if( NULL == pStream || NULL == pStream->GetBuf() || 0 == pStream->GetBufLen() || NULL == con )
		return FALSE;

	return SendQuery(con, pStream->GetBuf(), pStream->GetBufLen());
}

//---------------------------------------------------------------------------------------------------------------
// 不指定连接的阻塞式数据库检测执行操作，如果成功，返回影响的记录个数，否则返回-1
//---------------------------------------------------------------------------------------------------------------
INT DataBase::CheckExecute(const char* szStr)
{
	if( NULL == szStr ) return GT_INVALID;

	// 任意获得一条连接
	Connection* con = GetFreeConnection();

	BOOL bRet = SendQuery(con, szStr, strlen(szStr));

	ReturnConnection(con);

	return (bRet ? (INT)mysql_affected_rows(con->m_Mysql) : GT_INVALID);
}

INT DataBase::CheckExecute(const MyStream* pStream)
{
	if( NULL == pStream || NULL == pStream->GetBuf() || 0 == pStream->GetBufLen() )
		return GT_INVALID;

	// 任意获得一条连接
	Connection* con = GetFreeConnection();

	BOOL bRet = SendQuery(con, pStream->GetBuf(), pStream->GetBufLen());

	ReturnConnection(con);

	return (bRet ? (INT)mysql_affected_rows(con->m_Mysql) : GT_INVALID);
}

//---------------------------------------------------------------------------------------------------------------
// 指定连接的阻塞式数据库检测执行操作，如果成功，返回影响的记录个数，否则返回-1
//---------------------------------------------------------------------------------------------------------------
INT DataBase::CheckWaitExecute(const char* szStr, Connection* con)
{
	if( NULL == szStr || NULL == con ) return GT_INVALID;

	BOOL bRet = SendQuery(con, szStr, strlen(szStr));

	return (bRet ? (INT)mysql_affected_rows(con->m_Mysql) : GT_INVALID);
}

INT DataBase::CheckWaitExecute(const Beton::MyStream *pStream, Beton::Connection* con)
{
	if( NULL == pStream || NULL == pStream->GetBuf() || 0 == pStream->GetBufLen() || NULL == con )
		return GT_INVALID;

	BOOL bRet = SendQuery(con, pStream->GetBuf(), pStream->GetBufLen());

	return (bRet ? (INT)mysql_affected_rows(con->m_Mysql) : GT_INVALID);
}

//---------------------------------------------------------------------------------------------------------------
// 事务相关操作
//---------------------------------------------------------------------------------------------------------------
BOOL DataBase::BeginTransaction(Connection* con)
{
	return WaitExecute("START TRANSACTION", con);
}

BOOL DataBase::EndTransaction(Connection* con)
{
	return WaitExecute("COMMIT", con);
}

BOOL DataBase::RollBack(Connection* con)
{
	return WaitExecute("ROLLBACK", con);
}

//------------------------------------------------------------------------------------------------------------------
// 正式发送查询语句
//------------------------------------------------------------------------------------------------------------------
BOOL DataBase::SendQuery(Connection* con, const char* szSql, INT nLen, BOOL bSelf/* =FALSE */)
{
	INT nResult = mysql_real_query(con->m_Mysql, szSql, nLen);
	if( nResult != 0 )
	{
		if( bSelf == FALSE && HandleError(con, mysql_errno(con->m_Mysql), szSql) )
		{
			// 如果因为连接丢失
			return SendQuery(con, szSql, nLen, TRUE);
		}
		else
		{
			// 可能是连接丢失了，这里需要调用上层的回调函数来通知上层
		}
	}

	return (nResult == 0 ? TRUE : FALSE);
}

//-------------------------------------------------------------------------------------------------------------------
// 得到查询结果，只针对数据库的读操作，对数据库的写操作，该函数始终返回NULL
//-------------------------------------------------------------------------------------------------------------------
QueryResult* DataBase::StoreQueryResult(Connection* con)
{
	if( NULL == con ) return NULL;

	QueryResult* pResult = NULL;

	MYSQL_RES* pRes = mysql_store_result(con->m_Mysql);
	INT nRows = (INT)mysql_affected_rows(con->m_Mysql);
	INT nFields = (INT)mysql_field_count(con->m_Mysql);

	if( 0 == nFields || NULL == pRes )
	{
		if( pRes != NULL )
			mysql_free_result(pRes);

		return NULL;
	}

	pResult = new QueryResult(pRes, nRows, nFields);

	pResult->NextRow();

	return pResult;
}

//----------------------------------------------------------------------------------------------------------------------
// 处理sql语句错误
//----------------------------------------------------------------------------------------------------------------------
BOOL DataBase::HandleError(Connection* con, DWORD dwErrorCode, const char* szSql)
{
	if( NULL == con ) return FALSE;

	// 写入日志
	m_pLog->Write(_T("Error: %u\r\n"), dwErrorCode);
	m_pLog->Write(_T("Reason: %s\r\n"), m_pUtil->Unicode8ToUnicode(mysql_error(con->m_Mysql)));
	m_pLog->Write(_T("Sql: %s\r\n\r\n"), m_pUtil->AnsiToUnicode(szSql));

	switch(dwErrorCode)
	{
	case 2006:  // Mysql server has gone away
	case 2008:  // Client ran out of memory
	case 2013:  // Lost connection to sql server during query
	case 2055:  // Lost connection to sql server - system error
		{
			// 如果碰到以上返回码，则连接可能丢失，这时候尝试重连
			return Reconnect(con);
		}
		break;
	}

	return FALSE;
}

//----------------------------------------------------------------------------------------------------------------------
// 重连机制
//----------------------------------------------------------------------------------------------------------------------
BOOL DataBase::Reconnect(Connection* con)
{
	if( NULL == con ) return FALSE;

	MYSQL* temp = mysql_init(NULL);
	my_bool my_true = true;
	mysql_options(temp, MYSQL_SET_CHARSET_NAME, "utf8");
	mysql_options(temp, MYSQL_OPT_RECONNECT, &my_true);

	if( mysql_real_connect(temp, m_Hostname.c_str(), m_Username.c_str(), m_Password.c_str(), m_DatabaseName.c_str(), m_nPort, NULL , 0) )
	{
		mysql_close(temp);

		// 设置连接断开
		m_bConLost = TRUE;

		// 这里需要给上层报告，调用上层的回调函数
		if( m_fnWarning )
			(*m_fnWarning)(this, EDBE_System, 0);
		return FALSE;
	}

	if( con->m_Mysql != NULL )
		mysql_close(con->m_Mysql);

	con->m_Mysql = temp;
	return TRUE;
}

//----------------------------------------------------------------------------------------------------------------------
// 非阻塞数据库操作函数
//----------------------------------------------------------------------------------------------------------------------
UINT DataBase::ThreadAsynQuery()
{
#ifdef _DEBUG
	EXCEPTION_PROTECT_START;
#endif

	// 为该线程提供一条固定的连接
	Connection* con = GetFreeConnection();
	if( NULL == con ) return 1;

	while( FALSE == m_bTerminateAsynQuery )
	{
		while(TRUE)
		{
			MyStream* pStream = m_AsynStreamQueue.Get();
			if( NULL == pStream )
				break;

			WaitExecute(pStream, con);
			ReturnStream(pStream);
		}

		WaitForSingleObject(m_AsynStreamQueue.GetEvent(), 120);
	}

	// 释放掉可能还剩余的内容
	while(TRUE)
	{
		MyStream* pStream = m_AsynStreamQueue.Get();
		if( NULL == pStream )
			break;

		WaitExecute(pStream, con);
		ReturnStream(pStream);
	}

	// 交还连接
	ReturnConnection(con);

#ifdef _DEBUG
	EXCEPTION_PROTECT_END;
#endif
	_endthreadex(0);
	return 0;
}

//---------------------------------------------------------------------------------------------------------------------
// 检测线程，定期检测缓冲大小，资源池大小等等，如果超过一定值，就报告上层
//---------------------------------------------------------------------------------------------------------------------
UINT DataBase::ThreadCheck()
{
#ifdef _DEBUG
	EXCEPTION_PROTECT_START;
#endif

	BOOL bWarningQueueFullNotified = FALSE;
	BOOL bWarningPoolFullNotified = FALSE;

	while( FALSE == m_bTerminateCheck )
	{
		// 首先检测异步操作缓冲的容量
		INT nQueryNum = m_AsynStreamQueue.Size();
		if( nQueryNum >= WARNING_QUEUE_NUM && FALSE == bWarningQueueFullNotified )
		{
			// 调用上层回调函数
			if( m_fnWarning )
				(*m_fnWarning)(this, EDBE_QueueFull, nQueryNum);
			bWarningQueueFullNotified = TRUE;
		}
		else if( nQueryNum < WARNING_QUEUE_NUM && TRUE == bWarningQueueFullNotified )
		{
			bWarningQueueFullNotified = FALSE;
		}

		// 再检查底层资源池大小
		INT nPoolNum = m_StreamPool.GetAllStreamNum();
		if( nPoolNum >= WARNING_POOL_NUM && FALSE == bWarningPoolFullNotified )
		{
			// 调用上层回调函数
			if( m_fnWarning )
				(*m_fnWarning)(this, EDBE_PoolFull, nPoolNum);
			bWarningPoolFullNotified = TRUE;
		}
		else if( nPoolNum < WARNING_POOL_NUM && TRUE == bWarningPoolFullNotified )
		{
			bWarningPoolFullNotified = FALSE;
		}

		Sleep(2000);
	}

#ifdef _DEBUG
	EXCEPTION_PROTECT_END;
#endif
	_endthreadex(0);
	return 0;
}

//------------------------------------------------------------------------------
// 生成数据库日志文件名
//------------------------------------------------------------------------------
tstring DataBase::CreateLogFileName()
{
	tstring strFileName = _T("log\\");

	// 生成合适的log文件名
	TCHAR szTime[MAX_PATH], szTemp[MAX_PATH];
	GetModuleFileName(NULL, szTemp, MAX_PATH);

	// 截去路径
	TCHAR *pResult = _tcsrchr(szTemp, _T('\\'));
	pResult = pResult ?	pResult+1 :	pResult = (TCHAR *)szTemp;

	// 截去文件扩展名
	TCHAR* pResult2 = _tcsrchr(pResult, _T('.'));
	if( pResult2 )
		*pResult2 = _T('\0');

	// 加上程序文件名
	strFileName += pResult;
	strFileName += _T("_");

	// 加上数据库名称
	strFileName += m_pUtil->Unicode8ToUnicode(m_DatabaseName.c_str());
	strFileName += _T("_");

	// 加上时间
	FILETIME CurrentTime;
	GetSystemTimeAsFileTime(&CurrentTime);
	szTime[0] = _T('\0');
	WORD wDate, wTime;
	if (FileTimeToLocalFileTime(&CurrentTime, &CurrentTime) &&
		FileTimeToDosDateTime(&CurrentTime, &wDate, &wTime))
	{
		// 年月日以及时分秒和进程id
		wsprintf(szTime, _T("[%d-%d-%d %02d%02d%02d %05d_%d].dblog"),
			(wDate / 32) & 15, wDate & 31, (wDate / 512) + 1980,
			(wTime >> 11), (wTime >> 5) & 0x3F, (wTime & 0x1F) * 2, 
			GetCurrentProcessId(), rand());
	}

	strFileName += szTime;

	return strFileName;
}

} // namespace Beton

