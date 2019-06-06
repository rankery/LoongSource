#include "stdafx.h"
#include "NavDebugMisc.h"
#include "..\NavKernel.h"

namespace WorldBase
{
	void _NavTrace(const TCHAR *lpszFormat, ...)
	{
		va_list args;
		va_start(args, lpszFormat);

		int nBuf;
		TCHAR szBuffer[512];

		nBuf = _vstprintf_s(szBuffer, 512, lpszFormat, args);
		ASSERT(nBuf < sizeof(szBuffer)); //Output truncated as it was > sizeof(szBuffer)

		OutputDebugString(szBuffer);
		va_end(args);
	}

	TCHAR _LOG_FILE_FULL_PATH[512];
	void _NavInitLogFile()
	{
		//--准备好目录
		TCHAR *szLogSubDir=_T(".\\Log\\");
		SetCurrentDirectory(NavKernel::Inst()->GetWorkPath());
		::CreateDirectory(szLogSubDir,NULL);
		//--计算好文件名称
		time_t curtime;
		time(&curtime);
		_stprintf_s(_LOG_FILE_FULL_PATH,512,_T("Log\\WorldBaseLog_%d.txt"),curtime);

		FILE *fp=NULL;
		if(_tfopen_s(&fp,_LOG_FILE_FULL_PATH,_T("wt"))==0)
		{
			TCHAR buf[512];
			::GetModuleFileName(NULL,buf,sizeof(buf));
			_ftprintf(fp,_T("[%s Log at "),buf);
			_tstrdate_s(buf);
			_ftprintf(fp,buf);
			_tstrtime_s(buf);
			_ftprintf(fp,_T(" %s]\r\n\r\n"),buf);
			fclose(fp);
		}
	}

	void _NavWriteLogString(LPCTSTR s,...)
	{
		static TCHAR buf[1024*100];
		va_list argptr;

		va_start(argptr, s);
		_vstprintf_s(buf, sizeof(buf)/sizeof(TCHAR), s, argptr);
		va_end(argptr);

		FILE *fp=NULL;
		if(_tfopen_s(&fp,_LOG_FILE_FULL_PATH,_T("at"))==0)
		{
			_ftprintf(fp,buf);
			fclose(fp);
		}
	}

	void _NavWriteLogStringA(LPCSTR s,...)
	{
		static char buf[1024*100];
		va_list argptr;

		va_start(argptr, s);
		vsprintf_s(buf, sizeof(buf)/sizeof(TCHAR), s, argptr);
		va_end(argptr);

		FILE *fp=NULL;
		if(_tfopen_s(&fp,_LOG_FILE_FULL_PATH,_T("at"))==0)
		{
			fprintf(fp,buf);
			fclose(fp);
		}
	}
}//namespace WorldBase