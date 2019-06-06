#pragma once
#include "imaterial.h"

namespace Cool3D
{
	//处理换装的：头发颜色贴图
	class MColorTex :
		public IMaterial
	{
	public:
		MColorTex(void);
		virtual ~MColorTex(void);
	};
}//namespace Cool3D