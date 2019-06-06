
#include "StdAfx.h"
#include "..\VFS\IFS.h"
#include "AABBTreeCollider.h"
#include "..\Device\DrawX.h"


namespace Cool3D
{
	AABBTreeCollider::AABBTreeCollider()
	{}

	AABBTreeCollider::~AABBTreeCollider()
	{}

	void DrawAABBTreeNodeR(AABBTree* pTree,AABBTreeNode *pNode)
	{
		if(pNode==NULL)
			return;
		AABBox box;
		box.max=pNode->m_boxMax;
		box.min=pNode->m_boxMin;
	    DrawX::DrawAABBox(box);

		if(pNode->m_childrenID[0]!=-1)
			DrawAABBTreeNodeR(pTree,pTree->GetNode(pNode->m_childrenID[0]));
		if(pNode->m_childrenID[1]!=-1)
			DrawAABBTreeNodeR(pTree,pTree->GetNode(pNode->m_childrenID[1]));
	}

	void AABBTreeCollider::Draw()
	{
		AABBTreeNode *pRoot=GetNode(0);
		DrawAABBTreeNodeR(this,pRoot);
	}

	TResult AABBTreeCollider::RayCollide(const Ray& ray,const Matrix4 *pObj2World,VertStream *pVS)
	{
		//--把World空间的ray变换到object空间
		Ray localRay;
		Matrix4 world2Obj;
		D3DXMatrixInverse(&world2Obj,NULL,pObj2World);
		D3DXVec3TransformCoord(&localRay.origin,&ray.origin,&world2Obj);
		D3DXVec3TransformNormal(&localRay.dir,&ray.dir,&world2Obj);
		
		//--碰撞检测
		TResult ret(false,FLOAT_MAX);
		RayCollideR(GetNode(0),localRay,ret,pVS);
		
		return ret;
	}

	void AABBTreeCollider::RayCollideR(AABBTreeNode *pNode,const Ray& localRay,TResult& lastTR,VertStream *pVS)
	{
		AABBox box;
		box.max = pNode->m_boxMax; 
		box.min = pNode->m_boxMin;

		//检测是否与自己相交
		TResult tr=localRay.Intersect(box);
		if(!tr.first)
			return;
		
		//检测节点内的每个三角形
		int numFaces=(int)pNode->m_faces.size();
		if(numFaces>0)
		{
			if( pVS->GetType()==EVType_PNT )
			{
				Vert_PNT *pVert=(Vert_PNT *)pVS->GetRawBuffer();
				for(int i=0;i<numFaces;i++)
				{
					TriFace& face=pNode->m_faces[i];
					tr=localRay.IntersectTriangle(pVert[face.index[0]].pos,
						pVert[face.index[1]].pos,
						pVert[face.index[2]].pos);
					if(tr.first
						&& tr.second>0.0f && tr.second<lastTR.second)
						lastTR=tr;
				}
			}
			else if( pVS->GetType()==EVType_PNT2 )
			{
				Vert_PNT2 *pVert=(Vert_PNT2 *)pVS->GetRawBuffer();
				for(int i=0;i<numFaces;i++)
				{
					TriFace& face=pNode->m_faces[i];
					tr=localRay.IntersectTriangle(pVert[face.index[0]].pos,
						pVert[face.index[1]].pos,
						pVert[face.index[2]].pos);
					if(tr.first
						&& tr.second>0.0f && tr.second<lastTR.second)
						lastTR=tr;
				}
			}
			else
			{
				ASSERT(0&&_T("顶点格式不符合！"));
			}

		}//endof if

		//--递归
		if(pNode->m_childrenID[0]!=-1)
		{
			RayCollideR(GetNode(pNode->m_childrenID[0]),localRay,lastTR,pVS);
		}

		if(pNode->m_childrenID[1]!=-1)
		{
			RayCollideR(GetNode(pNode->m_childrenID[1]),localRay,lastTR,pVS);
		}
	}

	//判断线段是否在树的叶子节点相交
	bool AABBTreeCollider::LineCollide(Vector3 vPoint1,Vector3 vPoint2,Matrix4 WorldMat)
	{
		AABBTreeNode *pRoot=GetNode(0);
		return LineCollideR(this,pRoot,vPoint1,vPoint2,WorldMat);
	}

	//判断线段是否在树的叶子节点相交（递归）
	bool AABBTreeCollider::LineCollideR(AABBTree* pTree,AABBTreeNode *pNode,Vector3 vPoint1,Vector3 vPoint2,Matrix4 WorldMat)
	{
    	if(pNode==NULL)
			return false;

		if(pNode->m_childrenID[0]==-1 && pNode->m_childrenID[1]==-1)
		{
			AABBox box;
			box.max = pNode->m_boxMax; 
			box.min = pNode->m_boxMin;

		    //计算线段的相对坐标
			Vector3 pos=Vector3(WorldMat._41,WorldMat._42,WorldMat._43);
			Vector3 p1=vPoint1-pos;
			Vector3 p2=vPoint2-pos;

			D3DXVec3TransformCoord(&p1,&p1,&WorldMat);
			p1=p1-pos;

			D3DXVec3TransformCoord(&p2,&p2,&WorldMat);
			p2=p2-pos;

			if(box.IntersectsWithLine(p1,p2))
			    return true;
		}

		if(pNode->m_childrenID[0]!=-1)
		{
		    bool bResult=LineCollideR(pTree,pTree->GetNode(pNode->m_childrenID[0]),vPoint1,vPoint2,WorldMat);
			if(bResult)
				return true;
		}
	
		if(pNode->m_childrenID[1]!=-1)
		{
			bool bResult=LineCollideR(pTree,pTree->GetNode(pNode->m_childrenID[1]),vPoint1,vPoint2,WorldMat);
			if(bResult)
				return true;
		}

		return false;
	}

}//namespace Cool3D