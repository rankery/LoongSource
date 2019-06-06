#pragma once
#include "..\..\MaxPublic\VertexDefine.h"
#include "..\..\MaxPublic\OctreeNode.h"
#include "..\..\MaxPublic\SubMesh.h"
#include "..\Math\GeomDefine.h"
using namespace Cool3D;
using namespace std;

class OctreeNodeBuilder;
class OctreeBuilder
{
public:
	struct tagNode 
	{
		Vector3	a;
		Vector3	b;
		Vector3 c;
		AABBox	box;
	};
public:
	OctreeBuilder();
	virtual ~OctreeBuilder();

	void Begin(BYTE *pVB,EVertType vertType);
	void AddSubMesh(TriFace *pFaces,int numTri,WORD numVert);
	void End();

	void SetBoxSize(float boxSize);

	void WriteFile(FILE *fp);

	//--
	int GetNumBox() { return m_boxes.size(); }

private:
	void GetPos(UINT i,Vector3& out);
	void BuildAABBox(const TriFace& faceBuffer,tagNode& box);

public:
	vector<AABBox>		m_boxes;

private:
	BYTE				*m_pVB;
	EVertType			m_vertType;
	vector<tagNode>		m_nodes;
	OctreeNodeBuilder	*m_pRoot;
	float				m_boxSize;
	WORD				m_vertIndex;
};