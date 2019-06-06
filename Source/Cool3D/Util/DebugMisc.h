#include "..\Cool3D.h"
#include <assert.h>
#pragma once

namespace Cool3D
{
	//----------------------------
#ifndef SAFE_DELETE
	#define SAFE_DELETE(pointer) if(pointer!=NULL) {delete pointer;pointer=NULL;}
#endif
#ifndef SAFE_RELEASE
	#define SAFE_RELEASE(pointer) if(pointer!=NULL)	{pointer->Release();pointer=NULL;}
#endif
#ifndef SAFE_FREE
	#define SAFE_FREE(pointer) if(pointer!=NULL) {free(pointer);pointer=NULL;}
#endif

#define SAFE_DELETEA(pointer) if(pointer!=NULL) {delete[] pointer;pointer=NULL;}
	//----------------------------

	Cool3D_API bool MyAssert(const TCHAR* szExp,const TCHAR* szFile,long int nLine);
	//------------------------
#ifndef ASSERT
#ifdef _DEBUG
#define ASSERT assert
#else
#define ASSERT	__noop
#endif
#endif//#ifndef ASSERT
	
	//------------------------
	Cool3D_API void _Trace(const TCHAR *lpszFormat, ...);
#ifndef TRACE
#ifdef _DEBUG
#define TRACE _Trace
#else
#define TRACE __noop
#endif
#endif//#ifndef TRACE
	
	//LogÎÄ¼þ
	void _InitLogFile();
	Cool3D_API void _WriteLogString(LPCTSTR s,...);
	Cool3D_API void _WriteLogStringA(LPCSTR s,...);
#define LOG		_WriteLogString
#define LOGA	_WriteLogStringA
	
}//namespace Cool3D