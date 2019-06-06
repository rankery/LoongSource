#include "stdafx.h"
#include "NavException.h"

#include <dxerr9.h>
#include <imagehlp.h>

#pragma comment(lib,"dxerr9.lib")
#pragma comment(lib,"imagehlp.lib")

namespace WorldBase
{
	//--class NavException----------------------------------------------------
	NavException::~NavException()
	{
	}

	const TCHAR* NavException::What()
	{
		return m_szMsg;
	}

	NavException::NavException(const TCHAR *msg, const TCHAR *srcfile, int line)
	{
		_sntprintf_s(m_szSource,sizeof(m_szSource)/sizeof(TCHAR)-1,_T("%s(%d)"),srcfile,line);
		_sntprintf_s(m_szMsg,sizeof(m_szMsg)/sizeof(TCHAR)-1,_T("%s"),msg);

		_NavWriteLogString(_T("%s:%s\r\n"),m_szSource,m_szMsg);
	}

	NavException::NavException(const TCHAR* msg,const TCHAR* msg2,const TCHAR* srcfile,int line)
	{
		_sntprintf_s(m_szSource,sizeof(m_szSource)/sizeof(TCHAR)-1,_T("%s(%d)"),srcfile,line);
		_sntprintf_s(m_szMsg,sizeof(m_szMsg)/sizeof(TCHAR)-1,_T("%s"),msg);

		_NavWriteLogString(_T("%s:%s,%s\r\n"),m_szSource,m_szMsg,msg2);
	}

	NavException::NavException(HRESULT hResult,const TCHAR* msg,TCHAR* srcfile,int line)
	{
		_sntprintf_s(m_szSource,sizeof(m_szSource)/sizeof(TCHAR)-1,_T("%s(%d)"),srcfile,line);
		_sntprintf_s(m_szMsg,sizeof(m_szMsg)/sizeof(TCHAR)-1,_T("%s"),DXGetErrorString9(hResult));

		_NavWriteLogString(_T("%s:%s \t"),m_szSource,m_szMsg);
		if(msg)
			_NavWriteLogString(_T("%s\r\n"),msg);
		else
			_NavWriteLogString(_T("\r\n"));
	}

}//namespace Cool3D