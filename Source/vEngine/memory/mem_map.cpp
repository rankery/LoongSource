//-----------------------------------------------------------------------------
//!\file mem_map.cpp
//!\author Lyp
//!
//!\date 2004-06-17
//! last 2008-02-15
//!
//!\brief 内存分析
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "mem_map.h"

#ifdef _MEM_TRACE_


#undef new
#undef malloc
#undef calloc
#undef realloc
#undef free

vEngine::MemMap g_MemMap;	// 全局对象，以便于程序结束后析构
//-----------------------------------------------------------------------------
//! AllocateMemory
//-----------------------------------------------------------------------------
LPVOID AllocateMemory(size_t uiSize, LPCSTR szFile, INT nLine, BOOL bArray, LPVOID pAddress)
{
	if( pAddress )
		DeallocateMemory(pAddress, FALSE);

	return g_MemMap.Alloc(szFile, nLine, uiSize, bArray);
}

//-----------------------------------------------------------------------------
//! DeallocateMemory
//-----------------------------------------------------------------------------
VOID DeallocateMemory(LPVOID pAddress, BOOL bArray)
{
	if( 0 == pAddress )
		return;  // ANSI states that delete will allow NULL pointers.

	g_MemMap.Free(pAddress, bArray);
}

VOID WriteMem()
{
	g_MemMap.WriteFile();

}

