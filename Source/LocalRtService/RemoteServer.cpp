// RemoteServer.cpp : 定义控制台应用程序的入口点。
#include "stdafx.h"
#include "RemoteServer.h"
#include "service.h"

SERVICE_STATUS          ssStatus;       // current status of the service
SERVICE_STATUS_HANDLE   sshStatusHandle;
DWORD                   dwErr = 0;
TCHAR                   szErr[256];

CService				service;

// To perform actual initialization of the service
void WINAPI service_main(DWORD dwArgc, LPTSTR *lpszArgv)
{
	// register our service control handler:
	sshStatusHandle = RegisterServiceCtrlHandler( TEXT(SZSERVICENAME), service_ctrl);
	if( !sshStatusHandle )
		goto cleanup;

	// SERVICE_STATUS members that don't change in example
	ssStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	ssStatus.dwServiceSpecificExitCode = 0;

	// report the status to the service control manager.
	if( !ReportStatusToSCMgr( SERVICE_START_PENDING, NO_ERROR, 3000) )
		goto cleanup;

	service.ServiceStart( dwArgc, lpszArgv );

cleanup:
	// try to report the stopped status to the service control manager.
	if (sshStatusHandle)
		(VOID)ReportStatusToSCMgr(SERVICE_STOPPED, dwErr, 0);

	return;
}



// This function is called by the SCM whenever
// ControlService() is called on this service.
// dwCtrlCode - type of control requested
VOID WINAPI service_ctrl(DWORD dwCtrlCode)
{
	// Handle the requested control code.
	//
	switch (dwCtrlCode)
	{
		// Stop the service.
		//
		// SERVICE_STOP_PENDING should be reported before
		// setting the Stop Event - hServerStopEvent - in
		// ServiceStop().  This avoids a race condition
		// which may result in a 1053 - The Service did not respond...
		// error.
	case SERVICE_CONTROL_STOP:
		ReportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 0);
		service.ServiceStop();
		return;

		// Update the service status.
	case SERVICE_CONTROL_INTERROGATE:
		break;

	default:
		break;

	}

	ReportStatusToSCMgr(ssStatus.dwCurrentState, NO_ERROR, 0);
}



// Sets the current status of the service and
// reports it to the Service Control Manager
// dwCurrentState - the state of the service
// dwWin32ExitCode - error code to report
// dwWaitHint - worst case estimate to next checkpoint
BOOL ReportStatusToSCMgr(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
	static DWORD dwCheckPoint = 1;
	BOOL fResult = TRUE;

	if (dwCurrentState == SERVICE_START_PENDING)
		ssStatus.dwControlsAccepted = 0;
	else
		ssStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

	ssStatus.dwCurrentState = dwCurrentState;
	ssStatus.dwWin32ExitCode = dwWin32ExitCode;
	ssStatus.dwWaitHint = dwWaitHint;

	if ( ( dwCurrentState == SERVICE_RUNNING ) ||
		( dwCurrentState == SERVICE_STOPPED ) )
		ssStatus.dwCheckPoint = 0;
	else
		ssStatus.dwCheckPoint = dwCheckPoint++;


	// Report the status of the service to the service control manager.
	if (!(fResult = SetServiceStatus( sshStatusHandle, &ssStatus)))
	{
		AddToMessageLog(TEXT("SetServiceStatus"));
	}

	return fResult;
}


// Allows any thread to log an error message
VOID AddToMessageLog(LPTSTR lpszMsg)
{
	TCHAR szMsg [(sizeof(SZSERVICENAME) / sizeof(TCHAR)) + 100 ];
	HANDLE  hEventSource;
	LPTSTR  lpszStrings[2];

	dwErr = GetLastError();

	// Use event logging to log the error.
	hEventSource = RegisterEventSource(NULL, TEXT(SZSERVICENAME));

	_stprintf(szMsg, TEXT("%s error: %d"), TEXT(SZSERVICENAME), dwErr);
	lpszStrings[0] = szMsg;
	lpszStrings[1] = lpszMsg;

	if( hEventSource != NULL )
	{
		ReportEvent(hEventSource, EVENTLOG_INFORMATION_TYPE, 0, 0, NULL, 2, 0,
			(LPCTSTR*)lpszStrings, NULL);

		DeregisterEventSource(hEventSource);
	}
}



