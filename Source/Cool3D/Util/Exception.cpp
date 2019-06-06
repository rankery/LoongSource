
#include "stdafx.h"
#include "Exception.h"

#include <dxerr9.h>

// imagehlp.h must be compiled with packing to eight-byte-boundaries,
// but does nothing to enforce that.
#pragma pack( push, before_imagehlp, 8 )
#include <imagehlp.h>
#pragma pack( pop, before_imagehlp )

#pragma comment(lib,"dxerr9.lib")
#pragma comment(lib,"imagehlp.lib")

namespace Cool3D
{	

	void Cool_SEH_Translation(UINT unCode,EXCEPTION_POINTERS* pExp)
	{
		throw Exception(pExp);
	}

	//--class Exception----------------------------------------------------
	Exception::~Exception()
	{
	}
	
	const TCHAR* Exception::What()
	{
		return m_szMsg;
	}
	
	Exception::Exception(const TCHAR *msg, const TCHAR *srcfile, int line)
	{
		_sntprintf_s(m_szSource,sizeof(m_szSource)/sizeof(TCHAR)-1,_T("%s(%d)"),srcfile,line);
		_sntprintf_s(m_szMsg,sizeof(m_szMsg)/sizeof(TCHAR)-1,_T("%s"),msg);
		
		_WriteLogString(_T("%s:%s\r\n"),m_szSource,m_szMsg);
	}

	Exception::Exception(const TCHAR* msg,const TCHAR* msg2,const TCHAR* srcfile,int line)
	{
		_sntprintf_s(m_szSource,sizeof(m_szSource)/sizeof(TCHAR)-1,_T("%s(%d)"),srcfile,line);
		_sntprintf_s(m_szMsg,sizeof(m_szMsg)/sizeof(TCHAR)-1,_T("%s"),msg);
		
		_WriteLogString(_T("%s:%s,%s\r\n"),m_szSource,m_szMsg,msg2);
	}
	
	Exception::Exception(HRESULT hResult,const TCHAR* msg,TCHAR* srcfile,int line)
	{
		_sntprintf_s(m_szSource,sizeof(m_szSource)/sizeof(TCHAR)-1,_T("%s(%d)"),srcfile,line);
		_sntprintf_s(m_szMsg,sizeof(m_szMsg)/sizeof(TCHAR)-1,_T("%s"),DXGetErrorString9(hResult));
		
		_WriteLogString(_T("%s:%s \t"),m_szSource,m_szMsg);
		if(msg)
			_WriteLogString(_T("%s\r\n"),msg);
		else
			_WriteLogString(_T("\r\n"));
	}

	Exception::Exception(EXCEPTION_POINTERS* pExp)
	{
		_tcscpy(m_szMsg,_T("SEH exception,see log"));
		_tcscpy(m_szSource,_T("SEH exception,see log"));

		SymSetOptions(SYMOPT_DEFERRED_LOADS|SYMOPT_UNDNAME 
			|SYMOPT_LOAD_LINES|SYMOPT_OMAP_FIND_NEAREST);

		SymInitialize(GetCurrentProcess(),"symbol",TRUE);

		DumpExceptionInfo(pExp->ExceptionRecord);
		DumpCallStack(pExp->ContextRecord);

		SymCleanup(GetCurrentProcess());
	}

	void Exception::DumpExceptionInfo( PEXCEPTION_RECORD pExp )
	{
		_WriteLogStringA("The program caused %s (0x%08x)\r\n",
			GetExceptionDesc(pExp->ExceptionCode),
			pExp->ExceptionCode);

		// 如果发生的是非法操作,记录是读还是写操作以及试图操作的位置
		if( pExp->ExceptionCode==STATUS_ACCESS_VIOLATION &&
			pExp->NumberParameters>=2)
		{
			if(pExp->ExceptionInformation[0])
				_WriteLogStringA("Access violation: write to location %08x\r\n",pExp->ExceptionInformation[1]);
			else
				_WriteLogStringA("Access violation: read from location %08x\r\n",pExp->ExceptionInformation[1]);
		}
	}

	const char* Exception::GetExceptionDesc( DWORD dwExceptionCode )
	{
		switch(dwExceptionCode)
		{
		case EXCEPTION_ACCESS_VIOLATION:		return "EXCEPTION_ACCESS_VIOLATION";
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:	return "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
		case EXCEPTION_BREAKPOINT:				return "EXCEPTION_BREAKPOINT"; 
		case EXCEPTION_DATATYPE_MISALIGNMENT:	return "EXCEPTION_DATATYPE_MISALIGNMENT";
		case EXCEPTION_FLT_DENORMAL_OPERAND:	return "EXCEPTION_FLT_DENORMAL_OPERAND";
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:		return "EXCEPTION_FLT_DIVIDE_BY_ZERO";
		case EXCEPTION_FLT_INEXACT_RESULT:		return "EXCEPTION_FLT_INEXACT_RESULT";
		case EXCEPTION_FLT_INVALID_OPERATION:	return "EXCEPTION_FLT_INVALID_OPERATION";
		case EXCEPTION_FLT_OVERFLOW:			return "EXCEPTION_FLT_OVERFLOW";
		case EXCEPTION_FLT_STACK_CHECK:			return "EXCEPTION_FLT_STACK_CHECK";
		case EXCEPTION_FLT_UNDERFLOW:			return "EXCEPTION_FLT_UNDERFLOW";
		case EXCEPTION_ILLEGAL_INSTRUCTION:		return "EXCEPTION_ILLEGAL_INSTRUCTION";
		case EXCEPTION_IN_PAGE_ERROR:			return "EXCEPTION_IN_PAGE_ERROR";
		case EXCEPTION_INT_DIVIDE_BY_ZERO:		return "EXCEPTION_INT_DIVIDE_BY_ZERO";
		case EXCEPTION_INT_OVERFLOW:			return "EXCEPTION_INT_OVERFLOW";
		case EXCEPTION_INVALID_DISPOSITION:		return "EXCEPTION_INVALID_DISPOSITION";
		case EXCEPTION_NONCONTINUABLE_EXCEPTION:return "EXCEPTION_NONCONTINUABLE_EXCEPTION";
		case EXCEPTION_PRIV_INSTRUCTION:		return "EXCEPTION_PRIV_INSTRUCTION";
		case EXCEPTION_SINGLE_STEP:				return "EXCEPTION_SINGLE_STEP";
		case EXCEPTION_STACK_OVERFLOW:			return "EXCEPTION_STACK_OVERFLOW";
		}

		return "unknow exception";
	}

