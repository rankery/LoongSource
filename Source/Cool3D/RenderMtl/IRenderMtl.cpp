#include "StdAfx.h"
#include ".\irendermtl.h"

namespace Cool3D
{
	IMP_RTTI(IRenderMtl,IMaterial);

	IRenderMtl::IRenderMtl(void)
	{
		m_bEnableLight=true;
		m_bTwoSide=false;
	}

	IRenderMtl::~IRenderMtl(void)
	{
	}

	void IRenderMtl::Serialize(FArchive& ar)
	{
		ar<<m_bEnableLight;
		ar<<m_bTwoSide;

		IMaterial::Serialize(ar);
	}

	void IRenderMtl::Deserialize(FArchive& ar)
	{
		ar>>m_bEnableLight;
		ar>>m_bTwoSide;

		IMaterial::Deserialize(ar);
	}
}//namespace Cool3D