#pragma once
#include "scenenode.h"

namespace Cool3D
{
	class RVizQuery;
	class RPointSprites;
	/**	\class SGLightFlare 
		\brief 灯光光晕
		\remarks 此node应作为SGLightNode的child使用;
		\par 灯光光晕和镜头光晕并不是一个概念.
	*/
	class SGLightFlare : public SceneNode
	{
	public:
		SGLightFlare(void);
		virtual ~SGLightFlare(void);

	private:
		RVizQuery		*m_pRenderQuery;
		RPointSprites	*m_pRenderNode;
	};
}//namespace Cool3D