	const char* _GetFileName(const char* pSource)
	{
		if(pSource)
		{
			const char *pResult = strrchr(pSource, '\\');
			if(pResult)
				pResult++;
			else
				pResult=pSource;
			return pResult;
		}
		return NULL;
	}

	bool Exception::GetFunctionName(char* szDest,void* pFunction)
	{
		char SymbolBuffer[sizeof(IMAGEHLP_SYMBOL)+1024];   
		DWORD dwTemp = 0;    

		PIMAGEHLP_SYMBOL pSymbol = (PIMAGEHLP_SYMBOL)SymbolBuffer;    
		memset(SymbolBuffer, 0, sizeof(SymbolBuffer));    
		pSymbol->SizeOfStruct = sizeof(SymbolBuffer);    
		pSymbol->MaxNameLength = 512;
		if( SymGetSymFromAddr(GetCurrentProcess(), (DWORD)pFunction, &dwTemp, pSymbol) )
		{        
			IMAGEHLP_LINE l;
			ZeroMemory(&l, sizeof(l));
			l.SizeOfStruct = sizeof(l);

			// 获取源文件名和源码行数
			if( SymGetLineFromAddr(GetCurrentProcess(), (DWORD)pFunction, &dwTemp, &l) )
			{
				sprintf(szDest, "%s() %s Line%lu", pSymbol->Name,_GetFileName(l.FileName),l.LineNumber);
			}
			else
			{
				strcpy( szDest, pSymbol->Name );
				strcat( szDest, "()" );

				CHAR szModulePathName[MAX_PATH+1024];
				ZeroMemory(szModulePathName, sizeof(szModulePathName));
				MEMORY_BASIC_INFORMATION MemInfo;

				// 设法取得函数所在模块名称
				if( VirtualQuery(pFunction, &MemInfo, sizeof(MemInfo)) &&
					(GetModuleFileNameA((HINSTANCE)MemInfo.AllocationBase,
					szModulePathName,
					sizeof(szModulePathName)-2) > 0) )
				{
					strcat( szDest, " in " );
					strcat( szDest, _GetFileName(szModulePathName) );
				}
			}

			return true;    
		}
		else	// 得不到函数符号
		{
			CHAR szModulePathName[MAX_PATH+1024];
			ZeroMemory(szModulePathName, sizeof(szModulePathName));
			MEMORY_BASIC_INFORMATION MemInfo;

			// 如果得不到函数符号,就设法取得函数所在模块名称
			if( VirtualQuery(pFunction, &MemInfo, sizeof(MemInfo)) &&
				(GetModuleFileNameA((HINSTANCE)MemInfo.AllocationBase,
				szModulePathName, sizeof(szModulePathName)-2) > 0) )
			{
				strcpy( szDest, _GetFileName(szModulePathName) );
			}
			else
			{
				strcpy( szDest, "Unknown Function" );
			}
			return false;
		}
	}

	void Exception::DumpCallStack( PCONTEXT pContext )
	{
		_WriteLogStringA("\r\n");
		_WriteLogStringA("CallStack:\r\n");

		STACKFRAME StackFrame;
		memset(&StackFrame, 0, sizeof(STACKFRAME));
		StackFrame.AddrPC.Mode = AddrModeFlat;    
		StackFrame.AddrPC.Offset = pContext->Eip;    
		StackFrame.AddrStack.Mode = AddrModeFlat;    
		StackFrame.AddrStack.Offset = pContext->Esp;    
		StackFrame.AddrFrame.Mode = AddrModeFlat;    
		StackFrame.AddrFrame.Offset = pContext->Ebp;

		BOOL bRet=TRUE;
		char szName[1024];

		// StackWalk会更改context,所以这里我们必须复制一份给它用
		// 因为CONTEXT结构中没有指针成员,所以可以安全的完全复制
		CONTEXT context;
		memcpy(&context,pContext,sizeof(context));

		for(int i=0; i<256&&bRet; i++)    
		{       
			// 上溯函数
			bRet = StackWalk(IMAGE_FILE_MACHINE_I386, GetCurrentProcess(), 
				GetCurrentThread(), &StackFrame, &context, NULL, 
				SymFunctionTableAccess, SymGetModuleBase, NULL);

			if(bRet)
			{
				GetFunctionName(szName, (PVOID)StackFrame.AddrPC.Offset);
				_WriteLogStringA("%s at %#08lx\r\n", szName, 
					StackFrame.AddrPC.Offset);
			}
		}

	}

}//namespace Cool3D