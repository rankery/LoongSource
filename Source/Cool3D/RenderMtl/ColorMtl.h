#pragma once
#include "imaterial.h"

namespace F3D
{
	/**	\class ColorMtl 
		\brief 只使用一个颜色值,不使用任何贴图的材质
	*/
	class ColorMtl : public IMaterial
	{
	public:
		ColorMtl(void);
		virtual ~ColorMtl(void);

		DECL_RTTI_DYNC(ColorMtl);
	};
}//namespace F3D