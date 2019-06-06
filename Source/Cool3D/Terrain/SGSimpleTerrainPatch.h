#pragma once
#include "..\SceneGraph\scenenode.h"
#include ".\TerrainPatchRes.h"
#include ".\Heightmap.h"
#include "..\ResSys\VertStream.h"

namespace Cool3D
{
	class RSimpleTerrainPatch;
	class SceneGraph;
	/**	\class SGSimpleTerrainPatch 
		\brief 远处地形块
	*/
	class SGSimpleTerrainPatch : public SceneNode
	{
	public:
		enum
		{
			MAX_LOD=2,
		};
	public:
		SGSimpleTerrainPatch(void);
		virtual ~SGSimpleTerrainPatch(void);

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

		//--Editor support
		void CreateGeom(Heightmap* pHMap,RECT& rc);

		inline int GetPatchX() const { return m_patchX; }
		inline int GetPatchZ() const { return m_patchZ; }

	protected:

		VertStream			m_vertStream;
		AABBox				m_box;
		int					m_patchX;
		int					m_patchZ;

		RSimpleTerrainPatch* m_pRenderNode;

		DECL_RTTI_DYNC(SGSimpleTerrainPatch);
	};
}//namespace Cool3D