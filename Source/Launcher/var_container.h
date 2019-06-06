//-----------------------------------------------------------------------------
// File: var_container
// Auth: Lyp
// Date: 2004/04/02
// Last: 2004/11/17
//
// Desc: 变量容器
// Copyright (c) 1985-2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

class Util;
//-----------------------------------------------------------------------------
// 线程安全的变量容器
//-----------------------------------------------------------------------------
class VarContainer
{
public:
	// 添加元素
	BOOL	Add(LPCTSTR pStr, LPCTSTR szName);
	BOOL	Add(DWORD dwData, LPCTSTR szName);
	BOOL	Add(FLOAT fData, LPCTSTR szName);


	// 读取元素
	LPCTSTR	GetString(LPCTSTR szName);
	DWORD	GetDword(LPCTSTR szName);
	FLOAT	GetFloat(LPCTSTR szName);

	LPCTSTR	GetString(LPCTSTR szName, LPCTSTR szField);
	DWORD	GetDword(LPCTSTR szName, LPCTSTR szField);
	FLOAT	GetFloat(LPCTSTR szName, LPCTSTR szField);

	LPCTSTR	GetString(LPCTSTR szName, LPCTSTR szField, LPCTSTR szDefault);
	DWORD	GetDword(LPCTSTR szName, LPCTSTR szField, DWORD dwDefault);
	FLOAT	GetFloat(LPCTSTR szName, LPCTSTR szField, FLOAT fDefault);

	// 设置元素，如果没有就添加并返回FALSE
	BOOL	Set(LPCTSTR szValue, LPCTSTR szName);
	BOOL	Set(DWORD dwValue, LPCTSTR szName);
	BOOL	Set(FLOAT fValue, LPCTSTR szName);


	// 删除元素
	BOOL	Erase(LPCTSTR szName);
	BOOL	Erase(LPCTSTR szName, LPCTSTR szField);
	VOID	Clear() { m_mapData.clear(); }


	// 其它辅助操作
	DWORD	GetVarNum() { return m_mapData.size(); }	// 得到元素数目
	std::list<tstring>&	GetVarName();	// 得到容器所有键名

	BOOL Load(LPCSTR szVFS, LPCTSTR szFileName, LPCSTR szKeyName="name", std::list<tstring>* pFieldList = NULL);	// 读取文件
	BOOL Save(LPCTSTR szFileName);	// 保存文件


	VarContainer();
	~VarContainer();

private:
	Util*						m_pUtil;
	std::map<tstring, tstring>	m_mapData;
	std::list<tstring>			m_listName;
	CRITICAL_SECTION			m_Lock;
};


