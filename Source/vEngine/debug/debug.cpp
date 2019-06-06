//-----------------------------------------------------------------------------
// File: debug
// Desc: Game Tool debug 2.0
// Auth: Lyp
// Date: 2003/12/11
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "debug.h"

// 使用其它部件
#include "exception.h"


namespace vEngine {
//-----------------------------------------------------------------------------
// 显示一般调试消息，并获取系统出错信息
//-----------------------------------------------------------------------------
INT Debug::ErrMsg(LPCTSTR lpFormat, ...)
{
	TCHAR szBuf[1024];
	LPVOID lpMsgBuf;
	DWORD dwErr;
	INT nResult;

	lpMsgBuf = NULL;

	// 首先获得系统错误代码
	dwErr = ::GetLastError();

	va_list argptr;
	va_start(argptr, lpFormat);
	_vstprintf_s(szBuf, sizeof(szBuf)/sizeof(TCHAR), lpFormat, argptr);
	va_end(argptr);

	if( dwErr != 0 )	// 从系统代码得到错误信息
	{
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM 
			| FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwErr,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf, 0, NULL);
		wsprintf(szBuf, _T("%s\r\nSystemCode:%lu Info:%s"), szBuf, dwErr, (CONST PTCHAR)lpMsgBuf);
		LocalFree( lpMsgBuf );
	}

	lstrcat(szBuf, _T("\r\nContinue?"));
	nResult = ::MessageBox(NULL, szBuf, _T(" Message:"), MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON1);
	if( IDNO == nResult )	// 重新抛出一个结构化异常，以便记录当前堆栈等信息
	{
		// Microsoft C++ exception code
		CONST UINT C_PLUS_PLUS_EXCEPTION	= 0xe06d7363;
		RaiseException(C_PLUS_PLUS_EXCEPTION, 0, 0, NULL);
	}

	return nResult;
}




}	// vEngine
