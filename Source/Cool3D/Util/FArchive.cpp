#include "StdAfx.h"
#include ".\farchive.h"
#include "..\Kernel.h"

namespace Cool3D
{
	FArchive::FArchive(void)
	{
		m_mode=-1;
		m_pWriteFile=NULL;
		m_pFS=NULL;
		m_hReadFile=NULL;
	}

	FArchive::~FArchive(void)
	{
		Close();
	}

	void FArchive::OpenForWrite(const TCHAR* szFileName)
	{
		Close();
		//--防止外部调用SetCurrentDirector改变工作路径,每次打开文件都设置一次
		//注意:MFC的CFileDialog会自动改变系统的当前路径.
		BOOL workDirOK=::SetCurrentDirectory(Kernel::Inst()->GetWorkPath());
		ASSERT(workDirOK);
		//--
		m_pWriteFile=NULL;
		if(_tfopen_s(&m_pWriteFile,szFileName,_T("wb"))!=0)
			THROW_NULLEX(NULL,_T("Archive open for write failed."),szFileName);
		m_mode=1;
	}

	void FArchive::OpenForRead(IFS* pFS,const TCHAR* szFileName)
	{
		Close();
		ASSERT(pFS!=NULL);
		
		THROW_NULL(pFS,_T("Bad file system."));

		m_hReadFile=pFS->Open(szFileName);
		m_pFS=pFS;
		THROW_NULLEX(m_hReadFile,_T("Archive open for read failed."),szFileName);
		m_mode=0;
	}

	void FArchive::Close()
	{
		switch(m_mode)
		{
		case 0:
			if(m_pFS!=NULL && m_hReadFile!=NULL)
				m_pFS->Close(m_hReadFile);
			m_pFS=NULL;
			m_hReadFile=NULL;
			break;
		case 1:
			if(m_pWriteFile!=NULL)
				fclose(m_pWriteFile);
			m_pWriteFile=NULL;
		}//endof switch
	}
}//namespace Cool3D