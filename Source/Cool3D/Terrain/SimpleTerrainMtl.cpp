#include "StdAfx.h"
#include ".\SimpleTerrainMtl.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(SimpleTerrainMtl,IMaterial);

	SimpleTerrainMtl::SimpleTerrainMtl(void)
	{
		m_color=Color4ub(255,255,255,255);
	}

	SimpleTerrainMtl::~SimpleTerrainMtl(void)
	{
	}

	void SimpleTerrainMtl::Serialize(FArchive& ar)
	{
		ASSERT(0 && _T("not implement yet"));
		IMaterial::Serialize(ar);
	}

	void SimpleTerrainMtl::Deserialize(FArchive& ar)
	{
		ASSERT(0 && _T("not implement yet"));
		IMaterial::Deserialize(ar);
	}

	bool SimpleTerrainMtl::IsResourceReady() const
	{
		return true;
	}

}//namespace Cool3D