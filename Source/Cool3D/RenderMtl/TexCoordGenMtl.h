#pragma once
#include "diffusemtl.h"

namespace F3D
{
	/**	\class TexCoordGenMtl 
		\brief 使用Diffuse贴图,并需要为第二层贴图自动产生纹理坐标的材质
		\remarks 支持Sphere map, cube map
	*/
	class TexCoordGenMtl : public DiffuseMtl
	{
	public:
		TexCoordGenMtl(void);
		virtual ~TexCoordGenMtl(void);

		DECL_RTTI_DYNC(TexCoordGenMtl);
	};
}//namespace F3D