#pragma once
#include "mmsystem.h"

#ifdef _UNICODE
#define WIDEN2(x) L##x
#else
#define WIDEN2(x) x
#endif
#define WIDEN(x) WIDEN2(x)


#define CHECK_TIME() TimeChecker _check(WIDEN(__FILE__), __LINE__, WIDEN(__FUNCTION__));
#define CHECK_TIME_WITH_INFO(a , ot) TimeChecker _check(WIDEN(__FILE__), __LINE__, WIDEN(__FUNCTION__), a, ot);


#define CHECK_TIME_START(a)	_check.TimeStartCheck(WIDEN(__FILE__), __LINE__, WIDEN(__FUNCTION__), a);
#define CHECK_TIME_COMMIT()	_check.TimeCommitCheck();

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
	static TObjRef<Log> s_logger;
	static void Init();
public:
	TimeChecker(const TCHAR * pFile, int line, const TCHAR* pFunc, const CHAR* pInfo = NULL,DWORD dwOverTime = TICK_TIME)
	{
		m_pFile  = pFile;
		m_nLine = line;
		m_pFuncName = pFunc;
		m_pInfo = pInfo;
		m_dwStartTime =timeGetTime();
		m_dwOverTime = dwOverTime;
	}

	~TimeChecker()
	{
		DWORD usedTime = timeGetTime() - m_dwStartTime;
		if( usedTime >= m_dwOverTime )
		{	
			// 得到当前时间
			SYSTEMTIME sys_time;
			GetLocalTime(&sys_time);

			Init();
			a2w a(m_pInfo?m_pInfo: "NULL"); 
			s_logger->Write(_T("TimeCheck too much time used. File: %s Line: %d Funciont: %s TimeUsed: %u, Info:%s Date:%02d/%02d/%04d %02d:%02d:%02d\r\n")
				, m_pFile, m_nLine, m_pFuncName, usedTime, (const wchar_t*)a
				,sys_time.wMonth, sys_time.wDay, sys_time.wYear
				,sys_time.wHour, sys_time.wMinute, sys_time.wSecond);
		}
	}

	void TimeStartCheck(const TCHAR * pFile, int line, const TCHAR* pFunc, const CHAR* pInfo = NULL)
	{
		m_pFile  = pFile;
		m_nLine = line;
		m_pFuncName = pFunc;
		m_pInfo = pInfo;
		m_dwStartTime =timeGetTime();
	}

	void TimeCommitCheck()
	{
		DWORD usedTime = timeGetTime() - m_dwStartTime;
		if( usedTime >= m_dwOverTime )
		{
			// 得到当前时间
			SYSTEMTIME sys_time;
			GetLocalTime(&sys_time);

			Init();
			a2w a(m_pInfo?m_pInfo: "NULL"); 
			s_logger->Write(_T("TimeCheck too much time used. File: %s Line: %d Funciont: %s TimeUsed: %u, Info:%s  Date:%02d/%02d/%04d %02d:%02d:%02d\r\n")
				, m_pFile, m_nLine, m_pFuncName, usedTime, (const wchar_t*)a
				,sys_time.wMonth, sys_time.wDay, sys_time.wYear
				,sys_time.wHour, sys_time.wMinute, sys_time.wSecond);
		}

	}
private:
	DWORD m_dwOverTime;
};