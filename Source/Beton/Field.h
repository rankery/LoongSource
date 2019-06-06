//--------------------------------------------------------------------------------------------------
// File: Field.h
// Desc: Database column value and attributes
// Time: 2008-05-04
// Auth: Aslan
//
// Copyright (c) 2008 Tengreng All rights reserved.
//---------------------------------------------------------------------------------------------------
#pragma once

#include "StdAfx.h"
#include "Config.h"

namespace Beton {

//----------------------------------------------------------------------------------------------------
/**	Filed类 - 提供数据库结构集中当前某一行某一列的当前属性
	1. 所有列的当前值均由char*存储，并提供按特定类型提取数据的功能，对BOLB字段做单独处理
	2. 提供外部查询的列属性，列最大程度，当前行当前列的长度，是否为NULL，字段类型等等
	3. 对Unicode字符串提供支持，只允许数据表中存储utf8类型的字符串，所以m_szValue用char*表示
*/
//----------------------------------------------------------------------------------------------------
class BETON_API Field
{
public:
	typedef enum						// 类型枚举
	{
		EDBT_UNKNOWN	= 0,			// 未知
		EDBT_INTEGER	= 1,			// 整型
		EDBT_STRING		= 2,			// 字符串型
		EDBT_FLOAT		= 3,			// 浮点型
		EDBT_BLOB		= 4				// 二进制类型
	} DataTypes;

public:
	Field();
	Field(char* szValue, char* szName, ulong uLen, ulong uMaxLen, DataTypes eType);
	Field(Field& field);
	
	~Field() {}

	BETON_INLINE VOID SetValue(char* szValue)	{ m_szValue = szValue; }
	BETON_INLINE VOID SetName(char* szName)		{ m_szName = szName; }	
	BETON_INLINE VOID SetType(DataTypes eType)		{ m_eType = eType; }
	BETON_INLINE VOID SetLen(ulong uLen)			{ m_uLen = uLen; }
	BETON_INLINE VOID SetMaxLen(ulong uMaxLen)		{ m_uMaxLen = uMaxLen; }
	BETON_INLINE VOID SetALL(char* szName, DataTypes eType, ulong uMaxLen)
	{
		SetName(szName); SetType(eType); SetMaxLen(uMaxLen);
	}

	BETON_INLINE BOOL IsNull() const { return m_szValue == NULL; }

	BETON_INLINE const char* GetName()	const	{ return m_szName; }
	BETON_INLINE DataTypes GetType() const		{ return m_eType; }
	BETON_INLINE ulong GetLen() const			{ return m_uLen; }
	BETON_INLINE ulong GetMaxLen() const		{ return m_uMaxLen; }

	BETON_INLINE const char* GetString() const 		{ return m_szValue; }
	BETON_INLINE BOOL GetBool()	const				{ return m_szValue ? atol(m_szValue) : 0; }
	BETON_INLINE INT GetInt() const					{ return m_szValue ? static_cast<INT>(atoi(m_szValue)) : 0; }
	BETON_INLINE DWORD GetDword() const				{ return m_szValue ? static_cast<DWORD>(_atoi64(m_szValue)) : 0; }
	BETON_INLINE INT64 GetLong() const				{ return m_szValue ? static_cast<LONG64>(_atoi64(m_szValue)) : 0; }
	BETON_INLINE BYTE GetByte() const				{ return m_szValue ? static_cast<BYTE>(_atoi64(m_szValue)) : 0; }
	BETON_INLINE SHORT GetShort() const				{ return m_szValue ? static_cast<SHORT>(_atoi64(m_szValue)) : 0; }
	BETON_INLINE FLOAT GetFloat() const				{ return m_szValue ? static_cast<FLOAT>(atof(m_szValue)) : 0.0f; }
	BETON_INLINE DOUBLE GetDouble() const			{ return m_szValue ? static_cast<DOUBLE>(atof(m_szValue)) : 0.0;}

	INT GetTCHAR(TCHAR* szValue, INT nLen, BOOL bCheckType = TRUE) const;
	BOOL GetBlob(VOID* buffer, ulong nLen) const;
	sstring GetUnicodeString() const;

private:
	char*		m_szValue;				// 该列的值
	char*		m_szName;				// 该列的名称
	DWORD		m_dwNameCrc;			// 该列的名称CRC
	DataTypes	m_eType;				// 字段类型
	ulong		m_uLen;					// 当前某行该列的长度，主要为blog字段使用
	ulong		m_uMaxLen;				// 该列的最大程度
};


//------------------------------------------------------------------------------------------------------
// 构造函数
//------------------------------------------------------------------------------------------------------
inline Field::Field() 
: m_szValue(NULL), m_szName(NULL), m_eType(EDBT_UNKNOWN), m_uLen(0), m_uMaxLen(0)
{

}

inline Field::Field(Field& field) 
:m_szValue(field.m_szValue), m_szName(field.m_szName), m_eType(field.m_eType), m_uLen(field.m_uLen), m_uMaxLen(field.m_uMaxLen)
{

}

inline Field::Field(char* szValue, char* szName, ulong uLen, ulong uMaxLen, DataTypes eType)
: m_szValue(szValue), m_szName(szName), m_eType(eType), m_uLen(uLen), m_uMaxLen(uMaxLen)
{

}

} // namespace Beton

