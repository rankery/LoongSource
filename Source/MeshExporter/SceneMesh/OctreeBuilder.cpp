#include "OctreeBuilder.h"
#include "OctreeNodeBuilder.h"
using namespace Cool3D;

OctreeBuilder::OctreeBuilder()
: m_pVB(0)
, m_boxSize(50.0f)
, m_pRoot(0)
, m_vertIndex(0)
{
}

OctreeBuilder::~OctreeBuilder()
{
	if(m_pRoot!=NULL)
	{
		delete m_pRoot;
		m_pRoot=NULL;
	}
}

void OctreeBuilder::GetPos(UINT i,Vector3& out)
{
	switch(m_vertType)
	{
	case EVType_PNGT:
		{
			Vert_PNGT *pV=(Vert_PNGT *)m_pVB;
			pV+=i;
			out=pV->pos;
		}
		break;
	case EVType_PNT:
		{
			Vert_PNT *pV=(Vert_PNT *)m_pVB;
			pV+=i;
			out=pV->pos;
			break;
		}
	case EVType_PNT2:
		{
			Vert_PNT2 *pV=(Vert_PNT2 *)m_pVB;
			pV+=i;
			out=pV->pos;
			break;
		}
	case EVType_PT2:
		{
			Vert_PT2 *pV=(Vert_PT2 *)m_pVB;
			pV+=i;
			out=pV->pos;
			break;
		}
	case EVType_PD:
		{
			Vert_PD *pV=(Vert_PD *)m_pVB;
			pV+=i;
			out=pV->pos;
			break;
		}
	case EVType_PTD:
		{
			Vert_PTD *pV=(Vert_PTD *)m_pVB;
			pV+=i;
			out=pV->pos;
			break;
		}
	case EVType_PTp:
		{
			Vert_PTp *pV=(Vert_PTp *)m_pVB;
			pV+=i;
			out=pV->pos;
			break;
		}
	case EVType_Water:
		{
			Vert_Water *pV=(Vert_Water *)m_pVB;
			pV+=i;
			out=pV->pos;
			break;
		}
	case EVType_PT:
		{
			Vert_PT *pV=(Vert_PT *)m_pVB;
			pV+=i;
			out=pV->pos;
			break;
		}
	}
}

void OctreeBuilder::BuildAABBox(const TriFace& face,tagNode& node)
{
	node.box.max=Vector3(-FLOAT_MAX,-FLOAT_MAX,-FLOAT_MAX);
	node.box.min=Vector3(FLOAT_MAX,FLOAT_MAX,FLOAT_MAX);

	GetPos(face.index[0],node.a);
	GetPos(face.index[1],node.b);
	GetPos(face.index[2],node.c);

	Vector3 pos;
	for(int j=0;j<3;j++)
	{
		GetPos(face.index[j], pos);
		if(pos.x > node.box.max.x)
			node.box.max.x=pos.x;
		if(pos.y > node.box.max.y)
			node.box.max.y=pos.y;
		if(pos.z > node.box.max.z)
			node.box.max.z=pos.z;

		if(pos.x < node.box.min.x)
			node.box.min.x=pos.x;
		if(pos.y < node.box.min.y)
			node.box.min.y=pos.y;
		if(pos.z < node.box.min.z)
			node.box.min.z=pos.z;
	}

	//--盒子的最小尺寸为1
	if(fabsf(node.box.max.x-node.box.min.x)<1.0e-6f)
		node.box.max.x=node.box.min.x+1.0f;
	if(fabsf(node.box.max.y-node.box.min.y)<1.0e-6f)
		node.box.max.y=node.box.min.y+1.0f;
	if(fabsf(node.box.max.z-node.box.min.z)<1.0e-6f)
		node.box.max.z=node.box.min.z+1.0f;
}

void OctreeBuilder::Begin( BYTE *pVB,EVertType vertType )
{
	m_pVB = pVB;
	m_vertType = vertType;
	m_vertIndex = 0;
	m_boxes.clear();
	m_nodes.clear();

	if( m_pRoot != NULL )
	{
		delete m_pRoot;
		m_pRoot = NULL;
	}
	m_pRoot = new OctreeNodeBuilder;
	m_pRoot->SetBoxSize(m_boxSize);
}

void OctreeBuilder::AddSubMesh( TriFace *pFaces,int numTri,WORD numVert )
{
	for(int i=0;i<numTri;i++)
	{
		TriFace face;
		face.index[0] = pFaces[i].index[0] + m_vertIndex + numVert;
		face.index[1] = pFaces[i].index[1] + m_vertIndex + numVert;
		face.index[2] = pFaces[i].index[2] + m_vertIndex + numVert;

		tagNode node;
		BuildAABBox(face,node);
		m_nodes.push_back(node);
	}

	m_vertIndex+=numVert;
}

void OctreeBuilder::End()
{
	if( m_pRoot )
	{
		m_pRoot->AddMesh(m_nodes,m_boxes);
		m_pRoot->Build(m_boxes,0,0);
	}
}

void OctreeBuilder::WriteFile( FILE *fp )
{
	FWriteValVector(fp,m_boxes);
	m_pRoot->WriteFile(fp);
}

void OctreeBuilder::SetBoxSize( float boxSize )
{
	m_boxSize = boxSize;
}