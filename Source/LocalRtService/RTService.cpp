// RTService.cpp : 定义控制台应用程序的入口点。
#include "stdafx.h"
#include "RemoteServer.h"

//-----------------------------------------------------------------------------
// main() either performs the command line task, or
// call StartServiceCtrlDispatcher to register the
// main service thread.  When the this call returns,
// the service has stopped, so exit.
//-----------------------------------------------------------------------------
void __cdecl main(int argc, char **argv)
{
	if ( (argc > 1) && ((*argv[1] == '-') || (*argv[1] == '/')) )
	{
		if ( _stricmp( "install", argv[1]+1 ) == 0 )
			CmdInstallService();
		else if ( _stricmp( "remove", argv[1]+1 ) == 0 )
			CmdRemoveService();
		else
			goto dispatch;
		
		exit(0);
	}

	// if it doesn't match any of the above parameters,
	// the service control manager may be starting the service
	// so we must call StartServiceCtrlDispatcher
dispatch:
	_tprintf( _T("Loong server control service\n") );
	_tprintf( _T("%s /install  Install service\n"), _T(SZAPPNAME) );
	_tprintf( _T("%s /remove   Remove service\n"), _T(SZAPPNAME) );

	SERVICE_TABLE_ENTRY dispatchTable[] =
	{
		{ TEXT(SZSERVICENAME), (LPSERVICE_MAIN_FUNCTION)service_main },
		{ NULL, NULL }
	};

	StartServiceCtrlDispatcher(dispatchTable);
	return;
}
