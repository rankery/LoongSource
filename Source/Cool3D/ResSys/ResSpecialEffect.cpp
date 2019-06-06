#include "StdAfx.h"
#include ".\resspecialeffect.h"
#include "..\Util\Exception.h"
#include ".\ResMgr.h"
#include "..\SpecialEffect\SGEffectNode.h"

namespace Cool3D
{
	IMP_RTTI(ResSpecialEffect,ResBase);

	ResSpecialEffect::ResSpecialEffect(const TCHAR* szName) : ResBase(szName)
	{
	}

	ResSpecialEffect::~ResSpecialEffect(void)
	{
	}

	UINT ResSpecialEffect::CreateContent(DWORD param)
	{
		const TCHAR* szName=GetName();

		if(m_memFS.ReadFile(GetSafeFS(),szName)==false)
			THROW_NULL(0,_T("open special effect file failed"));

		return ResBase::CreateContent(param);
	}

	void ResSpecialEffect::DeserializeEffectNode(SGEffectNode *pEffectNode)
	{
		FArchive ar;
		ar.OpenForRead(&m_memFS,GetName());
		pEffectNode->Deserialize(ar);
		ar.Close();
	}

}//namespace Cool3D

