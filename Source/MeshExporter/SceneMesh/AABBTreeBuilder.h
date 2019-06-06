#pragma once
#include <string>
#include <vector>
using namespace std;
#include "..\..\MaxPublic\VertexDefine.h"
#include "..\..\MaxPublic\AABBTree.h"
using namespace Cool3D;

class AABBTreeBuilder :	public AABBTree
{
public:
	AABBTreeBuilder(void);
	virtual ~AABBTreeBuilder(void);

	void BeginBuild(BYTE *pVB, EVertType vertType);
	void AddSubMesh(TriFace *pFaces,int numTri,WORD numVert);
	//!返回一共有多少个node
	int EndBuild();

private:
	//! 将这个Node进行递归的切分
	void SplitNode(AABBTreeNode* pNode);
	//! 这个Node是否需要继续切分
	bool ContinueSplit(AABBTreeNode* pNode);


	//!为指定的Node中的faces建立aabbox
	void BuildAABBox(AABBTreeNode* pNode);
	//!根据Node的AABBox选出将Node一分为而的Plane
	Vector4 SelectSplitPlane(AABBTreeNode* pNode);
	int CalSplitPlaneScore(AABBTreeNode* pNode,Vector4 plane);
	//!计算指定Face的中心点
	Vector3 FaceCenter(TriFace *pFace);
	//!将tree中加入一个Node,返回id
	int AddNode(AABBTreeNode* pNode)	
	{	
		int ret=m_nodes.size();
		m_nodes.push_back(pNode);
		return ret;
	}
	void GetPos(UINT i,Vector3& out);
	
private:
	BYTE			*m_pVB;
	EVertType		m_vertType;
	FaceBuffer		m_buildBuffer;
	DWORD			m_vertIndex;
};
