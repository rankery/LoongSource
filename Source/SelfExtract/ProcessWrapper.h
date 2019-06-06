#pragma once


class CProcessWrapper
{
public:
	CProcessWrapper(void);

	~CProcessWrapper(void);

public:

	bool		QuitProcess( const TCHAR* szProcessName, BOOL bFullPath = FALSE );

	bool		ShutDownProcess( const TCHAR* szProcessName, BOOL bFullPath = FALSE  );

	bool		ExecuteProcess( const TCHAR* szProcessName );


	//查找进程，返回进程的ID
	HANDLE		FindProcess( DWORD& dwProcessID,  const TCHAR* szProcessName, BOOL bFullPath = FALSE );

	//查找所有相关进程，返回进程ID信息
	int			FindAllProcess( HANDLE* pProcessHandle, DWORD* dwID,  const TCHAR* szProcessName, BOOL bFullPath = FALSE );

private:

	HANDLE		m_hSnapShot;


};
