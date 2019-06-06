#pragma once

namespace F3D
{
	/**	\class ProjectShadow
		\brief 投射阴影,将物体的侧影画到一个贴图上,然后将此贴图投射的接收影子的物体上
	*/
	class ProjectShadow
	{
	public:
		ProjectShadow(void);
		virtual ~ProjectShadow(void);
	};
}//namespace F3D