#pragma once
#include "..\Math\GeomDefine.h"
#include "..\..\MaxPublic\VertexDefine.h"
#include "..\..\MaxPublic\SubMesh.h"
using namespace Cool3D;

class BintreeNodeBuilder;
class BintreeBuilder
{
public:
	struct tagNode
	{
		Vector3 a;
		Vector3 b;
		Vector3 c;
		AABBox	box;
		WORD	id;
	};

public:
	BintreeBuilder();
	virtual ~BintreeBuilder();


	void Begin(BYTE *pVB,EVertType vertType,int numVert);
	void AddSubMesh(TriFace *pFaces,int numTri);
	void End();
	void Build(vector<AABBox>& nodes);

	void WriteFile(FILE *fp);

	//--
	int GetNumNode() { return m_nodes.size(); }

private:
	void BuildAABBox(const TriFace& face,tagNode& node);
	void GetPos(UINT i,Vector3& out);

private:
	BYTE				*m_pVB;
	int					m_numVert;
	vector<Vector3>		m_vertices;
	FaceBuffer			m_FaceBuffer;
	EVertType			m_vertType;
	BintreeNodeBuilder *m_pRoot;
	vector<tagNode>		m_nodes;
};