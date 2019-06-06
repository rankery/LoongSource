//-----------------------------------------------------------------------------
// File: CPKFast.h
// Desc: game resource file system 1.0
// Time: 2003-1-2
// Auth: CTCRST
//
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine{

//-----------------------------------------------------------------------------
/*	CPK 文件包装设计目标：
 1.	快速
 2.	兼容多种压缩方法
 3.	目录式访问
 4.	方便地添加删除单个文件、目录

	设计观念
 1.	用表项来记录单个文件的信息，用CRC(通过文件名计算)标志来区分文件；
 2.	CRC是按照文件的全名(即包括完整路径)计算的；
 3.	将简单文件名(不包括路径)以及ExtraInfo保存在各文件具体信息之后；
 4.	一开始就分配一些表项，添加文件时就可以不需要移动所有文件信息；
 5.	表项用完之后再分配一定量表项；
 6.	删除文件时只将表项标记为无效；
 7.	添加文件时，寻找空闲表项以及空闲文件空间插入，如果没有合适的空闲
    文件空间，则添加到包文件末尾；
 8.	在项目进行到一定阶段时，可以对包进行一次碎片整理，可以排除掉由于
	经常添加、删除、修改文件造成的空间碎片，而且还可以对表项进行一次
	排序整理，以便加快文件查找速度；
 9.	采用过滤器的思想，压缩、解压、加密、解密、凡是文件格式的转换都可
	以看作是过滤，只要有通用的接口，就可以与很多种压缩方式兼容；
 11.如果从包里错删文件，还可以尝试进行恢复
 12.对于大于8MB的文件，因为其一般都是动画或音乐，所以不采用任何压缩方
	式，直接存在包中。
 13.支持嵌套包含，cpk文件里可以包含cpk文件，支持解析
*/
//-----------------------------------------------------------------------------
// 快速单包访问
//-----------------------------------------------------------------------------
class CPK:public Package
{
public:
	// 针对包的操作
	BOOL Load(LPCTSTR szFileName);
	BOOL Unload();

	// 针对包内单个文件的操作
	DWORD	Open(LPCTSTR szFileName);	// return handle
	BOOL	Close(DWORD dwHandle);
	DWORD	Read(PVOID lpMemOut, DWORD dwSize, DWORD dwHandle);
	DWORD	Seek(DWORD dwHandle, INT nOffset, DWORD dwOrigin);
	DWORD	Tell(DWORD dwHandle);
	
	// 读包中单个文件到目标内存，返回原文件字节大小，
	// 与Open+Read相比因为直接读入指定内存，少了一次Memcpy
	// 如果内存目标lpMemOut为空，就仅返回原文件(未压缩时)大小
	DWORD	LoadFile(LPVOID lpMemOut, LPCTSTR szFileName);


	CPK();~CPK();

private:
	tagCPKHeader	m_CPKHeader;
	tstring			m_strCPKFileName;
	tagCPKTable*	m_pCPKTable;
	
	BOOL			m_bLoaded;	// 是否有CPK载入
	DWORD			m_dwAllocGranularity;
	DWORD			m_dwCPKHandle;
	DWORD			m_dwCPKMappingHandle;
	volatile INT	m_nOpenedFileNum;


	SafeMemPool*	m_pMemPool;

	// 根据文件名找到对应的表项
	INT GetTableIndex(LPCTSTR pFullName);
	INT GetTableIndexFromCRC(DWORD dwCRC);

	//文件映射方式打开单个文件
	DWORD OpenForMap(INT nTableIndex, LPCTSTR szFileName);
	//普通方式打开单个文件
	DWORD OpenForCommon(INT nTableIndex, LPCTSTR szFileName);

};







} // namespace vEngine {
