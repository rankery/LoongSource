#pragma once
#include "diffusemtl.h"

namespace F3D
{
	/**	\class BumpMtl 
		\brife 使用Diffuse和Bump Map的材质
	*/
	class BumpMtl :	public DiffuseMtl
	{
	public:
		BumpMtl(void);
		virtual ~BumpMtl(void);

		DECL_RTTI_DYNC(BumpMtl);
	};
}//namespace F3D