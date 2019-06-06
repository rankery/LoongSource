#include "StdAfx.h"
#include "NavResHeightmap.h"
#include "NavHeightmap.h"

namespace WorldBase
{
	IMP_RTTI(NavResHeightmap,NavResBase);

	NavResHeightmap::NavResHeightmap(const TCHAR* szName): NavResBase(szName)
	{
		m_pHMap=NULL;
	}

	NavResHeightmap::~NavResHeightmap(void)
	{
		SAFE_DELETE(m_pHMap);
	}

	UINT NavResHeightmap::CreateContent( DWORD param )
	{
		LoadFromFile(GetSafeFS(),GetName());

		return NavResBase::CreateContent(param);
	}

	void NavResHeightmap::LoadFromFile(IFS *pFS,const TCHAR* szName)
	{
		ASSERT(pFS!=NULL);
		ASSERT(szName!=NULL);
		ASSERT(_tcslen(szName)>0);

		DWORD hFile=pFS->Open(szName);
		THROW_NULLEX(hFile,_T("NavResHeightmap open file failed."),szName);

		SAFE_DELETE(m_pHMap);
		m_pHMap=new NavHeightmap;
		m_pHMap->ReadFromFile(pFS,hFile);

		pFS->Close(hFile);
	}
}//namespace WorldBase