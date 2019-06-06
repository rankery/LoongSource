//--------------------------------------------------------------------------------------------------
// File: Database.h
// Desc: Database structure
// Time: 2008-05-05
// Auth: Aslan
//
// Copyright (c) 2008 Tengreng All rights reserved.
//---------------------------------------------------------------------------------------------------
#pragma once

#include "StdAfx.h"
#include "Config.h"
#include "Mutex.h"
#include "MysqlStream.h"
#include "SafeStreamQueue.h"
#include "StreamPool.h"
#include "QueryResult.h"

namespace Beton {

class Mutex;
class QueryResult;
class MyStream;
class SafeStreamQueue;
class StreamPool;

class DataBase;

typedef VOID (WINAPI* WARNINGCALLBACK)(DataBase*, INT nReason, INT nParam);

//----------------------------------------------------------------------------------------------------
/**	Database类 - 提供数据库操作类，包括连接的建立及维护，各种查询及更新操作机制等等
1. Database对象与数据库一一对应，如果上层应用程序需要多个库，要建立多个Database对象
1. 连接池机制，方便多线程的并发操作
2. 提供对数据库连接参数以及当前数据库状态信息的提取
3. 提供三种查询及操作方式——阻塞式操作，回调式操作及无返回操作
4. 提供简单的事务处理机制，日后添加完整事务处理支持
*/
//----------------------------------------------------------------------------------------------------
const INT WARNING_QUEUE_NUM = 1000;		// 异步操作队列的容量如果超过该值，则警报
const INT WARNING_POOL_NUM = 20000;		// 底层Mystream池的容量超过这个限度，则警报

struct BETON_API Connection
{
	Mutex		m_mutex;	// 并发锁
	MYSQL*		m_Mysql;

	Connection();
};

class BETON_API DataBase
{
public:
	typedef enum			// 数据库警报枚举信息，目前是两个
	{
		EDBE_System,		// mysql服务器出错
		EDBE_QueueFull,		// 队列中内容过多，查看数据库是否阻塞
		EDBE_PoolFull,		// 底层Stream池分配的内存过多了
	} EDBError;

public:
	DataBase();
	virtual ~DataBase();
	
private:
	DataBase(const DataBase&);
	DataBase& operator = (const DataBase&);

public:
	VOID SetWarningCallBack(WARNINGCALLBACK fnWarning) { m_fnWarning = fnWarning; }

public:

	//---------------------------------------------------------------------------
	// 初始化及结束
	//---------------------------------------------------------------------------
	BOOL Init(LPCTSTR strHost, LPCTSTR strUser, LPCTSTR strPassword, LPCTSTR strDatabase, INT nPort, INT nConNum=2);
	VOID ShutDown();

	//---------------------------------------------------------------------------
	// 连接相关
	//---------------------------------------------------------------------------
	BETON_INLINE Connection* GetFreeConnection();
	BETON_INLINE VOID ReturnConnection(Connection* con);

	//---------------------------------------------------------------------------
	// 查看连接丢失及重连
	//---------------------------------------------------------------------------
	BOOL Reconnect();
	BETON_INLINE BOOL IsConnLost()		{ return m_bConLost; }


	//---------------------------------------------------------------------------
	// Stream相关
	//---------------------------------------------------------------------------
	BETON_INLINE MyStream* GetStream() { return m_StreamPool.AllocStream(); }
	BETON_INLINE VOID ReturnStream(MyStream* pStream) { m_StreamPool.FreeStream(pStream); }

	//----------------------------------------------------------------------------
	// 查询相关
	//----------------------------------------------------------------------------
	BETON_INLINE QueryResult* Query(const char* szStr);
	BETON_INLINE QueryResult* Query(const MyStream* pStream);
	BETON_INLINE QueryResult* WaitQuery(const char* szStr, Connection* con);
	BETON_INLINE QueryResult* WaitQuery(const MyStream* pStream, Connection* con);

	//-----------------------------------------------------------------------------
	// 操作相关
	//-----------------------------------------------------------------------------
	BETON_INLINE BOOL Execute(const char* szStr);
	BETON_INLINE BOOL Execute(const MyStream* pStream);
	BETON_INLINE BOOL WaitExecute(const char* szStr, Connection* con);
	BETON_INLINE BOOL WaitExecute(const MyStream* pStream, Connection* con);

