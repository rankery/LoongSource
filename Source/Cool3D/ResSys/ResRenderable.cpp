#include "StdAfx.h"
#include <stack>
#include ".\resrenderable.h"
#include "..\RenderMtl\IMaterial.h"
#include "..\RenderMtl\MGeneral.h"
#include "ResMgr.h"

namespace Cool3D
{
	IMP_RTTI(ResRenderable,ResBase);

	ResRenderable::ResRenderable(const TCHAR* szName):ResBase(szName)
	{
	}

	ResRenderable::~ResRenderable(void)
	{
	}

	void ResRenderable::CreatemtlArrayFromDesc(vector<IMaterial*>&	mtls,const vector<tagMtlDesc>& descs,UINT mipLevels,const TCHAR* szLightMap) const
	{
		mtls.clear();
		for(size_t i=0;i<descs.size();i++)
		{
			const tagMtlDesc& mtlDesc=descs[i];
			MGeneral *pNewMtl=new MGeneral;
			pNewMtl->CreateFromMaxExport(mtlDesc,mipLevels,szLightMap,1.0f);
			mtls.push_back(pNewMtl);
		}
	}
}//namespace Cool3D