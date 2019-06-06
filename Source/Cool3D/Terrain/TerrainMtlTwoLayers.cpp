#include "StdAfx.h"
#include ".\TerrainMtlTwoLayers.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(TerrainMtlTwoLayers,IMaterial);

	TerrainMtlTwoLayers::TerrainMtlTwoLayers(void)
	{
		m_color.m_bEnableLight=true;
		m_color.m_bTwoSide=false;
		m_color.m_color.diffuse=Color4f(1.0f,1.0f,1.0f,1.0f);
		m_color.m_color.ambient=Color4f(1.0f,0.7f,0.7f,0.7f);
		m_color.m_color.power=8;
		m_color.m_color.specular=Color4f(1.0f,1.0f,1.0f,1.0f);

		m_pCombineTex = NULL;
		m_pDetailTex = NULL;
	}

	TerrainMtlTwoLayers::~TerrainMtlTwoLayers(void)
	{
		SAFE_DELETE(m_pDetailTex);
		SAFE_DELETE(m_pCombineTex);
	}

	bool TerrainMtlTwoLayers::IsResourceReady() const
	{
		return m_pDetailTex!=NULL && m_pDetailTex->IsResourceReady() 
			&& m_pCombineTex!=NULL && m_pCombineTex->IsResourceReady();
	}

	void TerrainMtlTwoLayers::Serialize(FArchive& ar)
	{
		IMaterial::Serialize(ar);
	}

	void TerrainMtlTwoLayers::Deserialize(FArchive& ar)
	{
		IMaterial::Deserialize(ar);
	}

}//namespace Cool3D