	//-----------------------------------------------------------------------------
	// 检测操作相关
	//-----------------------------------------------------------------------------
	BETON_INLINE INT CheckExecute(const char* szStr);
	BETON_INLINE INT CheckExecute(const MyStream* pStream);
	BETON_INLINE INT CheckWaitExecute(const char* szStr, Connection* con);
	BETON_INLINE INT CheckWaitExecute(const MyStream* pStream, Connection* con);

	//------------------------------------------------------------------------------
	// 记录集相关
	//------------------------------------------------------------------------------
	BETON_INLINE VOID FreeQueryResult(QueryResult* pRet) { SAFE_DEL(pRet); }

	//------------------------------------------------------------------------------
	// 异步操作相关
	//------------------------------------------------------------------------------
	BETON_INLINE VOID AddQuery(MyStream* pStream) { m_AsynStreamQueue.Add(pStream); }

	//------------------------------------------------------------------------------
	// 事物相关
	//------------------------------------------------------------------------------
	BETON_INLINE BOOL BeginTransaction(Connection* con);
	BETON_INLINE BOOL EndTransaction(Connection* con);
	BETON_INLINE BOOL RollBack(Connection* con);

	//-------------------------------------------------------------------------------
	// 系统信息相关
	//-------------------------------------------------------------------------------
	BETON_INLINE const std::string& GetHostName() const { return m_Hostname; }
	BETON_INLINE const std::string& GetUserName() const { return m_Username; }
	BETON_INLINE const std::string& GetPassWord() const { return m_Password; }
	BETON_INLINE const std::string& GetDatabaseName() const { return m_DatabaseName; }
	BETON_INLINE const INT GetPort() const { return m_nPort; }

	//-------------------------------------------------------------------------------
	// 运行时信息相关
	//-------------------------------------------------------------------------------
	BETON_INLINE INT GetAvailableStreamNum() { return m_StreamPool.GetAvailableStreamNum(); }
	BETON_INLINE INT GetAllStreamNum() { return m_StreamPool.GetAllStreamNum(); }
	BETON_INLINE INT GetUnhandledAsynStreamNum() { return m_AsynStreamQueue.Size(); }

	//-------------------------------------------------------------------------------
	// Log相关
	//-------------------------------------------------------------------------------
	tstring CreateLogFileName();

private:
	VOID SetParam(const char* szHost, const char* szUser, const char* szPassword, const char* szDatabase, INT nPort, INT nConNum);
	VOID SetParam(const WCHAR* szHost, const WCHAR* szUser, const WCHAR* szPassword, const WCHAR* szDatabase, INT nPort, INT nConNum);

private:
	BOOL Start();

private:
	BETON_INLINE BOOL HandleError(Connection* con, DWORD dwErrorCode, const char* szSql);
	BETON_INLINE BOOL Reconnect(Connection* con);

	BETON_INLINE BOOL SendQuery(Connection* con, const char* szSql, INT nLen, BOOL bSelf=FALSE);
	BETON_INLINE QueryResult* StoreQueryResult(Connection* con);

private:
	UINT ThreadAsynQuery();
	UINT ThreadCheck();
	


private:
	TSFPTrunk<DataBase>			m_Trunk;
	TObjRef<Util>				m_pUtil;

	Log*						m_pLog;
	tstring						m_LogFileName;

	std::string					m_Hostname;				// mysql服务器主机
	std::string					m_Username;				// 用户名
	std::string					m_Password;				// 密码
	std::string					m_DatabaseName;			// 数据库名
	INT							m_nPort;				// mysql服务器端口号

	WARNINGCALLBACK				m_fnWarning;			// 通知上层报警的回调函数

	Connection*					m_Con;					// 连接池
	INT							m_nConNum;				// 连接池的连接数量
	BOOL						m_bConLost;				// 连接丢失

	StreamPool					m_StreamPool;			// 空的sql语句池
	SafeStreamQueue				m_AsynStreamQueue;		// 非阻塞的操作语句队列

	HANDLE						m_hThreadAsynQuery;		// 异步操作线程
	HANDLE						m_hThreadCheck;			// 检测运行时系统信息

	VOLATILE BOOL				m_bTerminateAsynQuery;	// 异步操作线程结束标识位
	VOLATILE BOOL				m_bTerminateCheck;		// 检测线程结束标识位
};

} // namespace Beton