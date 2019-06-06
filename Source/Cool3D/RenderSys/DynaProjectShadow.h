#pragma once
#include "projectshadow.h"

namespace F3D
{
	/**	\class DynaProjectShadow 
		\brief 动态的投射阴影,几何形状和贴图都会改变
		\remarks 因为其主体在世界中移动,所以投射到的几何体会改变;
		因为其主体会有动画,所以贴图也要改变
	*/
	class DynaProjectShadow : public ProjectShadow
	{
	public:
		DynaProjectShadow(void);
		virtual ~DynaProjectShadow(void);
	};
}//namespace F3D