#pragma once
#include "resrenderable.h"
#include "..\Math\Ray.h"

namespace Cool3D
{
	class AABBTreeNode;
	class DynamicMesh;
	class VertStream;
	/**	\class ResSceneMesh 
		\brief 场景静态Mesh,相当于ResStaticMesh附加碰撞信息(AABBTree)
	*/
	class Cool3D_API ResSceneMesh : public ResRenderable
	{
	public:
		ResSceneMesh(const TCHAR* szName);
		virtual ~ResSceneMesh(void);

		//--ResBase
		virtual UINT CreateContent(DWORD param);
		//--ResRenderable
		virtual UINT GetNumRenderNode() const;
		virtual RenderNode *GetRenderNode(UINT i) const;
		virtual const AABBox	*GetAABBox() const;
		virtual int GetNumMtl() const;
		virtual void CreateMtlArray(vector<IMaterial*>&	mtls,const TCHAR* szLightMap,float mtlDiffuseFactor=1.0f) const;

		void DrawAABBTree();
		void DrawOctree();

		//利用AABB树进行线段与物体的碰撞检测
		bool TestLineCollision(Vector3 vPoint1,Vector3 vPoint2,Matrix4 WorldMat);
		TResult RayCollide(const Ray& ray,const Matrix4 *pObj2World);

		//--editor support
		VertStream* GetVertStream();
		void GetIndexBuffer(vector<WORD>& ib);

		//--lightmap
		int GetLightMapSize(void) const;
		bool SetLightMapSize(int size);

		/**创建贴花Mesh（参考GPG2-4.8），为了优化效率使用AABBox剪裁
		*/
		void BuildDecalMesh( const AABBox& boxLocal,DynamicMesh *pMesh, const Matrix4& matWorld, const Vector3* pNormal );
		void BuildDecalMeshByAABBTree( AABBTreeNode* pNode, const AABBox& boxLocal, DynamicMesh *pMesh, const Matrix4& matWorld, const Vector3* pNormalLocal );
	private:
		class Member;
		Member	*m_p;

		DECL_RTTI(ResSceneMesh);
	};
}//namespace Cool3D