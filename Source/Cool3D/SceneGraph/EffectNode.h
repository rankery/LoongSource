#pragma once
#include "scenenode.h"

namespace F3D
{
	/**	\class EffectNode 
		\brief 光等影响渲染结果的节点
		\todo 灯光单独使用一个class处理, 去掉这个class???
	*/
	class EffectNode : public SceneNode
	{
	public:
		EffectNode(void);
		virtual ~EffectNode(void);

		DECL_RTTI(EffectNode);
	};
}//namespace F3D