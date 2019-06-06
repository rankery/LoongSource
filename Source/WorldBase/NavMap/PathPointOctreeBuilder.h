#pragma once
#include "NPCNavGraph.h"

namespace WorldBase
{
	class PathPointOctreeNodeBuilder;

	class PathPointOctreeBuilder
	{
	public:
		PathPointOctreeBuilder(void);
		~PathPointOctreeBuilder(void);

		void Begin(void);
		void AddPoint(const Vector3& point);
		bool End(void);
		bool Save(const TCHAR *szFilePath);

	private:
		PathPointOctreeNodeBuilder	*m_pRoot;

		class Member;
		Member	*m_p;
	};
}//namespace WorldBase
