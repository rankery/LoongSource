#pragma once
#include "IRenderMtl.h"
#include "..\RenderMtl\MConstColor.h"

namespace Cool3D
{
	class MBitmap;
	class ResPixelShader;
	/**	\class MWater 
		\brief Ë®Ãæ²ÄÖÊ
	*/
	class MWater : public IMaterial
	{
	public:
		MWater(void);
		virtual ~MWater(void);

		//--IMaterial
		virtual bool IsResourceReady() const;
		virtual void UpdateModifiers(float deltaTime);

		//--
		void SetFrames(const TCHAR* szBaseName,int waterFrm,float waterFps);
		void SetAlphaMap(const TCHAR* szName);

	public:
		MBitmap			*m_pFrames;
		MBitmap			*m_pAlphaMap;
		MConstColor		m_color;

		DECL_RTTI_DYNC(MWater);
	};
}//namespace Cool3D