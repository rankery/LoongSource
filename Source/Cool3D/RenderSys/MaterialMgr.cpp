#include "StdAfx.h"
#include ".\materialmgr.h"
#include "Material.h"

namespace F3D
{
	MaterialMgr::MaterialMgr(void)
	{
	}

	MaterialMgr::~MaterialMgr(void)
	{
	}

	MaterialMgr *MaterialMgr::Inst()
	{
		static MaterialMgr g_mtlMgr;
		return &g_mtlMgr;
	}


	Material *MaterialMgr::CreateMtl(tagMtlDesc *pDesc)
	{
		return new Material(*pDesc);
	}

	void MaterialMgr::DestroyMtl(Material *pMtl)
	{
		delete pMtl;
	}
}//namespace F3D