#pragma once
#include "NPCNavGraph.h"

namespace WorldBase
{
	/** \class PathPointOctreeNode
		\brief 导航点八叉树的结点
	*/
	class PathPointOctreeNode
	{
	public:
		PathPointOctreeNode(void);
		~PathPointOctreeNode(void);

		void ReadFile(IFS* pFS,DWORD hFile);
		void Clone(PathPointOctreeNode* pTarget);

		void QueryPathPoints(const AABBox& box,const vector<NPCNavGraph::tagPathPointRT>& points,list<pair<int,float> >& out);

	protected:
		vector<int>				m_content;
		AABBox					m_box;
		PathPointOctreeNode		*m_pChildren[8];
	};
};//namespace WorldBase
