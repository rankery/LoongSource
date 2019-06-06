#ifndef tstring
#if _UNICODE
#define tstring wstring
#else
#define tstring string
#endif
#endif
#include <max.h>
#include "..\MsgBox.h"
#include ".\aabbtreebuilder.h"


AABBTreeBuilder::AABBTreeBuilder(void)
: m_pVB(0)
, m_vertIndex(0)
{
	
}

AABBTreeBuilder::~AABBTreeBuilder(void)
{
	m_pVB=NULL;
}

void AABBTreeBuilder::BeginBuild(BYTE *pVB, EVertType vertType)
{
	m_pVB=pVB;
	m_vertType=vertType;
	m_buildBuffer.clear();
	m_vertIndex=0;
}

void AABBTreeBuilder::AddSubMesh(TriFace *pFaces,int numTri,WORD numVert)
{
	for(int i=0;i<numTri;i++)
	{
		TriFace face;
		face.index[0] = pFaces[i].index[0] + m_vertIndex + numVert;
		face.index[1] = pFaces[i].index[1] + m_vertIndex + numVert;
		face.index[2] = pFaces[i].index[2] + m_vertIndex + numVert;

		m_buildBuffer.push_back(face);
	}

	m_vertIndex+=numVert;
}

int AABBTreeBuilder::EndBuild()
{
	AABBTreeNode *pRoot=new AABBTreeNode;
	pRoot->m_faces=m_buildBuffer;
	pRoot->m_lv=0;
	BuildAABBox(pRoot);

	AddNode(pRoot);
	SplitNode(pRoot);

#if 1
	g_msgBox.ShowMsg(_T("Begin AABBtree report:\r\n"));
	for(size_t i=0;i<m_nodes.size();i++)
	{
		AABBTreeNode *pNode=m_nodes[i];
		if(pNode->m_childrenID[0]==-1 && pNode->m_childrenID[1]==-1)
		{
			g_msgBox.ShowMsg(_T("Leaf Node %d,lv=%d,num faces=%d\r\n"),
				i,pNode->m_lv,pNode->m_faces.size());
		}
	}
	g_msgBox.ShowMsg(_T("End ofAABBtree report.\r\n"));
#endif

	return m_nodes.size();
}

void AABBTreeBuilder::SplitNode(AABBTreeNode* pNode)
{
	if(ContinueSplit(pNode))
	{
		AABBTreeNode *pNewNode1=new AABBTreeNode;
		AABBTreeNode *pNewNode2=new AABBTreeNode;

		//将pNode中所有face分发到两个new node中
		Vector4 plane=SelectSplitPlane(pNode);
		for(size_t i=0;i<pNode->m_faces.size();i++)
		{
			TriFace& face=pNode->m_faces[i];
			Vector3 p=FaceCenter(&face);
			float d=plane.x*p.x+plane.y*p.y+plane.z*p.z+plane.w;
			if(d>0)
				pNewNode1->m_faces.push_back(face);
			else
				pNewNode2->m_faces.push_back(face);
		}
		pNode->m_faces.clear();
		BuildAABBox(pNewNode1);
		BuildAABBox(pNewNode2);

		//--添加到tree中
		pNewNode1->m_lv=pNode->m_lv+1;
		pNewNode2->m_lv=pNode->m_lv+1;
		
		int id1=-1;
		if(pNewNode1->m_faces.size()>0)//如果Tree是平衡的话,应该不会出现一个孩子是空的情况
			id1=AddNode(pNewNode1);			//但是,如果这个孩子是空,那就没必要保存他了
		else
		{
			delete pNewNode1;
			pNewNode1=NULL;
		}
		int id2=-1;
		if(pNewNode2->m_faces.size()>0)
			id2=AddNode(pNewNode2);
		else
		{
			delete pNewNode2;
			pNewNode2=NULL;
		}
		
		pNode->m_childrenID[0]=id1;
		pNode->m_childrenID[1]=id2;

		//递归
		if(pNewNode1)
			SplitNode(pNewNode1);
		if(pNewNode2)
			SplitNode(pNewNode2);
	}//endof split
}

bool AABBTreeBuilder::ContinueSplit(AABBTreeNode* pNode)
{
	if(pNode->m_faces.size() <= 8)//是否有face
		return false;
	
	return pNode->m_lv<20;
}

void AABBTreeBuilder::GetPos(UINT i,Vector3& out)
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

void AABBTreeBuilder::BuildAABBox(AABBTreeNode* pNode)
{
	pNode->m_boxMax=Vector3(-FLOAT_MAX,-FLOAT_MAX,-FLOAT_MAX);
	pNode->m_boxMin=Vector3(FLOAT_MAX,FLOAT_MAX,FLOAT_MAX);
	Vector3& max=pNode->m_boxMax;
	Vector3& min=pNode->m_boxMin;

	Vector3 pos;
	for(size_t i=0;i<pNode->m_faces.size();i++)
	{
		TriFace& face=pNode->m_faces[i];
		for(int j=0;j<3;j++)
		{
			GetPos(face.index[j], pos);
			if(pos.x > max.x)
				max.x=pos.x;
			if(pos.y > max.y)
				max.y=pos.y;
			if(pos.z > max.z)
				max.z=pos.z;

			if(pos.x < min.x)
				min.x=pos.x;
			if(pos.y < min.y)
				min.y=pos.y;
			if(pos.z < min.z)
				min.z=pos.z;
		}
	}
}

Vector4 AABBTreeBuilder::SelectSplitPlane(AABBTreeNode* pNode)
{
	// 根据面的集合计算m_faces的中心点，判断每根轴的平衡系数，选取最平衡的进行划分
	UINT nbFace = pNode->m_faces.size();
	Vector3 center(0.0f,0.0f,0.0f);
	Vector3 pos;
	for( UINT i=0; i<nbFace; i++ )
	{
		TriFace& tri = pNode->m_faces[i];

		GetPos(tri.index[0], pos);
		center += pos;
	}
	center /= nbFace;

	float results[3];
	// X轴 yz plane
	Vector4 xPlane( 1.0f, 0.0f, 0.0f, -center.x );
	results[0] = CalSplitPlaneScore( pNode, xPlane );

	// Y轴 xz plane
	Vector4 yPlane( 0.0f, 1.0f, 0.0f, -center.y );
	results[1] = CalSplitPlaneScore( pNode, yPlane );

	// Z轴 xy plane
	Vector4 zPlane( 0.0f, 0.0f, 1.0f, -center.z );
	results[2] = CalSplitPlaneScore( pNode, zPlane );

	UINT min = 0;
	if( results[1] < results[min] )	min = 1;
	if( results[2] < results[min] )	min = 2;
	
	if( min == 0 )
		return xPlane;
	else if( min == 1 )
		return yPlane;
	else
		return zPlane;
}

int AABBTreeBuilder::CalSplitPlaneScore(AABBTreeNode* pNode,Vector4 plane)
{
	int front=0,back=0;

	for(size_t i=0;i<pNode->m_faces.size();i++)
	{
		TriFace& face=pNode->m_faces[i];
		Vector3 p=FaceCenter(&face);
		float d=plane.x*p.x+plane.y*p.y+plane.z*p.z+plane.w;
		if(d>0)
			front++;
		else
			back++;
	}

	return abs(front-back);
}

Vector3 AABBTreeBuilder::FaceCenter(TriFace *pFace)
{
	Vector3 pos0,pos1,pos2;
	GetPos(pFace->index[0], pos0);
	GetPos(pFace->index[1], pos1);
	GetPos(pFace->index[2], pos2);
	return (pos0+pos1+pos2)/3;
}