#pragma once
#include "farchive.h"

namespace Cool3D
{
	/**	\class FArchiveAdv 
		\brief 增加了取得内部文件指针的接口
		\remark 主要是因为外部程序可能需要对文件进行Seek操作
	*/
	class FArchiveAdv :	public FArchive
	{
	public:
		FArchiveAdv(void)			{}
		virtual ~FArchiveAdv(void)	{}

		FILE* GetWriteFile() const	{	return FArchive::m_pWriteFile;}
		DWORD GetReadFile() const	{	return FArchive::m_hReadFile;}
		IFS* GetReadFS() const		{	return FArchive::m_pFS;}
	};
}//namespace Cool3D