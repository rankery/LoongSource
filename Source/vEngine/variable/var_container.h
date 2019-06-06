//-----------------------------------------------------------------------------
// File: var_container
// Auth: Lyp
// Date: 2004/04/02
// Last: 2004/11/17
//
// Desc: 变量容器
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine {


//-----------------------------------------------------------------------------
// 线程安全的变量容器
//-----------------------------------------------------------------------------
class VENGINE_API VarContainer
{
public:
	// 添加元素
	__forceinline  BOOL	Add(tstring& strStr, tstring& strName);

	__forceinline BOOL	Add(LPCTSTR pStr, LPCTSTR szName);
	__forceinline BOOL	Add(DWORD dwData, LPCTSTR szName);
	__forceinline BOOL	Add(FLOAT fData, LPCTSTR szName);

	// 读取元素
	__forceinline LPCTSTR GetString(LPCTSTR szName);
	__forceinline DWORD	GetDword(LPCTSTR szName);
	__forceinline FLOAT	GetFloat(LPCTSTR szName);
	__forceinline INT32	GetInt(LPCTSTR szName);

	__forceinline LPCTSTR GetString(LPCTSTR szName, LPCTSTR szField);
	__forceinline DWORD	GetDword(LPCTSTR szName, LPCTSTR szField);
	__forceinline FLOAT	GetFloat(LPCTSTR szName, LPCTSTR szField);
	__forceinline INT32	GetInt(LPCTSTR szName, LPCTSTR szField);

	__forceinline LPCTSTR GetString(LPCTSTR szName, LPCTSTR szField, LPCTSTR szDefault);
	__forceinline DWORD	GetDword(LPCTSTR szName, LPCTSTR szField, DWORD dwDefault);
	__forceinline FLOAT	GetFloat(LPCTSTR szName, LPCTSTR szField, FLOAT fDefault);
	__forceinline INT32	GetInt(LPCTSTR szName, LPCTSTR szField, INT32 nDefault);

	__forceinline LPCTSTR GetString(LPCTSTR szName, LPCTSTR szNamePostfix, LPCTSTR szField, LPCTSTR szDefault);
	__forceinline DWORD	GetDword(LPCTSTR szName, LPCTSTR szNamePostfix, LPCTSTR szField, DWORD dwDefault);
	__forceinline FLOAT	GetFloat(LPCTSTR szName, LPCTSTR szNamePostfix, LPCTSTR szField, FLOAT fDefault);
	__forceinline INT32	GetInt(LPCTSTR szName, LPCTSTR szNamePostfix, LPCTSTR szField, INT32 nDefault);

	// 设置元素，如果没有就添加并返回FALSE
	BOOL	Set(LPCTSTR szValue, LPCTSTR szName);
	BOOL	Set(DWORD dwValue, LPCTSTR szName);
	BOOL	Set(FLOAT fValue, LPCTSTR szName);

	// 删除元素
	BOOL	Erase(LPCTSTR szName);
	BOOL	Erase(LPCTSTR szName, LPCTSTR szField);
	VOID	Clear() { m_mapData.clear(); m_mapName.clear(); }

	// 其它辅助操作
	DWORD	GetVarNum() { return m_mapData.size(); }	// 得到元素数目
	std::list<tstring>&	GetVarName();	// 得到容器所有键名

	BOOL Load(LPCSTR szVFS, LPCTSTR szFileName, LPCSTR szID_KeyName="name", std::list<tstring>* pFieldList = NULL);	// 读取文件
	BOOL Save(LPCTSTR szFileName);	// 保存文件

	// 设置是否支持存盘
	VOID SetSaveSupport(BOOL bSupportSave) { m_bSupportSave = bSupportSave;	}

	// 默认不支持存盘
	VarContainer():m_bSupportSave(FALSE){}

private:
	TObjRef<Util>				m_pUtil;
	std::map<DWORD, tstring>	m_mapData;

	BOOL						m_bSupportSave;
	std::map<DWORD, tstring>	m_mapName;
	std::list<tstring>			m_listName;
};

//-----------------------------------------------------------------------------
// 添加字符串元素
//-----------------------------------------------------------------------------
BOOL VarContainer::Add(tstring& strStr, tstring& strName)
{
	DWORD dwID = m_pUtil->Crc32(strName.c_str());
	if( m_mapData.insert(std::pair<DWORD, tstring>(dwID, strStr)).second == NULL )	
		ERR(_T("Couldn't add <%s> twice to var_container"), strName.c_str());	// ID 重复

	if( m_bSupportSave )
		m_mapName.insert(std::pair<DWORD, tstring>(dwID, strName));
	return TRUE;
}

//-----------------------------------------------------------------------------
// 添加字符串元素
//-----------------------------------------------------------------------------
BOOL VarContainer::Add(LPCTSTR pStr, LPCTSTR szName)
{
	DWORD dwID = m_pUtil->Crc32(szName);
	tstring str = pStr;

	if( NULL == m_mapData.insert(std::pair<DWORD, tstring>(dwID, str)).second )	
		ERR(_T("Couldn't add <%s> twice to var_container"), szName);	// ID 重复

	if( m_bSupportSave )
	{
		tstring strName = szName;
		m_mapName.insert(std::pair<DWORD, tstring>(dwID, strName));
	}

	return TRUE;
}


//-----------------------------------------------------------------------------
// 添加DWORD元素
//-----------------------------------------------------------------------------
BOOL VarContainer::Add(DWORD dwData, LPCTSTR szName)
{
	TCHAR mess[X_LONG_STRING];
	_itot(dwData, mess, 10);
	return this->Add(mess, szName);
}


//-----------------------------------------------------------------------------
// 添加FLOAT元素
//-----------------------------------------------------------------------------
BOOL VarContainer::Add(FLOAT fData, LPCTSTR szName)
{
	CHAR mess[X_LONG_STRING];
	_gcvt(fData, 6, mess);
	return this->Add(m_pUtil->Unicode8ToUnicode(mess), szName);
}



//-----------------------------------------------------------------------------
// 读取字符串元素
//-----------------------------------------------------------------------------
LPCTSTR VarContainer::GetString(LPCTSTR szName)
{
	std::map<DWORD, tstring>::iterator it;
	DWORD dwID = m_pUtil->Crc32(szName);

	it = m_mapData.find(dwID);
	if( it == m_mapData.end() )
	{
		ERR(_T("Couldn't find <%s> in var_container"), szName);
		return NULL;	// 找不到
	}

	LPCTSTR pReturn = it->second.c_str();
	return pReturn;
}


//-----------------------------------------------------------------------------
// 读取数字元素
//-----------------------------------------------------------------------------
DWORD VarContainer::GetDword(LPCTSTR szName)
{
	LPCTSTR szValue = this->GetString(szName);
	return _tcstol(szValue, NULL, 10);
}


//-----------------------------------------------------------------------------
// 读取浮点数字元素
//-----------------------------------------------------------------------------
FLOAT VarContainer::GetFloat(LPCTSTR szName)
{
	LPCTSTR szValue = this->GetString(szName);
	return (FLOAT)_tstof(szValue);
}

//-----------------------------------------------------------------------------
// 读取整型数字元素
//-----------------------------------------------------------------------------
INT32 VarContainer::GetInt(LPCTSTR szName)
{
	LPCTSTR szValue = this->GetString(szName);
	return (INT32)_tcstol(szValue, NULL, 10);
}


//-----------------------------------------------------------------------------
// 读取字符串元素
//-----------------------------------------------------------------------------
LPCTSTR VarContainer::GetString(LPCTSTR szName, LPCTSTR szField)
{
	tstring str = szName;
	str.append(_T(" "));
	str.append(szField);
	return GetString(str.c_str());
}

//-----------------------------------------------------------------------------
// 读取数字元素
//-----------------------------------------------------------------------------
DWORD VarContainer::GetDword(LPCTSTR szName, LPCTSTR szField)
{
	tstring str = szName;
	str.append(_T(" "));
	str.append(szField);
	return GetDword(str.c_str());
}


//-----------------------------------------------------------------------------
// 读取浮点数字元素
//-----------------------------------------------------------------------------
FLOAT VarContainer::GetFloat(LPCTSTR szName, LPCTSTR szField)
{
	tstring str = szName;
	str.append(_T(" "));
	str.append(szField);
	return GetFloat(str.c_str());
}

//-----------------------------------------------------------------------------
// 读取整型数字元素
//-----------------------------------------------------------------------------
INT32 VarContainer::GetInt(LPCTSTR szName, LPCTSTR szField)
{
	tstring str = szName;
	str.append(_T(" "));
	str.append(szField);
	return GetInt(str.c_str());
}


//-----------------------------------------------------------------------------
// 读取字符串元素
//-----------------------------------------------------------------------------
LPCTSTR VarContainer::GetString(LPCTSTR szName, LPCTSTR szField, LPCTSTR szDefault)
{
	std::map<DWORD, tstring>::iterator it;
	TCHAR mess[X_LONG_STRING];

	_tcscpy(mess, szName);
	if( P_VALID(szField) )
	{
		_tcscat(mess, _T(" "));
		_tcscat(mess, szField);
	}

	DWORD dwID = m_pUtil->Crc32(mess);
	it = m_mapData.find(dwID);
	if( it == m_mapData.end() )
		return szDefault;	// 找不到

	return it->second.c_str();
}


//-----------------------------------------------------------------------------
// 读取数字元素
//-----------------------------------------------------------------------------
DWORD VarContainer::GetDword(LPCTSTR szName, LPCTSTR szField, DWORD dwDefault)
{
	LPCTSTR szReturn = this->GetString(szName, szField, NULL);
	if( NULL == szReturn )
		return dwDefault;

	return _tcstol(szReturn, NULL, 10);
}


//-----------------------------------------------------------------------------
// 读取浮点数字元素
//-----------------------------------------------------------------------------
FLOAT VarContainer::GetFloat(LPCTSTR szName, LPCTSTR szField, FLOAT fDefault)
{
	LPCTSTR szReturn = this->GetString(szName, szField, NULL);
	if( NULL == szReturn )
		return fDefault;

	return (FLOAT)_tstof(szReturn);
}

//-----------------------------------------------------------------------------
// 读取整型数字元素
//-----------------------------------------------------------------------------
INT32 VarContainer::GetInt(LPCTSTR szName, LPCTSTR szField, INT32 nDefault)
{
	LPCTSTR szReturn = this->GetString(szName, szField, NULL);
	if( NULL == szReturn )
		return nDefault;

	return (INT32)_tcstol(szReturn, NULL, 10);
}

//-----------------------------------------------------------------------------
// 读取字符串元素
//-----------------------------------------------------------------------------
LPCTSTR	VarContainer::GetString(LPCTSTR szName, LPCTSTR szNamePostfix, LPCTSTR szField, LPCTSTR szDefault)
{
	std::map<DWORD, tstring>::iterator it;
	tstring title = szName;
	title += szNamePostfix;
	if( P_VALID(szField) )
	{
		title.append(_T(" "));
		title.append(szField);
	}

	DWORD dwID = m_pUtil->Crc32(title.c_str());
	it = m_mapData.find(dwID);
	if( it == m_mapData.end() )
		return szDefault;	// 找不到

	return it->second.c_str();
}

//-----------------------------------------------------------------------------
// 读取数字元素
//-----------------------------------------------------------------------------
DWORD VarContainer::GetDword(LPCTSTR szName, LPCTSTR szNamePostfix, LPCTSTR szField, DWORD dwDefault)
{
	LPCTSTR szReturn = this->GetString(szName, szNamePostfix, szField, NULL);
	if( NULL == szReturn )
		return dwDefault;

	return _tcstol(szReturn, NULL, 10);
}


//-----------------------------------------------------------------------------
// 读取浮点数字元素
//-----------------------------------------------------------------------------
FLOAT VarContainer::GetFloat(LPCTSTR szName, LPCTSTR szNamePostfix, LPCTSTR szField, FLOAT fDefault)
{
	LPCTSTR szReturn = this->GetString(szName, szNamePostfix, szField, NULL);
	if( NULL == szReturn )
		return fDefault;

	return (FLOAT)_tstof(szReturn);
}

//-----------------------------------------------------------------------------
// 读取整型数字元素
//-----------------------------------------------------------------------------
INT32 VarContainer::GetInt(LPCTSTR szName, LPCTSTR szNamePostfix, LPCTSTR szField, INT32 nDefault)
{
	LPCTSTR szReturn = this->GetString(szName, szNamePostfix, szField, NULL);
	if( NULL == szReturn )
		return nDefault;

	return (INT32)_tcstol(szReturn, NULL, 10);
}



} // namespace vEngine {
