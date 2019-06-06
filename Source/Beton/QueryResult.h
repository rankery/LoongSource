//--------------------------------------------------------------------------------------------------
// File: QueryResult.h
// Desc: Database record set values and attributes
// Time: 2008-05-05
// Auth: Aslan
//
// Copyright (c) 2008 Tengreng All rights reserved.
//---------------------------------------------------------------------------------------------------
#pragma once

#include "StdAfx.h"
#include "Config.h"
#include "Field.h"

namespace Beton {

//----------------------------------------------------------------------------------------------------
/**	QueryResult类 - 提供数据库结果集的相关功能，对结果集所保存的数据和属性进行管理
	1. 提供按行依序提取结果的功能，每行新的列值会更新到各个Field中
	2. 提供按索引和字符串检索列的功能
	3. 提供查询属性的功能，最大行数，最大列数等等
	4. 提供游标的功能，可以定位到对应行和对应列（后续完成）
	
	注意事项
	1. 该结果集只适用于使用mysql_store_result()返回的结果集，对于mysql_use_reslut()的结果集现在还不提供支持
	2. 对于mysql_use_result使用的结果集，一般不推荐使用，后续会提供该功能的简单类
*/
//----------------------------------------------------------------------------------------------------
class Field;

class BETON_API QueryResult
{
public:
	QueryResult(MYSQL_RES* result, INT nRows, INT nCols);
	~QueryResult();

	BETON_INLINE INT GetRowCount() const		{ return m_nRows; }
	BETON_INLINE INT GetFieldCount() const		{ return m_nCols; }
	BETON_INLINE const Field* Fetch() const		{ return m_CurrentRow; } 

	BOOL NextRow();
	BETON_INLINE Field& operator [] (INT nIndex) const { ASSERT(nIndex >= 0 && nIndex < m_nCols && m_CurrentRow != NULL); return m_CurrentRow[nIndex]; }
	BETON_INLINE Field& operator [] (char* szFieldName) const;

private:
	Field::DataTypes ConvertType(enum_field_types mysqlType) const;
	
private:
	MYSQL_RES*		m_Result;			// mysql结果集
	INT				m_nRows;			// 结果集行数
	INT				m_nCols;			// 结果集列数
	Field*			m_CurrentRow;		// 当前行列集

};

} // namespace Beton