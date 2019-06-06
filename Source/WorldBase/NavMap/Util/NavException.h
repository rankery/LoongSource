#include "..\..\WorldBase.h"
#include "NavDebugMisc.h"
#pragma once

namespace WorldBase
{

#define NAV_EXCEPTION(info)	NavException(info,__TFILE__,__LINE__)

#define NAV_THROW_NULL(expression,info) if((expression)==NULL) throw NAV_EXCEPTION(info)
#define NAV_THROW_NULLEX(expression,info,info2) if((expression)==NULL) throw NavException(info,info2,__TFILE__,__LINE__)
#define NAV_THROW_FAILED(hResult,info)	if(FAILED(hResult))	throw NavException(hResult,info,__TFILE__,__LINE__)
#define NAV_THROW_FALSE(bResult,info)	if(bResult==false)	throw NavException(info,__TFILE__,__LINE__)

	/** \class NavException
		\brief 异常类. 提供一些Debug便利, 自动记录到log文件
	*/
	class WORLD_BASE_API NavException  
	{
	public:
		NavException(const TCHAR* msg,const TCHAR* srcfile,int line);
		NavException(const TCHAR* msg,const TCHAR* msg2,const TCHAR* srcfile,int line);
		NavException(HRESULT hResult,const TCHAR* msg,TCHAR* srcfile,int line);
		const TCHAR* What();
		virtual ~NavException();

	protected:
		TCHAR	m_szMsg[256];
		TCHAR	m_szSource[256];
	};

}//namespace Cool3D