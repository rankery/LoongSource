#ifndef RES_GUARD_H
#define RES_GUARD_H

#pragma once
#include <Windows.h>
#include <memory>

class CResGuard
{
public:
	CResGuard()
	{
	#if (_WIN32_WINNT >= 0x0403)
		::InitializeCriticalSectionAndSpinCount(&m_cs, 4000);
	#else	
		::InitializeCriticalSection(&m_cs);
	#endif
	}

#if (_WIN32_WINNT >= 0x0403)
	CResGuard(DWORD spinCnt)
	{
		::InitializeCriticalSectionAndSpinCount(&m_cs, spinCnt);
	}
#endif

	~CResGuard()
	{
		::DeleteCriticalSection(&m_cs);
	}

public:
	class CGuard
	{
	public:
		CGuard(CResGuard &rg): m_rg(rg)
		{
			m_rg.Guard();
		}

		~CGuard()
		{
			m_rg.Unguard();
		}

	private:
		CResGuard &m_rg;
	};

private:
	void Guard()
	{
		::EnterCriticalSection(&m_cs);
	}

	void Unguard()
	{
		::LeaveCriticalSection(&m_cs);
	}

	friend class CGuard;
private:
	CRITICAL_SECTION m_cs;
};


#endif