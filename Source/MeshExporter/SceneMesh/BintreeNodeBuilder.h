#pragma once
#include "..\..\MaxPublic\BintreeNode.h"
#include "..\Math\GeomDefine.h"
#include "BintreeBuilder.h"
using namespace Cool3D;

class BintreeNodeBuilder : public BintreeNode
{
public:
	BintreeNodeBuilder();
	virtual ~BintreeNodeBuilder();

	void Build(vector<BintreeBuilder::tagNode>& nodes,const AABBox& box,int lv);

private:
	bool IfNeedSplit(vector<BintreeBuilder::tagNode>& nodes);
	void SplitBalanceBox(vector<BintreeBuilder::tagNode>& nodes,AABBox splitBox[8]);
	int CalSplitPlaneScore(vector<BintreeBuilder::tagNode>& nodes,Vector4 plane);
};