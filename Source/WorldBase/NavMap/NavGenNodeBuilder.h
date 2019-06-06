#pragma once

#include "NavGenNode.h"
#include "NavGenBuilder.h"

namespace WorldBase
{
	class NavGenNodeBuilder : public NavGenNode
	{
		struct tagNode 
		{
			AABBox	box;
			WORD	id;
		};

		typedef list<AABBox>::iterator boxListIter;

	public:
		NavGenNodeBuilder();
		virtual ~NavGenNodeBuilder();

		void Build( vector<NavGenBuilder::tagNode>& nodes,vector<AABBox>& boxes,int id,int lv,NavGenBuilder::EBoxSize boxSize );

	private:
		bool BoxIfNeedSplit(const AABBox& box);
		bool NodeIfNeedSplit(UINT numNodes);
		void BuildChildBoxes(const AABBox& parentBox,AABBox *pBoxes,int numBoxes);
		static int NexID()	{	return ++m_idHold;}
		const AABBox& MergeNode(const NavGenBuilder::tagNode& node);
		void Prepare(vector<NavGenBuilder::tagNode>& nodes,vector<AABBox>& mergedBoxes,AABBox& parentBox);
		void BuildOctree(vector<tagNode>& nodes,int id,int level,const AABBox& box);
		void ComputeBoxForNodes(vector<tagNode>& nodes,AABBox& box);
		int CalSplitPlaneScore( vector<tagNode>& nodes,Vector4 plane );
		void SplitBalanceBox( vector<tagNode>& nodes,AABBox splitBox[8] );
		void MergeOctreeBoxes(vector<AABBox>& octreeBoxes);
		void MergeOctreeBoxesByXAxis(vector<AABBox>& octreeBoxes);
		void MergeOctreeBoxesByZAxis(vector<AABBox>& octreeBoxes);
		void MergeOctreeBoxesByYAxis(vector<AABBox>& octreeBoxes);

		Vector4 SelectSplitPlane( vector<tagNode>& nodes,AABBox splitBox[2] );

	private:
		static int	m_idHold;
		Vector3		m_boxSize;
		float		m_epsilon;
	};
}// WorldBase