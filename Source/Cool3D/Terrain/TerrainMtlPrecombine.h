#pragma once
#include "..\RenderMtl\imaterial.h"
#include "..\RenderMtl\MBitmap.h"
#include "..\rendermtl\MConstColor.h"

namespace Cool3D
{
	class ResPixelShader;
	/** \class TerrainMtlPrecombine
		\brief 地形预混合贴图材质
	*/
	class TerrainMtlPrecombine :	public IMaterial
	{
	public:
		TerrainMtlPrecombine(void);
		virtual ~TerrainMtlPrecombine(void);

		//--IMaterial
		virtual bool IsResourceReady() const;
		virtual DWORD GetRenderTrait1() const
		{
			return IMaterial::GetRenderTrait1();
		}
		virtual DWORD GetRenderTrait2() const
		{
			if(m_pPrecombineTex)
                return m_pPrecombineTex->GetRenderTrait2();
			else
				return IMaterial::GetRenderTrait2();
		}

		//--ISerializable
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);

		//--
		void LoadShader();

	public:
		MConstColor		m_color;
		MBitmap*		m_pPrecombineTex;
		ResPixelShader* m_pPS;

		DECL_RTTI_DYNC(TerrainMtlPrecombine);
	};

}//namespace Cool3D