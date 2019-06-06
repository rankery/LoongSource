//-----------------------------------------------------------------------------
// File: mini_dump
// Auth: Lyp
// Date: 2003/12/11
// Desc: 记录异常发生的环境
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine {
typedef unsigned long ULONG3264, *PULONG3264, DWORD3264, *PDWORD3264;

#define EXCEPTION_PROTECT_START	_set_se_translator( vEngine::SEH_Translation ); try{
#define EXCEPTION_PROTECT_END	} catch(vEngine::Exception){}

// 当EXCEPTION_PROTECT_START和EXCEPTION_PROTECT_END组合不方便使用时，可用
#define EXCEPTION_PROTECT ::SetUnhandledExceptionFilter(vEngine::SEH_ExceptionFilter);

//-----------------------------------------------------------------------------
/* 标准用法: 
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR lpCmdLine, INT nCmdShow)
{
	INT nResult = -1;
	_set_se_translator( vEngine::SEH_Translation );
	try
	{
		CApp app;
		app.Initialize(hInst, lpCmdLine, nCmdShow);
		nResult = app.MainLoop();
	}
	catch(vEngine::Exception)
	{
	}
	return nResult;
}*/

// 程序中手动抛出异常时
// throw( "your exception's description" );

//-----------------------------------------------------------------------------
class VENGINE_API Exception
{
public:

	Exception(UINT unCode, EXCEPTION_POINTERS* pExp);
	static VOID Init(LPCTSTR szIP, LPCTSTR szUser, LPCTSTR szPassword, LPCTSTR szVersion=NULL);

private:
	
	static TCHAR	m_szDumpFileName[1024];
	static TCHAR	m_szSendLogIP[256];
	static TCHAR	m_szSendLogUser[256];
	static TCHAR	m_szSendLogPassword[256];


};


}	// namespace vEngine {

/*
	Finally, a hint if you use VC++ 6.0 and exception handling. For VC++, 
	a "synchronous" exception is one raised by a "throw" statement; everything 
	else is an asynchronous exception (i.e., one which the compiler cannot 
	anticipate). If you think your code may throw Structured Exceptions -- even 
	if you have a translator function! -- you must enable support for asynchronous 
	exceptions. To that end, I strongly recommend unchecking the "enable exception 
	handling" checkbox in the C++ page of the Project Options dialog and typing the 
	option "/EHsa" into the options text box. (The checkbox enabling exception 
	handling sets the equivalent of /EHcs -- no asynchronous exception support, 
	and any 'extern "C"'-funtion is assumed to never throw an exception. Now there 
	is what I call an optimistic assumption.)
*/