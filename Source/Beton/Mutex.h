//--------------------------------------------------------------------------------------------------
// File: Mutex.h
// Desc: Thread mutex
// Time: 2008-05-06
// Auth: Aslan
//
// Copyright (c) 2008 Tengreng All rights reserved.
//---------------------------------------------------------------------------------------------------
#pragma once

#include "StdAfx.h"
#include "Config.h"

namespace Beton {

//----------------------------------------------------------------------------------------------------
/**	Mutex类 - 提供用户态的操作锁
*/
//----------------------------------------------------------------------------------------------------
class Mutex
{
public:
	Mutex()		{ if(FALSE == ::InitializeCriticalSectionAndSpinCount(&cs, 4000)) { abort(); } }
	~Mutex()	{ ::DeleteCriticalSection(&cs); }

	BETON_INLINE VOID Acquire() { ::EnterCriticalSection(&cs); }
	BETON_INLINE VOID Release() { ::LeaveCriticalSection(&cs); }
	BETON_INLINE BOOL TryAcquire() { return ::TryEnterCriticalSection(&cs); }

private:
	CRITICAL_SECTION cs;

};

} // namespace Beton