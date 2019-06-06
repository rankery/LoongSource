//-----------------------------------------------------------------------------
// File: CPKCore.h
// Desc: cpk file format
// Time: 2005-5-31
// Auth: lyp,tidy
//
// Copyright (c) 2005 Softstar Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma	   once
#include "cpkdefine.h"

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

//我们规定所有路径(可含文件名)起始位置和结束位置不能附加'\\'否则视为无效!!!
//-----------------------------------------------------------------------------
// 快速单包访问
//-----------------------------------------------------------------------------
//#define GT_CPK_RAW_BUFFER_SIZE          5242880  //104857600

class CMD5;
class CTSMemMgr;
class CMemMgr;
//-------------------------------------------------------------------------------------

class CPKVfs
{
public:
	CPKVfs();
	~CPKVfs();

public:		//****初始化和结束接口******//

	// 创建新的管理包,如果存在就直接打开，不存在就创建并打开，其实包含了load功能
	BOOL	Create(LPCSTR pFileName);	
	//加载cpk文件，不存在就返回错误
	BOOL	Load(LPCSTR pFileName);
	//卸载cpk文件包
	BOOL	Unload();
	BOOL	IncreaseTable();	//增加表项空间

public:		//*****读取接口*****//

	// 针对包内单个文件的操作
	DWORD	Open(INT nTableIndex, DWORD dwLength = 0,  BOOL bCpk = false );
	DWORD	Open(LPCTSTR szFileName);	// return handle
	BOOL	Close(DWORD dwHandle);
	DWORD	Read(PVOID lpMemOut, DWORD dwSize, DWORD dwHandle);
	DWORD	Seek(DWORD dwHandle, INT nOffset, DWORD dwOrigin);
	DWORD	Tell(DWORD dwHandle);

public:		//*****写入(添加,删除等编辑操作)辅助接口*****//

	//添加文件或路径,我们约定,当该文件已经存在时，覆盖他
	BOOL AddFile(LPCSTR pFullName, DWORD dwFatherCRC);
	BOOL AddFile(LPCSTR pFullName, DWORD dwFatherCRC, tagCPKTable *pTable, LPVOID p);
	// 在包里指定目录创建一个目录, 返回CRC
	DWORD CreateDir(LPCSTR pSimpleDirName, DWORD dwFatherCRC);
	//这种方式不添加文件，只添加他的路径，根路径作为dwFatherCrc的子目录．为其它操作做准备
	DWORD AddDirCyc(LPCSTR pFullName, DWORD dwFatherCRC);
	// 删除包里的文件或者目录，使用完整路径
	BOOL  RemoveFile(DWORD dwCRC);
	// 删除文件或目录，会删除其下所有子项
	BOOL	RemoveFileOrDir(INT nTableIndex);
	//*********初始化
	// 往包里的当前目录添加文件,这种方式比较特殊，它本身是从另外一个ｃｐｋ包里转移过来
	//因此我们坚持一个原则:对文件内容不压缩(也许他已经压缩过),扩展信息不变
	//*********初始化
	INT InitFileCPKToCPK(LPCSTR pFullName, DWORD dwFatherCRC, tagCPKTable *pTable);
	//*********写入数据
	// 往包里的当前目录添加文件,这种方式比较特殊，它本身是从另外一个ｃｐｋ包里转移过来
	//因此我们坚持一个原则:对文件内容不压缩(也许他已经压缩过),扩展信息不变
	//*********写入数据
	BOOL WriteFileCPKToCPK(INT nTableIndex, DWORD dwWritePos, LPVOID p, DWORD dwDataSize);
	//*********结束添加文件，写入表和头部
	// 往包里的当前目录添加文件,这种方式比较特殊，它本身是从另外一个ｃｐｋ包里转移过来
	//因此我们坚持一个原则:对文件内容不压缩(也许他已经压缩过),扩展信息不变
	//*********结束添加文件，写入表和头部
	BOOL FinishFileCPKToCPK(INT nTableIndex);


public:		//*****辅助相关接口*****//

	// 检查是否是合法的CPK文件包
	BOOL IsValidCPK(LPCSTR pFileName);
	//得到相关信息
	tagCPKHeader *GetCPKHeader() { return &m_CPKHeader; }
	tagCPKTable	 *GetCPKTable()  { return m_pCPKTable; }
	DWORD GetCPKFileHanlde() { return m_dwCPKFileHandle; }