// Installs the service
void CmdInstallService()
{
	SC_HANDLE   schService;
	SC_HANDLE   schSCManager;

	TCHAR szPath[512];

	if ( GetModuleFileName( NULL, szPath, 512 ) == 0 )
	{
		_tprintf(TEXT("Unable to install %s - %s\n"), TEXT(SZSERVICEDISPLAYNAME), 
			GetLastErrorText(szErr, 256));
		return;
	}

	schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE);
	if( schSCManager )
	{
		schService = CreateService(
			schSCManager,               // SCManager database
			TEXT(SZSERVICENAME),        // name of service
			TEXT(SZSERVICEDISPLAYNAME), // name to display
			SERVICE_ALL_ACCESS,         // desired access
			SERVICE_WIN32_SHARE_PROCESS|SERVICE_INTERACTIVE_PROCESS,// service type
			SERVICE_AUTO_START,			// start type
			SERVICE_ERROR_NORMAL,       // error control type
			szPath,                     // service's binary
			NULL,                       // no load ordering group
			NULL,                       // no tag identifier
			TEXT(SZDEPENDENCIES),       // dependencies
			NULL,	                    // LocalSystem account
			NULL);	                    // no password

		if ( schService )
		{
			_tprintf(TEXT("%s installed.\n"), TEXT(SZSERVICEDISPLAYNAME) );

			SERVICE_DESCRIPTION info;
			info.lpDescription = TEXT(SZSERVICEDESCRIPTION);
			ChangeServiceConfig2(schService, SERVICE_CONFIG_DESCRIPTION, &info);

			BOOL bRet = StartService(schService, 0, NULL);
			CloseServiceHandle(schService);
		}
		else
		{
			_tprintf(TEXT("CreateService failed - %s\n"), GetLastErrorText(szErr, 256));
		}

		CloseServiceHandle(schSCManager);
	}
	else
		_tprintf(TEXT("OpenSCManager failed - %s\n"), GetLastErrorText(szErr,256));
}



// Stops and removes the service
void CmdRemoveService()
{
	SC_HANDLE   schService;
	SC_HANDLE   schSCManager;

	schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if( schSCManager )
	{
		schService = OpenService(schSCManager, TEXT(SZSERVICENAME), DELETE | SERVICE_STOP | SERVICE_QUERY_STATUS);
		if (schService)
		{
			// try to stop the service
			if ( ControlService( schService, SERVICE_CONTROL_STOP, &ssStatus ) )
			{
				_tprintf(TEXT("Stopping %s."), TEXT(SZSERVICEDISPLAYNAME));
				Sleep( 1000 );

				while ( QueryServiceStatus( schService, &ssStatus ) )
				{
					if ( ssStatus.dwCurrentState == SERVICE_STOP_PENDING )
					{
						_tprintf(TEXT("."));
						Sleep( 1000 );
					}
					else
						break;
				}

				if ( ssStatus.dwCurrentState == SERVICE_STOPPED )
					_tprintf(TEXT("\n%s stopped.\n"), TEXT(SZSERVICEDISPLAYNAME) );
				else
					_tprintf(TEXT("\n%s failed to stop.\n"), TEXT(SZSERVICEDISPLAYNAME) );

			}

			// now remove the service
			if ( DeleteService(schService) )
				_tprintf(TEXT("%s removed.\n"), TEXT(SZSERVICEDISPLAYNAME) );
			else
				_tprintf(TEXT("DeleteService failed - %s\n"), GetLastErrorText(szErr,256));


			CloseServiceHandle(schService);
		}
		else
			_tprintf(TEXT("OpenService failed - %s\n"), GetLastErrorText(szErr,256));

		CloseServiceHandle(schSCManager);
	}
	else
		_tprintf(TEXT("OpenSCManager failed - %s\n"), GetLastErrorText(szErr,256));
}



// copies error message text to string
LPTSTR GetLastErrorText( LPTSTR lpszBuf, DWORD dwSize )
{
	DWORD dwRet;
	LPTSTR lpszTemp = NULL;

	dwRet = FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM 
		|FORMAT_MESSAGE_ARGUMENT_ARRAY,	NULL, GetLastError(), LANG_NEUTRAL, (LPTSTR)&lpszTemp,
		0, NULL );

	// supplied buffer is not long enough
	if( !dwRet || ( (long)dwSize < (long)dwRet+14 ) )
		lpszBuf[0] = TEXT('\0');
	else
	{
		lpszTemp[lstrlen(lpszTemp)-2] = TEXT('\0');  //remove cr and newline character
		_stprintf( lpszBuf, TEXT("%s (0x%x)"), lpszTemp, GetLastError() );
	}

	if ( lpszTemp )
		LocalFree((HLOCAL) lpszTemp );

	return lpszBuf;
}

