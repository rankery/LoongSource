#pragma once
#include "irendermtl.h"
#include "MBitmap.h"
#include "MRenderTex.h"
#include "..\Math\Transform.h"
#include "..\RenderMtl\MConstColor.h"

namespace Cool3D
{
	class ResPixelShader;
	class ResVertShader;

	/**	\class MWaterReflect
		\brief 水面折射效果材质
	*/	
	class Cool3D_API MWaterRefract :	public IMaterial
	{
	public:
		MWaterRefract(void);
		virtual ~MWaterRefract(void);

		//--IMaterial
		virtual bool IsResourceReady() const;
		virtual DWORD GetRenderTrait1() const
		{
			return IMaterial::GetRenderTrait1();
		}
		virtual DWORD GetRenderTrait2() const
		{
			return IMaterial::GetRenderTrait2();
		}

		//--
		void SetAlphaMap(const TCHAR *szName);
	public:
		MRenderTex*		m_pReflectMap;
		MRenderTex*		m_pRefractMap;
		MBitmap*		m_pBumpMap;
		MBitmap*		m_pAlphaMap;
		Color4f			m_blendColor;	 // 混合因子和颜色
		float			m_bumpScale;	 // bump map缩放值
		Vector2         m_bumpUVScroll;  // bump map滚动速度
		float           m_specularScale; // 高光缩放值
		MConstColor		m_color;

		DECL_RTTI_DYNC(MWaterRefract);
	};

}//namespace Cool3D