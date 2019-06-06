//-----------------------------------------------------------------------------
//!\file thread.h
//!\author Lyp
//!
//!\date 2004-08-09
//! last 2004-08-09
//!
//!\brief 线程管理工具
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine {


//-----------------------------------------------------------------------------
//!\brief 线程管理工具
//!
//!
//-----------------------------------------------------------------------------
class VENGINE_API Thread
{
public:

	//! 建立指定名字的线程
	BOOL CreateThread(LPCTSTR szName, THREADPROC pFn, LPVOID Param);
	//! 等待指定名字的线程退出,如果指定时间内没有退出则返回FALSE
	//! 注意,使用此函数你必须自己建立线程退出的机制,此函数并不负责通知线程
	BOOL WaitForThreadDestroy(LPCTSTR szName, DWORD dwTimeOut);
	// destroy all thread 永远等待
	VOID WaitForAllThreadDestroy();

	// 检查县程是否运行中
	BOOL IsThreadActive(LPCTSTR szName);

	// 得到指定线程的handle
	HANDLE GetThreadHandle(LPCTSTR szName);
	

	Thread();
	~Thread();

private:
	TObjRef<Util>				m_pUtil;
	TSafeMap<DWORD, HANDLE>		m_mapHandle;

};






} // namespace vEngine {
