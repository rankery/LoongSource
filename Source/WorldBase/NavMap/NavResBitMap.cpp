#include "StdAfx.h"
#include "NavResBitMap.h"

namespace WorldBase
{
	IMP_RTTI(NavResBitMap,NavResBase);

	NavResBitMap::NavResBitMap(const TCHAR* szName): NavResBase(szName)
	{
		m_pBitMap=NULL;
	}

	NavResBitMap::~NavResBitMap(void)
	{
		SAFE_DELETE(m_pBitMap);
	}

	UINT NavResBitMap::CreateContent( DWORD param )
	{
		LoadFromFile(GetSafeFS(),GetName());

		return NavResBase::CreateContent(param);
	}

	void NavResBitMap::LoadFromFile(IFS *pFS,const TCHAR* szName)
	{
		ASSERT(pFS!=NULL);
		ASSERT(szName!=NULL);
		ASSERT(_tcslen(szName)>0);

		DWORD hFile=pFS->Open(szName);
		THROW_NULLEX(hFile,_T("NavResBitMap open file failed."),szName);

		SAFE_DELETE(m_pBitMap);
		m_pBitMap=new BitMap;
		m_pBitMap->ReadFile(pFS,hFile);

		pFS->Close(hFile);
	}
}//namespace WorldBase