//-----------------------------------------------------------------------------
//!\file unit_pool.h
//!\author Lyp
//!
//!\date 2004-07-01
//! last 2004-07-06
//!
//!\brief 完成端口网络通讯底层内存池
//-----------------------------------------------------------------------------
#pragma once
#include "safe_unit_queue.h"

namespace vEngine {

#ifdef _DEBUG_UNIT
	extern TSafeMap<tagUnitData*, tagUnitData*>	mapUnitAddress;	// for DEBUG
#endif // _DEBUG_UNIT
//-----------------------------------------------------------------------------
//!\brief 完成端口网络通讯底层内存池
//!	名称定义：针对每个unit, 开放了1个缓冲区，为windows定义的wbuf
//-----------------------------------------------------------------------------
class VENGINE_API UnitPool
{
public:
	// nUnitNum = 初始化时先实际alloc多少unit
	BOOL Init(INT nUnitNum);
	BOOL Destroy();

	// 失败返回-1,成功返回unit index
	__forceinline tagUnitData*	AllocUnit();	
	__forceinline VOID			FreeUnit(tagUnitData*);

	// 尝试进入锁定区
	__forceinline tagUnitData*	TryAllocUnit();
	__forceinline BOOL			TryFreeUnit(tagUnitData*);

	// 得到当前unit个数
	INT	GetSize() { return m_FreeUnitQueue.GetNum(); }
	// 得到从本池一共实际alloc的unit个数,包括内部alloc的但外部未申请的
	INT	GetTotalNum()	{ return m_nTotalNum; }


	UnitPool();
	~UnitPool();

private:
	SafeQueue<tagUnitData*>	m_FreeUnitQueue;	// 数据
	VOLATILE INT			m_nDefaultNum;
	VOLATILE INT			m_nTotalNum;

	BOOL		RealAlloc(INT nUnitNum);
	BOOL		RealFree(INT nUnitNum);
};


//-----------------------------------------------------------------------------
// 失败返回NULL
//-----------------------------------------------------------------------------
tagUnitData* UnitPool::AllocUnit()
{
	tagUnitData* pUnit = m_FreeUnitQueue.Get();
	while( NULL == pUnit )
	{
		RealAlloc(m_nDefaultNum);	// 没有了，咱们再申请一倍
		pUnit = m_FreeUnitQueue.Get();
	}

	pUnit->eState = EUS_Null;

#ifdef _DEBUG_UNIT
	mapUnitAddress.Add(pUnit, pUnit) ;	// FOR DEBUG
#endif

	return pUnit;
}


//-----------------------------------------------------------------------------
// free unit
//-----------------------------------------------------------------------------
VOID UnitPool::FreeUnit(tagUnitData* pUnit)
{
	m_FreeUnitQueue.Add(pUnit);

#ifdef _DEBUG_UNIT
	mapUnitAddress.Erase(pUnit);	// FOR DEBUG
#endif

	// 空闲的太多,交还内存
	INT n = m_FreeUnitQueue.GetNum();
	if( n > m_nDefaultNum * 8 )
		RealFree(n/4);
}


//-----------------------------------------------------------------------------
// 尝试进入锁定区,alloc unit
//-----------------------------------------------------------------------------
tagUnitData* UnitPool::TryAllocUnit()
{
	tagUnitData* pUnit = NULL;
	if( m_FreeUnitQueue.GetNum() <=0 )
	{
		// 没有了，咱们再申请一倍
		this->RealAlloc(m_nDefaultNum);
	}

	pUnit = m_FreeUnitQueue.TryGet();
	
	if( pUnit )
	{
		pUnit->eState = EUS_Null;

#ifdef _DEBUG_UNIT
		mapUnitAddress.Add(pUnit, pUnit) ;	// FOR DEBUG
#endif
	}
	
	return pUnit;
}


//-----------------------------------------------------------------------------
// 尝试进入锁定区,free unit
//-----------------------------------------------------------------------------
BOOL UnitPool::TryFreeUnit(tagUnitData* pUnit)
{
	if( FALSE == m_FreeUnitQueue.TryAdd(pUnit) )
		return FALSE;

	// 空闲的太多,交还内存
	INT n = m_FreeUnitQueue.GetNum();
	if( n > m_nDefaultNum * 8 )
		RealFree(n/4);


#ifdef _DEBUG_UNIT
	mapUnitAddress.Erase(pUnit) ;	// FOR DEBUG
#endif

	return TRUE;
}




} // namespace wEngine {
