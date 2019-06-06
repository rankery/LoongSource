//--------------------------------------------------------------------------------------------------
// File: StreamPool.h
// Desc: Mysql SQL statement pool
// Time: 2008-05-05
// Auth: Aslan
//
// Copyright (c) 2008 Tengreng All rights reserved.
//---------------------------------------------------------------------------------------------------
#pragma once

#include "StdAfx.h"
#include "Config.h"
#include "MysqlStream.h"
#include "SafeStreamQueue.h"

namespace Beton {

class MyStream;
class SafeStreamQueue;
//----------------------------------------------------------------------------------------------------
/**	StreamPool类 - 为上层提供MysqlStream池，供上层提取使用
1. 预先分配一定大小的资源，如不足可动态扩充，如分配过多，可在不需要时释放多余内存
2. 线程安全
*/
//-----------------------------------------------------------------------------------------------------
class BETON_API StreamPool
{
public:
	StreamPool(INT nDefaultSize=1000);
	virtual ~StreamPool();

private:
	StreamPool(const StreamPool&);
	StreamPool& operator = (const StreamPool&);

public:
	BETON_INLINE MyStream* AllocStream();
	BETON_INLINE VOID FreeStream(MyStream* pStream);

	BETON_INLINE LONG GetAvailableStreamNum() { return m_FreeQueue.Size(); }
	BETON_INLINE INT GetAllStreamNum() { return m_nTotalNum; }

private:
	BETON_INLINE VOID RealAlloc(INT nNum);
	BETON_INLINE VOID RealFree(INT nNum);
	BETON_INLINE VOID RealFreeAll();

private:
	Mutex				m_Mutex;
	SafeStreamQueue		m_FreeQueue;
	VOLATILE INT		m_nDefaultSize;		// 池扩充时的默认大小
	VOLATILE INT		m_nTotalNum;		// 该池一共分配了多少个stream
};

} // namespace Beton