//-----------------------------------------------------------------------------
//!\file process.h
//!\author Lyp
//!
//!\date 2004-08-03
//! last 2004-08-03
//!
//!\brief 进程相关功能
//-----------------------------------------------------------------------------
#pragma once
#include <psapi.h>
#include <tlhelp32.h>

namespace vEngine {

namespace ENUM_METHOD 
{const int NONE    = 0x0;
 const int PSAPI   = 0x1;
 const int TOOLHELP= 0x2;
 const int PROC16  = 0x4;
} 

const int MAX_FILENAME = 256;

// Functions loaded from PSAPI
typedef BOOL (WINAPI *PFEnumProcesses)(
	DWORD * lpidProcess, DWORD cb, DWORD * cbNeeded
);

typedef BOOL (WINAPI *PFEnumProcessModules)(
    HANDLE hProcess, HMODULE * lphModule, DWORD cb, LPDWORD lpcbNeeded
);

typedef DWORD (WINAPI *PFGetModuleFileNameEx)(
  HANDLE hProcess, HMODULE hModule, LPTSTR lpFilename, DWORD nSize         
);

//Functions loaded from Kernel32
typedef HANDLE (WINAPI *PFCreateToolhelp32Snapshot)(
  DWORD dwFlags,       
  DWORD th32ProcessID  
);

typedef BOOL (WINAPI *PFProcess32First)(
  HANDLE hSnapshot,      
  LPPROCESSENTRY32 lppe  
);

typedef BOOL (WINAPI *PFProcess32Next)(
  HANDLE hSnapshot,      
  LPPROCESSENTRY32 lppe  
);
 
typedef BOOL (WINAPI *PFModule32First)(
  HANDLE hSnapshot,     
  LPMODULEENTRY32 lpme  
);

typedef BOOL (WINAPI *PFModule32Next)(
  HANDLE hSnapshot,     
  LPMODULEENTRY32 lpme  
);



  
class EnumProcess  
{
public:

    struct ProcessEntry
    {
     LPTSTR lpFilename;
     DWORD  dwPID;
     WORD   hTask16;
     // Constructors/Destructors
     ProcessEntry() : dwPID(0), hTask16(0) 
     {lpFilename = new TCHAR[MAX_FILENAME];}
     ProcessEntry(ProcessEntry &e) : dwPID(e.dwPID), hTask16(e.hTask16)
     {lstrcpy(lpFilename, e.lpFilename);}
     virtual ~ProcessEntry()
     {delete[](lpFilename);}
    };

    struct ModuleEntry
    {
     LPTSTR lpFilename;
     PVOID pLoadBase;
     PVOID pPreferredBase;
     // Constructors/Destructors
     ModuleEntry() : pLoadBase(NULL), pPreferredBase(NULL)
     {lpFilename = new TCHAR[MAX_FILENAME];}
     ModuleEntry(ModuleEntry &e) : pLoadBase(e.pLoadBase), pPreferredBase(e.pPreferredBase)
     {lstrcpy(lpFilename, e.lpFilename);}
     virtual ~ModuleEntry()
     {delete[](lpFilename);}
    };

    EnumProcess();
	virtual ~EnumProcess();

    BOOL GetModuleNext(DWORD dwPID, ModuleEntry* pEntry);
	BOOL GetModuleFirst(DWORD dwPID, ModuleEntry* pEntry);
	BOOL GetProcessNext(ProcessEntry *pEntry);    
    BOOL GetProcessFirst(ProcessEntry* pEntry);

	int GetAvailableMethods();
    int GetSuggestedMethod();
    int SetMethod(int method);



protected:

	PVOID GetModulePreferredBase(DWORD dwPID, PVOID pModBase);
    // General members
	int m_method;

    // PSAPI related members
    HMODULE PSAPI;   //Handle to the module
    int m_MAX_COUNT; 
    DWORD* m_pProcesses, *m_pCurrentP; // Process identifiers
    long m_cProcesses, m_cModules;     // Number of Processes/Modules found
    HMODULE* m_pModules, *m_pCurrentM; // Handles to Modules 
    // PSAPI related functions
    PFEnumProcesses       FEnumProcesses;           // Pointer to EnumProcess
    PFEnumProcessModules  FEnumProcessModules; // Pointer to EnumProcessModules
    PFGetModuleFileNameEx FGetModuleFileNameEx;// Pointer to GetModuleFileNameEx
	BOOL FillPStructPSAPI(DWORD pid, ProcessEntry* pEntry);
	BOOL FillMStructPSAPI(DWORD dwPID, HMODULE mMod, ModuleEntry* pEntry);

    // ToolHelp related members
    HANDLE m_hProcessSnap, m_hModuleSnap;
    HMODULE TOOLHELP;   //Handle to the module (Kernel32)
    PROCESSENTRY32 m_pe;
    MODULEENTRY32  m_me;
    // ToolHelp related functions
    PFCreateToolhelp32Snapshot FCreateToolhelp32Snapshot;
    PFProcess32First FProcess32First;
    PFProcess32Next  FProcess32Next;
    PFModule32First  FModule32First;
    PFModule32Next   FModule32Next;   
};


class VENGINE_API CProcessWrapper
{
public:
	bool		QuitProcess( const TCHAR* szProcessName, BOOL bFullPath = FALSE );
	bool		ShutDownProcess( const TCHAR* szProcessName, BOOL bFullPath = FALSE  );
	bool		ExecuteProcess( const TCHAR* szProcessName );


	//查找进程，返回进程的ID
	HANDLE		FindProcess( DWORD& dwProcessID,  const TCHAR* szProcessName, BOOL bFullPath = FALSE );
	INT			GetProcNum(DWORD& dwProcessID, const TCHAR* szProcessName, BOOL bFullPath);

	//查找所有相关进程，返回进程ID信息
	int			FindAllProcess( HANDLE* pProcessHandle, DWORD* dwID,  const TCHAR* szProcessName, BOOL bFullPath = FALSE );

	CProcessWrapper(void);
	~CProcessWrapper(void);

private:

	HANDLE		m_hSnapShot;


};


}