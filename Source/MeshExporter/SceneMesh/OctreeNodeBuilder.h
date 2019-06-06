#pragma once
#include "..\..\MaxPublic\OctreeNode.h"
#include "..\Math\GeomDefine.h"
#include "OctreeBuilder.h"
#include <list>
using namespace Cool3D;

class OctreeNodeBuilder : public OctreeNode
{
	struct tagNode 
	{
		AABBox	box;
		WORD	id;
	};

	typedef list<AABBox>::iterator boxListIter;

public:
	OctreeNodeBuilder();
	virtual ~OctreeNodeBuilder();

	void SetBoxSize(float boxSize) 
	{ 
		m_boxSize=boxSize; m_xzEpsilon=boxSize/2.5f; 
		m_yEpsilon=boxSize/2.5f;
		if( m_yEpsilon>5.0f )
		{
			m_yEpsilon = 5.0f;
		}
	}
	void AddMesh(vector<OctreeBuilder::tagNode>& nodes,vector<AABBox>& boxes);
	void Build(vector<AABBox>& boxes,int id,int lv);
	void GetChildrenR(vector<OctreeNode*>& out);

private:
	bool BoxIfNeedSplit(const AABBox& box);
	bool NodeIfNeedSplit(UINT numNodes);
	void BuildChildBoxes(const AABBox& parentBox,AABBox *pBoxes,int& numBoxes);
	static int NexID()	{	return ++m_idHold;}
	const AABBox& MergeNode(const OctreeBuilder::tagNode& node);
	void Prepare(vector<OctreeBuilder::tagNode>& nodes,vector<AABBox>& mergedBoxes,AABBox& parentBox);
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
	float m_boxSize;
	float m_xzEpsilon;
	float m_yEpsilon;
};