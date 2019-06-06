#pragma once

#include "CPKCore/CPKVfs.h"
class CTSMemMgr;
#include "MemMgr/ts_memmgr.h"

//解析手动更新文件，读出文件包含的cpk文件的信息
class CExtractHanle : public CPKVfs
{
public:
	CExtractHanle(CTSMemMgr*);

public:

	//加载手动更新文件，读取合并在文件里的cpk文件的头信息
	BOOL LoadExtractFile( const char* szFileName );

	//检测版本是否在次手动更新程序的范围内
	BOOL IsVersionCorrect( std::string& strVersion );

	// 去掉tstring首尾的空格
	VOID CutSpaceOfStringHeadAndTail(std::string& string);

	const std::string& GetVersionDest() { return m_strVersionDest; }

private:

	BOOL	ReadCPkHeader();

	DWORD	m_dwCPKStartAddress;			//cpk文件的起始地址

	std::string m_strVersionSrc;
	std::string m_strVersionDest;
};
