#pragma once

// internal function prototypes
VOID WINAPI service_ctrl(DWORD dwCtrlCode);
VOID WINAPI service_main(DWORD dwArgc, LPTSTR *lpszArgv);

VOID CmdInstallService();
VOID CmdRemoveService();

LPTSTR GetLastErrorText( LPTSTR lpszBuf, DWORD dwSize );


BOOL ReportStatusToSCMgr(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint);
void AddToMessageLog(LPTSTR lpszMsg);
