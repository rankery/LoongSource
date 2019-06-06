#pragma once
#include "ColorMtl.h"

namespace F3D
{
	/**	\class DiffuseMtl 
		\brief 只使用diffuse贴图的材质
	*/
	class DiffuseMtl : public ColorMtl
	{
	public:
		DiffuseMtl(void);
		virtual ~DiffuseMtl(void);

		DECL_RTTI_DYNC(DiffuseMtl);
	};
}//namespace F3D