//-----------------------------------------------------------------------------
// File: sysinfo.cpp
// Auth: Lyp
// Date: 2004/02/03
// Desc: 得到系统信息
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "sysinfo.h"


namespace vEngine{
//-----------------------------------------------------------------------------
// CPU特殊功能
//-----------------------------------------------------------------------------
enum
{
	CPU_MMX       = 0x00000001,
	CPU_MMX_EXT   = 0x00000002,
	CPU_3DNOW     = 0x00000004,
	CPU_3DNOW_EXT = 0x00000008,
	CPU_SSE       = 0x00000010,
	CPU_SSE2      = 0x00000020,
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
DWORD SysInfo::GetCpuCaps()
{
	__asm
	{
		pushfd
		pop     eax
		mov     ebx,eax
		xor     eax,0x200000
		push    eax
		popfd
		pushfd
		pop     eax
		cmp     eax,ebx			; test if cpu can do CPUID
		jnz     $cpuid_found
		xor     eax,eax
		jmp     $cpu_caps_exit

		$cpuid_found:
		xor     eax,eax			; no caps
		
		push    eax
		mov     eax,1
		cpuid
		pop     eax
		test    edx,0x800000	; detect MMX
		jz      $no_mmx
		or      eax,1			; MMX detected.

		$no_mmx:
		test    edx,0x2000000	; detect Streaming Simd Extensions ( SSE )
		jz      $no_sse
		or      eax,16			; SSE detected.

		$no_sse:
		test    edx,0x4000000	; detect Streaming Simd Extensions 2 ( SSE2 )
		jz      $no_sse2
		or      eax,32			; SSE2 detected.

		$no_sse2:
		push    eax
		mov     eax,0x80000001
		cpuid
		pop     eax
		test    edx,0x80000000	; detect AMD 3DNow!
		jz      $no_3dnow
		or      eax,4			; 3DNow! detected.

		$no_3dnow:
		push    eax
		mov     eax,0x80000001
		cpuid
		pop     eax				; detect AMD Extensions to the 3DNow!
		test    edx,0x40000000
		jz      $no_3dnowext
		or      eax,8			; 3DNow!+ detected.

		$no_3dnowext:
		push    eax
		mov     eax,0x80000001
		cpuid
		pop     eax
		test    edx,0x00400000	; detect AMD Extensions to the MMX
		jz      $no_mmxext
		or      eax,2			; MMX+ detected.

		$no_mmxext:
		$cpu_caps_exit:
	}

}


// 操作系统类型定义字符串表
LPCSTR WUNKNOWNSTR		= "unknown Windows version";
LPCSTR W95STR			= "Windows 95";
LPCSTR W95SP1STR		= "Windows 95 SP1";
LPCSTR W95OSR2STR		= "Windows 95 OSR2";
LPCSTR W98STR			= "Windows 98";
LPCSTR W98SP1STR		= "Windows 98 SP1";
LPCSTR W98SESTR			= "Windows 98 SE";
LPCSTR WMESTR			= "Windows ME";
LPCSTR WNT351STR		= "Windows NT 3.51";
LPCSTR WNT4STR			= "Windows NT 4";
LPCSTR W2KSTR			= "Windows 2000";
LPCSTR WXPSTR			= "Windows XP";
LPCSTR W2003SERVERSTR	= "Windows 2003 Server";
LPCSTR WCESTR			= "Windows CE";


// from winbase.h
#ifndef VER_PLATFORM_WIN32s
#define VER_PLATFORM_WIN32s             0
#endif
#ifndef VER_PLATFORM_WIN32_WINDOWS
#define VER_PLATFORM_WIN32_WINDOWS      1
#endif
#ifndef VER_PLATFORM_WIN32_NT
#define VER_PLATFORM_WIN32_NT           2
#endif
#ifndef VER_PLATFORM_WIN32_CE
#define VER_PLATFORM_WIN32_CE           3
#endif

// 记录各种异常信息字符串
struct ExceptionNames
{
	DWORD	ExceptionCode;
	CHAR*	ExceptionName;
};

ExceptionNames ExceptionMap[] =
{
	{0x40010005, "Control-C"},
	{0x40010008, "Control-Break"},
	{0x80000002, "Datatype Misalignment"},
	{0x80000003, "Breakpoint"},
	{0xc0000005, "Access Violation"},
	{0xc0000006, "In Page Error"},
	{0xc0000017, "No Memory"},
	{0xc000001d, "Illegal Instruction"},
	{0xc0000025, "Noncontinuable Exception"},
	{0xc0000026, "Invalid Disposition"},
	{0xc000008c, "Array Bounds Exceeded"},
	{0xc000008d, "Float Denormal Operand"},
	{0xc000008e, "Float Divide by Zero"},
	{0xc000008f, "Float Inexact Result"},
	{0xc0000090, "Float Invalid Operation"},
	{0xc0000091, "Float Overflow"},
	{0xc0000092, "Float Stack Check"},
	{0xc0000093, "Float Underflow"},
	{0xc0000094, "Integer Divide by Zero"},
	{0xc0000095, "Integer Overflow"},
	{0xc0000096, "Privileged Instruction"},
	{0xc00000fD, "Stack Overflow"},
	{0xc0000142, "DLL Initialization Failed"},
	{0xe06d7363, "Microsoft C++ Exception"},
};



//-----------------------------------------------------------------------------
// 版本数据非官方给出
//-----------------------------------------------------------------------------
BOOL SysInfo::GetWinVer(LPSTR pszVersion, LPSTR pszBuild)
{
	strcpy(pszVersion, WUNKNOWNSTR);
	strcpy(pszBuild, "");

	OSVERSIONINFOEXA osinfo;
	osinfo.dwOSVersionInfoSize = sizeof(osinfo);

	if( !GetVersionExA((OSVERSIONINFOA*)&osinfo) )
		return FALSE;

	DWORD dwPlatformId   = osinfo.dwPlatformId;
	DWORD dwMinorVersion = osinfo.dwMinorVersion;
	DWORD dwMajorVersion = osinfo.dwMajorVersion;
	DWORD dwBuildNumber  = osinfo.dwBuildNumber & 0xFFFF;	// Win 95 needs this

	sprintf(pszBuild, "%u.%u.%u", 
		dwMajorVersion, dwMinorVersion, dwBuildNumber);

	if( (dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) && (dwMajorVersion == 4) )
	{
		if( (dwMinorVersion < 10) && (dwBuildNumber == 950) )
		{
			strcpy(pszVersion, W95STR);
		}
		else if( (dwMinorVersion < 10) && 
				((dwBuildNumber > 950) && (dwBuildNumber <= 1080)) )
		{
			strcpy(pszVersion, W95SP1STR);
		}
		else if( (dwMinorVersion < 10) && (dwBuildNumber > 1080) )
		{
			strcpy(pszVersion, W95OSR2STR);
		}
		else if( (dwMinorVersion == 10) && (dwBuildNumber == 1998) )
		{
			strcpy(pszVersion, W98STR);
		}
		else if( (dwMinorVersion == 10) && 
				((dwBuildNumber > 1998) && (dwBuildNumber < 2183)) )
		{
			strcpy(pszVersion, W98SP1STR);
		}
		else if( (dwMinorVersion == 10) && (dwBuildNumber >= 2183) )
		{
			strcpy(pszVersion, W98SESTR);
		}
		else if( dwMinorVersion == 90 )
		{
			strcpy(pszVersion, WMESTR);
		}
	}
	else if( dwPlatformId == VER_PLATFORM_WIN32_NT )
	{
		if( (dwMajorVersion == 3) && (dwMinorVersion == 51) )
		{
			strcpy(pszVersion, WNT351STR);
		}
		else if( (dwMajorVersion == 4) && (dwMinorVersion == 0) )
		{
			strcpy(pszVersion, WNT4STR);
		}
		else if( (dwMajorVersion == 5) && (dwMinorVersion == 0) )
		{
			strcpy(pszVersion, W2KSTR);
		}
		else if( (dwMajorVersion == 5) && (dwMinorVersion == 1) )
		{
			strcpy(pszVersion, WXPSTR);
		}
		else if( (dwMajorVersion == 5) && (dwMinorVersion == 2) )
		{
			strcpy(pszVersion, W2003SERVERSTR);
		}
	}
	else if( dwPlatformId == VER_PLATFORM_WIN32_CE )
	{
		strcpy(pszVersion, WCESTR);
	}
	return TRUE;
}


//-----------------------------------------------------------------------------
// 检查是否NT操作系统
//-----------------------------------------------------------------------------
BOOL SysInfo::IsNT()
{
	OSVERSIONINFO osinfo;
	osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if( !GetVersionEx(&osinfo) )
		return FALSE;

	if( VER_PLATFORM_WIN32_NT == osinfo.dwPlatformId )
		return TRUE;
	else
		return FALSE;
}



typedef HRESULT(WINAPI * DIRECTDRAWCREATE)(GUID*,LPDIRECTDRAW*,IUnknown*);
typedef HRESULT(WINAPI * DIRECTDRAWCREATEEX)(GUID*,VOID**,REFIID,IUnknown*);
typedef HRESULT(WINAPI * DIRECTINPUTCREATE)(HINSTANCE,DWORD,LPDIRECTINPUT*,IUnknown*);
//-----------------------------------------------------------------------------
// Name: GetDXVersion()
// Desc: This function returns the DirectX version number as follows:
//          0x0000 = No DirectX installed
//          0x0100 = DirectX version 1 installed
//          0x0200 = DirectX 2 installed
//          0x0300 = DirectX 3 installed
//          0x0500 = At least DirectX 5 installed.
//          0x0600 = At least DirectX 6 installed.
//          0x0601 = At least DirectX 6.1 installed.
//          0x0700 = At least DirectX 7 installed.
//          0x0800 = At least DirectX 8 installed.
// 
//       Please note that this code is intended as a general guideline. Your
//       app will probably be able to simply query for functionality (via
//       QueryInterface) for one or two components.
//
//       Please also note:
//          "if( dwDXVersion != 0x500 ) return FALSE;" is VERY BAD. 
//          "if( dwDXVersion <  0x500 ) return FALSE;" is MUCH BETTER.
//       to ensure your app will run on future releases of DirectX.
//-----------------------------------------------------------------------------
DWORD SysInfo::GetDXVersion()
{

    DIRECTDRAWCREATE     DirectDrawCreate   = NULL;
    DIRECTDRAWCREATEEX   DirectDrawCreateEx = NULL;
    DIRECTINPUTCREATE    DirectInputCreate  = NULL;
    HINSTANCE            hDDrawDLL          = NULL;
    HINSTANCE            hDInputDLL         = NULL;
    HINSTANCE            hD3D8DLL           = NULL;
    LPDIRECTDRAW         pDDraw             = NULL;
    LPDIRECTDRAW2        pDDraw2            = NULL;
    LPDIRECTDRAWSURFACE  pSurf              = NULL;
    LPDIRECTDRAWSURFACE3 pSurf3             = NULL;
    LPDIRECTDRAWSURFACE4 pSurf4             = NULL;
    DWORD                dwDXVersion        = 0;
    HRESULT              hr;

    // First see if DDRAW.DLL even exists.
    hDDrawDLL = LoadLibrary( _T("DDRAW.DLL") );
    if( hDDrawDLL == NULL )
    {
        dwDXVersion = 0;
        return dwDXVersion;
    }

    // See if we can create the DirectDraw object.
    DirectDrawCreate = (DIRECTDRAWCREATE)GetProcAddress( hDDrawDLL, "DirectDrawCreate" );
    if( DirectDrawCreate == NULL )
    {
        dwDXVersion = 0;
        FreeLibrary( hDDrawDLL );
        OutputDebugString( _T("Couldn't LoadLibrary DDraw\r\n") );
        return dwDXVersion;
    }

    hr = DirectDrawCreate( NULL, &pDDraw, NULL );
    if( FAILED(hr) )
    {
        dwDXVersion = 0;
        FreeLibrary( hDDrawDLL );
        OutputDebugString( _T("Couldn't create DDraw\r\n") );
        return dwDXVersion;
    }

    // So DirectDraw exists.  We are at least DX1.
    dwDXVersion = 0x100;

    // Let's see if IID_IDirectDraw2 exists.
    hr = pDDraw->QueryInterface( IID_IDirectDraw2, (VOID**)&pDDraw2 );
    if( FAILED(hr) )
    {
        // No IDirectDraw2 exists... must be DX1
        pDDraw->Release();
        FreeLibrary( hDDrawDLL );
        OutputDebugString( _T("Couldn't QI DDraw2\r\n") );
        return dwDXVersion;
    }

    // IDirectDraw2 exists. We must be at least DX2
    pDDraw2->Release();
    dwDXVersion = 0x200;


	//-------------------------------------------------------------------------
    // DirectX 3.0 Checks
	//-------------------------------------------------------------------------

    // DirectInput was added for DX3
    hDInputDLL = LoadLibrary( _T("DINPUT.DLL") );
    if( hDInputDLL == NULL )
    {
        // No DInput... must not be DX3
        OutputDebugString( _T("Couldn't LoadLibrary DInput\r\n") );
        pDDraw->Release();
        return dwDXVersion;
    }

    DirectInputCreate = (DIRECTINPUTCREATE)GetProcAddress( hDInputDLL,
                                                        "DirectInputCreateA" );
    if( DirectInputCreate == NULL )
    {
        // No DInput... must be DX2
        FreeLibrary( hDInputDLL );
        FreeLibrary( hDDrawDLL );
        pDDraw->Release();
        OutputDebugString( _T("Couldn't GetProcAddress DInputCreate\r\n") );
        return dwDXVersion;
    }

    // DirectInputCreate exists. We are at least DX3
    dwDXVersion = 0x300;
    FreeLibrary( hDInputDLL );

    // Can do checks for 3a vs 3b here


	//-------------------------------------------------------------------------
    // DirectX 5.0 Checks
	//-------------------------------------------------------------------------

    // We can tell if DX5 is present by checking for the existence of
    // IDirectDrawSurface3. First, we need a surface to QI off of.
    DDSURFACEDESC ddsd;
    ZeroMemory( &ddsd, sizeof(ddsd) );
    ddsd.dwSize         = sizeof(ddsd);
    ddsd.dwFlags        = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    hr = pDDraw->SetCooperativeLevel( NULL, DDSCL_NORMAL );
    if( FAILED(hr) )
    {
        // Failure. This means DDraw isn't properly installed.
        pDDraw->Release();
        FreeLibrary( hDDrawDLL );
        dwDXVersion = 0;
        OutputDebugString( _T("Couldn't Set coop level\r\n") );
        return dwDXVersion;
    }

    hr = pDDraw->CreateSurface( &ddsd, &pSurf, NULL );
    if( FAILED(hr) )
    {
        // Failure. This means DDraw isn't properly installed.
        pDDraw->Release();
        FreeLibrary( hDDrawDLL );
        dwDXVersion = 0;
        OutputDebugString( _T("Couldn't CreateSurface\r\n") );
        return dwDXVersion;
    }

    // Query for the IDirectDrawSurface3 interface
    if( FAILED( pSurf->QueryInterface( IID_IDirectDrawSurface3,
                                       (VOID**)&pSurf3 ) ) )
    {
        pDDraw->Release();
        FreeLibrary( hDDrawDLL );
        return dwDXVersion;
    }

    // QI for IDirectDrawSurface3 succeeded. We must be at least DX5
    dwDXVersion = 0x500;


	//-------------------------------------------------------------------------
    // DirectX 6.0 Checks
	//-------------------------------------------------------------------------

    // The IDirectDrawSurface4 interface was introduced with DX 6.0
    if( FAILED( pSurf->QueryInterface( IID_IDirectDrawSurface4,
                                       (VOID**)&pSurf4 ) ) )
    {
        pDDraw->Release();
        FreeLibrary( hDDrawDLL );
        return dwDXVersion;
    }

    // IDirectDrawSurface4 was create successfully. We must be at least DX6
    dwDXVersion = 0x600;
    pSurf->Release();
    pDDraw->Release();


	//-------------------------------------------------------------------------
    // DirectX 6.1 Checks
	//-------------------------------------------------------------------------

    // Check for DMusic, which was introduced with DX6.1
    LPDIRECTMUSIC pDMusic = NULL;
    CoInitialize( NULL );
    hr = CoCreateInstance( CLSID_DirectMusic, NULL, CLSCTX_INPROC_SERVER,
                           IID_IDirectMusic, (VOID**)&pDMusic );
    if( FAILED(hr) )
    {
        OutputDebugString( _T("Couldn't create CLSID_DirectMusic\r\n") );
        FreeLibrary( hDDrawDLL );
        return dwDXVersion;
    }

    // DirectMusic was created successfully. We must be at least DX6.1
    dwDXVersion = 0x601;
    pDMusic->Release();
    CoUninitialize();
    

	//-------------------------------------------------------------------------
    // DirectX 7.0 Checks
	//-------------------------------------------------------------------------

    // Check for DirectX 7 by creating a DDraw7 object
    LPDIRECTDRAW7 pDD7;
    DirectDrawCreateEx = (DIRECTDRAWCREATEEX)GetProcAddress( hDDrawDLL,
                                                       "DirectDrawCreateEx" );
    if( NULL == DirectDrawCreateEx )
    {
        FreeLibrary( hDDrawDLL );
        return dwDXVersion;
    }

    if( FAILED( DirectDrawCreateEx( NULL, (VOID**)&pDD7, IID_IDirectDraw7,
                                    NULL ) ) )
    {
        FreeLibrary( hDDrawDLL );
        return dwDXVersion;
    }

    // DDraw7 was created successfully. We must be at least DX7.0
    dwDXVersion = 0x700;
    pDD7->Release();


	//-------------------------------------------------------------------------
    // DirectX 8.0 Checks
	//-------------------------------------------------------------------------

    // Simply see if D3D8.dll exists.
    hD3D8DLL = LoadLibrary( _T("D3D8.DLL") );
    if( hD3D8DLL == NULL )
    {
	    FreeLibrary( hDDrawDLL );
        return dwDXVersion;
    }

    // D3D8.dll exists. We must be at least DX8.0
    dwDXVersion = 0x800;


	//-------------------------------------------------------------------------
    // End of checking for versions of DirectX 
	//-------------------------------------------------------------------------

    // Close open libraries and return
    FreeLibrary( hDDrawDLL );
    FreeLibrary( hD3D8DLL );
    
    return dwDXVersion;

}




//-----------------------------------------------------------------------------
// 得到CPU个数
//-----------------------------------------------------------------------------
INT SysInfo::GetProcessorNum()
{
	SYSTEM_INFO  sysinfo;
    GetSystemInfo(&sysinfo);
    return (INT)sysinfo.dwNumberOfProcessors;
}


//-----------------------------------------------------------------------------
// 根据异常码得到异常描述字符串
//-----------------------------------------------------------------------------
const CHAR *SysInfo::GetExceptionDescription(DWORD ExceptionCode)
{
	for (int i = 0; i < sizeof(ExceptionMap) / sizeof(ExceptionMap[0]); i++)
		if (ExceptionCode == ExceptionMap[i].ExceptionCode)
			return ExceptionMap[i].ExceptionName;

	return "Unknown exception type";
}



//-----------------------------------------------------------------------------
// FormatTime 将时间输出成人可以阅读的格式
//-----------------------------------------------------------------------------
VOID SysInfo::FormatTime(LPSTR pDest, FILETIME TimeToPrint)
{
	pDest[0] = '\0';
	WORD wDate, wTime;
	if (FileTimeToLocalFileTime(&TimeToPrint, &TimeToPrint) &&
				FileTimeToDosDateTime(&TimeToPrint, &wDate, &wTime))
	{
		sprintf(pDest, "%d/%d/%d %02d:%02d:%02d",
			(wDate / 32) & 15, wDate & 31, (wDate / 512) + 1980,
			(wTime >> 11), (wTime >> 5) & 0x3F, (wTime & 0x1F) * 2);
	}
}



//-----------------------------------------------------------------------------
// GetFilePart
//-----------------------------------------------------------------------------
CHAR* SysInfo::GetFilePart(LPCSTR pSource)
{
	if( pSource )
	{
		CHAR *pResult = (CHAR*)strrchr(pSource, '\\');
		if( pResult )
			pResult++;
		else
			pResult = (CHAR *)pSource;
		return pResult;
	}

	return NULL;
}





//-----------------------------------------------------------------------------
// 关闭系统
//-----------------------------------------------------------------------------
BOOL SysInfo::SystemShutdown()
{
   HANDLE hToken;
   TOKEN_PRIVILEGES tkp;

   // Get a token for this process.
   if( !OpenProcessToken(GetCurrentProcess(),
        TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken) )
      return( FALSE );

   // Get the LUID for the shutdown privilege.
   LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,
        &tkp.Privileges[0].Luid);

   tkp.PrivilegeCount = 1;  // one privilege to set
   tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

   // Get the shutdown privilege for this process.
   AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
        (PTOKEN_PRIVILEGES)NULL, 0);

   if( GetLastError() != ERROR_SUCCESS )
      return FALSE;

   // Shut down the system and force all applications to close.
   if( !ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0) )
      return FALSE;

   return TRUE;
}


}