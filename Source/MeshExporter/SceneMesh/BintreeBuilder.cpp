#include "BintreeBuilder.h"
#include "BintreeNodeBuilder.h"

BintreeBuilder::BintreeBuilder()
{
	m_pRoot=NULL;
}

BintreeBuilder::~BintreeBuilder()
{
	m_vertices.clear();
	m_FaceBuffer.clear();
	if(m_pRoot!=NULL)
	{
		delete m_pRoot;
		m_pRoot=NULL;
	}
}

void BintreeBuilder::GetPos(UINT i,Vector3& out)
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

void BintreeBuilder::BuildAABBox(const TriFace& face,tagNode& node)
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
}

void BintreeBuilder::WriteFile( FILE *fp )
{
	FWriteValVector(fp,m_vertices);
	FWriteValVector(fp,m_FaceBuffer);
	m_pRoot->WriteFile(fp);
}

void BintreeBuilder::Begin( BYTE *pVB,EVertType vertType,int numVert )
{
	m_pVB=pVB;
	m_numVert=numVert;
	m_vertType=vertType;
	m_FaceBuffer.clear();
}

void BintreeBuilder::AddSubMesh( TriFace *pFaces,int numTri )
{
	for(int i=0;i<numTri;i++)
		m_FaceBuffer.push_back(pFaces[i]);
}

void BintreeBuilder::End()
{
	if(m_pRoot!=NULL)
	{
		delete m_pRoot;
		m_pRoot=NULL;
	}

	m_pRoot=new BintreeNodeBuilder;
	AABBox rootBox;
	rootBox.min=Vector3(FLOAT_MAX,FLOAT_MAX,FLOAT_MAX);
	rootBox.max=Vector3(-FLOAT_MAX,-FLOAT_MAX,-FLOAT_MAX);
	for(UINT i=0;i<m_FaceBuffer.size();i++)
	{
		tagNode node;
		BuildAABBox(m_FaceBuffer[i],node);
		node.id=i;
		m_nodes.push_back(node);
		rootBox.Merge(node.box);
	}
	for(int vertIndex=0;vertIndex<m_numVert;vertIndex++)
	{
		Vector3 pos;
		GetPos(vertIndex,pos);
		m_vertices.push_back(pos);
	}

	m_pRoot->Build(m_nodes,rootBox,0);
}