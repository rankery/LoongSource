//-----------------------------------------------------------------------------
// File: exception
// Auth: Lyp
// Date: 2003/12/11
// Desc: 记录异常发生的环境
//-----------------------------------------------------------------------------
#pragma once
/*
namespace vEngine {
typedef unsigned long ULONG3264, *PULONG3264, DWORD3264, *PDWORD3264;

#define EXCEPTION_PROTECT_START	_set_se_translator( vEngine::SEH_Translation ); try{
#define EXCEPTION_PROTECT_END	} catch(vEngine::Exception){}

// 当EXCEPTION_PROTECT_START和EXCEPTION_PROTECT_END组合不方便使用时，可用
#define EXCEPTION_PROTECT ::SetUnhandledExceptionFilter(vEngine::SEH_ExceptionFilter);

enum EBasicType  // 来自 CVCONST.H (Microsoft DIA 2.0 SDK)
{
	EBT_NoType = 0,
	EBT_Void = 1,
	EBT_Char = 2,
	EBT_WChar = 3,
	EBT_Int = 6,
	EBT_UInt = 7,
	EBT_Float = 8,
	EBT_BCD = 9,
	EBT_Bool = 10,
	EBT_Long = 13,
	EBT_ULong = 14,
	EBT_Currency = 25,
	EBT_Date = 26,
	EBT_Variant = 27,
	EBT_Complex = 28,
	EBT_Bit = 29,
	EBT_BSTR = 30,
	EBT_Hresult = 31
};

//-----------------------------------------------------------------------------
// Exception 设计思想:
// 1.使程序能够方便的抛出异常
// 2.能够捕获Windows结构化异常,并转化为C++异常
// 3.在任何异常抛出时(包括非法操作等),能记录调用堆栈,寄存器状态等信息

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
/*
//-----------------------------------------------------------------------------
class VENGINE_API Exception
{
public:

	Exception(UINT unCode, EXCEPTION_POINTERS* pExp);
	Exception(PCONTEXT pContext);	// 手动记录函数调用堆栈时使用
	~Exception();

	static BOOL				m_bSendLog;
	static BOOL				m_bSendLogWindow;

private:
	VOID InitDebugHelp();
	BOOL GetFunctionName(LPSTR szDest, PVOID pFunction);
	
	INT  DumpAll(EXCEPTION_POINTERS *pEx);
	VOID DumpExceptionInfo(EXCEPTION_POINTERS *pEx);	// 异常信息
	VOID DumpSystemInfo();								// 普通系统信息
	VOID DumpCallStack(PCONTEXT pContext);				// 调用堆栈
	VOID DumpRegisters(PCONTEXT pContext);				// 寄存器状态
	VOID DumpInstructions(PCONTEXT pContext);			// 发生异常处的指令
	VOID DumpVariable(PCONTEXT pContext);				// 变量

	VOID SendLog();	// 打包并发送Log信息
	VOID GetSymbolPath(std::string& strPath);
	
	static BOOL CALLBACK SymEnumSymbolsProc(PSYMBOL_INFO, ULONG, PVOID);
	static BOOL FormatSymbolValue(PSYMBOL_INFO pSym, STACKFRAME * sf);
	static BOOL DumpTypeIndex(DWORD64, DWORD, DWORD3264, INT nCycle);
	static VOID FormatOutputValue(EBasicType, DWORD64, PVOID);
	static EBasicType GetBasicType( DWORD, DWORD64);

	static UINT				m_unExceptionCode;
	static HANDLE			m_hProcess;

	static ExceptionLog*	m_pLog;
	static SysInfo*			m_pSysInfo;
	static Util*			m_pUtil;
	static DiscIOMgr*		m_pDiscIO;

};


}	// namespace vEngine {
*/
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