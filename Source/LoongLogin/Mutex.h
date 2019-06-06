//-----------------------------------------------------------------------------
//!\file Mutex.h
//!
//!\date 2008-07-07
//!
//!\brief ·â×° CRITICAL_SECTION
//-----------------------------------------------------------------------------
#pragma once

class LoginMutex
{
public:

	LoginMutex()				{	InitializeCriticalSection(&cs);	}
	~LoginMutex()				{	DeleteCriticalSection(&cs);		}

	VOID Acquire()			{	EnterCriticalSection(&cs);		}
	VOID Release()			{	LeaveCriticalSection(&cs);		}
	BOOL AttemptAcquire()	{	return TryEnterCriticalSection(&cs);	}

private:
	 
	CRITICAL_SECTION cs;
};
