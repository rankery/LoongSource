#pragma once
#include "..\SceneGraph\scenenode.h"
#include "..\Math\Ray.h"
#include "TerrainRes.h"
#include "TerrainPatchRes.h"
#include "TerrainPatchRenderGroupsRes.h"

namespace Cool3D
{
	class IMaterial;
	class DynamicMesh;
	class Heightmap;
	class TerrainPatchRender;
	class TerrainRes;
	class TerrainPatchRes;
	class TerrainPatchRenderGroupsRes;
	/**	\class TerrainPatch 
		\brief 地形块
	*/
	class TerrainPatch : public SceneNode
	{
	public:
		enum{MAX_LOD=2};
		enum EMtlType
		{
			EMType_PreCombine=0,
			EMType_TwoLayers,
			EMType_Editor,
			EMType_Num
		};
	public:
		TerrainPatch(void);
		virtual ~TerrainPatch(void);

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
		/**	\todo res创建好之后创建mtl
		*/
		virtual void Update(float deltaTime,SceneGraph *pSG);
		virtual void ForceUpdate(float deltaTime,SceneGraph *pSG);

		//--创建贴花mesh
		void BuildDecalMesh( const AABBox& box,DynamicMesh *pMesh, const Vector3* pNormal );

		int GetLod(){ return m_sgLod;}

		void LoadTerrainRes(const TCHAR* szName);
		void LoadTerrainPatchRes(const TCHAR* szName);
		void LoadTerrainPatchRenderGroupsRes(const TCHAR* szName);

		TerrainRes*	GetTerrainRes() { return m_pTerrainRes;}
		TerrainPatchRes* GetTerrainPatchRes(){ return m_pTerrainPatchRes;}
		TerrainPatchRenderGroupsRes* GetTerrainPatchRenderGroupsRes() { return m_pTerrainPatchRenderGroupsRes;}

		IMaterial* GetRenderGroupMtl(int group);

		//--editor support
		void GetVertexBuffer(VertStream* vb);
		void GetIndexBuffer(vector<WORD>& ib);

		//--
		virtual bool IsLoaded();

	protected:
		//--通过TerrainPatchRes创建贴花mesh
		void BuildDecalFromRes(const AABBox& box, DynamicMesh *pMesh);
		void BuildDecalClipTri( const Vector3* pTri, const AABBox& box, DynamicMesh* pMesh );
	protected:
		tstring					m_szTerrainRes;
		tstring					m_szTerrainPatchRes;
		tstring					m_szTerrainPatchRenderGroupsRes;

		TerrainRes*				m_pTerrainRes;
		TerrainPatchRes*		m_pTerrainPatchRes;
		TerrainPatchRenderGroupsRes* 
								m_pTerrainPatchRenderGroupsRes;
		TerrainPatchRender*		m_pRender;

		IMaterial*				m_mtls[EMType_Num];
		vector<IMaterial*>		m_renderGroupsMtl;

		int						m_sgLod;
		DWORD					m_lastForceUpdateTime;

		DECL_RTTI_DYNC(TerrainPatch);
	};
}//namespace Cool3D