//-----------------------------------------------------------------------------
//!\file net_mem_pool.cpp
//!\author Lyp
//!
//!\date 2004-07-01
//! last 2004-09-16
//!
//!\brief 网络通讯底层内存池
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "unit_pool.h"


namespace vEngine {

#ifdef _DEBUG_UNIT
	TSafeMap<tagUnitData*, tagUnitData*>	mapUnitAddress;	// for DEBUG
#endif // _DEBUG_UNIT
//-----------------------------------------------------------------------------
//! construction
//-----------------------------------------------------------------------------
UnitPool::UnitPool()
{
	m_nDefaultNum = 0;
	m_nTotalNum = 0;
}


//-----------------------------------------------------------------------------
//! destruction
//-----------------------------------------------------------------------------
UnitPool::~UnitPool()
{
	Destroy();
}


//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL UnitPool::Init(INT nUnitNum)
{
	m_nDefaultNum = nUnitNum;
	return RealAlloc(nUnitNum);
}



//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
BOOL UnitPool::Destroy()
{
	return RealFree(m_FreeUnitQueue.GetNum());
}





//-----------------------------------------------------------------------------
// 内部函数，实际真正alloc unit space
//-----------------------------------------------------------------------------
BOOL UnitPool::RealAlloc(INT nUnitNum)
{
	tagUnitData* pData = NULL;
	for( INT n=0; n<nUnitNum; n++ )
	{
		pData = new tagUnitData;
		if( NULL == pData )
			break;

		pData->wbuf.buf = new CHAR[GT_MAX_PACKAGE_LEN + 8]; // +8为了MMX加密用
		if( NULL == pData->wbuf.buf )
		{
			delete(pData);
			break;
		}

		pData->wbuf.len = GT_MAX_PACKAGE_LEN;
		
		InterlockedIncrement((LONG*)&m_nTotalNum);
		m_FreeUnitQueue.Add(pData);
	}

	return TRUE;
}


//-----------------------------------------------------------------------------
// 内部函数，实际真正free unit space
//-----------------------------------------------------------------------------
BOOL UnitPool::RealFree(INT nUnitNum)
{
	tagUnitData* pUnit = NULL;
	for( INT n=0; n<nUnitNum; n++ )
	{
		if( pUnit = m_FreeUnitQueue.Get() )
		{
			SAFE_DEL_ARRAY(pUnit->wbuf.buf);
			SAFE_DEL(pUnit);
			InterlockedDecrement((LONG*)&m_nTotalNum);
		}
		else
		{
			return FALSE;
		}
	}
	return TRUE;
}


} // namespace wEngine {