	//从crc得到对应的表项
	INT		GetTableIndexFromeCrc(DWORD dwCrc);
	//得到全路径文件名,通过crc
	BOOL	GetFullNameFromCRC(DWORD dwCRC, LPSTR szFullName);
	// 由CRC标识得到简单文件名
	BOOL	GetSimpleFileNameFromCRC(LPSTR pSimpleFileName, DWORD dwCRC);
	// 由磁盘全名转为包中文件名，必须知道父CRC, 并全部转为小写
	BOOL	ConvertFullNameToCPKFullName(LPSTR pOutName, LPCSTR pInName, DWORD dwFatherCRC);
	// 连接简单文件名或目录名到目标目录名
	BOOL	LinkSimpleNameToDirName(LPSTR pFullName, LPCSTR pSimpleName);
	// 得到包中文件的扩展名(带"."号)
	BOOL	GetFileExtName(LPTSTR pExtName, INT nTableIndex);
	// 检测该文件名是否表示解包之后要打入cpk包，比如: data.cpk\system\gongfu.exe,
	// 外部必须确保是文件,而不是目录，比如,data.cpk，这里无从知道他是否目录
	BOOL	IsFileForCPK(LPCSTR szFullName, LPCSTR szCPKName, LPCSTR szFileName);

	// 得到包中指定目录下的所有一级子文件
	BOOL GetFirstLevelChild(DWORD dwDirCRC, DWORD& dwChildNum, DWORD* pdwCRCArray=NULL);
	// 得到包中指定目录下的所有一级子目录
	BOOL GetFirstLevelChildDir(DWORD dwDirCRC, DWORD& dwChildNum, DWORD* pdwCRCArray=NULL);
	// 由CRC标识得到对象是否是目录
	BOOL IsDir(DWORD dwCRC);

private:		//*****私有辅助接口*****//

	// 在Load Package时调用一次以便得到所有碎片信息
	INT		GetAllFragment();
	//获取系统信息
	DWORD	GetAllocationGranularity();
	// 检查是否包中有与所给CRC重复的CRC
	BOOL	HaveRepeatCRC(DWORD dwCRC);
	// 计算附加信息长度
	DWORD	CalFileExtraInfoSize(LPCSTR pFileName);
	// 根据所给的CRC得到空闲的Table项
	INT		GetFreeTableItem(DWORD dwCRC);
	// 得到CPK中可以放指定大小内容的空间起始点
	DWORD	FindCPKFreePlace(DWORD dwSize);
	// 将文件头写入磁盘包
	BOOL	WriteHeaderToDisc();
	// 将文件表写入磁盘包
	BOOL	WriteTableToDisc();
	// 向文件中插入数据
	BOOL	AddRealDataToCPK(INT nTableIndex, LPCSTR pFileName,  VOID* pSrc, DWORD dwSrcFileSize);
	// 由任意文件名得到简单文件名(不带路径)
	BOOL	GetSimpleFileName(LPSTR pSimpleName, LPCSTR pFullName);
	// 是否是不可压缩文件  mp3/bik
	BOOL	IsUnCompressable(LPCSTR pFullName);

private:
	CHAR		m_szCPKFileName[MAX_PATH];		//该cpk文件名

	BOOL		m_bLoaded;	// 是否有CPK载入
	DWORD		m_dwCPKFileHandle;		//cpk文件句柄
	DWORD		m_dwCPKMappingHandle;

	//相关文件内容
	tagCPKHeader m_CPKHeader;	// 文件头
	tagCPKTable *m_pCPKTable;	//表项空间
	DWORD	m_dwMaxCPKTable;	//申请的表项空间

	std::map< DWORD, DWORD >	m_mapValidTable;		//关键词crc，存储有效的table
	INT*		m_pFragment;	// 记录碎片所属的Table项
	INT*		m_pIndexToFragment;// 记录Table所属的碎片项

	//系统相关信息
	DWORD		m_dwAllocGranularity;

	CTSMemMgr*	m_pTSMemMgr;			//内存池,有外部传入
	CMemMgr*	m_pMemMgr;

	Util*		m_pUtil;

};

