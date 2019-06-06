#include "StdAfx.h"
#include ".\resbase.h"
#include "..\Kernel.h"
#include "..\Util\Exception.h"
#include "AsyncResIO.h"
#include "..\VFS\IFS.h"


namespace Cool3D
{
	IMP_RTTI(ResBase,RefObj);

	ResBase::ResBase(const TCHAR* szName):m_szName(szName)
	{
		m_pFS=NULL;
		m_bCreated=0;
		m_size=0;
	}

	ResBase::~ResBase(void)
	{
		m_pFS=NULL;
	}
	
	IFS	*ResBase::GetSafeFS()
	{
		if(m_pFS==NULL)
			return Kernel::Inst()->DefaultFS();
		return m_pFS;
	}

	UINT ResBase::CreateContent(DWORD param)
	{
		m_size=GetSafeFS()->GetSize(GetName());
		InterlockedIncrement(&m_bCreated);
		return m_size;
	}	
	
	void ResBase::CreateContentAsync(DWORD param)
	{
		AsyncResIO::Inst()->QueueRequest(EResIO_Load,this,param);
	}
}//namespace Cool3D