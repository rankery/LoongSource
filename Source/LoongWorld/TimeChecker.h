#pragma once
#include "mmsystem.h"

#ifdef _UNICODE
#define WIDEN2(x) L##x
#else
#define WIDEN2(x) x
#endif
#define WIDEN(x) WIDEN2(x)

#define CHECK_TIME()	
#define CHECK_TMIE_WITH_TIMEOUT(n)	
#define CHECK_TIME_WITH_INFO(a)
#define CHECK_TIME_MAP(m)
/*
#define CHECK_TIME()				TimeChecker _check(WIDEN(__FILE__), __LINE__, WIDEN(__FUNCTION__));
#define CHECK_TMIE_WITH_TIMEOUT(n)	TimeChecker _check(WIDEN(__FILE__), __LINE__, WIDEN(__FUNCTION__), NULL, n);
#define CHECK_TIME_WITH_INFO(a)		TimeChecker _check(WIDEN(__FILE__), __LINE__, WIDEN(__FUNCTION__), a);
#define CHECK_TIME_MAP(m)			TimeChecker _check(WIDEN(__FILE__), __LINE__, m);
*/
class TimeChecker
{
	class a2w
	{
		wchar_t* buffer;
	public:
		explicit a2w(const char* str):buffer(0)
		{
			if(str) {
				size_t nu = strlen(str);
				size_t n = ::MultiByteToWideChar(CP_ACP,0,(const char *)str,int(nu),NULL,0);
				buffer = new wchar_t[n+1];
				::MultiByteToWideChar(CP_ACP,0,(const char *)str,int(nu),buffer,int(n));
				buffer[n] = 0;
			}
		}
		~a2w() {  delete[] buffer;  }

		operator const wchar_t*() { return buffer; }

	}; 

	const TCHAR* m_pFile;
	int m_nLine;
	const TCHAR* m_pFuncName;
	const CHAR* m_pInfo;
	DWORD m_dwStartTime; 
	DWORD m_dTimeOut;
	static TObjRef<Log> s_logger;
	static void Init();
public:
	TimeChecker(const TCHAR * pFile, int line, const TCHAR* pFunc, const CHAR* pInfo = NULL, DWORD dTimeOut = TICK_TIME)
	{
		m_pFile  = pFile;
		m_nLine = line;
		m_pFuncName = pFunc;
		m_pInfo = pInfo;
		m_dwStartTime =timeGetTime();
		m_dTimeOut = dTimeOut;
	}

	~TimeChecker()
	{
		DWORD usedTime = timeGetTime() - m_dwStartTime;
		if( usedTime >= m_dTimeOut )
		{
			Init();
			a2w a(m_pInfo?m_pInfo: "NULL"); 
			s_logger->Write(_T("TimeCheck too much time used. File: %s Line: %d Funciont: %s TimeUsed: %u, Info:%s\r\n"), m_pFile, m_nLine, m_pFuncName, usedTime, (const wchar_t*)a);
		}
	}
};