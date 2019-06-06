#pragma once
#include "imaterial.h"
#include "MConstColor.h"
#include "MBitmap.h"

namespace Cool3D
{
	class MMAlphaMask;
	//处理换装的：面部细节贴图
	class Cool3D_API MTwoLayers :	public IMaterial
	{
	public:
		MTwoLayers(void);
		virtual ~MTwoLayers(void);

		//--IMaterial
		virtual bool IsResourceReady() const;
		virtual DWORD GetRenderTrait1() const
		{
			return IMaterial::GetRenderTrait1();
		}
		virtual DWORD GetRenderTrait2() const
		{
			if(m_pLayer0)
                return m_pLayer0->GetRenderTrait2();
			else
				return IMaterial::GetRenderTrait2();
		}

		//--ISerializable
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);

		//--
		MMAlphaMask* GetAlphaMask();

	public:
		MConstColor*	m_pColor;
		MBitmap*		m_pLayer0;
		MBitmap*		m_pLayer1;

		DECL_RTTI_DYNC(MTwoLayers);
	};

}//namespace Cool3D