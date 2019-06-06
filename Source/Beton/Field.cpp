//--------------------------------------------------------------------------------------------------
// File: Field.cpp
// Desc: Database column value and attributes
// Time: 2008-05-04
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
// 得到TCHAR字符串
// 函数会根据传进的nLen的值来进行相应动作，如果nLen为0，且转换成功，则返回值为需要外部提供的存储空间
// 的字符数；如果nLen不为0，则函数进行字符转换，如果转换成功，则返回转换成功的字符数，否则返回0
//----------------------------------------------------------------------------------------------------
INT Field::GetTCHAR(TCHAR* szValue, INT nLen, BOOL bCheckType) const
{
	if( bCheckType && ( NULL == szValue || NULL == m_szValue || EDBT_BLOB == m_eType || EDBT_UNKNOWN == m_eType ))
		return 0;

#if defined _UNICODE

	return MultiByteToWideChar(CP_UTF8, 0, m_szValue, -1, szValue, nLen);

#else

	if( nLen < m_uLen + 1 ) return 0;
	memcpy(szValue, m_szValue, m_uLen+1);
	return m_uLen+1;

#endif
}

//------------------------------------------------------------------------------------------------------
// 得到sstring字符串
// 如果当前设置UNICODE，函数内部进行转换，将转后后的字符串写入sstring中，否则直接拷贝
//------------------------------------------------------------------------------------------------------
sstring Field::GetUnicodeString() const
{
	if( NULL == m_szValue || EDBT_BLOB == m_eType || EDBT_UNKNOWN == m_eType )
		return sstring(_T(""));

#if defined _UNICODE

	INT nDesiredLen = MultiByteToWideChar(CP_UTF8, 0, m_szValue, -1, NULL, 0);
	if( nDesiredLen == 0 ) return sstring(_T(""));

	TCHAR* szResultValue = new TCHAR[nDesiredLen];
	if( 0 == MultiByteToWideChar(CP_UTF8, 0, m_szValue, -1, szResultValue, nDesiredLen) )
	{
		delete []szResultValue;
		return sstring(_T(""));
	}

	sstring str(szResultValue);
	delete[] szResultValue;
	return str;

#else

	return sstring(m_szValue);

#endif

}

//--------------------------------------------------------------------------------------------------------
// 得到blog字段的值，返回拷贝的字节数，如果长度不够，则返回0
//--------------------------------------------------------------------------------------------------------
BOOL Field::GetBlob(VOID* buffer, ulong uLen) const
{
	if( uLen < m_uLen ) return FALSE;

	memcpy(buffer, m_szValue, m_uLen);
	return TRUE;
}

} // namespace Beton