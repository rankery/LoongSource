//-----------------------------------------------------------------------------
// File: log
// Desc: Game Tool log 1.0
// Auth: Lyp
// Date: 2003/12/18
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine {


//-----------------------------------------------------------------------------
// log
//-----------------------------------------------------------------------------
class VENGINE_API Log
{
public:

	BOOL Create(LPCTSTR szFileName=NULL);
	BOOL Write(LPCTSTR lpFormat, ...);
	VOID Close();
	Log();~Log();
private:
	TObjRef<DiscIOMgr>		m_pDiscIOMgr;	// 穿过了VFS系统
	DWORD					m_dwFileHandle;
	TCHAR					m_szDefaultName[1024];
};


//-----------------------------------------------------------------------------
// 专门提供给异常模块使用的log,外部应调用上面的log模块
//-----------------------------------------------------------------------------
class ExceptionLog
{
public:

	BOOL Create();
	BOOL Write(LPCSTR lpFormat, ...);
	VOID Close();
	LPCTSTR	GetDefaultFileName() { return m_szDefaultName; }

	ExceptionLog();~ExceptionLog();

private:
	TObjRef<DiscIOMgr>		m_pDiscIOMgr;	// 穿过了VFS系统
	DWORD					m_dwFileHandle;
	TCHAR					m_szDefaultName[1024];
};


} // namespace vEngine {
