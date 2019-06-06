//-----------------------------------------------------------------------------
// File: Util.cpp
// Desc: Game System Util 2.0
// Auth: CTCRST
//
// Copyright (c) 2002 CTCRST Corporation All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "Util.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


DWORD CUtil::m_dwIDHolder = 0;	// 记录ID使用情况
BOOL  CUtil::m_bInitCrc = FALSE;
DWORD CUtil::CrcTable[256];


//-----------------------------------------------------------------------------
// generate the table of CRC remainders for all possible bytes
//-----------------------------------------------------------------------------
VOID CUtil::InitCrcTable()
{
	static bool bInited = false;

	if (bInited)	// 确保只初始化一次
		return;

	register int i, j;
	register unsigned long crc_accum;
	for ( i = 0;  i < 256;  i++ )
	{ 
		crc_accum = ( (unsigned long) i << 24 );
		for ( j = 0;  j < 8;  j++ )
		{
			if( crc_accum & 0x80000000L ) 
				crc_accum = ( crc_accum << 1 ) ^ POLYNOMIAL;
			else 
				crc_accum = ( crc_accum << 1 ); 
		}
		CrcTable[i] = crc_accum; 
	}

	bInited = true;
}




//-----------------------------------------------------------------------------
// calculate 32 crc
//-----------------------------------------------------------------------------
DWORD CUtil::Crc(LPCTSTR lpData)
{
	if( FALSE == m_bInitCrc )
	{
		InitCrcTable();
		m_bInitCrc = TRUE;
	}
	unsigned int result;

	//对超过127的字符， 必须用unsigned char, 否则会出错
	const unsigned char *data = (const unsigned char *)lpData;
	if( data[0] == 0 ) return 0;

	result  = *data++ << 24;
	if( *data )
	{
		result |= *data++ << 16;
		if( *data )
		{
			result |= *data++ << 8;
			if( *data ) result |= *data++;
		}
	}
	result = ~ result;

	while( *data )
	{
		result = (result << 8 | *data) ^ CrcTable[result >> 24];
		data++;
	}

	return ~result;
}



//-----------------------------------------------------------------------------
// 显示简单的信息
//-----------------------------------------------------------------------------
VOID CUtil::MsgBox(LPCTSTR lpFormat, ...)
{
//	ASSERT(0);
	static TCHAR szBuf[2048];	// FIXME: make the array safe
	va_list argptr;
	va_start(argptr, lpFormat);
	vsprintf(szBuf, lpFormat, argptr);
	va_end(argptr);
	
	::MessageBox(NULL, szBuf, "Message", MB_OK);
//	ASSERT(0);
}



//-----------------------------------------------------------------------------
// 显示简单的信息
//-----------------------------------------------------------------------------
INT CUtil::MsgBox(UINT uType, LPCTSTR lpFormat, ...)
{
	static TCHAR szBuf[2048];	// FIXME: make the array safe
	va_list argptr;
	va_start(argptr, lpFormat);
	vsprintf(szBuf, lpFormat, argptr);
	va_end(argptr);
	
	return ::MessageBox(NULL, szBuf, "Message", uType);
}




//-----------------------------------------------------------------------------
// 显示错误信息
//-----------------------------------------------------------------------------
VOID CUtil::ErrMsg(UINT uType, LPCTSTR lpFormat, ...)
{

	TCHAR szBuf[4096];	// FIXME: make the array safe
	LPVOID lpMsgBuf = NULL;

	DWORD dwErr = ::GetLastError();

	va_list argptr;
	va_start(argptr, lpFormat);
	vsprintf(szBuf, lpFormat, argptr);
	va_end(argptr);
   
	//Read Error message from system
	FormatMessage( 
    FORMAT_MESSAGE_ALLOCATE_BUFFER | 
    FORMAT_MESSAGE_FROM_SYSTEM | 
    FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
	dwErr,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (LPTSTR) &lpMsgBuf,
    0,
    NULL);

	_stprintf(szBuf, _T("%s\nSystem Error Code : %lu\nError Message : %s"), szBuf,
		dwErr, (LPCTSTR)lpMsgBuf);
	
	LocalFree( lpMsgBuf );

	::MessageBox(NULL, szBuf, "Error", uType);
}


//-----------------------------------------------------------------------------
// 显示调试信息，Release版本将不起作用
//-----------------------------------------------------------------------------
VOID CUtil::DebugMsgBox(LPCTSTR lpFormat, ...)
{

#ifdef _DEBUG
	static TCHAR szBuf[2048];	// FIXME: make the array safe
	va_list argptr;
	va_start(argptr, lpFormat);
	vsprintf(szBuf, lpFormat, argptr);
	va_end(argptr);
	
	::MessageBox(NULL, szBuf, "Message", MB_OK);
#endif

}




//-----------------------------------------------------------------------------
// 得到下一个空闲的ID
//-----------------------------------------------------------------------------
DWORD CUtil::GetFreeID()
{
	m_dwIDHolder++;		// 记录ID使用状态

	if (m_dwIDHolder >= 0x7fffffff)	// 此种情况应该永远不会发生
	{
		ASSERT(0);
		m_dwIDHolder = 1;
	}

	return m_dwIDHolder;
}





CExcept::CExcept( char* sMsg, char* sFileName, int nLineNum )
{
	strcpy(m_sMsg, sMsg);
	strcpy(m_sFileName, sFileName);
	m_nLineNum = nLineNum;
}

