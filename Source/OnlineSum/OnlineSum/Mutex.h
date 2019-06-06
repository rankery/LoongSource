#pragma once
#include "stdafx.h"

//------------------------------------------------------------------------------------------------------
// 临界区简单实现类
//------------------------------------------------------------------------------------------------------
class Mutex
{
public:
	Mutex()		{ if(FALSE == InitializeCriticalSectionAndSpinCount(&cs, 4000)) { abort(); } nCishu = 0; }
	~Mutex()	{ nCishu; DeleteCriticalSection(&cs); }

	VOID Acquire() { EnterCriticalSection(&cs); InterlockedExchangeAdd((LPLONG)&nCishu, 1); }
	VOID Release() { LeaveCriticalSection(&cs); InterlockedExchangeAdd((LPLONG)&nCishu, -1); }
	BOOL TryAcquire() { return TryEnterCriticalSection(&cs); }

private:
	CRITICAL_SECTION cs;
	volatile INT nCishu;

};
