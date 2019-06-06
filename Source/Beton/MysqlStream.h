//--------------------------------------------------------------------------------------------------
// File: MySqlStream.h
// Desc: Mysql SQL statement structure
// Time: 2008-05-05
// Auth: Aslan
//
// Copyright (c) 2008 Tengreng All rights reserved.
//---------------------------------------------------------------------------------------------------
#pragma once

#include "StdAfx.h"
#include "Config.h"
#include "Database.h"

namespace Beton {

struct Connection;
//----------------------------------------------------------------------------------------------------
/**	MysqlStream类 - 非线程安全的MysqlStream类，用于存储转换和转义后的sql语句
*/
//----------------------------------------------------------------------------------------------------
class BETON_API MyStream
{
public:
	MyStream(INT nDefaultSize=1024);
	virtual ~MyStream();

private:
	MyStream(const MyStream&);
	MyStream& operator = (MyStream&);

public:
	//--------------------------------------------------------------------------------------------------
	// 字符串及二进制字段单独处理
	//--------------------------------------------------------------------------------------------------
	MyStream& FillString(const char* p, Connection* con=NULL);
	MyStream& FillString(const WCHAR* p, Connection* con=NULL);
	MyStream& FillBlob(const VOID* p, INT nSize, Connection* con);
	MyStream& FillBlob(VOID* p, INT nSize);	// 调用该方法的二进制数据不需要转义

	//--------------------------------------------------------------------------------------------------
	// 流操作符重载
	//--------------------------------------------------------------------------------------------------
	BETON_INLINE MyStream& operator << (const INT p);
	BETON_INLINE MyStream& operator << (const DWORD p);
	BETON_INLINE MyStream& operator << (const INT64 p);
	BETON_INLINE MyStream& operator << (const DOUBLE p);
	BETON_INLINE MyStream& operator << (const FLOAT p)		{ return operator << (static_cast<DOUBLE>(p)); }
	BETON_INLINE MyStream& operator << (const WORD p)		{ return operator << (static_cast<DWORD>(p));}
	BETON_INLINE MyStream& operator << (const BYTE p)		{ return operator << (static_cast<DWORD>(p));}

public:
	//--------------------------------------------------------------------------------------------------
	// 基本SQL语句操作
	//--------------------------------------------------------------------------------------------------
	BETON_INLINE MyStream& SetCopyItem(const char* szTab1, const char* szSeclect, const char* szTab2)
	{

		Clear();
		return (*this).FillString("insert into ").FillString(szTab1).FillString(" select ").FillString(szSeclect).FillString(" from ").FillString(szTab2);
	}

	BETON_INLINE MyStream& SetOrder(const char* szKeyColumn, BOOL bDesc)
	{
		if(bDesc)
			return (*this).FillString(" order by ").FillString(szKeyColumn).FillString(" desc");
		else
			return (*this).FillString(" order by ").FillString(szKeyColumn);
	}

	BETON_INLINE MyStream& SetSelect(const char* szTab, const char* szSeclect)
	{

		Clear();
		return (*this).FillString("select ").FillString(szSeclect).FillString(" from ").FillString(szTab);
	}

	BETON_INLINE MyStream& SetUpdate(const char* szTab)
	{
		Clear();
		return (*this).FillString("update ").FillString(szTab).FillString(" set ");
	}

	BETON_INLINE MyStream& SetInsert(const char* szTab)
	{
		Clear();
		return (*this).FillString("insert into ").FillString(szTab).FillString(" set ");
	}

	BETON_INLINE MyStream& SetInsert(const char* szTab, const char* szPostfix)
	{
		Clear();
		return (*this).FillString("insert into ").FillString(szTab).FillString(szPostfix).FillString(" set ");
	}

	BETON_INLINE MyStream& SetReplace(const char* szTab)
	{
		Clear();
		return (*this).FillString("replace into ").FillString(szTab).FillString(" set ");
	}

	BETON_INLINE MyStream& SetDelete(const char* szTab)
	{
		Clear();
		return (*this).FillString("delete from ").FillString(szTab);
	}
	
	BETON_INLINE MyStream& SetEmpty(const char* szTab)
	{
		Clear();
		return (*this).FillString("truncate table ").FillString(szTab);
	}

	BETON_INLINE MyStream& SetWhere()
	{
		return (*this).FillString(" where ");
	}

	BETON_INLINE MyStream& SetLimit(INT nNum, INT nBegin = 0)
	{
		(*this).FillString(" limit ");
		if( nBegin > 0 )
		{
			(*this) << nBegin;
			(*this).FillString(",");
		}
		return (*this) << nNum;
	}

public:
	BETON_INLINE VOID			Grow(INT nSize);
	BETON_INLINE BOOL			Seek(INT nPos);
	BETON_INLINE VOID			End()					{ m_pBuf[m_nPos] = '\0'; }
	BETON_INLINE const char*	GetBuf() const			{ return m_pBuf; }
	BETON_INLINE DWORD			GetBufLen() const		{ return m_nPos; }
	BETON_INLINE BOOL			IsEmpty() const			{ return m_nPos == 0; }
	BETON_INLINE VOID			Clear()					{ m_nPos = 0; End(); }

private:
	char*		m_pBuf;				// 语句缓冲
	INT			m_nBufLen;			// 缓冲的大小
	INT			m_nPos;				// 当前输入所在的位置
	INT			m_nDefaultSize;		// 扩充的大小
};

} // namespace Beton
