#pragma once

#include "LockableObject.h"

namespace Cool3D
{
	class ThreadLock
	{
	public:
		ThreadLock(LockableObject *pLockable)
		{
			m_pLockable = pLockable;
			m_pLockable->Lock();
		}

		~ThreadLock(void)
		{
			m_pLockable->Unlock();
		}

	private:
		LockableObject *m_pLockable;
	};
}//namespace Cool3D