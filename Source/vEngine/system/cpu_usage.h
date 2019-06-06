//-----------------------------------------------------------------------------
//!\file cpu_usage.h
//!\author Lyp
//!
//!\date 2004-08-03
//! last 2004-08-03
//!
//!\brief CPU使用情况
//-----------------------------------------------------------------------------
#pragma once


namespace vEngine {

// Little helper to work with GetTickCount(), since this is an 32bit
// free running counter which pass from 0xFFFFFFFF to 0, it would be
// a good idea to use TKDelay, which counts delays in milliseconds up
// to (about) 24 days (2147483647 msec).
class TKDelay
{
public:
    void    Mark(){ m_mark = ::GetTickCount(); };
    int     MSec(){ return( (::GetTickCount()-m_mark)&0x7FFFFFFF); };
private:
    DWORD   m_mark;
};


typedef BOOL ( __stdcall * pfnGetSystemTimes)( LPFILETIME lpIdleTime, LPFILETIME lpKernelTime, LPFILETIME lpUserTime );

class VENGINE_API CPU
{
public:
    CPU( void );
    ~CPU( void );
    
    // return :
    // % of cpu usage for this process 
    // % cpu systemUsage 
    int GetUsage( int* pSystemUsage);

private:
    static TKDelay	s_delay;
    static __int64	s_time;
    static __int64	s_idleTime;
    static __int64	s_kernelTime;
    static __int64	s_userTime;
    static int		s_lastCpu;
    static int		s_cpu[5];
    static __int64	s_kernelTimeProcess;
    static __int64	s_userTimeProcess;
    static int		s_lastCpuProcess;
    static int		s_cpuProcess[5];
    static int		s_count;
    static int		s_index;
    static __int64	s_lastUpTime;

    static HINSTANCE			s_hKernel;
    static pfnGetSystemTimes	s_pfnGetSystemTimes;

    CRITICAL_SECTION			m_lock;
};

} // namespace vEngine {
