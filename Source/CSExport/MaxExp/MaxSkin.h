#pragma once
#include "..\..\F3DMaxExp\MaxMesh.h"
#include "..\..\F3DMaxExp\MaxNodeType.h"

using namespace std;

class BoneInfluence
{
public:
	float		weight;
	tstring		szBoneName;
	Point3		offset;
public:
	BoneInfluence()	{ weight=0.0f; }
	~BoneInfluence(){}
	BoneInfluence(const BoneInfluence& other)
	{
		weight=other.weight;
		szBoneName=other.szBoneName;
		offset=other.offset;
	}
	const BoneInfluence& operator = (const BoneInfluence& other)
	{
		if(this==&other)
			return *this;
		weight=other.weight;
		szBoneName=other.szBoneName;
		offset=other.offset;
		return *this;
	}
};
typedef std::vector<BoneInfluence>	BoneInfArray;

class MaxSkin : public MaxMesh
{
public:
	MaxSkin(INode *pNode,TimeValue time);
	virtual ~MaxSkin();

	void BeginPhysiqueExport();
	void Vert_GetBoneInf(int vertexId,BoneInfArray& binfArray);
	void EndPhysiqueExport();

private:
	void AddBoneInf(BoneInfArray& binfArray,BoneInfluence& binf);
private:
	Modifier		*m_pModifier;
	size_t			m_maxVertBoneInf;//单个顶点最多有多少个骨骼

	//--
	IPhysiqueExport		*m_pPhysiqueExport;
	IPhyContextExport	*m_pContextExport;
};
