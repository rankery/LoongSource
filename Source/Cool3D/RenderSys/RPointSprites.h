#pragma once
#include "rendernode.h"

namespace Cool3D
{
	/**	\class RPointSprites 
		\brief Point sprites 渲染,可是使用Device提供的功能,如果Device不支持,则需要使用软件模拟 
	*/
	class RPointSprites : public RenderNode
	{
	public:
		RPointSprites(void);
		virtual ~RPointSprites(void);
	};
}//namespace Cool3D