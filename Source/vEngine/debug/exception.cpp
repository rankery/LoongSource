//-----------------------------------------------------------------------------
// File: exception
// Auth: Lyp
// Date: 2003/12/11
// Last: 2004/3/7
// Desc: 记录异常发生的环境
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "exception.h"

/*
// 使用其它部件
#include "..\resource\resource.h"
#include "..\system\sysinfo.h"
#include "..\system\disciomgr.h"
#include "xzip.h"
#include "..\network\ftpclient.h"
#include "..\network\stream_transport.h"
#include <psapi.h>		// for EnumProcessModules
//#include <mapi.h>

//#pragma comment(lib, "dbghelp.lib")	// 此库由微软发行


namespace vEngine {
#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

#define WRITE_SEPARATOR()	{ m_pLog->Write("\r\n"); }


CONST INT MAX_CODE_BYTE_DUMP	= 16;	// 记录多少字节指令
CONST INT ONE_KILO				= 1024;
CONST INT ONE_MEGA				= ONE_KILO*ONE_KILO;

UINT			Exception::m_unExceptionCode = 0;
HANDLE			Exception::m_hProcess = 0;
BOOL			Exception::m_bSendLog = TRUE;
BOOL			Exception::m_bSendLogWindow = TRUE;
ExceptionLog*	Exception::m_pLog = 0;
SysInfo*		Exception::m_pSysInfo = 0;
Util*			Exception::m_pUtil = 0;
DiscIOMgr*		Exception::m_pDiscIO = 0;
//-----------------------------------------------------------------------------
// construction / destruction
//-----------------------------------------------------------------------------
Exception::Exception(UINT unCode, EXCEPTION_POINTERS* pExp)
{
	m_pLog = TObjRef<ExceptionLog>().operator->();
	m_pSysInfo = TObjRef<SysInfo>().operator->();
	m_pUtil = TObjRef<Util>().operator->();
	m_pDiscIO = TObjRef<DiscIOMgr>().operator->();

	m_unExceptionCode = unCode;
	InitDebugHelp();
	DumpAll(pExp);

	SendLog();	// 打包并发送Log信息
}


// 手动记录函数调用堆栈时使用
Exception::Exception(PCONTEXT pContext)
{
	m_pLog = TObjRef<ExceptionLog>().operator->();
	m_pSysInfo = TObjRef<SysInfo>().operator->();
	m_pUtil = TObjRef<Util>().operator->();
	m_pDiscIO = TObjRef<DiscIOMgr>().operator->();

	FILETIME CurrentTime;
	GetSystemTimeAsFileTime(&CurrentTime);
	CHAR szTime[100];
	m_pSysInfo->FormatTime(szTime, CurrentTime);

	WRITE_SEPARATOR();
	WRITE_SEPARATOR();
	WRITE_SEPARATOR();

	m_pLog->Write( "manual record(%s):\r\n", szTime );

	InitDebugHelp();
	DumpCallStack(pContext);
	DumpRegisters(pContext);
	DumpInstructions(pContext);
	DumpVariable(pContext);

	SendLog();	// 打包并发送Log信息
}



Exception::~Exception()
{
	SymCleanup(GetCurrentProcess());
}


//-----------------------------------------------------------------------------
// 计算Symbol路径
//-----------------------------------------------------------------------------
VOID Exception::GetSymbolPath(std::string& strPath)
{
	CHAR szPath[MAX_PATH*2];
	GetModuleFileNameA(NULL, szPath, MAX_PATH);
	CHAR *pResult = strrchr(szPath, '\\');	// 截去路径
	if(pResult)
		*pResult = 0;
	sprintf(szPath, "%s\\symbol", szPath);	// 得到了绝对路径

	CHAR szCurPath[MAX_PATH*2];
	::GetCurrentDirectoryA(MAX_PATH*2-1, szCurPath);

	if( 0 == strcmp(szPath, szCurPath) )
	{
		// 当前路径既是符号表路径
		strPath = ".";
		return;
	}

	// 查找相对路径,因为绝对路径可能包含中文字符，而SymInitialize等函数无法识别中文字符
	pResult = strstr(szPath, szCurPath);	// 在全路径中寻找当前路径
	if( pResult && strlen(szPath) > strlen(szCurPath) )
	{
		// 得到相对路径
		CHAR* pStart = szPath + strlen(szCurPath) + 1;
		strPath = pStart;
	}
	else
	{
		// 无法取到相对路径，只能用绝对路径了
		strPath = szPath;		
	}
}


//-----------------------------------------------------------------------------
// 初始化符号表
//-----------------------------------------------------------------------------
VOID Exception::InitDebugHelp()
{
	m_pLog->Create();

	std::string strSymPath;
	this->GetSymbolPath(strSymPath);

	SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_UNDNAME 
		| SYMOPT_LOAD_LINES | SYMOPT_OMAP_FIND_NEAREST);

	if( FALSE == m_pSysInfo->IsNT() )
		m_hProcess = (HANDLE)GetCurrentProcessId();
	else
		m_hProcess = GetCurrentProcess();

	if( FALSE == SymInitialize(m_hProcess, strSymPath.c_str(), TRUE) )
	{
		typedef  BOOL (WINAPI *MYPROC1)(HANDLE, HMODULE*, DWORD, LPDWORD); 
		typedef DWORD (WINAPI *MYPROC2)(HANDLE, HMODULE, LPSTR, DWORD);

		MYPROC1 ProcEnumProcessModules;
		MYPROC2 ProcGetModuleFileNameEx;

		HMODULE hHandle = LoadLibrary(_T("psapi.dll"));	// for EnumProcessModules
		if( NULL != hHandle )
		{
			ProcEnumProcessModules = (MYPROC1)GetProcAddress(hHandle, "EnumProcessModules");
			ProcGetModuleFileNameEx = (MYPROC2)GetProcAddress(hHandle, "GetModuleFileNameExA");
			if( NULL != ProcEnumProcessModules && NULL != ProcGetModuleFileNameEx )
			{
				// 设置符号寻找路径
				SymSetSearchPath(m_hProcess, strSymPath.c_str());

				HMODULE hMods[1024];
				DWORD cbNeeded;
				// 载入所有模块的符号表
				// Get a list of all the modules in this process.
				HANDLE hProcess = ::GetCurrentProcess();
				if( (ProcEnumProcessModules)(hProcess, hMods, sizeof(hMods), &cbNeeded) )
				{
					for( unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++ )
					{
						CHAR szModName[MAX_PATH+1];

						// Get the full path to the module's file.
						if( (ProcGetModuleFileNameEx)( hProcess, hMods[i], szModName,
							sizeof(szModName)))
						{
							// load all modules' symbol
							SymLoadModule(m_hProcess, NULL, szModName, NULL, 0, 0);
						}
					}
				}
			}
		}
	}
}


//-----------------------------------------------------------------------------
// 通过函数起始地址获得函数名
//-----------------------------------------------------------------------------
BOOL Exception::GetFunctionName(LPSTR szDest, PVOID pFunction)
{    
	CHAR SymbolBuffer[sizeof(IMAGEHLP_SYMBOL)+1024];   
	DWORD dwTemp = 0;    
	
	PIMAGEHLP_SYMBOL pSymbol = (PIMAGEHLP_SYMBOL)SymbolBuffer;    
	memset(SymbolBuffer, 0, sizeof(SymbolBuffer));    
	pSymbol->SizeOfStruct = sizeof(SymbolBuffer);    
	pSymbol->MaxNameLength = 512;
	if( SymGetSymFromAddr(m_hProcess, (DWORD)pFunction, &dwTemp, pSymbol) )
	{        
		IMAGEHLP_LINE l;
		ZeroMemory(&l, sizeof(l));
		l.SizeOfStruct = sizeof(l);

		// 获取源文件名和源码行数
		if( SymGetLineFromAddr(m_hProcess, (DWORD)pFunction, &dwTemp, &l) )
		{
			sprintf(szDest, "%s() %s Line%lu", pSymbol->Name, 
				m_pSysInfo->GetFilePart(l.FileName), l.LineNumber);
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
				strcat( szDest, m_pSysInfo->GetFilePart(szModulePathName) );
			}
		}

		return TRUE;    
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
			strcpy( szDest, m_pSysInfo->GetFilePart(szModulePathName) );
		}
		else
		{
			strcpy( szDest, "Unknown Function" );
		}
		return FALSE;
	}
}


//-----------------------------------------------------------------------------
// 记录当前环境
//-----------------------------------------------------------------------------
INT Exception::DumpAll(EXCEPTION_POINTERS *pEx)
{
	static BOOL bFirstTime = TRUE;
	
	// 避免递归调用,递归调用表示此函数本身发生异常
	if( !bFirstTime )
		return EXCEPTION_CONTINUE_SEARCH;
	bFirstTime = FALSE;

	// 注意下列这些调用中不能对pEx所指向的区域的内容进行修改
	// 否则后继调用没法正常进行
	DumpExceptionInfo(pEx);
	DumpSystemInfo();
	DumpCallStack(pEx->ContextRecord);
	DumpRegisters(pEx->ContextRecord);
	DumpInstructions(pEx->ContextRecord);
	DumpVariable(pEx->ContextRecord);

	// 使系统停止继续搜索异常匹配
	return EXCEPTION_EXECUTE_HANDLER;
}


//-----------------------------------------------------------------------------
// 记录异常发生的信息
//-----------------------------------------------------------------------------
VOID Exception::DumpExceptionInfo(EXCEPTION_POINTERS *pEx)
{
	// 记录程序名和用户名和时间
	CHAR szModuleName[MAX_PATH*2];
	ZeroMemory(szModuleName, sizeof(szModuleName));
	if( GetModuleFileNameA(0, szModuleName, MAX_PATH*2-2) <= 0 )
		strcpy(szModuleName, "Unknown");

	CHAR* pszFilePart = m_pSysInfo->GetFilePart(szModuleName);

	// Extract the file name portion and remove it's file extension
	CHAR szFileName[MAX_PATH*2];
	strcpy(szFileName, pszFilePart);

	CHAR szUserName[200];
	ZeroMemory(szUserName, sizeof(szUserName));
	DWORD UserNameSize = 200-2;
	if( !GetUserNameA(szUserName, &UserNameSize) )
		strcpy(szUserName, "Unknown");

	FILETIME CurrentTime;
	GetSystemTimeAsFileTime(&CurrentTime);
	CHAR szTime[100];
	m_pSysInfo->FormatTime(szTime, CurrentTime);

	WRITE_SEPARATOR();
	m_pLog->Write( "%s run by %s in %s crashed.\r\n", szFileName, szUserName, szTime );

	// 记录异常名称
	PEXCEPTION_RECORD Exception = pEx->ExceptionRecord;
	PCONTEXT          Context   = pEx->ContextRecord;
	m_pLog->Write("The program caused an exception:%s (0x%08x)\r\n",
				m_pSysInfo->GetExceptionDescription(Exception->ExceptionCode),
				Exception->ExceptionCode);

	// 记录发生异常的模块
	CHAR szCrashModulePathName[MAX_PATH*2];
	ZeroMemory(szCrashModulePathName, sizeof(szCrashModulePathName));

	CHAR *pszCrashModuleFileName = "Unknown";
	MEMORY_BASIC_INFORMATION MemInfo;

	// VirtualQuery can be used to get the allocation base associated with a
	// code address, which is the same as the ModuleHandle. This can be used
	// to get the filename of the module that the crash happened in.
	if( VirtualQuery((void*)Context->Eip, &MemInfo, sizeof(MemInfo)) &&
						(GetModuleFileNameA((HINSTANCE)MemInfo.AllocationBase,
										  szCrashModulePathName,
										  sizeof(szCrashModulePathName)-2) > 0) )
	{
		pszCrashModuleFileName = m_pSysInfo->GetFilePart(szCrashModulePathName);
	}

	m_pLog->Write( "in module %s at %04x:%08x.\r\n",
				pszCrashModuleFileName, Context->SegCs, Context->Eip );

	// 如果发生的是非法操作,记录是读还是写操作以及试图操作的位置
	if( Exception->ExceptionCode == STATUS_ACCESS_VIOLATION &&
				Exception->NumberParameters >= 2)
	{
		CHAR szDebugMessage[1024];
		const CHAR* readwrite = "Access violation: read from";
		if( Exception->ExceptionInformation[0])
			readwrite = "Access violation: write to";
		sprintf(szDebugMessage, "%s location %08x\r\n",
					readwrite, Exception->ExceptionInformation[1]);

#ifdef	_DEBUG
		// Visual C++ 调试器缺少此功能,我们也给他加上
		OutputDebugString(_T("Exception handler: "));
		OutputDebugStringA(szDebugMessage);
#endif

		m_pLog->Write(szDebugMessage);
	}


}


//-----------------------------------------------------------------------------
// 记录系统信息
//-----------------------------------------------------------------------------
VOID Exception::DumpSystemInfo()
{
	WRITE_SEPARATOR();
	// print out operating system
	CHAR szWinVer[50], szBuild[50];
	m_pSysInfo->GetWinVer(szWinVer, szBuild);
	m_pLog->Write("Operating system:  %s (%s).\r\n", szWinVer, szBuild);

	// 仅记录第一个CPU的型号
	SYSTEM_INFO	SystemInfo;
	GetSystemInfo(&SystemInfo);
	m_pLog->Write("%d processor(s), type %d.\r\n",
				SystemInfo.dwNumberOfProcessors, SystemInfo.dwProcessorType);

	MEMORYSTATUS MemInfo;
	MemInfo.dwLength = sizeof(MemInfo);
	GlobalMemoryStatus(&MemInfo);

	// Print out info on memory, rounded up.
	m_pLog->Write("Physical memory: Total-%dMB Free-%dMB.\r\n",
		(MemInfo.dwTotalPhys + ONE_MEGA - 1) / ONE_MEGA,
		(MemInfo.dwAvailPhys + ONE_MEGA - 1) / ONE_MEGA );

	m_pLog->Write("Paging File: Total-%dMB Free-%dMB.\r\n",
		(MemInfo.dwTotalPageFile + ONE_MEGA - 1) / ONE_MEGA,
		(MemInfo.dwAvailPageFile + ONE_MEGA - 1) / ONE_MEGA );
}



//-----------------------------------------------------------------------------
// 记录调用堆栈
//-----------------------------------------------------------------------------
VOID Exception::DumpCallStack(PCONTEXT pContext)
{
	WRITE_SEPARATOR();
	m_pLog->Write("CallStack:\r\n");

	STACKFRAME StackFrame;
	memset(&StackFrame, 0, sizeof(STACKFRAME));
	StackFrame.AddrPC.Mode = AddrModeFlat;    
	StackFrame.AddrPC.Offset = pContext->Eip;    
	StackFrame.AddrStack.Mode = AddrModeFlat;    
	StackFrame.AddrStack.Offset = pContext->Esp;    
	StackFrame.AddrFrame.Mode = AddrModeFlat;    
	StackFrame.AddrFrame.Offset = pContext->Ebp;

	BOOL bRet = TRUE;
	CHAR szName[1024];
	CHAR szTemp[1024];
	
	// StackWalk会更改context,所以这里我们必须复制一份给它用
	// 因为CONTEXT结构中没有指针成员,所以可以安全的完全复制
	CONTEXT context;
	memcpy( &context, pContext, sizeof(context) );

	for(INT nCount=0; nCount < 256 && bRet; nCount++)    
	{       
		// 上溯函数
		bRet = StackWalk(IMAGE_FILE_MACHINE_I386, GetCurrentProcess(), 
			GetCurrentThread(), &StackFrame, &context, NULL, 
			SymFunctionTableAccess, SymGetModuleBase, NULL);

		if( bRet )
		{
			GetFunctionName(szName, (PVOID)StackFrame.AddrPC.Offset);
			sprintf(szTemp, "%s at %#08lx\r\n", szName, 
				StackFrame.AddrPC.Offset);

			m_pLog->Write(szTemp);
		}
	}
}





//-----------------------------------------------------------------------------
// 记录寄存器状态
//-----------------------------------------------------------------------------
VOID Exception::DumpRegisters(PCONTEXT pContext)
{
	// Print out the register values in an XP error window compatible format.
	WRITE_SEPARATOR();
	m_pLog->Write("Context:\r\n");
	m_pLog->Write("EDI:    0x%08x  ESI: 0x%08x  EAX:   0x%08x\r\n",
				pContext->Edi, pContext->Esi, pContext->Eax );
	m_pLog->Write("EBX:    0x%08x  ECX: 0x%08x  EDX:   0x%08x\r\n",
				pContext->Ebx, pContext->Ecx, pContext->Edx);
	m_pLog->Write("EIP:    0x%08x  EBP: 0x%08x  SegCs: 0x%08x\r\n",
				pContext->Eip, pContext->Ebp, pContext->SegCs);
	m_pLog->Write("EFlags: 0x%08x  ESP: 0x%08x  SegSs: 0x%08x\r\n",
				pContext->EFlags, pContext->Esp, pContext->SegSs);
				
}



//-----------------------------------------------------------------------------
// 记录当前指令
//-----------------------------------------------------------------------------
VOID Exception::DumpInstructions(PCONTEXT pContext)
{
	// 有可能EIP寄存器指向无效地址,所以这里需要用异常处理
	WRITE_SEPARATOR();
	m_pLog->Write("Bytes at CS:EIP:  (XX means unaccessible)\r\n");
	BYTE * pCode = (BYTE *)pContext->Eip;
	for( int nCodeByte = 0; nCodeByte < MAX_CODE_BYTE_DUMP; nCodeByte++ )
	{
		__try
		{
			m_pLog->Write("%02x ", pCode[nCodeByte]);
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			m_pLog->Write("XX ");
		}
	}
	
	WRITE_SEPARATOR();
}




//-----------------------------------------------------------------------------
// 记录变量
//-----------------------------------------------------------------------------
VOID Exception::DumpVariable(PCONTEXT pContext)
{
	WRITE_SEPARATOR();
	m_pLog->Write("Variable:\r\n");

	STACKFRAME StackFrame;
	memset(&StackFrame, 0, sizeof(STACKFRAME));
	StackFrame.AddrPC.Mode = AddrModeFlat;    
	StackFrame.AddrPC.Offset = pContext->Eip;    
	StackFrame.AddrStack.Mode = AddrModeFlat;    
	StackFrame.AddrStack.Offset = pContext->Esp;    
	StackFrame.AddrFrame.Mode = AddrModeFlat;    
	StackFrame.AddrFrame.Offset = pContext->Ebp;
	
	BOOL bRet = TRUE;
	CHAR szName[1024];
	CHAR szTemp[1024];
	
	// StackWalk会更改context,所以这里我们必须复制一份给它用
	// 因为CONTEXT结构中没有指针成员,所以可以安全的完全复制
	CONTEXT context;
	memcpy( &context, pContext, sizeof(context) );

	for(INT nCount=0; nCount < 256 && bRet; nCount++)    
	{       
		// 上溯函数
		bRet = StackWalk(IMAGE_FILE_MACHINE_I386, m_hProcess, 
			GetCurrentThread(), &StackFrame, &context, NULL, 
			SymFunctionTableAccess, SymGetModuleBase, NULL);

		if( bRet )
		{
			BOOL bResult = GetFunctionName(szName, (PVOID)StackFrame.AddrPC.Offset);
			if( FALSE == bResult )
				continue;

			sprintf(szTemp, "%s at %#08lx\r\n", szName, StackFrame.AddrPC.Offset);
			m_pLog->Write(szTemp);

			IMAGEHLP_STACK_FRAME iStackFrame;
			ZeroMemory(&iStackFrame, sizeof(iStackFrame));
			iStackFrame.InstructionOffset = StackFrame.AddrPC.Offset;
			iStackFrame.StackOffset = StackFrame.AddrStack.Offset;
			iStackFrame.FrameOffset = StackFrame.AddrFrame.Offset;
			iStackFrame.ReturnOffset = StackFrame.AddrReturn.Offset;
			iStackFrame.BackingStoreOffset = StackFrame.AddrBStore.Offset;
			iStackFrame.FuncTableEntry = (ULONG64)StackFrame.FuncTableEntry;
			bResult = SymSetContext( m_hProcess, &iStackFrame, 0 );

			// 我们将context传给回调函数,以便其使用
			SymEnumSymbols( m_hProcess, 0, NULL, SymEnumSymbolsProc, &StackFrame );
		}
	}
}




//-----------------------------------------------------------------------------
// 局部变量枚举回调函数
//-----------------------------------------------------------------------------
BOOL CALLBACK Exception::SymEnumSymbolsProc(PSYMBOL_INFO pSym, ULONG dwSize, 
											 PVOID pContext)
{
    __try
    {
        if( FALSE == FormatSymbolValue(pSym, (STACKFRAME*)pContext) )
			m_pLog->Write("failed on formatting symbol %s\r\n", pSym->Name );
    }
    __except( 1 )
    {
        m_pLog->Write("kicked out at symbol %s\r\n", pSym->Name );
    }

    return TRUE;
}



//-----------------------------------------------------------------------------
// Given a SYMBOL_INFO representing a particular variable, displays its
// contents.  If it's a user defined type, display the members and their
// values.
//-----------------------------------------------------------------------------
BOOL Exception::FormatSymbolValue(PSYMBOL_INFO pSym, STACKFRAME * sf)
{
    // 不处理函数
    if ( pSym->Tag == 5 )   // SymTagFunction from CVCONST.H from the DIA SDK
        return FALSE;

    DWORD3264 pVariable = 0;    // 指向变量的实际数据
    if ( pSym->Flags & IMAGEHLP_SYMBOL_INFO_REGRELATIVE )
    {
		pVariable = sf->AddrFrame.Offset;
		pVariable += (DWORD3264)pSym->Address;
    }
    else 
	{
		if ( pSym->Flags & IMAGEHLP_SYMBOL_INFO_REGISTER )
	        return FALSE;   // Don't try to report register variable
		else
            pVariable = (DWORD3264)pSym->Address;
    }

    // 符号是参数还是变量
    if ( pSym->Flags & IMAGEHLP_SYMBOL_INFO_PARAMETER )
        m_pLog->Write("-Parameter ");
    else if ( pSym->Flags & IMAGEHLP_SYMBOL_INFO_LOCAL )
        m_pLog->Write("-Local ");
	else
		m_pLog->Write("-Unknow data kind ");

	// 在这里输出变量名
	CHAR szName[2048]={0};
	strncpy(szName, pSym->Name, pSym->NameLen);
    m_pLog->Write("\'%s\'", szName);

    // Determine if the variable is a user defined type (UDT).  IF so, bHandled
    // will return true.
	DWORD dwTypeIndex = pSym->TypeIndex;
	EBasicType basicType = EBT_NoType;
	BOOL bHandled = FALSE;

    bHandled = DumpTypeIndex(pSym->ModBase, dwTypeIndex, pVariable, 0);

//     if ( !bHandled )
//     {
//         // The symbol wasn't a UDT, so do basic, stupid formatting of the
//         // variable.  Based on the size, we're assuming it's a char, WORD, or
//         // DWORD.
//         basicType = GetBasicType( pSym->TypeIndex, pSym->ModBase );
//         FormatOutputValue(basicType, pSym->Size, (PVOID)pVariable ); 
//     }


    return true;
}

//-----------------------------------------------------------------------------
// If it's a user defined type (UDT), recurse through its members until we're
// at fundamental types.  When he hit fundamental types, return
// bHandled = false, so that FormatSymbolValue() will format them.
//-----------------------------------------------------------------------------
BOOL Exception::DumpTypeIndex(DWORD64 Base64, DWORD dwTypeIndex, 
							   DWORD3264 pOffset, INT nCycle)
{
	if( nCycle > 3 )	// 递归次数太多
		return TRUE;

	DWORD dwSymTag = 0;
	SymGetTypeInfo( m_hProcess, Base64, dwTypeIndex, TI_GET_SYMTAG, &dwSymTag );
	if( dwSymTag == 5 )	// 不处理函数类型
		return TRUE;

	// m_pLog->Write( _T("tag=%d"), dwSymTag );
    
	// 输出类型名
	WCHAR* pwszTypeName;
    if( SymGetTypeInfo(m_hProcess, Base64, dwTypeIndex, TI_GET_SYMNAME, &pwszTypeName) )
    {
		for( INT n=0; n<nCycle; n++ )
			m_pLog->Write("    ");
        m_pLog->Write("%ls", pwszTypeName );
        LocalFree( pwszTypeName );
    }


	// 先尝试输出自己
	ULONG64 length;
	if( SymGetTypeInfo(m_hProcess, Base64, dwTypeIndex, TI_GET_LENGTH, &length) )
	{
		EBasicType basicType = GetBasicType( dwTypeIndex, Base64 );
		FormatOutputValue(basicType, length, (PVOID)pOffset ); 
	}

	if( dwSymTag == 0xe )	// 如果是指针类型
	{
		// 得到成员的实际"TypeId"
		SymGetTypeInfo( m_hProcess, Base64, dwTypeIndex,
			TI_GET_TYPEID, &dwTypeIndex );

		pOffset = *(DWORD*)pOffset;
	}


    // 检查有多少个成员
	DWORD dwChildrenCount = 0;
	SymGetTypeInfo( m_hProcess, Base64, dwTypeIndex, TI_GET_CHILDRENCOUNT, &dwChildrenCount );

	// 没有成员
	if( !dwChildrenCount )     
		return FALSE;

	// Prepare to get an array of "TypeIds", representing each of the children.
	// SymGetTypeInfo(TI_FINDCHILDREN) expects more memory than just a
	// TI_FINDCHILDREN_PARAMS struct has.  Use derivation to accomplish this.
	struct FINDCHILDREN : TI_FINDCHILDREN_PARAMS
	{
        ULONG   MoreChildIds[1024];
        FINDCHILDREN(){Count = sizeof(MoreChildIds) / sizeof(MoreChildIds[0]);}
    } children;

    children.Count = dwChildrenCount;
    children.Start= 0;

    // Get the array of TypeIds, one for each child type
    if( !SymGetTypeInfo( m_hProcess, Base64, dwTypeIndex, TI_FINDCHILDREN, &children ) )
        return FALSE;

    // 检查每个成员
    for( DWORD i = 0; i < dwChildrenCount; i++ )
    {
        // 递归搜索成员
        BOOL bHandled2 =
			DumpTypeIndex( Base64, children.ChildId[i], pOffset, nCycle+1 );

        if( !bHandled2 )	// 成员不是UDT,我们试着输出他的实际值
        {
            // 得到成员相对于父结构的偏移量
            DWORD dwMemberOffset;
            SymGetTypeInfo( m_hProcess, Base64, children.ChildId[i],
                            TI_GET_OFFSET, &dwMemberOffset );

            // 得到成员的实际"TypeId"
            DWORD typeId = 0;
            if( !SymGetTypeInfo(m_hProcess, Base64, children.ChildId[i], TI_GET_TYPEID, &typeId) )
				continue;

            // 得到成员的大小
            ULONG64 lengthOfMember;
            if( !SymGetTypeInfo(m_hProcess, Base64, typeId, TI_GET_LENGTH, &lengthOfMember) )
				continue;

            // 计算成员的地址
            DWORD3264 dwFinalOffset = pOffset + dwMemberOffset;
            EBasicType basicType = GetBasicType(children.ChildId[i], Base64 );
            FormatOutputValue( basicType, lengthOfMember, (PVOID)dwFinalOffset ); 
        }
    }

    return TRUE;
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
VOID Exception::FormatOutputValue(EBasicType eType, DWORD64 Len64, PVOID pAddress)
{
	__try
	{
		switch( Len64 )
		{
		case 1:
			m_pLog->Write(" = 0x%X", *(PBYTE)pAddress );
			break;

		case 2:
			m_pLog->Write(" = 0x%X", *(PWORD)pAddress );
			break;

		case 4:
			if( eType == EBT_Float )
			{
				m_pLog->Write(" = %f", *(PFLOAT)pAddress);
			}
			else if( eType == EBT_Char )
			{
				if( !IsBadStringPtr( *(LPTSTR*)pAddress, 1024) )
					m_pLog->Write(" = \"%s\"", *(PDWORD)pAddress );
				else
					m_pLog->Write(" = 0x%X", *(PDWORD)pAddress );
			}
			else
				m_pLog->Write(" = 0x%X", *(PDWORD)pAddress );
			break;

		case 8:
			if( eType == EBT_Float )
				m_pLog->Write(" = %lf", *(double *)pAddress );
			else
				m_pLog->Write(" = 0x%I64X", *(DWORD64*)pAddress );
			break;

		default:
			m_pLog->Write(" = unable to format value(size:%d, type:%d)", Len64, eType);
			break;
		}

		WRITE_SEPARATOR();
	}
	__except(1)
	{
		m_pLog->Write(" = unable to get value");
		WRITE_SEPARATOR();
	}
}


//-----------------------------------------------------------------------------
// 根据给定dwTypeIndex,试着得到基本类型
//-----------------------------------------------------------------------------
EBasicType Exception::GetBasicType( DWORD dwTypeIndex, DWORD64 Base64 )
{
    EBasicType eType = EBT_NoType;
    if( SymGetTypeInfo(m_hProcess, Base64, dwTypeIndex, TI_GET_BASETYPE, &eType) )
        return eType;

    // 可能是指向基本类型的指针,先试着获取真正的类型
    DWORD dwTypeId;
    if( SymGetTypeInfo(m_hProcess, Base64, dwTypeIndex, TI_GET_TYPEID, &dwTypeId) )
    {
        if( SymGetTypeInfo(m_hProcess, Base64, dwTypeId, TI_GET_BASETYPE, &eType) )
            return eType;
    }

    return EBT_NoType;
}


//-----------------------------------------------------------------------------
// 打包并发送Log信息
//-----------------------------------------------------------------------------
VOID Exception::SendLog()
{
	if( !m_bSendLog )
		return;

	TCHAR mess[1024];
	TCHAR szFileName[1024];
	m_pLog->Close();
	_tcsncpy(szFileName, m_pLog->GetDefaultFileName(), 1024);

	// 先检查log文件是否存在
	DWORD dwSize = m_pDiscIO->GetSize(szFileName);
	if( dwSize == 0 || dwSize == GT_INVALID )
		return;

	// 截去文件扩展名
	LPTSTR pResult = _tcsrchr(szFileName, _T('.'));
	if( pResult )
		_stprintf(pResult, _T(".zip"));

	HZIP hZip = NULL;
	hZip = CreateZip((LPVOID)(LPCTSTR)szFileName, 0, ZIP_FILENAME);
	if (!hZip)
		return;

	// zip is open, add file
	ZRESULT zr = ZipAdd(hZip, _T("log.txt"), (LPVOID)m_pLog->GetDefaultFileName(), 0, ZIP_FILENAME);
	if( zr != ZR_OK )
		return;
	
	CloseZip(hZip);

	// 删除原文件
	::DeleteFile( m_pLog->GetDefaultFileName());

	// 确定是否发送
	if( m_bSendLogWindow )
	{
		LoadString(g_hInstance, IDS_ERROR, mess, 1024);
		if( ::MessageBox(NULL, mess, _T("Critical Error"), MB_APPLMODAL|MB_ICONERROR|MB_OKCANCEL|MB_DEFBUTTON1) == IDCANCEL )
			return;
	}

	// 创建目标文件名
	TCHAR szDest[1024];
	LPTSTR pszDest = _tcsrchr(szFileName, _T('\\'));
	if( !pszDest )
		pszDest = szFileName;

	_tcsncpy(szDest, pszDest, 1024);

	// FTP发送
	TObjRef<StreamTransport>()->Init();

	nsFTP::CFTPClient ftpClient;
	nsFTP::CLogonInfo logonInfo(_T("219.239.35.249"), 21, _T("ctcrst"), _T("ctcrst"));

	if( ftpClient.Login(logonInfo) )
	{
		ftpClient.UploadFile(szFileName, szDest);
		ftpClient.Logout();
	}

	return;
}









} // namespace vEngine {



*/









