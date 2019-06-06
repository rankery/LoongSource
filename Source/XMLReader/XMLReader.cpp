#include "stdafx.h"
#include "XMLReader.h"

#include "tinyxml.h"

//-----------------------------------------------------------------------------
// construction / destruction
//-----------------------------------------------------------------------------
XMLReader::XMLReader()
{
}

XMLReader::~XMLReader()
{
}

//-----------------------------------------------------------------------------
// 添加字符串元素
//-----------------------------------------------------------------------------
BOOL XMLReader::Add(LPCTSTR pStr, LPCTSTR szName)
{
	assert(szName);
	std::map<tstring, tstring>::iterator it;
	tstring title = szName;

	it = m_mapData.find(title);
	if( it != m_mapData.end() )
	{
		return FALSE;
	}
	
	tstring str = pStr;
	m_mapData.insert(std::pair<tstring, tstring>(title, str));
	return TRUE;
}


//-----------------------------------------------------------------------------
// 添加DWORD元素
//-----------------------------------------------------------------------------
BOOL XMLReader::Add(DWORD dwData, LPCTSTR szName)
{
	TCHAR mess[MAX_STR_LEN];
	_itot(dwData, mess, 10);
	return this->Add(mess, szName);
}


//-----------------------------------------------------------------------------
// 添加FLOAT元素
//-----------------------------------------------------------------------------
BOOL XMLReader::Add(FLOAT fData, LPCTSTR szName)
{
	CHAR mess[MAX_STR_LEN];
	_gcvt(fData, 10, mess);
	return this->Add(Unicode8ToUnicode( mess ), szName);
}



//-----------------------------------------------------------------------------
// 读取字符串元素
//-----------------------------------------------------------------------------
LPCTSTR XMLReader::GetString(LPCTSTR szName)
{
	assert(szName);
	std::map<tstring, tstring>::iterator it;
	tstring title = szName;

	it = m_mapData.find(title);
	if( it == m_mapData.end() )
	{
		return NULL;	// 找不到
	}

	LPCTSTR pReturn = it->second.c_str();
	return pReturn;
}


//-----------------------------------------------------------------------------
// 读取数字元素
//-----------------------------------------------------------------------------
DWORD XMLReader::GetDword(LPCTSTR szName)
{
	tstring str = this->GetString(szName);
	CutSpaceOfStringHeadAndTail(str);	// 去掉头尾的空格
	return _tcstol(str.c_str(), NULL, 10);
}


//-----------------------------------------------------------------------------
// 读取浮点数字元素
//-----------------------------------------------------------------------------
FLOAT XMLReader::GetFloat(LPCTSTR szName)
{
	tstring str = this->GetString(szName);
	CutSpaceOfStringHeadAndTail(str);	// 去掉头尾的空格
	return (FLOAT)_tstof(str.c_str());
}


//-----------------------------------------------------------------------------
// 读取字符串元素
//-----------------------------------------------------------------------------
LPCTSTR XMLReader::GetString(LPCTSTR szName, LPCTSTR szField)
{
	tstring str = szName;
	str += _T(" ");
	str += szField;
	return GetString(str.c_str());
}

//-----------------------------------------------------------------------------
// 读取数字元素
//-----------------------------------------------------------------------------
DWORD XMLReader::GetDword(LPCTSTR szName, LPCTSTR szField)
{
	tstring str = szName;
	str += _T(" ");
	str += szField;
	return GetDword(str.c_str());
}


//-----------------------------------------------------------------------------
// 读取浮点数字元素
//-----------------------------------------------------------------------------
FLOAT XMLReader::GetFloat(LPCTSTR szName, LPCTSTR szField)
{
	tstring str = szName;
	str += _T(" ");
	str += szField;
	return GetFloat(str.c_str());
}



