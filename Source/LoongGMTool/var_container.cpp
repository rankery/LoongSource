//-----------------------------------------------------------------------------
// File: var_container.cpp
// Auth: Lyp
// Date: 2004/04/02
// Last: 2004/11/17
//
// Desc: 变量容器
// Copyright (c) 1985-2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "var_container.h"

#include "tinyxml.h"

//-----------------------------------------------------------------------------
// construction / destruction
//-----------------------------------------------------------------------------
VarContainer::VarContainer()
{
	InitializeCriticalSection(&m_Lock);
	m_pUtil = new Util;
}

VarContainer::~VarContainer()
{
	DeleteCriticalSection(&m_Lock);
	SAFE_DEL(m_pUtil);
}

//-----------------------------------------------------------------------------
// 添加字符串元素
//-----------------------------------------------------------------------------
BOOL VarContainer::Add(LPCTSTR pStr, LPCTSTR szName)
{
	ASSERT(szName);
	std::map<tstring, tstring>::iterator it;
	tstring title = szName;

	EnterCriticalSection(&m_Lock);	
	it = m_mapData.find(title);
	if( it != m_mapData.end() )
	{
		LeaveCriticalSection(&m_Lock);
		return FALSE;
	}
	
	tstring str = pStr;
	m_mapData.insert(std::pair<tstring, tstring>(title, str));
	LeaveCriticalSection(&m_Lock);
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
	ASSERT(szName);
	std::map<tstring, tstring>::iterator it;
	tstring title = szName;

	EnterCriticalSection(&m_Lock);
	it = m_mapData.find(title);
	if( it == m_mapData.end() )
	{
		LeaveCriticalSection(&m_Lock);
		return NULL;	// 找不到
	}

	LPCTSTR pReturn = it->second.c_str();
	LeaveCriticalSection(&m_Lock);
	return pReturn;
}


//-----------------------------------------------------------------------------
// 读取数字元素
//-----------------------------------------------------------------------------
DWORD VarContainer::GetDword(LPCTSTR szName)
{
	tstring str = this->GetString(szName);
	m_pUtil->CutSpaceOfStringHeadAndTail(str);	// 去掉头尾的空格
	return _tcstol(str.c_str(), NULL, 10);
}


//-----------------------------------------------------------------------------
// 读取浮点数字元素
//-----------------------------------------------------------------------------
FLOAT VarContainer::GetFloat(LPCTSTR szName)
{
	tstring str = this->GetString(szName);
	m_pUtil->CutSpaceOfStringHeadAndTail(str);	// 去掉头尾的空格
	return (FLOAT)_tstof(str.c_str());
}


//-----------------------------------------------------------------------------
// 读取字符串元素
//-----------------------------------------------------------------------------
LPCTSTR VarContainer::GetString(LPCTSTR szName, LPCTSTR szField)
{
	tstring str = szName;
	str += _T(" ");
	str += szField;
	return GetString(str.c_str());
}

//-----------------------------------------------------------------------------
// 读取数字元素
//-----------------------------------------------------------------------------
DWORD VarContainer::GetDword(LPCTSTR szName, LPCTSTR szField)
{
	tstring str = szName;
	str += _T(" ");
	str += szField;
	return GetDword(str.c_str());
}


//-----------------------------------------------------------------------------
// 读取浮点数字元素
//-----------------------------------------------------------------------------
FLOAT VarContainer::GetFloat(LPCTSTR szName, LPCTSTR szField)
{
	tstring str = szName;
	str += _T(" ");
	str += szField;
	return GetFloat(str.c_str());
}



//-----------------------------------------------------------------------------
// 读取字符串元素
//-----------------------------------------------------------------------------
LPCTSTR VarContainer::GetString(LPCTSTR szName, LPCTSTR szField, LPCTSTR szDefault)
{
	std::map<tstring, tstring>::iterator it;
	tstring title = szName;
	if( P_VALID(szField) )
	{
		title += _T(" ");
		title += szField;
	}

	EnterCriticalSection(&m_Lock);
	it = m_mapData.find(title);
	if( it == m_mapData.end() )
	{
		LeaveCriticalSection(&m_Lock);
		return szDefault;	// 找不到
	}

	LPCTSTR pReturn = it->second.c_str();
	LeaveCriticalSection(&m_Lock);
	return pReturn;
}


//-----------------------------------------------------------------------------
// 读取数字元素
//-----------------------------------------------------------------------------
DWORD VarContainer::GetDword(LPCTSTR szName, LPCTSTR szField, DWORD dwDefault)
{
	LPCTSTR szReturn = this->GetString(szName, szField, NULL);
	if( NULL == szReturn )
		return dwDefault;
	
	tstring str = szReturn;
	m_pUtil->CutSpaceOfStringHeadAndTail(str);	// 去掉头尾的空格
	return _tcstol(str.c_str(), NULL, 10);
}


//-----------------------------------------------------------------------------
// 读取浮点数字元素
//-----------------------------------------------------------------------------
FLOAT VarContainer::GetFloat(LPCTSTR szName, LPCTSTR szField, FLOAT fDefault)
{
	LPCTSTR szReturn = this->GetString(szName, szField, NULL);
	if( NULL == szReturn )
		return fDefault;

	tstring str = szReturn;
	m_pUtil->CutSpaceOfStringHeadAndTail(str);	// 去掉头尾的空格
	return (FLOAT)_tstof(str.c_str());
}



//-----------------------------------------------------------------------------
// 删除指定元素
//-----------------------------------------------------------------------------
BOOL VarContainer::Erase(LPCTSTR szName)
{
	ASSERT(szName);
	std::map<tstring, tstring>::iterator it;
	tstring title = szName;

	EnterCriticalSection(&m_Lock);
	it = m_mapData.find(title);
	if( it == m_mapData.end() )
	{
		LeaveCriticalSection(&m_Lock);
		return FALSE;	// 找不到
	}

	m_mapData.erase(it);
	LeaveCriticalSection(&m_Lock);
	return TRUE;
}


