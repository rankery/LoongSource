//-----------------------------------------------------------------------------
//!\file csv_obj.cpp
//!\brief csv文件对象
//!
//!\date 2004-02-11
//! last 2004-04-03
//!\author Lyp
//!
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "var_mgr.h"


namespace vEngine {
//-----------------------------------------------------------------------------
//! construction
//-----------------------------------------------------------------------------
VarMgr::VarMgr()
{
}

VarMgr::~VarMgr()
{
	// 注销全部
	tagVarDataItem* pData;
	m_mapData.ResetIterator();
	while(m_mapData.PeekNext(pData))
	{
		SAFE_DEL(pData->pVecEnum);
		SAFE_DEL(pData);
	}
}



//-----------------------------------------------------------------------------
//! 存到指定区域
//-----------------------------------------------------------------------------
VOID VarMgr::Save(XmlElement* pXmlElement, BOOL bIgnoreNull)
{
	CHAR szMess[X_LONG_STRING];
	tagVarDataItem* pData=NULL;
	m_listData.ResetIterator();

	while(m_listData.PeekNext(pData))
	{
		std::string strName(m_pUtil->UnicodeToUnicode8(pData->strName.c_str()));
		switch(pData->eType)
		{
		case EXDT_Int:
			if( !bIgnoreNull || *(INT*)pData->pVar )
				pXmlElement->SetAttribute(strName, *(INT*)pData->pVar);
			break;
		case EXDT_Enum:
			if( !bIgnoreNull || *(INT*)pData->pVar )
				pXmlElement->SetAttribute(strName, 
					m_pUtil->UnicodeToUnicode8((*pData->pVecEnum)[*(INT*)pData->pVar].c_str()));
			break;
		case EXDT_Dword:
			if( !bIgnoreNull || *(DWORD*)pData->pVar )
			{
				sprintf(szMess, "0x%X", *(DWORD*)pData->pVar);
				pXmlElement->SetAttribute(strName, szMess);
			}
			break;
		case EXDT_Float:
			if( !bIgnoreNull || *(FLOAT*)pData->pVar )
			{
				sprintf(szMess, "%f", *(FLOAT*)pData->pVar);
				pXmlElement->SetAttribute(strName, szMess);
			}
			break;
		case EXDT_String:
			if( !bIgnoreNull ||  !((tstring*)pData->pVar)->empty() )
				pXmlElement->SetAttribute(strName, m_pUtil->UnicodeToUnicode8(((tstring*)pData->pVar)->c_str()));
			break;
		}
	}

}



//-----------------------------------------------------------------------------
//! 从指定内存读取
//-----------------------------------------------------------------------------
BOOL VarMgr::Load(XmlElement* pXmlElement)
{
	XmlAttribute* pAttrib = pXmlElement->FirstAttribute();
	while(pAttrib)
	{
		DWORD dwID = m_pUtil->Crc32(m_pUtil->Unicode8ToUnicode(pAttrib->Name()));
		tagVarDataItem* pData = m_mapData.Peek(dwID);
		if( P_VALID(pData) )
		{
			switch(pData->eType)
			{
			case EXDT_Int:
				*((INT*)pData->pVar) = pAttrib->IntValue();
				break;
			case EXDT_Enum:	
				if( TIXML_WRONG_TYPE == pAttrib->QueryIntValue((INT*)pData->pVar) )	// 可能写的是数字
					for(INT n=0; n<(INT)pData->pVecEnum->size(); n++)	// 如果是字符串就搜索匹配枚举字符串
					{
						if( (*pData->pVecEnum)[n] == m_pUtil->Unicode8ToUnicode(pAttrib->Value()) )
						{
							*((INT*)pData->pVar) = n;
							break;
						}
					}
				break;
			case EXDT_Dword:
				if( pAttrib->IntValue() == 0 )
					*((DWORD*)pData->pVar) = strtoul(pAttrib->Value(), NULL, 16);
				else
					*((DWORD*)pData->pVar) = pAttrib->IntValue();
				break;
			case EXDT_Float:
				*((FLOAT*)pData->pVar) = (FLOAT)pAttrib->DoubleValue();
				break;
			case EXDT_String:
				((tstring*)pData->pVar)->assign(m_pUtil->Unicode8ToUnicode(pAttrib->Value()));
				break;
			}
		}
		pAttrib = pAttrib->Next();
	}

	return TRUE;
}


//-----------------------------------------------------------------------------
//! 为float做的特化
//-----------------------------------------------------------------------------
BOOL VarMgr::Register(LPCTSTR szName, FLOAT* pVar)
{
	DWORD dwID = m_pUtil->Crc32(szName);
	ASSERT( FALSE == m_mapData.IsExist(dwID) );	// 重复注册

	tagVarDataItem* pNew = new tagVarDataItem;
	if( !pNew )
		return FALSE;

	pNew->strName.assign(szName);
	pNew->pVar = pVar;
	pNew->eType = EXDT_Float;
	*pVar = 0.0f;

	m_mapData.Add(dwID, pNew);
	m_listData.PushBack(pNew);
	return TRUE;
}




//-----------------------------------------------------------------------------
// 为String*作的特化
//-----------------------------------------------------------------------------
BOOL VarMgr::Register(LPCTSTR szName, tstring* pVar)
{
	DWORD dwID = m_pUtil->Crc32(szName);
	ASSERT( FALSE == m_mapData.IsExist(dwID) );	// 重复注册

	tagVarDataItem* pNew = new tagVarDataItem;
	if( !pNew )
		return FALSE;

	pNew->strName.assign(szName);
	pNew->pVar = pVar;
	pNew->eType = EXDT_String;

	m_mapData.Add(dwID, pNew);
	m_listData.PushBack(pNew);
	return TRUE;
}



//-----------------------------------------------------------------------------
// 为Int/枚举作的特化
//-----------------------------------------------------------------------------
BOOL VarMgr::Register(LPCTSTR szName, INT* pVar, LPCTSTR szEnumText)
{
	DWORD dwID = m_pUtil->Crc32(szName);
	ASSERT( FALSE == m_mapData.IsExist(dwID) );	// 重复注册

	tagVarDataItem* pNew = new tagVarDataItem;
	if( !pNew )
		return FALSE;

	pNew->strName.assign(szName);
	pNew->pVar = pVar;
	if( szEnumText )
	{
		pNew->eType = EXDT_Enum;
		pNew->pVecEnum = new std::vector<tstring>;
		if( !pNew->pVecEnum )
		{
			delete(pNew);
			return FALSE;
		}
		m_pUtil->StringToToken(*pNew->pVecEnum, szEnumText);
	}
	else
	{
		pNew->eType = EXDT_Int;
	}
	
	*pVar = 0;
	m_mapData.Add(dwID, pNew);
	m_listData.PushBack(pNew);
	return TRUE;
}


//-----------------------------------------------------------------------------
// 为DWORD作的特化
//-----------------------------------------------------------------------------
BOOL VarMgr::Register(LPCTSTR szName, DWORD* pVar)
{
	DWORD dwID = m_pUtil->Crc32(szName);
	ASSERT( FALSE == m_mapData.IsExist(dwID) );	// 重复注册

	tagVarDataItem* pNew = new tagVarDataItem;
	if( !pNew )
		return FALSE;

	pNew->strName.assign(szName);
	pNew->pVar = pVar;
	pNew->eType = EXDT_Dword;
	*pVar = 0;
	m_mapData.Add(dwID, pNew);
	m_listData.PushBack(pNew);
	return TRUE;
}



//-----------------------------------------------------------------------------
//! 为tagPoint做的特化
//-----------------------------------------------------------------------------
BOOL VarMgr::Register(LPCTSTR szName, tagPoint* pVar)
{
	TCHAR szRealName[X_LONG_STRING];
	_sntprintf(szRealName, X_SHORT_STRING, _T("%s_x"), szName);

	// x
	DWORD dwID = m_pUtil->Crc32(szRealName);
	ASSERT( FALSE == m_mapData.IsExist(dwID) );	// 重复注册

	tagVarDataItem* pNew = new tagVarDataItem;
	if( !pNew )
		return FALSE;
	pNew->strName.assign(szRealName);
	pNew->pVar = &(pVar->x);
	pNew->eType = EXDT_Int;
	m_mapData.Add(dwID, pNew);
	m_listData.PushBack(pNew);

	// y
	_sntprintf(szRealName, X_SHORT_STRING, _T("%s_y"), szName);
	dwID = m_pUtil->Crc32(szRealName);
	ASSERT( FALSE == m_mapData.IsExist(dwID) );	// 重复注册
	pNew = new tagVarDataItem;
	if( !pNew )
		return FALSE;
	pNew->strName.assign(szRealName);
	pNew->pVar = &(pVar->y);
	pNew->eType = EXDT_Int;
	m_mapData.Add(dwID, pNew);
	m_listData.PushBack(pNew);

	pVar->x = 0;
	pVar->y = 0;
	return TRUE;
}


//-----------------------------------------------------------------------------
//! 为tagRect做的特化
//-----------------------------------------------------------------------------
BOOL VarMgr::Register(LPCTSTR szName, tagRect* pVar)
{
	TCHAR szRealName[X_LONG_STRING];
	_sntprintf(szRealName, X_SHORT_STRING, _T("%s_left"), szName);

	// left
	DWORD dwID = m_pUtil->Crc32(szRealName);
	ASSERT( FALSE == m_mapData.IsExist(dwID) );	// 重复注册

	tagVarDataItem* pNew = new tagVarDataItem;
	if( !pNew )
		return FALSE;
	pNew->strName.assign(szRealName);
	pNew->pVar = &(pVar->left);
	pNew->eType = EXDT_Int;
	m_mapData.Add(dwID, pNew);
	m_listData.PushBack(pNew);

	// top
	_sntprintf(szRealName, X_SHORT_STRING, _T("%s_top"), szName);
	dwID = m_pUtil->Crc32(szRealName);
	ASSERT( FALSE == m_mapData.IsExist(dwID) );	// 重复注册
	pNew = new tagVarDataItem;
	if( !pNew )
		return FALSE;
	pNew->strName.assign(szRealName);
	pNew->pVar = &(pVar->top);
	pNew->eType = EXDT_Int;
	m_mapData.Add(dwID, pNew);
	m_listData.PushBack(pNew);
	
	// right
	_sntprintf(szRealName, X_SHORT_STRING, _T("%s_right"), szName);
	dwID = m_pUtil->Crc32(szRealName);
	ASSERT( FALSE == m_mapData.IsExist(dwID) );	// 重复注册
	pNew = new tagVarDataItem;
	if( !pNew )
		return FALSE;
	pNew->strName.assign(szRealName);
	pNew->pVar = &(pVar->right);
	pNew->eType = EXDT_Int;
	m_mapData.Add(dwID, pNew);
	m_listData.PushBack(pNew);
	
	// bottom
	_sntprintf(szRealName, X_SHORT_STRING, _T("%s_bottom"), szName);
	dwID = m_pUtil->Crc32(szRealName);
	ASSERT( FALSE == m_mapData.IsExist(dwID) );	// 重复注册
	pNew = new tagVarDataItem;
	if( !pNew )
		return FALSE;
	pNew->strName.assign(szRealName);
	pNew->pVar = &(pVar->bottom);
	pNew->eType = EXDT_Int;
	m_mapData.Add(dwID, pNew);
	m_listData.PushBack(pNew);

	pVar->left = 0;
	pVar->top = 0;
	pVar->right = 0;
	pVar->bottom = 0;

	return TRUE;
}



}	// namespace vEngine {
