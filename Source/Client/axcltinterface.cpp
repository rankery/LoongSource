#include "stdafx.h"
#include "axcltinterface.h"

#include <stdio.h>
#include <stdlib.h>
#include "windows.h"

typedef int    __stdcall SH_START_PROC( _FUNC_C_SEND pfSend , _FUNC_C_REC & pfRec );
typedef void   __stdcall SH_DONE_PROC(void);

static		   SH_START_PROC *		g_sh_start		= NULL;
static		   SH_DONE_PROC *		g_sh_exit		= NULL;
static		   HMODULE				g_Hmodule		= NULL;

#define		   FaceDllName			"axclt.dll"
void GetFullPath(char * szFullPath)
{
	char szPath[ X_LONG_NAME ]		= { 0 };
	char szDrive[ X_LONG_NAME ]	= { 0 };
	char szDir[ X_LONG_NAME ]		= { 0 };
	char szFName[ X_LONG_NAME ]	= { 0 };
	char szExt[ X_LONG_NAME ]		= { 0 };

	GetModuleFileNameA( NULL, szPath, X_LONG_NAME );
	_splitpath( szPath, szDrive, szDir, szFName, szExt );
	sprintf( szFullPath,"%s%s%s",szDrive,szDir,FaceDllName);

}
extern "C" int ax_CHCStart( _FUNC_C_SEND pfSend ,_FUNC_C_REC & pfRec )
{

	char szPath[ X_LONG_NAME ] = { 0 };
	GetFullPath( szPath );

	HMODULE g_Hmodule	= LoadLibraryA( szPath );
	if( !g_Hmodule )
		return 1;
	g_sh_start			= (SH_START_PROC *) GetProcAddress( g_Hmodule, "d_CHCStart" );
	g_sh_exit			= ( SH_DONE_PROC*) GetProcAddress( g_Hmodule, "d_CHCEnd" );	

	if ( !g_sh_exit || !g_sh_start )
	{
		if(!g_Hmodule)

			FreeLibrary( g_Hmodule );

		g_Hmodule		=NULL;
		g_sh_exit		= NULL;
		g_sh_start		= NULL;
		return 2;
	}
	int nRet			= g_sh_start( pfSend, pfRec );
	return nRet;	
}
extern "C" void ax_CHCEnd()
{
	if ( g_sh_exit )
	{
		g_sh_exit();
	}

	if( NULL != g_Hmodule )
		FreeLibrary( g_Hmodule );

	g_Hmodule			= NULL;
	g_sh_exit			= NULL;
	g_sh_start			= NULL;
		
}