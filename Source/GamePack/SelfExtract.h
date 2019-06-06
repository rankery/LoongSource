#pragma once
#include "./CPKCore/CPKCore.h"

//合并解压程序和更新包形成一个文件----手动补丁包
//文件的格式，分为3部分，自解压程序+更新包文件+尾信息（记录更新包文件开始的地址和此更新包的版本号）
class CSelfExtract :public CPK
{
public:
	//创建更新文件，如果bAddExe= true，加载自解压程序写在文件的开头，
	//否则调用基类的函数创建自动更新文件（.cpk）
	BOOL	CreateSaveFile( const TCHAR* szFileName, BOOL bAddExe );

	//写入尾部信息，记录更新包文件开始的地址和此更新包的版本号
	BOOL	WriteTail( CString& strVersionSrc, CString& strVersionDest );


protected:

	//写入头文件信息
	BOOL	WriteHeader();
};