//-----------------------------------------------------------------------------
// 删除指定元素
//-----------------------------------------------------------------------------
BOOL VarContainer::Erase(LPCTSTR szName, LPCTSTR szField)
{
	tstring str = szName;
	str += _T(" ");
	str += szField;
	return Erase(str.c_str());
}


//-----------------------------------------------------------------------------
// 获取所有元素的名称标示
//-----------------------------------------------------------------------------
std::list<tstring>& VarContainer::GetVarName()
{
	m_listName.clear();
	std::map<tstring, tstring>::iterator it;

	DWORD dwNum = 0;
	EnterCriticalSection(&m_Lock);
	for( it=m_mapData.begin(); it!=m_mapData.end(); it++ )
	{
		m_listName.push_back(it->first);
		dwNum++;
	}

	LeaveCriticalSection(&m_Lock);
	return m_listName;
}


//-----------------------------------------------------------------------------
// 读取文件
//-----------------------------------------------------------------------------
BOOL VarContainer::Load(LPCSTR szVFS, LPCTSTR szFileName, LPCSTR szKeyName/* = "name"*/, std::list<tstring>* pFieldList/* = NULL*/)
{
	tstring strFileName = szFileName;
	if( strFileName.find(_T(".xml")) != tstring::npos )
	{
		XmlDocument doc;
		if( !doc.LoadFile(szFileName) )
			return FALSE;

		XmlHandle docHandle( &doc );
		XmlHandle eleHandle = docHandle.FirstChildElement("root").FirstChildElement();
		XmlElement* pElement = eleHandle.Element();
		if( !P_VALID(pElement) )	// 是否符合存盘格式
			return FALSE;

		while( P_VALID(pElement) )	// 读入所有属性
		{
			LPCSTR szField = pElement->Attribute(szKeyName);
			if( !P_VALID(szField) )
				continue;

			tstring strField = m_pUtil->Unicode8ToUnicode(szField);
			XmlAttribute* pAttrib = pElement->FirstAttribute();
			if( pFieldList && pAttrib )	// 如果Field需要导出
			{
				pFieldList->push_back(strField);
			}

			while(pAttrib)
			{
				tstring strName = m_pUtil->Unicode8ToUnicode(pAttrib->Name());
				tstring strValue = m_pUtil->Unicode8ToUnicode(pAttrib->Value());
				tstring strFullName = strName + _T(" ") + strField;

				this->Add(strValue.c_str(), strFullName.c_str());

				// 如果是value则再添加一个直接以name为键的变量
				if( strName == _T("value") )
					this->Add(strValue.c_str(), strField.c_str());

				pAttrib = pAttrib->Next();
			}

			pElement = pElement->NextSiblingElement();
		}

		return TRUE;
	}

	return FALSE;
}



//-----------------------------------------------------------------------------
// 保存文件
//-----------------------------------------------------------------------------
BOOL VarContainer::Save(LPCTSTR szFileName)
{
	tstring strFileName = szFileName;
	if( strFileName.find(_T(".xml")) != tstring::npos )
	{
		XmlDocument doc;
		XmlDeclaration declaration("1.0", "UTF-8", "");
		doc.InsertEndChild(declaration);
		
		XmlElement eleRoot("root");
		
		std::map<tstring, tstring>::iterator it;
		EnterCriticalSection(&m_Lock);
		for( it=m_mapData.begin(); it!=m_mapData.end(); it++ )
		{
			std::string strName = m_pUtil->UnicodeToUnicode8(it->first.c_str());
			std::string strValue = m_pUtil->UnicodeToUnicode8(it->second.c_str());
			
			if( strName.find(" ") == std::string::npos )
				continue;

			std::string strField;
			strField.assign(strName, strName.find(" ")+1, strName.size()-strName.find(" ")+1);
			strName.assign(strName, 0, strName.find(" "));

			// 找到对应的ele
			BOOL bFound = FALSE;
			XmlElement* pElement = eleRoot.FirstChildElement();
			while( pElement )
			{
				if( 0 == strcmp(pElement->Attribute("name"), strField.c_str()) )
				{
					pElement->SetAttribute(strName, strValue);
					bFound = TRUE;
					break;
				}

				pElement = pElement->NextSiblingElement();
			}
			
			if( !bFound )
			{
				XmlElement ele("item");
				ele.SetAttribute("name", strField);
				ele.SetAttribute(strName, strValue);
				eleRoot.InsertEndChild(ele);
			}
		}

		LeaveCriticalSection(&m_Lock);
		doc.InsertEndChild(eleRoot);
		doc.SaveFile(szFileName);

	}
	else
	{
		return FALSE; // INIObj不支持写
	}

	return TRUE;
}


//-----------------------------------------------------------------------------
// set
//-----------------------------------------------------------------------------
BOOL VarContainer::Set( LPCTSTR szValue, LPCTSTR szName )
{
	BOOL bResult = this->Erase(szName);
	this->Add(szValue, szName);
	return bResult;
}


//-----------------------------------------------------------------------------
// set
//-----------------------------------------------------------------------------
BOOL VarContainer::Set( DWORD dwValue, LPCTSTR szName )
{
	BOOL bResult = this->Erase(szName);
	this->Add(dwValue, szName);
	return bResult;
}


//-----------------------------------------------------------------------------
// set
//-----------------------------------------------------------------------------
BOOL VarContainer::Set( FLOAT fValue, LPCTSTR szName )
{
	BOOL bResult = this->Erase(szName);
	this->Add(fValue, szName);
	return bResult;
}