//-----------------------------------------------------------------------------
// 读取字符串元素
//-----------------------------------------------------------------------------
LPCTSTR XMLReader::GetString(LPCTSTR szName, LPCTSTR szField, LPCTSTR szDefault)
{
	std::map<tstring, tstring>::iterator it;
	tstring title = szName;
	if( NULL != szField )
	{
		title += _T(" ");
		title += szField;
	}

	it = m_mapData.find(title);
	if( it == m_mapData.end() )
	{
		return szDefault;	// 找不到
	}

	LPCTSTR pReturn = it->second.c_str();
	return pReturn;
}


//-----------------------------------------------------------------------------
// 读取数字元素
//-----------------------------------------------------------------------------
DWORD XMLReader::GetDword(LPCTSTR szName, LPCTSTR szField, DWORD dwDefault)
{
	LPCTSTR szReturn = this->GetString(szName, szField, NULL);
	if( NULL == szReturn )
		return dwDefault;
	
	tstring str = szReturn;
	CutSpaceOfStringHeadAndTail(str);	// 去掉头尾的空格
	return _tcstol(str.c_str(), NULL, 10);
}


//-----------------------------------------------------------------------------
// 读取浮点数字元素
//-----------------------------------------------------------------------------
FLOAT XMLReader::GetFloat(LPCTSTR szName, LPCTSTR szField, FLOAT fDefault)
{
	LPCTSTR szReturn = this->GetString(szName, szField, NULL);
	if( NULL == szReturn )
		return fDefault;

	tstring str = szReturn;
	CutSpaceOfStringHeadAndTail(str);	// 去掉头尾的空格
	return (FLOAT)_tstof(str.c_str());
}



//-----------------------------------------------------------------------------
// 删除指定元素
//-----------------------------------------------------------------------------
BOOL XMLReader::Erase(LPCTSTR szName)
{
	assert(szName);
	std::map<tstring, tstring>::iterator it;
	tstring title = szName;

	it = m_mapData.find(title);
	if( it == m_mapData.end() )
	{
		return FALSE;	// 找不到
	}
	m_mapData.erase(it);
	return TRUE;
}


//-----------------------------------------------------------------------------
// 删除指定元素
//-----------------------------------------------------------------------------
BOOL XMLReader::Erase(LPCTSTR szName, LPCTSTR szField)
{
	tstring str = szName;
	str += _T(" ");
	str += szField;
	return Erase(str.c_str());
}


//-----------------------------------------------------------------------------
// 获取所有元素的名称标示
//-----------------------------------------------------------------------------
std::list<tstring>& XMLReader::GetVarName()
{
	m_listName.clear();
	std::map<tstring, tstring>::iterator it;

	DWORD dwNum = 0;
	for( it=m_mapData.begin(); it!=m_mapData.end(); ++it )
	{
		m_listName.push_back(it->first);
		dwNum++;
	}

	return m_listName;
}


//-----------------------------------------------------------------------------
// 读取文件
//-----------------------------------------------------------------------------
BOOL XMLReader::Load(IFS *pFS, LPCTSTR szFileName, LPCSTR szKeyName/* = "name"*/, std::list<tstring>* pFieldList/* = NULL*/)
{
	tstring strFileName = szFileName;
	if( strFileName.find(_T(".xml")) != tstring::npos )
	{
		XmlDocument doc;
		if( !doc.LoadFile(pFS, szFileName) )
			return FALSE;

		XmlHandle docHandle( &doc );
		XmlHandle eleHandle = docHandle.FirstChildElement("root").FirstChildElement();
		XmlElement* pElement = eleHandle.Element();
		if( NULL == pElement )	// 是否符合存盘格式
			return FALSE;

		while( NULL != pElement )	// 读入所有属性
		{
			LPCSTR szField = pElement->Attribute(szKeyName);
			if( NULL == szField )
				continue;

			tstring strField = Unicode8ToUnicode(szField);
			XmlAttribute* pAttrib = pElement->FirstAttribute();
			if( pFieldList && pAttrib )	// 如果Field需要导出
			{
				pFieldList->push_back(strField);
			}

			while(pAttrib)
			{
				tstring strName = Unicode8ToUnicode(pAttrib->Name());
				tstring strValue = Unicode8ToUnicode(pAttrib->Value());
				tstring strFullName = strName + _T(" ") + strField;

				this->Add(strValue.c_str(), strFullName.c_str());

				// 如果是value则再添加一个直接以name为键的变量
				if( strName == _T("value") )
					this->Add(strValue.c_str(), strField.c_str());

				pAttrib = pAttrib->Next();
			}

			pElement = pElement->NextSiblingElement();
		}
	}


	return TRUE;
}



