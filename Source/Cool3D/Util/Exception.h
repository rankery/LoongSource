#include "..\Cool3D.h"
#include "DebugMisc.h"
#pragma once

namespace Cool3D
{

#ifndef EXCEPTION
#define EXCEPTION(info)	Cool3D::Exception(info,__TFILE__,__LINE__)
#endif
	
#ifndef THROW_NULL
	#define THROW_NULL(expression,info) if((expression)==NULL) throw EXCEPTION(info)
#endif

#ifndef THROW_NULLEX
	#define THROW_NULLEX(expression,info,info2) if((expression)==NULL) throw Cool3D::Exception(info,info2,__TFILE__,__LINE__)
#endif

#ifndef THROW_FAILED
	#define THROW_FAILED(hResult,info)	if(FAILED(hResult))	throw Cool3D::Exception(hResult,info,__TFILE__,__LINE__)
#endif

#ifndef THROW_FALSE
	#define THROW_FALSE(bResult,info)	if(bResult==false)	throw Cool3D::Exception(info,__TFILE__,__LINE__)
#endif

	/** \class Exception
		\brief 异常类. 提供一些Debug便利, 包括构造对象时记录调用堆栈, 自动记录到log文件
	*/
	class Cool3D_API Exception  
	{
	public:
		Exception(const TCHAR* msg,const TCHAR* srcfile,int line);
		Exception(const TCHAR* msg,const TCHAR* msg2,const TCHAR* srcfile,int line);
		Exception(HRESULT hResult,const TCHAR* msg,TCHAR* srcfile,int line);
		Exception(EXCEPTION_POINTERS* pExp);
		const TCHAR* What();
		virtual ~Exception();
		
	protected:
		bool GetFunctionName(char* szDest, void* pFunction);
		const char* GetExceptionDesc(DWORD dwExceptionCode);

		void DumpExceptionInfo(PEXCEPTION_RECORD pExp);
		void DumpCallStack(PCONTEXT pContext);

		TCHAR	m_szMsg[256];
		TCHAR	m_szSource[256];
	};

	/** 结构化异常回调函数
		\remark 外部需要调用 _set_se_translator(Cool3D::Cool_SEH_Translation)
	*/
	Cool3D_API void Cool_SEH_Translation(UINT unCode,EXCEPTION_POINTERS* pExp);
	
}//namespace Cool3D