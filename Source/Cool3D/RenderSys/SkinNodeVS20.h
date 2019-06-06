#pragma once
#include "skinnode.h"

namespace F3D
{
	/**	\class SkinNodeVS20
		\brief 使用Vertex shader 2.0计算Skin的Skin node
	*/
	class SkinNodeVS20 : public SkinNode
	{
	public:
		SkinNodeVS20(void);
		virtual ~SkinNodeVS20(void);
	};
}//namespace F3D