#pragma once
#include "resrenderable.h"

namespace F3D
{
	/**	\class ResCombine 
		\brief 组合节点,可能由多个模型+特效组成
	*/
	class ResCombine :	public ResRenderable
	{
	public:
		ResCombine(void);
		virtual ~ResCombine(void);
	};
}//namespace F3D