// F3DMaxExp.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include "MaxNodeType.h"
#include "MsgBox.h"

HANDLE g_hInst=NULL;
static int controlsInit = FALSE;
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	g_hInst=hModule;
	HINSTANCE hInstance=static_cast<HINSTANCE>(hModule);
	if ( !controlsInit ) 
	{
		controlsInit = TRUE;

		// Initialize MAX's custom controls
		InitCustomControls(hInstance);

		// initialize Chicago controls
		InitCommonControls();
	}
    return TRUE;
}

