#pragma once
#include "scenegraph.h"

namespace F3D
{
	class StaticSceneNode :	public SceneGraph
	{
	public:
		StaticSceneNode(void);
		virtual ~StaticSceneNode(void);
	};
}//namespace F3D