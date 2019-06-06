#pragma once
#include "quadtreescenegraph.h"

namespace F3D
{
	class QuadTreeBuilder :	public QuadTreeSceneGraph
	{
	public:
		QuadTreeBuilder(void);
		virtual ~QuadTreeBuilder(void);
	};
}//namespace F3D