namespace vEngine {
//-----------------------------------------------------------------------------
//! construction
//-----------------------------------------------------------------------------
MemMap::MemMap()
{
	m_dwBreakAddress = GT_INVALID;
	m_bTerminate = FALSE;
}

//-----------------------------------------------------------------------------
//! destruction
//-----------------------------------------------------------------------------
MemMap::~MemMap()
{
	m_bTerminate = TRUE;
	WriteFile();	// 程序结束时写出log
}


//-----------------------------------------------------------------------------
// InsertMemNode
//-----------------------------------------------------------------------------
LPVOID MemMap::Alloc(LPCSTR szFile, INT nLine, size_t uiSize, BOOL bArray)
{
	LPVOID pAddress = NULL;
	size_t uiNewSize = uiSize + sizeof(DWORD) * 2;	// 头尾防溢出标志
	
	while( true )	// 尝试分配内存
	{
		pAddress = malloc(uiNewSize);
		if( pAddress )	// 分配成功
		{
			if( m_bTerminate )
				return pAddress;
			break;
		}

		// 分配不成功，找出当前出错处理函数
		new_handler fGlobalHandler = set_new_handler(0);
		set_new_handler(fGlobalHandler);

		if( fGlobalHandler )
			(*fGlobalHandler)();	// 调用出错处理
		else 
			throw std::bad_alloc();
	}

	// 分配成功后写入首尾防溢出符号
	*(DWORD*)pAddress = 0xDeadBeef;
	*(DWORD*)((BYTE*)pAddress + uiSize + sizeof(DWORD)) = 0xDeadBeef;

	// 将指针后移到外部能用的地址
	pAddress = (BYTE*)pAddress + sizeof(DWORD);
	
	tagMemNode* pMemNode = (tagMemNode*)malloc(sizeof(tagMemNode));
	pMemNode->pAddress = pAddress;
	pMemNode->szFile = szFile;
	pMemNode->nLine = nLine;
	pMemNode->uiSize = uiSize;
	pMemNode->bArray = (bArray == TRUE);
	pMemNode->bArrayError = false;
	pMemNode->bBoundError = false;

	ASSERT( !m_mapMem.IsExist(pAddress) );
	m_mapMem.Add(pAddress, pMemNode);


	if( GT_VALID(m_dwBreakAddress) && (DWORD)pAddress == m_dwBreakAddress )
	{
		__asm int 3;	// 断点
	}

	return pAddress;
}


//-----------------------------------------------------------------------------
// RemoveMemNode
//-----------------------------------------------------------------------------
VOID MemMap::Free(LPVOID pAddress, BOOL bArray)
{
	if( m_bTerminate )	// 防止对象析构后被继续调用
		return;

	tagMemNode* pNode = m_mapMem.Peek(pAddress);
	if( P_VALID(pNode) )
	{
		m_mapMem.Erase(pAddress);
		pAddress = (BYTE*)pAddress - sizeof(DWORD);	// 得到实际地址

		BOOL bStartChanged = (*(DWORD*)pAddress != 0xDeadBeef);
		BOOL bEndChanged = (*(DWORD*)((BYTE*)pAddress + pNode->uiSize + sizeof(DWORD)) != 0xDeadBeef);

		if( bStartChanged || bEndChanged )	// 溢出检测
		{
			pNode->bBoundError = true;
			m_listBoundErrorNode.PushBack(pNode);
		}
		else if( pNode->bArray != (bArray==TRUE) ) // 数组释放检测
		{
			pNode->bArrayError = true;
			m_listArrayErrorNode.PushBack(pNode);
		}
		else	// 无错
		{
			free(pNode);	// 释放节点内容
		}
	}

	free(pAddress);	// 释放实际内存
}


//-----------------------------------------------------------------------------
// Refresh
//-----------------------------------------------------------------------------
VOID MemMap::WriteFile()
{
	// 生成合适的文件名
	CHAR szFileName[MAX_PATH], szTime[MAX_PATH], szTemp[MAX_PATH];
	GetModuleFileNameA(NULL, szTemp, MAX_PATH);

	// 截去路径
	CHAR *pResult = strrchr(szTemp, '\\');
	pResult = pResult ?	pResult+1 :	pResult = (CHAR *)szTemp;
	sprintf(szFileName, "log\\%s", pResult);
	// 截去文件扩展名
	pResult = strrchr(szFileName, '.');
	if( pResult )
		*pResult = '\0';
	// 加上时间
	FILETIME CurrentTime;
	GetSystemTimeAsFileTime(&CurrentTime);
	szTime[0] = '\0';
	WORD wDate, wTime;
	if (FileTimeToLocalFileTime(&CurrentTime, &CurrentTime) &&
		FileTimeToDosDateTime(&CurrentTime, &wDate, &wTime))
	{
		// 年月日以及时分秒和进程id
		sprintf(szTime, "_Mem[%d-%d-%d %02d%02d%02d %05d].log",
			(wDate / 32) & 15, wDate & 31, (wDate / 512) + 1980,
			(wTime >> 11), (wTime >> 5) & 0x3F, (wTime & 0x1F) * 2, 
			GetCurrentProcessId());
		strcat(szFileName, szTime);
	}

	FILE *fp = fopen(szFileName, "wb"); // 清空原先的内容
	if( !fp )
		return;

	std::list<tagMemNode*> listNode;
	std::list<tagMemNode*>::iterator it;
	
	// 内存泄露
	fprintf( fp, "-----------------  Memory Leaks  -----------------\r\n");
	UINT uiTotalSize = 0;
	m_mapMem.ExportAllValue(listNode);
	for(it= listNode.begin(); it!=listNode.end(); ++it)
	{
		fprintf(fp, "%s, line:%d, size:%d, address:0x%X\r\n", (*it)->szFile, (*it)->nLine, (*it)->uiSize, (*it)->pAddress);
		uiTotalSize += (*it)->uiSize;
	}
	fprintf( fp, "Total leak size:%d\r\n\r\n", uiTotalSize);


	// 释放类型错误
	listNode.clear();
	fprintf( fp, "-----------------  Free Array Error  -----------------\r\n");
	m_listArrayErrorNode.Export(listNode);
	for(it= listNode.begin(); it!=listNode.end(); ++it)
	{
		fprintf( fp, "%s, line:%d, size:%d\r\n", (*it)->szFile, (*it)->nLine, (*it)->uiSize);
	}

	// 越界错误
	listNode.clear();
	fprintf( fp, "-----------------  Bound Error  -----------------\r\n");
	m_listBoundErrorNode.Export(listNode);
	for(it= listNode.begin(); it!=listNode.end(); ++it)
	{
		fprintf( fp, "%s, line:%d, size:%d\r\n", (*it)->szFile, (*it)->nLine, (*it)->uiSize);
	}

	fclose( fp );
}


} // namespace vEngine {
#endif // #ifdef _MEM_TRACE_