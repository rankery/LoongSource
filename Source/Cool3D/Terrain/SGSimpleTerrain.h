#pragma once
#include "..\SceneGraph\scenenode.h"
#include "..\SceneGraph\QuadTreeData.h"

namespace Cool3D
{
	class RSimpleTerrain;
	class SGQuadTreeNode;
	class CameraBase;
	class Frustum;
	class SimpleTerrainMtl;
	/**	\class SGSimpleTerrain 
		\brief 远处地形
	*/
	class SGSimpleTerrain : public SceneNode
	{
	public:
		SGSimpleTerrain(void);
		virtual ~SGSimpleTerrain(void);

		//--序列化接口
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);
		//--SceneNode
		virtual bool IsInFrustum(const Frustum *pFrumstum) const;
		virtual void LoadRes(const TCHAR* szName);
		virtual UINT GetNumRenderNode() const;
		virtual RenderNode *GetRenderNode(UINT i) const;
		virtual const Matrix4& GetWorldMat() const;
		virtual void SetWorldMat(const Matrix4 *pMat);
		virtual void GetBox(AABBox& out) const;
		virtual IMaterial*	GetMtl(int mtlID);
		virtual void Update(float deltaTime,SceneGraph *pSG);
		virtual void ForceUpdate(float deltaTime,SceneGraph *pSG);

		//--
		void Update(Frustum& frustum,float deltaTime,SceneGraph *pSG);
		void InvalidViewZone(const AABBox& viewZone);
		void LoadQuadTree(IFS* pFS,const TCHAR* szFileName);
		void SetColor(const Color4ub& color);
		const vector<SceneNode*> GetVisPatchNodes(){ return m_visNodes;}

	protected:

		RSimpleTerrain* m_pRenderNode;
		SimpleTerrainMtl* m_pMtl;

		QuadTreeData m_data;
		SGQuadTreeNode* m_pRoot;

		vector<SceneNode*> m_visNodes;

		DECL_RTTI_DYNC(SGSimpleTerrain);
	};
}//namespace Cool3D