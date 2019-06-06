#include "StdAfx.h"
#include ".\stdfs.h"
#include "..\Util\Exception.h"
#include "..\Kernel.h"

#pragma warning(disable:4311)	//从“FILE *”到“DWORD”的指针截断
#pragma warning(disable:4312)	//从“DWORD”转换到更大的“FILE *”
#pragma warning(disable:4267)	//从“size_t”转换到“DWORD”，可能丢失数据


namespace Cool3D
{
	StdFS::StdFS(void)
	{
	}

	StdFS::~StdFS(void)
	{
	}

	BOOL StdFS::SetWorkingPackage(LPCTSTR szPackage)
	{
		return TRUE;
	}

	BOOL StdFS::HasWorkingPackage()
	{
		return TRUE;
	}

	DWORD	StdFS::Open(LPCTSTR szFileName)
	{
		//--防止外部调用SetCurrentDirector改变工作路径,每次打开文件都设置一次
		//注意:MFC的CFileDialog会自动改变系统的当前路径.
		BOOL workDirOK=::SetCurrentDirectory(Kernel::Inst()->GetWorkPath());
		ASSERT(workDirOK);

		//--
		tstring szFlag=_T("rb");
		FILE *file=_tfopen(szFileName,szFlag.c_str());
		return (DWORD)(file);
	}

	BOOL	StdFS::Close(DWORD dwHandle)
	{
		FILE *file=(FILE*)(dwHandle);
		int ret=fclose(file);
		return EOF!=ret;
	}

	DWORD	StdFS::Read(PVOID lpMemOut, DWORD dwSize, DWORD dwHandle)
	{
		FILE *file=(FILE*)(dwHandle);
		size_t readsize=fread(lpMemOut,dwSize,1,file);
		return readsize*dwSize;
	}

	DWORD	StdFS::Seek(DWORD dwHandle, INT nOffset, DWORD dwOrigin)
	{
		int orgn=0;
		switch(dwOrigin)
		{
		case FILE_CURRENT:
			orgn=SEEK_CUR;
			break;
		case FILE_END:
			orgn=SEEK_END;
			break;
		case FILE_BEGIN:
			orgn=SEEK_SET;
			break;
		default:
			ASSERT(0);
		}
		FILE *file=(FILE*)(dwHandle);
		DWORD ret=fseek(file,nOffset,orgn);
		return ret;
	}

	DWORD	StdFS::Tell(DWORD dwHandle)
	{
		FILE *file=(FILE*)(dwHandle);
		return ftell(file);
	}

	DWORD	StdFS::GetSize(LPCTSTR szFileName)
	{
		BOOL workDirOK=::SetCurrentDirectory(Kernel::Inst()->GetWorkPath());

		FILE *file=_tfopen(szFileName,_T("rb"));
		if(file == NULL)
			return -1;
		fseek(file,0,SEEK_END);
		DWORD size=ftell(file);
		fclose(file);
		return size;
	}

	DWORD	StdFS::LoadFile(LPVOID lpMemOut, LPCTSTR szFileName)
	{
		BOOL workDirOK=::SetCurrentDirectory(Kernel::Inst()->GetWorkPath());

		FILE *file=_tfopen(szFileName,_T("rb"));
		if(file == NULL)
			return 0;
		fseek(file,0,SEEK_END);
		DWORD size=ftell(file);
		fseek(file,0,SEEK_SET);
		DWORD read=fread(lpMemOut,size,1,file);
		fclose(file);
		return read;
	}

}//namespace Cool3D