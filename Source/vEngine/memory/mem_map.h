//-----------------------------------------------------------------------------
//!\file mem_map.h
//!\author Lyp
//!
//!\date 2004-06-17
//! last 2009-03-05
//!
//!\brief 内存分析
//-----------------------------------------------------------------------------
#pragma once

#ifdef _MEM_TRACE_

#undef new
#undef malloc
#undef calloc
#undef realloc
#undef free

namespace vEngine {
//-----------------------------------------------------------------------------
//!\brief 内存分析
//! 需要查找内存泄露调用堆栈时，可以先在Alloc函数设置一个断点，再启动程序，
//!	当程序断下来后，将m_dwBreakAddress修改为内存泄露地址，然后取消断点，继续
//! 运行程序。当发生设定地址的内存分配时，Alloc会自动断下来
//-----------------------------------------------------------------------------
class VENGINE_API MemMap
{
public:
	LPVOID Alloc(LPCSTR szFile, INT nLine, size_t uiSize, BOOL bArray);
	VOID Free(LPVOID pAddress, BOOL bArray);
	VOID WriteFile();

	MemMap();
	~MemMap();

private:
	struct tagMemNode
	{
		LPVOID	pAddress;		// 分配地址
		LPCSTR	szFile;			// 文件名
		INT		nLine;			// 行号
		size_t	uiSize;			// 分配大小
		bool	bArray;			// 是否数组请求
		bool	bArrayError;	// 数组释放错误
		bool	bBoundError;	// 越界操作
		bool	bReserved;		// 为了对齐而保留
	};

	VOLATILE BOOL					m_bTerminate;
	TSafeMap<LPVOID, tagMemNode*>	m_mapMem;
	TSafeList<tagMemNode*>			m_listArrayErrorNode;
	TSafeList<tagMemNode*>			m_listBoundErrorNode;

	DWORD m_dwBreakAddress;
};

} // namespace vEngine {
VOID VENGINE_API WriteMem();

LPVOID VENGINE_API AllocateMemory(size_t uiSize, LPCSTR szFile, INT nLine, BOOL bArray, LPVOID pAddress=NULL); 
VOID VENGINE_API DeallocateMemory(LPVOID pAddress, BOOL bArray);

inline LPVOID operator new(size_t uiSize, LPCSTR szFile, INT nLine) { return AllocateMemory(uiSize, szFile, nLine, FALSE); }
inline LPVOID operator new[](size_t uiSize, LPCSTR szFile, INT nLine) { return AllocateMemory(uiSize, szFile, nLine, TRUE); }
inline VOID operator delete(LPVOID pAddress) { DeallocateMemory(pAddress, FALSE); }
inline VOID operator delete[](LPVOID pAddress) { DeallocateMemory(pAddress, TRUE); }

// These two routines should never get called, unless an error occures during the 
// allocation process.  These need to be defined to make Visual C++ happy.
inline VOID operator delete(LPVOID pAddress, LPCSTR szFile, INT nLine) { szFile;nLine;DeallocateMemory(pAddress, FALSE); }
inline VOID operator delete[](LPVOID pAddress, LPCSTR szFile, INT nLine) { szFile;nLine;DeallocateMemory(pAddress, TRUE); }

#define new							new(__FILE__, __LINE__)
#define malloc(uiSize)				AllocateMemory(uiSize, __FILE__, __LINE__,  FALSE)
#define calloc(uiNum, uiSize)		AllocateMemory(((uiSize)*(uiNum)), __FILE__, __LINE__, FALSE)
#define realloc(pAddress, uiSize)	AllocateMemory(uiSize, __FILE__, __LINE__, FALSE, pAddress)
#define free(pAddress)				DeallocateMemory(pAddress, FALSE)


#endif // #ifdef _MEM_TRACE_