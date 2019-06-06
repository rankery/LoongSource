#pragma once
#include "SceneNode.h"
#include "..\Math\GeomDefine.h"
#include "..\Math\Ray.h"

namespace Cool3D
{
	class QuadTreeData;
	class RenderSys;
	class DynamicMesh;
	class TerrainPatch;
	/**	\class SGQuadTreeNode
		\brief 四叉树空间分割的节点
	*/
	class SGQuadTreeNode : public RTTIObj,public ISerializable
	{
	public:
		SGQuadTreeNode(void);
		virtual ~SGQuadTreeNode(void);

		//--序列化接口
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);
				
		void AttachToRenderSys(SceneGraph *pSG,RenderSys *pSys,Frustum *pFrumstum,float deltaTime,bool bParentVis);
		void AttachToRenderSysForShadowMap(SceneGraph *pSG,RenderSys *pSys,Frustum *pFrumstum,float deltaTime,bool bParentVis);
		void InvalidViewData(const AABBox& viewZone,QuadTreeData *pDataSrc);
		void ForceUpdate(SceneGraph *pSG);

		void BuildProjectorMesh_Trn(const AABBox& box,DynamicMesh *pMesh);
		void BuildDecalMesh( const AABBox& box,DynamicMesh *pMesh, const Vector3* pNormal );

		int GetID() const				{	return m_id;}
		const AABBox& GetBox() const	{	return m_box;}

		//测试视点是否包含在物体中以及遮挡视点的物体半透明化(输入参数为当前视点位置和上一次的视点位置以及看位置，返回为与视点碰撞的物体的包围盒)
		bool TestContainBetweenViewPointAndSceneObject(Vector3 vFrom,Vector3 vOldFrom,Vector3 vLookAt,Cool3D::AABBox& objbox);

		TResult RayCollide(const Ray& ray);

		void GetSceneNodes(const Frustum *pFrumstum,vector<SceneNode*>& sceneNodes,bool bTrnOnly,bool bIgnoreRvr,bool bIgnoreGrass=true);
		void GetSceneNodes(vector<SceneNode*>& sceneNodes);
			
	protected:
		bool IsInFrustum(const Frustum *pFrumstum) const;
		void RayCollideR(const Ray& ray,TResult& lastTR);
	protected:
		int		m_id;	//id in Quad tree scene graph
		int		m_lv;	//level in quad tree,lv 0 is root
		AABBox	m_box;

		//--冗余数据,主要是为了在没有child实例对象的时候检测child是否可见
		int					m_childrenID[4];
		AABBox				m_childrenBox[4];
		
		//--
		vector<SceneNode*>	m_pContent;//实现包含渲染内容的节点
		SGQuadTreeNode*		m_pChildren[4];

		DECL_RTTI_DYNC(SGQuadTreeNode);
	};
}//namespace Cool3D