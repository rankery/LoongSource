
#pragma once
#include "..\Math\GeomDefine.h"
#include "..\Math\Ray.h"

class IFS;
#define VFS_READ
#include "..\Public\AABBTree.h"
#include "VertStream.h"

namespace Cool3D
{
	class Cool3D_API AABBTreeCollider : public AABBTree
	{
	public:
		AABBTreeCollider();
		virtual ~AABBTreeCollider();

		void Draw();
		TResult RayCollide(const Ray& ray,const Matrix4 *pObj2World,VertStream *pVS);

		//判断线段是否在树的叶子节点相交
		bool LineCollide(Vector3 vPoint1,Vector3 vPoint2,Matrix4 WorldMat);

	private:
		void RayCollideR(AABBTreeNode *pNode,const Ray& localRay,TResult& lastTR,VertStream *pVS);
		//判断线段是否在树的叶子节点相交（递归）
		bool LineCollideR(AABBTree* pTree,AABBTreeNode *pNode,Vector3 vPoint1,Vector3 vPoint2,Matrix4 WorldMat);

	};
}//namespace Cool3D