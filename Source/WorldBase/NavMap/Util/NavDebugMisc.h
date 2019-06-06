#include "..\..\WorldBase.h"
#include <assert.h>
#pragma once

namespace WorldBase
{
	//------------------------
	WORLD_BASE_API void _NavTrace(const TCHAR *lpszFormat, ...);
#ifndef NAVTRACE
#ifdef _DEBUG
#define NAVTRACE _Trace
#else
#define NAVTRACE __noop
#endif
#endif//#ifndef TRACE

	//LogÎÄ¼þ
	void _NavInitLogFile();
	WORLD_BASE_API void _NavWriteLogString(LPCTSTR s,...);
	WORLD_BASE_API void _NavWriteLogStringA(LPCSTR s,...);
#define NAVLOG		_NavWriteLogString
#define NAVLOGA		_NavWriteLogStringA

}//namespace WorldBase