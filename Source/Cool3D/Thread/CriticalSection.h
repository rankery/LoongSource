#pragma once
#include "LockableObject.h"

namespace Cool3D
{
	class Cool3D_API CriticalSection : public LockableObject
	{
	public:
		CriticalSection(void)	
		{
			::InitializeCriticalSection(&m_critSect);
		}
		virtual ~CriticalSection(void) 
		{
			::DeleteCriticalSection(&m_critSect);
		}

		virtual void Lock()
		{
			::EnterCriticalSection(&m_critSect);
		}

		virtual void Unlock()
		{
			::LeaveCriticalSection(&m_critSect);
		}

	private:
		CRITICAL_SECTION m_critSect;
	};
}//namespace Cool3D