//-----------------------------------------------------------------------------
// 保存文件
//-----------------------------------------------------------------------------
BOOL XMLReader::Save(LPCTSTR szFileName,LPCSTR szID_KeyName)
{
	tstring strFileName = szFileName;
	if( strFileName.find(_T(".xml")) != tstring::npos )
	{
		XmlDocument doc;
		XmlDeclaration declaration("1.0", "UTF-8", "");
		doc.InsertEndChild(declaration);
		
		XmlElement eleRoot("root");
		
		std::map<tstring, tstring>::iterator it;
		for( it=m_mapData.begin(); it!=m_mapData.end(); ++it )
		{
			std::string strName = UnicodeToUnicode8(it->first.c_str());
			std::string strValue = UnicodeToUnicode8(it->second.c_str());
			
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
				if( 0 == strcmp(pElement->Attribute(szID_KeyName), strField.c_str()) )
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
				ele.SetAttribute(szID_KeyName, strField);
				ele.SetAttribute(strName, strValue);
				eleRoot.InsertEndChild(ele);
			}
		}

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
BOOL XMLReader::Set( LPCTSTR szValue, LPCTSTR szName )
{
	BOOL bResult = this->Erase(szName);
	this->Add(szValue, szName);
	return bResult;
}


//-----------------------------------------------------------------------------
// set
//-----------------------------------------------------------------------------
BOOL XMLReader::Set( DWORD dwValue, LPCTSTR szName )
{
	BOOL bResult = this->Erase(szName);
	this->Add(dwValue, szName);
	return bResult;
}


//-----------------------------------------------------------------------------
// set
//-----------------------------------------------------------------------------
BOOL XMLReader::Set( FLOAT fValue, LPCTSTR szName )
{
	BOOL bResult = this->Erase(szName);
	this->Add(fValue, szName);
	return bResult;
}

//-----------------------------------------------------------------------------
// 去掉tstring首尾的空格
//-----------------------------------------------------------------------------
VOID XMLReader::CutSpaceOfStringHeadAndTail(tstring& string)
{
	// 必须检查:如果字符串中只有空格,不能进行此操作
	if( string.find_first_not_of(_T(" ")) != -1 )
	{
		string.assign(string, string.find_first_not_of(_T(" ")),
			string.find_last_not_of(_T(" "))-string.find_first_not_of(_T(" "))+1);
	}
	else
	{
		string.clear();
	}
}

//-----------------------------------------------------------------------------
// 将unicode转成unicode8
//-----------------------------------------------------------------------------
LPCSTR XMLReader::UnicodeToUnicode8(LPCTSTR szSrc)
{
	m_pCharPool[0] = 0;
	WideCharToMultiByte(CP_UTF8, 0, szSrc, -1, m_pCharPool, MAX_STR_LEN, NULL, NULL);
	return m_pCharPool;
}

//-----------------------------------------------------------------------------
// 将unicode8转成unicode
//-----------------------------------------------------------------------------
LPCTSTR	XMLReader::Unicode8ToUnicode(LPCSTR szSrc)
{
	m_pTCharPool[0] = 0;
	MultiByteToWideChar(CP_UTF8, 0, szSrc, -1, m_pTCharPool, MAX_STR_LEN);
	return m_pTCharPool;
}