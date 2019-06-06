#pragma once
#include "projectshadow.h"

namespace F3D
{
	/**	\class StaticProjectShadow 
		\brief 静态的投射阴影,几何形状和贴图都不变
	*/
	class StaticProjectShadow :	public ProjectShadow
	{
	public:
		StaticProjectShadow(void);
		virtual ~StaticProjectShadow(void);
	};
}//namespace F3D