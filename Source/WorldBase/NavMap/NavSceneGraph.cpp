#include "stdafx.h"
#include "NavSceneGraph.h"

namespace WorldBase
{
	IMP_RTTI(NavSceneGraph,RTTIObj);

	NavSceneGraph::NavSceneGraph()
	{
		m_viewZoneSize=Vector3(1000.0f,1000.0f,1000.0f);
	}

	NavSceneGraph::~NavSceneGraph()
	{
	}

	void NavSceneGraph::CalCurrentViewZone( const Vector3& zoneCenter,AABBox& out )
	{
		out.max=zoneCenter+m_viewZoneSize;
		out.min=zoneCenter-m_viewZoneSize;
	}
}//namespace WorldBase