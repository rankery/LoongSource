
#include "stdafx.h"
#include "DebugMisc.h"
#include <time.h>
#include "..\Kernel.h"

namespace Cool3D
{
	void _Trace(const TCHAR *lpszFormat, ...)
	{
		va_list args;
		va_start(args, lpszFormat);
		
		int nBuf;
		TCHAR szBuffer[512];
		
		nBuf = _vstprintf_s(szBuffer, 512, lpszFormat, args);
		ASSERT(nBuf < sizeof(szBuffer)); //Output truncated as it was > sizeof(szBuffer)
		
		//OutputDebugString("Cool3D: ");
		OutputDebugString(szBuffer);
		va_end(args);
	}
	
	TCHAR _LOG_FILE_FULL_PATH[512];
	void _InitLogFile()
	{
		//--准备好目录
		TCHAR *szLogSubDir=_T(".\\Log\\");
		SetCurrentDirectory(Kernel::Inst()->GetWorkPath());
		//_stprintf_s(szLogSubDir,_T("%sLog\\"),Kernel::Inst()->GetWorkPath());
		//szLogSubDir=_T("..\\Log\\");
		::CreateDirectory(szLogSubDir,NULL);
		//--计算好文件名称
		time_t curtime;
		time(&curtime);
		_stprintf_s(_LOG_FILE_FULL_PATH,512,_T("Log\\Cool3DLog_%d.txt"),curtime);
		//_stprintf_s(_LOG_FILE_FULL_PATH,_T("%sLog\\Cool3DLog_%d.txt")
			//,Kernel::Inst()->GetWorkPath(),curtime);

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
	
	void _WriteLogString(LPCTSTR s,...)
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
	
	void _WriteLogStringA(LPCSTR s,...)
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
}//namespace Cool3D