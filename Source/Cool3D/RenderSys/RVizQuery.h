#pragma once
#include "rendernode.h"

namespace Cool3D
{
	/**	\class RVizQuery 
		\brief 对于指定的RenderNode执行Device上的VizQuery
		\see IDeviceVizQuery
	*/
	class RVizQuery : public RenderNode
	{
	public:
		RVizQuery(void);
		virtual ~RVizQuery(void);
	};
}//namespace Cool3D