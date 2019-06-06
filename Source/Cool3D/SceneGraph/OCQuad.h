#pragma once
#include "occluder.h"

namespace Cool3D
{
	/**	\class OCQuad 
		\brief 使用一个quad作为遮挡体
	*/
	class OCQuad : public Occluder
	{
	public:
		OCQuad(void);
		virtual ~OCQuad(void);
	};
}//namespace Cool3D