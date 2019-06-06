//-----------------------------------------------------------------------------
// File: interfacemgr
// Auth: Lyp
// Date: 2003/12/11
// Last: 2004/03/15
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "interfacemgr.h"

#undef new
#undef delete
#undef malloc
#undef calloc
#undef realloc
#undef free

namespace vEngine{
//-----------------------------------------------------------------------------
// construction
//-----------------------------------------------------------------------------
ObjMgr::ObjMgr()
{
	InitializeCriticalSection(&m_cs);
}


//-----------------------------------------------------------------------------
// 在这里析构所有对象
//-----------------------------------------------------------------------------
ObjMgr::~ObjMgr()
{
	std::map<DWORD, tagObj>::iterator it;

	EnterCriticalSection(&m_cs);

	// 按照注册的相反顺序析构对象
	while(!m_listObj.empty())
	{
		std::string str = m_listObj.back();
		Kill(str.c_str());
	}


	// 析构剩余对象
	for( it=m_mapObj.begin(); it!=m_mapObj.end(); ++it )
	{
		if( !P_VALID(it->second.pObj) )
			continue;
		m_AF.Destroy(it->second.pObj, it->second.dwClassID);
		it->second.pObj = NULL;
	}

	m_mapObj.clear();

	LeaveCriticalSection(&m_cs);
	DeleteCriticalSection(&m_cs);
}


//-----------------------------------------------------------------------------
// 根据szName获得对象
// 对于singleton对象: 如果对象还没有创建，则创建之, 线程安全(保证只有一个实例)
//-----------------------------------------------------------------------------
LPVOID ObjMgr::Get(LPCSTR szName)
{
	ASSERT(szName);
	DWORD dwID = Crc32(szName);
	LPVOID pObj = NULL;
	std::map<DWORD, tagObj>::iterator it;

	EnterCriticalSection(&m_cs);

	it = m_mapObj.find(dwID);	// 从对象中找
	if( it == m_mapObj.end() )
	{
		LeaveCriticalSection(&m_cs);
		return NULL;
	}

	if( it->second.pObj )
		pObj = it->second.pObj;
	else
	{
		// 对象还没有创建，准备创建
		it->second.pObj = m_AF.Create(dwID);
		pObj = it->second.pObj;
	}

	LeaveCriticalSection(&m_cs);
	return pObj;
}


LPVOID ObjMgr::GetByTypeName(LPCSTR szName)
{
	DWORD dwTypeID = Crc32(szName);
	std::map<DWORD, DWORD>::iterator it0;

	it0 = m_mapTypeObj.find(dwTypeID);	// 从对象中找
	if( it0 == m_mapTypeObj.end() )
		return  NULL;

	LPVOID pObj = NULL;
	DWORD dwID = it0->second;
	std::map<DWORD, tagObj>::iterator it;

	EnterCriticalSection(&m_cs);

	it = m_mapObj.find(dwID);	// 从对象中找
	if( it == m_mapObj.end() )
	{
		LeaveCriticalSection(&m_cs);
		return NULL;
	}

	if( it->second.pObj )
		pObj = it->second.pObj;
	else
	{
		// 对象还没有创建，准备创建
		it->second.pObj = m_AF.Create(dwID);
		pObj = it->second.pObj;
	}

	LeaveCriticalSection(&m_cs);
	return pObj;


}




//-----------------------------------------------------------------------------
// 根据注册过的对象创建出一个对象
//-----------------------------------------------------------------------------
BOOL ObjMgr::Create(LPCSTR szDestObjName, LPCSTR szSrcObjName)
{
	ASSERT(szSrcObjName);
	ASSERT(szDestObjName);
	ASSERT(CheckName(szDestObjName));	// 主要检查是否有重复对象

	EnterCriticalSection(&m_cs);

	tagObj objDesc;
	DWORD dwClassID = Crc32(szSrcObjName);
	objDesc.pObj = m_AF.Create(dwClassID);	// 创建对象
	if( !P_VALID(objDesc.pObj) )
	{
		LeaveCriticalSection(&m_cs);
		return FALSE;
	}
	objDesc.dwClassID = dwClassID;
	objDesc.strObjName.assign(szDestObjName);
	m_mapObj.insert( std::pair<DWORD, tagObj>(Crc32(szDestObjName), objDesc) );
	m_listObj.push_back(szDestObjName);

	LeaveCriticalSection(&m_cs);
	return TRUE;
}


//-----------------------------------------------------------------------------
// 析构对象
//-----------------------------------------------------------------------------
BOOL ObjMgr::Kill(LPCSTR szObjName)
{
	ASSERT(szObjName);
	DWORD dwID = Crc32(szObjName);
	std::map<DWORD, tagObj>::iterator it;
	std::list<std::string>::iterator itList;

	EnterCriticalSection(&m_cs);
	it = m_mapObj.find(dwID);	// 从对象中找
	if( it == m_mapObj.end() )
	{
		ASSERT(0);
		LeaveCriticalSection(&m_cs);
		return FALSE;	// 最终找不到
	}

	LPVOID pObj = it->second.pObj;
	DWORD dwClassID = it->second.dwClassID;
	
	// 先从物件列表中删除，避免Destory过程中又从m_mapObj查找到自己
	m_mapObj.erase( it );
	// 同时从lis中删除
	for(itList = m_listObj.begin(); itList!=m_listObj.end(); itList++)
	{
		if((*itList) == szObjName)
		{
			m_listObj.erase(itList);
			break;
		}
	}
	
	if( pObj )
		m_AF.Destroy(pObj, dwClassID);
		
	LeaveCriticalSection(&m_cs);
	return TRUE;
}


//-----------------------------------------------------------------------------
// 检查类名字的合法性
//-----------------------------------------------------------------------------
BOOL ObjMgr::CheckName(LPCSTR szName)
{
	ASSERT( strlen(szName) < 32 ); // name too long
	DWORD dwID = Crc32(szName);
	std::map<DWORD, tagObj>::iterator it;
	
	EnterCriticalSection(&m_cs);
	it = m_mapObj.find(dwID);
	ASSERT( it == m_mapObj.end() );	// repeated ID
	LeaveCriticalSection(&m_cs);
	
	return TRUE;
}


//-----------------------------------------------------------------------------
// 因为我们本身是Interface类，所以为了不获得CUtil，我们自己实现了一套CRC32
// 计算32位CRC,比一般算法快速很多,
//-----------------------------------------------------------------------------
DWORD ObjMgr::Crc32(LPCSTR lpData)
{
	UINT unResult;

	//对ASC超过127的字符， 必须用unsigned char, 否则会出错
	LPBYTE pData = (LPBYTE)lpData;
	if( 0 == pData[0] ) 
		return (DWORD)GT_INVALID;

	unResult = *pData++ << 24;
	if( *pData )
	{
		unResult |= *pData++ << 16;
		if( *pData )
		{
			unResult |= *pData++ << 8;
			if( *pData ) unResult |= *pData++;
		}
	}
	unResult = ~ unResult;

	while( *pData )
	{
		unResult = (unResult << 8 | *pData) ^ globe_Crc32Table[unResult >> 24];
		pData++;
	}

	return ~unResult;
}

}