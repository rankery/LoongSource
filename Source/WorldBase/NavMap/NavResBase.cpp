#include "StdAfx.h"
#include "NavResBase.h"
#include "NavAsyncResIO.h"
#include "NavResMgr.h"

namespace WorldBase
{
	IMP_RTTI(NavResBase,RefObj);

	NavResBase::NavResBase(const TCHAR* szName):m_szName(szName)
	{
		m_pFS=NULL;
		m_bCreated=0;
		m_size=0;
	}

	NavResBase::~NavResBase(void)
	{
		m_pFS=NULL;
	}

	IFS	*NavResBase::GetSafeFS()
	{
		if(m_pFS==NULL)
			return NavResMgr::Inst()->DefaultFS();
		return m_pFS;
	}

	UINT NavResBase::CreateContent(DWORD param)
	{
		m_size=GetSafeFS()->GetSize(GetName());
		InterlockedIncrement(&m_bCreated);
		return m_size;
	}	

	void NavResBase::CreateContentAsync(DWORD param)
	{
		NavAsyncResIO::Inst()->QueueRequest(EResIO_Load,this,param);
	}
}//namespace WorldBase