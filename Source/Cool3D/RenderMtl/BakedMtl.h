#pragma once
#include "diffusemtl.h"

namespace F3D
{
	/**	\class BakedMtl 
		\brief 使用Diffuse和LightingMap的才智
	*/
	class BakedMtl : public DiffuseMtl
	{
	public:
		BakedMtl(void);
		virtual ~BakedMtl(void);

		DECL_RTTI_DYNC(BakedMtl);
	};
}//namespace F3D