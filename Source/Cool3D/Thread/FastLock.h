#pragma once
#include "LockableObject.h"

namespace Cool3D
{
	class Cool3D_API FastLock : public LockableObject
	{
	public:
		FastLock(void) : m_lLock(0)	{}
		virtual ~FastLock(void)		{}

		virtual void Lock()
		{
			while(::InterlockedCompareExchange((LPLONG)&m_lLock, 1, 0)!= 0) ::Sleep(0);
		}

		virtual void Unlock()
		{
			::InterlockedExchange((LPLONG)(&m_lLock), 0);
		}

	private:
		volatile LONG m_lLock;
	};
}//namespace Cool3D