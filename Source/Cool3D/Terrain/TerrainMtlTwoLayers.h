#pragma once
#include "..\RenderMtl\imaterial.h"
#include "..\RenderMtl\MConstColor.h"
#include "..\RenderMtl\MBitmap.h"

namespace Cool3D
{
	/** \class TerrainMtlTwoLayers
		\brief Ë«²ãÌùÍ¼µØÐÎ²ÄÖÊ
		\remarks µ×²ãÎªÔ¤»ìºÏÌùÍ¼,ÉÏ²ãÎªÏ¸½ÚÎÆÀí
	*/
	class TerrainMtlTwoLayers :	public IMaterial
	{
	public:
		TerrainMtlTwoLayers(void);
		virtual ~TerrainMtlTwoLayers(void);

		//--IMaterial
		virtual bool IsResourceReady() const;
		virtual DWORD GetRenderTrait1() const
		{
			return IMaterial::GetRenderTrait1();
		}
		virtual DWORD GetRenderTrait2() const
		{
			if(m_pCombineTex)
                return m_pCombineTex->GetRenderTrait2();
			else
				return IMaterial::GetRenderTrait2();
		}

		//--ISerializable
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);

	public:
		MConstColor		m_color;
		MBitmap*		m_pDetailTex;
		MBitmap*		m_pCombineTex;

		DECL_RTTI_DYNC(TerrainMtlTwoLayers);
	};

}//namespace Cool3D